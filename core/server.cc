#include "core/server.h"

#include <cassert>
#include <chrono>

#include "base/log.h"
#include "base/preferences.h"

using namespace std::chrono_literals;

namespace reclip {

Server::Server(Delegate& delegate)
    : delegate_(&delegate), connection_(ServerConnection::Create(*this)) {
  assert(connection_);
  connection_->Connect(Preferences::GetInstance().GetHostSecret());
  state_ = ConnectionState::kConnecting;

  reconnect_timer_.setInterval(3s);
  reconnect_timer_.setSingleShot(true);
  connect(&reconnect_timer_, &QTimer::timeout, [this]() {
    connection_->Connect(Preferences::GetInstance().GetHostSecret());
  });
  reconnect_timer_.start();

  retry_process_task_timer_.setInterval(1s);
  retry_process_task_timer_.setSingleShot(true);
}

void Server::RequestFullSync(SyncCallback callback) {
  // TODO:
  (void)callback;
}

void Server::OnTextUpdated(const std::string& value) {
  // TODO:
  (void)value;
  (void)delegate_;
}

void Server::HandleConnect(bool is_connected) {
  if (is_connected) {
    state_ = ConnectionState::kConnected;
    // TODO: request sync
    TryProcessTask();
  } else {
    state_ = ConnectionState::kDisconnected;
    assert(!reconnect_timer_.isActive());
    reconnect_timer_.start();
  }
}

void Server::HandleDisconnected() {
  state_ = ConnectionState::kDisconnected;
  assert(!reconnect_timer_.isActive());
  reconnect_timer_.start();
}

void Server::HandleFullSync() {
  // TODO:
}

void Server::HandleTextSent(bool is_success) {
  if (is_success) {
    current_task_.reset();
    TryProcessTask();
  } else {
    assert(!retry_process_task_timer_.isActive());
    retry_process_task_timer_.start();
  }
}

void Server::HandleHostData(const HostId& id, const std::string& name) {
  assert(delegate_);
  delegate_->ProcessNewHost(id, name);
}

void Server::HandleNewText(const HostId& id, const std::string& text) {
  assert(delegate_);
  if (!delegate_->ProcessNewText(id, text)) {
    LOG(ERROR) << "Server has sent text data for unknown host, resync will be "
                  "requested: "
               << id;
    // TODO: request sync again.
  }
}

void Server::TryProcessTask() {
  if (current_task_) {
    current_task_->Process(*connection_);
  } else if (!tasks_queue_.empty()) {
    current_task_ = std::move(tasks_queue_.front());
    tasks_queue_.pop();
    current_task_->Process(*connection_);
  }
}

}  // namespace reclip
