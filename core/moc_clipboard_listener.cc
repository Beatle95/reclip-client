#include "core/moc_clipboard_listener.h"

#include <algorithm>
#include <cassert>

namespace reclip {
void MocClipboardListener::EmulateTextCopy(const std::string& value) {
  for (auto& observer : observers_) {
    observer->OnTextUpdated(value);
  }
}

void MocClipboardListener::AddObserver(ClipboardObserver& observer) {
  if (std::find(observers_.begin(), observers_.end(), &observer) ==
      observers_.end()) {
    observers_.push_back(&observer);
  }
}

void MocClipboardListener::RemoveObserver(ClipboardObserver& observer) {
  observers_.erase(std::find(observers_.begin(), observers_.end(), &observer));
}

}  // namespace reclip
