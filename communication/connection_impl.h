#pragma once
#include <QObject>
#include <QTcpSocket>

#include "communication/connection.h"
#include "communication/message_reassembler.h"
#include "communication/message_types.h"

namespace reclip {

// This class abstracts away connection and communication with server.
class ServerConnectionImpl : public QObject, public Connection {
  Q_OBJECT
 public:
  explicit ServerConnectionImpl(Connection::Delegate& delegate,
                                const QString& ip, quint16 port);
  ~ServerConnectionImpl() override = default;

  // Connection overrides
  void Connect() override;
  void Disconnect() override;
  bool SendMessage(uint64_t id, ClientMessageType type,
                   const QByteArray& data) override;

 private slots:
  void OnConnected();
  void OnDisconnected();
  void OnReadyRead();
  void OnError(QAbstractSocket::SocketError err);

 private:
  QString ip_;
  quint16 port_;

  Connection::Delegate* delegate_;
  QTcpSocket socket_;
  MessageReassembler reassembler_;
  // This field will help us to distinguish between errors upon connection and
  // errors upon communication process.
  bool connected_ = false;
};

}  // namespace reclip
