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

// This class abstracts server. It will automatically keep consistant the inner
// data representation with server (by tracking unsent data and checking
// received data). Server also manages connection with remote server.
class ServerImpl : public QObject,
                   public ClipboardObserver,
                   public ServerConnection::Delegate {
  Q_OBJECT
 public:
  enum class ConnectionState { kDisconnected, kConnecting, kSyncing, kConnected };
  class TestHelper {
   public:
    TestHelper();
    virtual ~TestHelper();
    virtual std::unique_ptr<ServerConnection> CreateConnection() = 0;
  };

  explicit ServerImpl(ServerDelegate& delegate);
  ~ServerImpl() override;

  // ClipboardObserver overrides
  void OnTextUpdated(const std::string& value) override;

  // ServerConnection::Delegate overrides
  void HandleConnect(bool is_connected) override;
  void HandleDisconnected() override;
  void HandleFullSync(ClipboardData this_host_data, std::vector<HostData> data,
                      bool is_success) override;
  void HandleTextSent(bool is_success) override;
  void HandleNewHost(const HostId& id, const std::string& name) override;
  void HandleNewText(const HostId& id, const std::string& text) override;

  ConnectionState GetStateForTesting() const;

 private:
  struct SendTask {
    enum class Type { kText };
    Type type;
    std::string data;
  };

  void ConnectImpl();
  void TryProcessTask();

  ServerDelegate* delegate_;
  QTimer reconnect_timer_;

  std::unique_ptr<ServerConnection> connection_;
  std::queue<std::unique_ptr<SendTask>> tasks_queue_;
  std::unique_ptr<SendTask> current_task_;

  ConnectionState state_ = ConnectionState::kDisconnected;
};

}  // namespace reclip
