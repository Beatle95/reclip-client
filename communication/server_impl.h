#pragma once
#include <QObject>
#include <QTimer>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "communication/connection.h"
#include "communication/server.h"

import core.host_types;

namespace reclip {

// This class abstracts server. It manages connection with remote server. It
// will automatically keep consistant the inner data representation with server.
class ServerImpl : public QObject, public Server, public Connection::Delegate {
  Q_OBJECT
 public:
  enum class ConnectionState {
    kDisconnected,
    kConnecting,
    kIntroducing,
    kSyncing,
    kConnected
  };

  class TestHelper {
   public:
    TestHelper();
    virtual ~TestHelper();
    virtual std::unique_ptr<Connection> CreateConnection() = 0;
  };

  explicit ServerImpl(ServerDelegate& delegate);
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
  static constexpr auto kNetworkTimeoutInterval = std::chrono::seconds(15);

  using ResponceCallback = std::function<void(const QByteArray&)>;
  struct AwaitingResponce {
    ResponceCallback callback;
    QTimer timeout_timer;
  };

  void ConnectImpl();
  void SendIntroduction();
  void RequestFullSync();

  void ProcessIntroduction(const QByteArray& data);
  void ProcessHostConnected(const QByteArray& data);
  void ProcessHostDisconnected(const QByteArray& data);
  void ProcessHostTextUpdate(const QByteArray& data);
  void ProcessHostSynced(const QByteArray& data);

  uint64_t GenerateId();
  void InitResponseAwaiting(
      uint64_t id, ResponceCallback callback,
      std::chrono::seconds timeout = kNetworkTimeoutInterval);
  void InitAndRunTimeoutTimer(QTimer& timer, std::chrono::seconds timeout);
  void Reset();

  ServerDelegate* delegate_;
  std::unique_ptr<Connection> connection_;
  std::unordered_map<uint64_t, AwaitingResponce> awaiting_responces_;
  uint64_t id_counter_ = 0;

  QTimer reconnect_timer_;
  QTimer connection_timer_;
  ConnectionState state_ = ConnectionState::kDisconnected;
};

}  // namespace reclip
