#pragma once
#include <QApplication>
#include <memory>

#include "base/scoped_observation.h"

namespace reclip {

class Clipboard;
class ClipboardModel;
class ClipboardController;
class CommunicationHost;

class ApplicationMain : public QApplication {
  Q_OBJECT
 public:
  ApplicationMain(int argc, char** argv);
  ~ApplicationMain();

 private:
  void ShowUi();
  void HideUi();
  void InitId();

  std::unique_ptr<Clipboard> clipboard_;
  std::unique_ptr<ClipboardModel> model_;
  std::unique_ptr<CommunicationHost> communication_host_;
  std::unique_ptr<ClipboardController> controller_;

  ScopedObservation<ClipboardModel, Clipboard> model_observation_;
  ScopedObservation<ClipboardController, ClipboardModel> controller_observation_;
};

}  // namespace reclip
