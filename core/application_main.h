#pragma once
#include <QApplication>
#include <memory>

#include "base/scoped_observation.h"

namespace reclip {

class Clipboard;
class ClipboardModel;
class ClipboardController;
class Server;

class ApplicationMain : public QApplication {
  Q_OBJECT
 public:
  ApplicationMain(int argc, char** argv);
  ~ApplicationMain();

 private:
  void ShowUi();
  void HideUi();

  std::unique_ptr<Clipboard> clipboard_;
  std::unique_ptr<ClipboardModel> model_;
  std::unique_ptr<Server> server_;
  std::unique_ptr<ClipboardController> controller_;

  ScopedObservation<ClipboardModel, Clipboard> model_observation_;
  ScopedObservation<Server, Clipboard> server_observation_;
};

}  // namespace reclip
