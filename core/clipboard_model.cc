#include "core/clipboard_model.h"

#include "base/log.h"

constexpr size_t kLogLen = 10;
constexpr size_t kClipboardSizeMax = 2;

namespace reclip {

void ClipboardModel::OnTextUpdated(const std::string& str) {
  DLOG(INFO) << "[EVENT] ClipboardModel's clipboard text changed: \""
             << str.substr(0, kLogLen)
             << (str.size() > kLogLen ? "...\"" : "\"");
  host_clipboard_content_.push_front(str);

  for (auto& observer : observers_) {
    observer->OnHostItemAdded();
  }

  while (host_clipboard_content_.size() > kClipboardSizeMax) {
    host_clipboard_content_.pop_back();
    for (auto& observer : observers_) {
      observer->OnLastHostItemRemoved();
    }
  }
}

const std::deque<std::string>& ClipboardModel::GetHostContent() const {
  return host_clipboard_content_;
}

}  // namespace reclip
