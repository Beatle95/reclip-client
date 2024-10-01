#include "core/clipboard_listener.h"
#include "core/moc_clipboard_listener.h"

namespace reclip {

std::unique_ptr<ClipboardListener> ClipboardListener::Create() {
    return std::make_unique<MocClipboardListener>();
}

}  // namespace reclip
