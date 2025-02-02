#include "core/application_main.h"

#include <QTimer>
#include <cassert>

#include "base/log.h"
#include "core/clipboard.h"
#include "core/clipboard_model.h"
#include "core/server.h"
#include "ui/clipboard_controller.h"

const auto kShowUiOnStartupArg = QStringLiteral("--show_ui_on_startup");

namespace reclip {

ApplicationMain::ApplicationMain(int argc, char** argv)
    : QApplication(argc, argv) {
  DLOG(INFO) << "Application::Ctor";
  model_ = std::make_unique<ClipboardModel>();

  clipboard_ = Clipboard::Create();
  clipboard_->Start();
  model_observation_.Reset(*model_, *clipboard_);

  server_ = std::make_unique<Server>();
  server_observation_.Reset(*server_, *clipboard_);

  controller_ = std::make_unique<ClipboardController>(model_.get(), clipboard_.get());
  controller_observation_.Reset(*controller_, *model_);

  if (arguments().contains(kShowUiOnStartupArg)) {
    QTimer::singleShot(0, [this]() {
      controller_->ShowUi();
    });
  }
}

ApplicationMain::~ApplicationMain() {
  DLOG(INFO) << "Application::Dtor";
  clipboard_->Stop();
}

}  // namespace reclip
