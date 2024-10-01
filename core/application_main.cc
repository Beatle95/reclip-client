#include "core/application_main.h"

#include <cassert>

#include "core/clipboard_listener.h"

namespace reclip {

ApplicationMain::ApplicationMain(int argc, char** argv)
    : QCoreApplication(argc, argv) {
    clipboard_listener_ = ClipboardListener::Create();
    clipboard_listener_->Start();
}

ApplicationMain::~ApplicationMain() {
    clipboard_listener_->Stop();
}

}  // namespace reclip
