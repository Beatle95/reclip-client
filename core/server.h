#pragma once
#include <QObject>
#include <QTimer>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "base/host_types.h"
#include "communication/server_connection.h"
#include "core/clipboard_observer.h"

namespace reclip {

class Server : public QObject,
               public ClipboardObserver,
               public ServerConnection::Delegate {
  Q_OBJECT
 public:
  using SyncCallback = std::function<void(std::vector<HostData> data)>;
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void ProcessNewHost(const HostId& id, const std::string& name) = 0;
    virtual bool ProcessNewText(const HostId& id, const std::string& text) = 0;
  };

  explicit Server(Delegate& delegate);

  void RequestFullSync(SyncCallback callback);

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

  Delegate* delegate_;
  std::unique_ptr<ServerConnection> connection_;
  QTimer reconnect_timer_;
  QTimer retry_process_task_timer_;

  std::queue<std::unique_ptr<ServerTask>> tasks_queue_;
  std::unique_ptr<ServerTask> current_task_;

  ConnectionState state_ = ConnectionState::kDisconnected;
};

}  // namespace reclip
