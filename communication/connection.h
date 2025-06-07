#pragma once
#include <QByteArray>

#include "communication/message_types.h"

namespace reclip {

// TODO: Rename to server connection.
class Connection {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void HandleConnected(bool is_connected) = 0;
    virtual void HandleDisconnected() = 0;
    virtual void HandleReceieved(uint64_t id, ServerMessageType type,
                                 const QByteArray& data) = 0;
  };

  virtual ~Connection() = default;

  virtual void Connect() = 0;
  virtual void Disconnect() = 0;
  virtual bool SendMessage(uint64_t id, ClientMessageType type,
                           const QByteArray& data) = 0;
};

}  // namespace reclip
