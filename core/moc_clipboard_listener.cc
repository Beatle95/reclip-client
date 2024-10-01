#include "core/moc_clipboard_listener.h"

#include <algorithm>
#include <cassert>

namespace reclip {
void MocClipboardListener::EmulateTextCopy(std::string_view value) {
    for (auto& observer : observers_) {
        observer->OnTextUpdated(value);
    }
}

void MocClipboardListener::AddObserver(
    const std::shared_ptr<ClipboardObserver>& observer) {
    assert(observer);
    observers_.push_back(observer);
}

void MocClipboardListener::RemoveObserver(
    const std::shared_ptr<ClipboardObserver>& observer) {
    observers_.erase(std::find(observers_.begin(), observers_.end(), observer));
}

}  // namespace reclip
