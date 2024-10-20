#include "core/clipboard_model.h"

#include "base/log.h"

constexpr size_t kLogLen = 10;

namespace reclip {

void ClipboardModel::OnTextUpdated(const std::string& str) {
  DLOG(INFO) << "[EVENT] ClipboardModel's clipboard text changed: \""
             << str.substr(0, kLogLen)
             << (str.size() > kLogLen ? "...\"" : "\"");
}

}  // namespace reclip
