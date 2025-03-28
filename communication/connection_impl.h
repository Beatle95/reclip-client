#pragma once
#include <QObject>
#include <QTcpSocket>

#include "communication/connection.h"

namespace reclip {

// This class abstracts away connection and communication with server.
class ServerConnectionImpl : public QObject, public Connection {
  Q_OBJECT
 public:
  explicit ServerConnectionImpl(Connection::Delegate& delegate);
  ~ServerConnectionImpl() override = default;

  // Connection overrides
  void Connect() override;
  void Disconnect() override;
  bool SendMessage(const QByteArray& data) override;

 private slots:
  void OnConnected();
  void OnDisconnected();
  void OnError(QAbstractSocket::SocketError err);

 private:
  Connection::Delegate* delegate_;
  QTcpSocket socket_;
  // This field will help us to distinguish between errors upon connection and
  // errors upon communication process.
  bool connected_ = false;
};

}  // namespace reclip
