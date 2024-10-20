#include "core/application_main.h"

#include <cassert>

#include "base/log.h"
#include "core/clipboard_model.h"
#include "core/clipboard_listener.h"
#include "core/server.h"

namespace reclip {

ApplicationMain::ApplicationMain(int argc, char** argv)
    : QGuiApplication(argc, argv) {
  DLOG(INFO) << "Application::Ctor";
  client_ = std::make_unique<ClipboardModel>();

  clipboard_listener_ = ClipboardListener::Create();
  clipboard_listener_->Start();
  clipboard_listener_->AddObserver(*client_);

  server_ = std::make_unique<Server>();
  clipboard_listener_->AddObserver(*server_);
}

ApplicationMain::~ApplicationMain() {
  DLOG(INFO) << "Application::Dtor";
  clipboard_listener_->Stop();
}

}  // namespace reclip
