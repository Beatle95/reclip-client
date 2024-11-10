#pragma once
#include <QApplication>
#include <memory>

namespace reclip {

class ContentWindow;
class Clipboard;
class ClipboardModel;
class Server;

class ApplicationMain : public QApplication {
  Q_OBJECT
 public:
  ApplicationMain(int argc, char** argv);
  ~ApplicationMain();

 private:
  void ShowUi();
  void HideUi();

  std::unique_ptr<Clipboard> clipboard_listener_;
  std::unique_ptr<ClipboardModel> client_;
  std::unique_ptr<Server> server_;
  std::unique_ptr<ContentWindow> view_;
};

}  // namespace reclip
