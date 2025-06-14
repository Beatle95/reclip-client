#include "communication/server_impl.h"
#include "moc_server_impl.cpp"

#include <cassert>
#include <chrono>
#include <cstdint>

#include "base/log.h"
#include "communication/connection.h"
#include "communication/connection_impl.h"

import base.preferences;
import communication.message_types;
import communication.serialization;

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace reclip {

namespace {

ServerImpl::TestHelper* g_test_helper = nullptr;

}  // namespace

ServerImpl::TestHelper::TestHelper() {
  assert(g_test_helper == nullptr);
  g_test_helper = this;
}

ServerImpl::TestHelper::~TestHelper() {
  assert(g_test_helper == this);
  g_test_helper = nullptr;
}

ServerImpl::ServerImpl(ServerDelegate& delegate) : delegate_(&delegate) {
  if (g_test_helper) {
    connection_ = g_test_helper->CreateConnection();
  } else {
    const auto prefs = Preferences::GetInstance();
    connection_ = std::make_unique<ServerConnectionImpl>(
        *this, QString::fromStdString(prefs.GetServerIp()),
        prefs.GetServerPort());
  }
  assert(connection_);

  reconnect_timer_.setInterval(0s);
  reconnect_timer_.setSingleShot(true);
  connect(&reconnect_timer_, &QTimer::timeout, this, &ServerImpl::ConnectImpl);
  // It is better if Connect will be handled in one place, so I don't call it in
  // constructor.
  reconnect_timer_.start();
}

// TODO: Before closing give to connection a few milliseconds to close.
ServerImpl::~ServerImpl() = default;

void ServerImpl::ConnectImpl() {
  LOG(INFO) << "Connecting to server\n";
  reconnect_timer_.setInterval(g_test_helper ? 0s : 3s);
  state_ = ConnectionState::kConnecting;
  connection_->Connect();
  InitAndRunTimeoutTimer(connection_timer_, kNetworkTimeoutInterval);
}

void ServerImpl::HandleConnected(bool is_connected) {
  assert(state_ == ConnectionState::kConnecting);
  if (is_connected) {
    LOG(INFO) << "Connected to server";
    SendIntroduction();
  } else {
    LOG(INFO) << "Connection is not successful";
    Reset();
  }
}

void ServerImpl::SendIntroduction() {
  assert(state_ == ConnectionState::kConnecting);
  state_ = ConnectionState::kIntroducing;
  const auto msg_id = GenerateId();
  const auto serialized_data =
      SerializeIntroduction(Preferences::GetInstance().GetHostSecret());
  if (!connection_->SendMessage(msg_id, ClientMessageType::kIntroduction,
                                serialized_data)) {
    LOG(ERROR) << "Sending introduction error";
    connection_->Disconnect();
    return;
  }
}

void ServerImpl::ProcessIntroduction(const QByteArray& data) {
  assert(state_ == ConnectionState::kIntroducing);
  const IntroductionResponse response = ParseIntroductionResponse(data);
  if (response.success) {
    LOG(INFO) << "Server introduction success.";
    RequestFullSync();
  } else {
    LOG(ERROR) << "Server introduction failure: '" << response.error << "'";
    connection_->Disconnect();
  }
}

void ServerImpl::RequestFullSync() {
  assert(state_ == ConnectionState::kIntroducing);
  state_ = ConnectionState::kSyncing;
  connection_timer_.stop();

  const auto msg_id = GenerateId();
  if (!connection_->SendMessage(msg_id, ClientMessageType::kFullSyncRequest,
                                {})) {
    LOG(ERROR) << "Sending sync request error";
    connection_->Disconnect();
    return;
  }

  InitResponseAwaiting(msg_id, [this](const QByteArray& data) {
    assert(state_ == ConnectionState::kSyncing);
    auto response = ParseSyncResponse(data);
    if (response.has_value()) {
      LOG(INFO) << "Server sync success";
      state_ = ConnectionState::kConnected;
      delegate_->OnFullSync(std::move(response->this_host_data),
                            std::move(response->hosts_data));
    } else {
      LOG(ERROR) << "Got wrong sync response from server";
      connection_->Disconnect();
    }
  });
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

  InitResponseAwaiting(
      msg_id, [this, callback = std::move(callback)](const QByteArray& data) {
        if (state_ == ConnectionState::kConnected) {
          callback(ParseHostData(data));
        }
      });
}

void ServerImpl::SyncThisHost(const HostData& data) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  connection_->SendMessage(GenerateId(), ClientMessageType::kSyncThisHost,
                           SerializeHostSync(data));
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

void ServerImpl::HandleDisconnected() {
  assert(state_ != ConnectionState::kDisconnected);
  assert(!reconnect_timer_.isActive());
  LOG(INFO) << "Disconnected from server";
  Reset();
}

void ServerImpl::HandleReceieved(uint64_t id, ServerMessageType type,
                                 const QByteArray& data) {
  switch (type) {
    case ServerMessageType::kIntroduction:
      ProcessIntroduction(data);
      return;
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

uint64_t ServerImpl::GenerateId() { return id_counter_++; }

void ServerImpl::InitResponseAwaiting(uint64_t id, ResponceCallback callback,
                                      std::chrono::seconds timeout) {
  assert(!awaiting_responces_.contains(id));
  auto& response = awaiting_responces_[id];
  response.callback = std::move(callback);
  InitAndRunTimeoutTimer(response.timeout_timer, timeout);
}

void ServerImpl::InitAndRunTimeoutTimer(QTimer& timer,
                                        std::chrono::seconds timeout) {
  connect(&timer, &QTimer::timeout, [this]() { connection_->Disconnect(); });
  timer.setSingleShot(true);
  timer.start(timeout);
}

void ServerImpl::Reset() {
  state_ = ConnectionState::kDisconnected;
  awaiting_responces_.clear();
  connection_timer_.stop();
  assert(!reconnect_timer_.isActive());
  reconnect_timer_.start();
}

}  // namespace reclip
