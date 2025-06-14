#include "app_main/application_main.h"
#include "moc_application_main.cpp"

#include <QTimer>
#include <cassert>

#include "base/log.h"
#include "core/clipboard.h"
#include "ui/clipboard_controller.h"

import base.preferences;
import core.clipboard_model;
import communication.communication_manager;

const auto kShowUiOnStartupArg = QStringLiteral("--show_ui_on_startup");

namespace reclip {

ApplicationMain::ApplicationMain(int argc, char** argv)
    : QApplication(argc, argv) {
  DLOG(INFO) << "Application::Ctor";
  PrefInit();
  model_ = std::make_unique<ClipboardModel>();

  clipboard_ = Clipboard::Create();
  clipboard_->Start();
  model_observation_.Reset(*model_, *clipboard_);

  communication_host_ = std::make_unique<CommunicationManager>(*model_);
  controller_ =
      std::make_unique<ClipboardController>(model_.get(), clipboard_.get());
  controller_observation_.Reset(*controller_, *model_);

  if (arguments().contains(kShowUiOnStartupArg)) {
    QTimer::singleShot(0, [this]() { controller_->ShowUi(); });
  }
}

ApplicationMain::~ApplicationMain() {
  DLOG(INFO) << "Application::Dtor";
  clipboard_->Stop();
}

void ApplicationMain::PrefInit() {
  Preferences::GetInstance().SetHostSecret("this_host_secret");
}

}  // namespace reclip
