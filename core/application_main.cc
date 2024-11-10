#include "core/application_main.h"

#include <QTimer>
#include <cassert>

#include "base/log.h"
#include "core/clipboard.h"
#include "core/clipboard_model.h"
#include "core/server.h"
#include "ui/content_window.h"

const auto kShowUiOnStartupArg = QStringLiteral("--show_ui_on_startup");

namespace reclip {

ApplicationMain::ApplicationMain(int argc, char** argv)
    : QApplication(argc, argv) {
  DLOG(INFO) << "Application::Ctor";
  client_ = std::make_unique<ClipboardModel>();

  clipboard_listener_ = Clipboard::Create();
  clipboard_listener_->Start();
  clipboard_listener_->AddObserver(client_.get());

  server_ = std::make_unique<Server>();
  clipboard_listener_->AddObserver(server_.get());

  if (arguments().contains(kShowUiOnStartupArg)) {
    QTimer::singleShot(0, this, &ApplicationMain::ShowUi);
  }
}

ApplicationMain::~ApplicationMain() {
  DLOG(INFO) << "Application::Dtor";
  clipboard_listener_->Stop();
}

void ApplicationMain::ShowUi() {
  DLOG(INFO) << "Showing ui...";
  view_ = std::make_unique<ContentWindow>();
}

void ApplicationMain::HideUi() {
  DLOG(INFO) << "Hiding ui...";
  view_.reset();
}

}  // namespace reclip
