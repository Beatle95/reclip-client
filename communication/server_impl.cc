#include "communication/server_impl.h"

#include <cassert>
#include <chrono>

#include "base/log.h"
#include "communication/connection_impl.h"
#include "communication/messages.h"

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

ServerImpl::ServerImpl(Client& client)
    : client_(&client),
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
  auto data = SerializeSyncRequest();
  if (!connection_->SendMessage(data.message)) {
    return;
  }

  assert(!awaiting_responces_.contains(data.id));
  auto& response = awaiting_responces_[data.id];
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

    client_->OnFullSync(std::move(std::get<0>(response.value())),
                        std::move(std::get<1>(response.value())));
  };
  InitAndRunTimeoutTimer(response.timeout_timer);
}

void ServerImpl::RequestHostSync(const HostId& id, HostSyncCallback callback) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  auto data = SerializeHostSencRequest(id);
  if (!connection_->SendMessage(data.message)) {
    return;
  }

  assert(!awaiting_responces_.contains(data.id));
  auto& response = awaiting_responces_[data.id];
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
  connection_->SendMessage(SerializeThisHostSync(data));
}

void ServerImpl::AddThisHostText(const std::string& text) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  connection_->SendMessage(SerializeTextUpdate(text));
}

void ServerImpl::ProcessHostConnected(const QByteArray& data) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  if (auto id = ParseHostId(data)) {
    client_->HostConnected(id.value());
  } else {
    LOG(ERROR) << "Unable to parse connected host id";
  }
}

void ServerImpl::ProcessHostDisconnected(const QByteArray& data) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  if (auto id = ParseHostId(data)) {
    client_->HostDisconnected(id.value());
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
  client_->HostTextAdded(std::get<0>(text_data.value()),
                         std::get<1>(text_data.value()));
}

void ServerImpl::ProcessHostSynced(const QByteArray& data) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  if (auto host_data = ParseHostData(data)) {
    client_->HostSynced(std::move(host_data.value()));
  } else {
    LOG(ERROR) << "Error parsing host data in host sync notification";
  }
}

void ServerImpl::HandleDisconnected() {
  assert(state_ != ConnectionState::kDisconnected);
  assert(!reconnect_timer_.isActive());

  LOG(INFO) << "Disconnected from server";
  state_ = ConnectionState::kDisconnected;
  reconnect_timer_.start();
  awaiting_responces_.clear();
}

void ServerImpl::HandleReceieved(const QByteArray& data) {
  const auto header_opt = ParseNetworkHeader(data);
  if (!header_opt.has_value()) {
    LOG(WARNING) << "Got server message with bad header";
    return;
  }

  const auto& header = *header_opt;
  switch (static_cast<MessageType>(header.type)) {
    case MessageType::kServerHostConnected:
      ProcessHostConnected(data.sliced(sizeof(NetworkHeader)));
      return;
    case MessageType::kServerHostDisconnected:
      ProcessHostDisconnected(data.sliced(sizeof(NetworkHeader)));
      return;
    case MessageType::kServerTextUpdate:
      ProcessHostTextUpdate(data.sliced(sizeof(NetworkHeader)));
      return;
    case MessageType::kServerHostSynced:
      ProcessHostSynced(data.sliced(sizeof(NetworkHeader)));
      return;

    case MessageType::kServerResponseToClient: {
      const auto it = awaiting_responces_.find(header.id);
      if (it != awaiting_responces_.end()) {
        it->second.callback(data.sliced(sizeof(NetworkHeader)));
        awaiting_responces_.erase(it);
      } else {
        LOG(ERROR) << "Received server responce for unknown message id: "
                   << header.id;
      }
      return;
    }

    case MessageType::kClientsResponseToServer:
      [[fallthrough]];
    case MessageType::kFullSyncRequest:
      [[fallthrough]];
    case MessageType::kHostSyncRequest:
      [[fallthrough]];
    case MessageType::kHostTextUpdate:
      [[fallthrough]];
    case MessageType::kHostSyncData:
      break;
  }
  LOG(ERROR) << "Server has sent the message with unexpected message type: "
             << header.type;
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
