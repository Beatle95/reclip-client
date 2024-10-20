#pragma once
#include <QGuiApplication>
#include <memory>

namespace reclip {

class ClipboardListener;
class ClipboardModel;
class Server;

class ApplicationMain : public QGuiApplication {
 public:
  ApplicationMain(int argc, char** argv);
  ~ApplicationMain();

 private:
  std::unique_ptr<ClipboardListener> clipboard_listener_;
  std::unique_ptr<ClipboardModel> client_;
  std::unique_ptr<Server> server_;
};

}  // namespace reclip
