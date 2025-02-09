#include "core/clipboard_model.h"

#include "base/constants.h"
#include "base/log.h"

constexpr size_t kThisHostModelIndex = 0;
constexpr size_t kClipboardSizeMax = 10;

namespace reclip {

ClipboardModel::ClipboardModel() {
  // We want at least one element for this host clipboard.
  clipboard_content_.resize(1);
}

void ClipboardModel::OnTextUpdated(const std::string& str) {
  DLOG(INFO) << "[EVENT] ClipboardModel's clipboard text changed: \""
             << str.substr(0, kMaxContentLogSize)
             << (str.size() > kMaxContentLogSize ? "...\"" : "\"");
  clipboard_content_[kThisHostModelIndex].push_front(str);

  for (auto* observer : observers_) {
    observer->OnItemPushed(kThisHostModelIndex);
  }

  while (clipboard_content_.size() > kClipboardSizeMax) {
    clipboard_content_.pop_back();
    for (auto* observer : observers_) {
      observer->OnItemPoped(kThisHostModelIndex);
    }
  }
}

size_t ClipboardModel::GetHostsCount() const {
  return clipboard_content_.size();
}

const ClipboardModel::HostClipboardQueue& ClipboardModel::GetContent(
    size_t host_index) const {
  assert(host_index < clipboard_content_.size());
  return clipboard_content_[host_index];
}

}  // namespace reclip
