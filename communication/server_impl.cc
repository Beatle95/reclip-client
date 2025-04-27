#include "communication/server_impl.h"

#include <cassert>
#include <chrono>

#include "base/log.h"
#include "communication/connection_impl.h"
#include "communication/serialization.h"

using namespace std::chrono_literals;
using namespace std::placeholders;

constexpr auto kNetworkTimeoutInterval = 15s;

namespace reclip {

namespace {
ServerImpl::TestHelper* g_test_helper = nullptr;
}

ServerImpl::TestHelper::TestHelper() {
  assert(g_test_helper == nullptr);
  g_test_helper = this;
}

ServerImpl::TestHelper::~TestHelper() {
  assert(g_test_helper == this);
  g_test_helper = nullptr;
}

ServerImpl::ServerImpl(ServerDelegate& delegate)
    : delegate_(&delegate),
      connection_(g_test_helper
                      ? g_test_helper->CreateConnection()
                      : std::make_unique<ServerConnectionImpl>(*this)) {
  assert(connection_);

  reconnect_timer_.setInterval(0s);
  reconnect_timer_.setSingleShot(true);
  connect(&reconnect_timer_, &QTimer::timeout, this, &ServerImpl::ConnectImpl);
  // It is better if Connect will be handled in one place, so I don't call it in
  // constructor.
  reconnect_timer_.start();
}

ServerImpl::~ServerImpl() = default;

void ServerImpl::ConnectImpl() {
  LOG(INFO) << "Connecting to server\n";
  reconnect_timer_.setInterval(g_test_helper ? 0s : 3s);
  state_ = ConnectionState::kConnecting;
  connection_->Connect();
  InitAndRunTimeoutTimer(connection_timer_);
}

void ServerImpl::HandleConnected(bool is_connected) {
  assert(state_ == ConnectionState::kConnecting);
  if (is_connected) {
    // TODO: We have to send secret first, and recieve version.
    LOG(INFO) << "Connection successful";
    RequestFullSync();
  } else {
    LOG(INFO) << "Connection is not successful";
    state_ = ConnectionState::kDisconnected;
    assert(!reconnect_timer_.isActive());
    reconnect_timer_.start();
  }
}

void ServerImpl::RequestFullSync() {
  assert(state_ == ConnectionState::kConnecting);
  state_ = ConnectionState::kSyncing;
  const auto msg_id = GenerateId();
  if (!connection_->SendMessage(msg_id, ClientMessageType::kFullSyncRequest,
                                {})) {
    return;
  }

  assert(!awaiting_responces_.contains(msg_id));
  auto& response = awaiting_responces_[msg_id];
  response.callback = [this](const QByteArray& data) {
    assert(state_ == ConnectionState::kSyncing);
    auto response = ParseSyncResponse(data);
    if (!response.has_value()) {
      LOG(ERROR) << "Got wrong sync response from server";
      connection_->Disconnect();
      return;
    }

    state_ = ConnectionState::kConnected;
    connection_timer_.stop();

    delegate_->OnFullSync(std::move(response->this_host_data),
                          std::move(response->hosts_data));
  };
  InitAndRunTimeoutTimer(response.timeout_timer);
}

void ServerImpl::RequestHostSync(const HostId& id, HostSyncCallback callback) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  const auto msg_id = GenerateId();
  if (!connection_->SendMessage(msg_id, ClientMessageType::kHostSyncRequest,
                                SerializeHostSyncRequest(id))) {
    return;
  }

  assert(!awaiting_responces_.contains(msg_id));
  auto& response = awaiting_responces_[msg_id];
  response.callback = [this,
                       callback = std::move(callback)](const QByteArray& data) {
    if (state_ != ConnectionState::kConnected) {
      return;
    }
    callback(ParseHostData(data));
  };

  InitAndRunTimeoutTimer(response.timeout_timer);
}

void ServerImpl::SyncThisHost(const HostData& data) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  connection_->SendMessage(GenerateId(), ClientMessageType::kSyncThisHost,
                           SerializeThisHostSync(data));
}

void ServerImpl::AddThisHostText(const std::string& text) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  connection_->SendMessage(GenerateId(), ClientMessageType::kHostTextUpdate,
                           SerializeTextUpdate(text));
}

void ServerImpl::ProcessHostConnected(const QByteArray& data) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  if (auto id = ParseHostId(data)) {
    delegate_->HostConnected(id.value());
  } else {
    LOG(ERROR) << "Unable to parse connected host id";
  }
}

void ServerImpl::ProcessHostDisconnected(const QByteArray& data) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  if (auto id = ParseHostId(data)) {
    delegate_->HostDisconnected(id.value());
  } else {
    LOG(ERROR) << "Unable to parse disconnected host id";
  }
}

void ServerImpl::ProcessHostTextUpdate(const QByteArray& data) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  auto text_data = ParseNewText(data);
  if (!text_data.has_value()) {
    LOG(ERROR) << "Got wrong text update notification from server";
    return;
  }
  delegate_->HostTextAdded(text_data->id, text_data->text);
}

void ServerImpl::ProcessHostSynced(const QByteArray& data) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  if (auto host_data = ParseHostData(data)) {
    delegate_->HostSynced(std::move(host_data.value()));
  } else {
    LOG(ERROR) << "Error parsing host data in host sync notification";
  }
}

uint64_t ServerImpl::GenerateId() { return id_counter_++; }

void ServerImpl::HandleDisconnected() {
  assert(state_ != ConnectionState::kDisconnected);
  assert(!reconnect_timer_.isActive());

  LOG(INFO) << "Disconnected from server";
  state_ = ConnectionState::kDisconnected;
  reconnect_timer_.start();
  awaiting_responces_.clear();
}

void ServerImpl::HandleReceieved(uint64_t id, ServerMessageType type,
                                 const QByteArray& data) {
  switch (type) {
    case ServerMessageType::kHostConnected:
      ProcessHostConnected(data);
      return;
    case ServerMessageType::kHostDisconnected:
      ProcessHostDisconnected(data);
      return;
    case ServerMessageType::kTextUpdate:
      ProcessHostTextUpdate(data);
      return;
    case ServerMessageType::kHostSynced:
      ProcessHostSynced(data);
      return;

    case ServerMessageType::kServerResponse: {
      const auto it = awaiting_responces_.find(id);
      if (it != awaiting_responces_.end()) {
        it->second.callback(data);
        awaiting_responces_.erase(it);
      } else {
        LOG(ERROR) << "Received server responce for unknown message id: " << id;
      }
      return;
    }
  }
  LOG(ERROR) << "Server has sent the message with unexpected message type: "
             << static_cast<int>(type);
}

ServerImpl::ConnectionState ServerImpl::GetStateForTesting() const {
  return state_;
}

void ServerImpl::InitAndRunTimeoutTimer(QTimer& timer) {
  connect(&timer, &QTimer::timeout, [this]() { connection_->Disconnect(); });
  timer.setSingleShot(true);
  timer.start(kNetworkTimeoutInterval);
}

}  // namespace reclip
