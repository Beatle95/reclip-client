#pragma once
#include <QObject>
#include <QTimer>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "communication/client_server_types.h"
#include "communication/connection.h"
#include "core/host_types.h"

namespace reclip {

// This class abstracts server. It manages connection with remote server. It
// will automatically keep consistant the inner data representation with server.
class ServerImpl : public QObject, public Server, public Connection::Delegate {
  Q_OBJECT
 public:
  enum class ConnectionState {
    kDisconnected,
    kConnecting,
    kSyncing,
    kConnected
  };

  class TestHelper {
   public:
    TestHelper();
    virtual ~TestHelper();
    virtual std::unique_ptr<Connection> CreateConnection() = 0;
  };

  explicit ServerImpl(Client& client);
  ~ServerImpl() override;

  // Server overrides:
  void RequestHostSync(const HostId& id, HostSyncCallback callback) override;
  void SyncThisHost(const HostData& data) override;
  void AddThisHostText(const std::string& text) override;

  // Connection::Delegate overrides:
  void HandleConnected(bool is_connected) override;
  void HandleDisconnected() override;
  void HandleReceieved(uint64_t id, ServerMessageType type,
                       const QByteArray& data) override;

  ConnectionState GetStateForTesting() const;

 private:
  using ResponceCallback = std::function<void(const QByteArray&)>;
  struct AwaitingResponce {
    ResponceCallback callback;
    QTimer timeout_timer;
  };

  void InitAndRunTimeoutTimer(QTimer& timer);

  void ConnectImpl();
  void RequestFullSync();

  void ProcessHostConnected(const QByteArray& data);
  void ProcessHostDisconnected(const QByteArray& data);
  void ProcessHostTextUpdate(const QByteArray& data);
  void ProcessHostSynced(const QByteArray& data);
  uint64_t GenerateId();

  Client* client_;
  std::unique_ptr<Connection> connection_;
  std::unordered_map<uint64_t, AwaitingResponce> awaiting_responces_;
  uint64_t id_counter_ = 0;

  QTimer reconnect_timer_;
  QTimer connection_timer_;
  ConnectionState state_ = ConnectionState::kDisconnected;
};

}  // namespace reclip
