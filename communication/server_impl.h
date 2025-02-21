#pragma once
#include <QObject>
#include <QTimer>
#include <memory>
#include <queue>
#include <string>

#include "base/host_types.h"
#include "communication/server_connection.h"
#include "core/clipboard_observer.h"
#include "core/server.h"

namespace reclip {

class ServerImpl : public QObject,
                   public Server,
                   public ClipboardObserver,
                   public ServerConnection::Delegate {
  Q_OBJECT
 public:
  explicit ServerImpl(Server::Delegate& delegate);
  ~ServerImpl() override;

  // ClipboardObserver overrides
  void OnTextUpdated(const std::string& value) override;

  // ServerConnection::Delegate overrides
  void HandleConnect(bool is_connected) override;
  void HandleDisconnected() override;
  void HandleFullSync() override;
  void HandleTextSent(bool is_success) override;
  void HandleHostData(const HostId& id, const std::string& name) override;
  void HandleNewText(const HostId& id, const std::string& text) override;

 private:
  enum class ConnectionState { kDisconnected, kConnecting, kConnected };
  class ServerTask {
   public:
    virtual ~ServerTask() = default;
    virtual void Process(ServerConnection& connection) = 0;
  };

  void TryProcessTask();

  Server::Delegate* delegate_;
  std::unique_ptr<ServerConnection> connection_;
  QTimer reconnect_timer_;
  QTimer retry_process_task_timer_;

  std::queue<std::unique_ptr<ServerTask>> tasks_queue_;
  std::unique_ptr<ServerTask> current_task_;

  ConnectionState state_ = ConnectionState::kDisconnected;
};

}  // namespace reclip
