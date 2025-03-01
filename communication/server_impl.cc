#include "communication/server_impl.h"

#include <cassert>
#include <chrono>

#include "base/log.h"
#include "base/preferences.h"
#include "communication/server_connection_impl.h"

using namespace std::chrono_literals;

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
  connection_->Connect(Preferences::GetInstance().GetHostSecret());
}

void ServerImpl::OnTextUpdated(const std::string& value) {
  tasks_queue_.push(std::make_unique<SendTask>(SendTask::Type::kText, value));
  if (state_ == ConnectionState::kConnected) {
    TryProcessTask();
  }
}

void ServerImpl::HandleConnect(bool is_connected) {
  assert(state_ == ConnectionState::kConnecting);
  if (is_connected) {
    LOG(INFO) << "Connection successful";
    state_ = ConnectionState::kSyncing;
    connection_->RequestFullSync();
  } else {
    LOG(INFO) << "Connection is not successful";
    state_ = ConnectionState::kDisconnected;
    assert(!reconnect_timer_.isActive());
    reconnect_timer_.start();
  }
}

void ServerImpl::HandleDisconnected() {
  assert(state_ != ConnectionState::kDisconnected);
  assert(!reconnect_timer_.isActive());

  LOG(INFO) << "Disconnected from server";
  state_ = ConnectionState::kDisconnected;
  reconnect_timer_.start();
}

void ServerImpl::HandleFullSync(ClipboardData this_host_data,
                                std::vector<HostData> data, bool is_success) {
  assert(state_ == ConnectionState::kSyncing);
  if (!is_success) {
    connection_->Disconnect();
    return;
  }
  state_ = ConnectionState::kConnected;
  delegate_->ProcessSyncData(std::move(this_host_data), std::move(data));
}

void ServerImpl::HandleTextSent(bool is_success) {
  assert(state_ == ConnectionState::kConnected);
  if (is_success) {
    current_task_.reset();
    TryProcessTask();
  } else {
    connection_->Disconnect();
  }
}

void ServerImpl::HandleNewHost(const HostId& id, const std::string& name) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  delegate_->ProcessNewHost(id, name);
}

void ServerImpl::HandleNewText(const HostId& id, const std::string& text) {
  if (state_ != ConnectionState::kConnected) {
    return;
  }
  if (!delegate_->ProcessNewText(id, text)) {
    LOG(ERROR)
        << "ServerImpl has sent text data for unknown host, resync will be "
           "requested: "
        << id;
    connection_->Disconnect();
  }
}

ServerImpl::ConnectionState ServerImpl::GetStateForTesting() const {
  return state_;
}

void ServerImpl::TryProcessTask() {
  assert(state_ == ConnectionState::kConnected);
  if (current_task_) {
    connection_->SendText(current_task_->data);
  } else if (!tasks_queue_.empty()) {
    current_task_ = std::move(tasks_queue_.front());
    tasks_queue_.pop();
    connection_->SendText(current_task_->data);
  }
}

}  // namespace reclip
