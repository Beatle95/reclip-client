#pragma once
#include <QByteArray>
#include <memory>

namespace reclip {

class Connection {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void HandleConnected(bool is_connected) = 0;
    virtual void HandleDisconnected() = 0;
    virtual void HandleReceieved(const QByteArray& data) = 0;
  };

  virtual ~Connection() = default;

  virtual void Connect() = 0;
  virtual void Disconnect() = 0;
  virtual bool SendMessage(const QByteArray& data) = 0;
};

}  // namespace reclip
