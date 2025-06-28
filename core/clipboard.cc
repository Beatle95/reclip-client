#include "core/clipboard.h"

#ifdef __APPLE__
#include "core/clipboard_mac.h"
#else
#include "core/clipboard_qt.h"
#endif

namespace reclip {

std::unique_ptr<Clipboard> Clipboard::Create() {
#ifdef __APPLE__
  return std::make_unique<ClipboardMac>();
#else
  return std::make_unique<ClipboardListenerQt>();
#endif
}

}  // namespace reclip
