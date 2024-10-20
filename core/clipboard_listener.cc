#include "core/clipboard_listener.h"

#ifdef __APPLE__
  #include "core/clipboard_listener_mac.h"
#else
  #include "core/clipboard_listener_qt.h"
#endif

namespace reclip {

std::unique_ptr<ClipboardListener> ClipboardListener::Create() {
#ifdef __APPLE__
  return std::make_unique<ClipboardListenerMac>();
#else
  return std::make_unique<ClipboardListenerQt>();
#endif
}

}  // namespace reclip
