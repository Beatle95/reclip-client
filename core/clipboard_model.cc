#include "core/clipboard_model.h"

#include <algorithm>

#include "base/constants.h"
#include "base/log.h"

namespace reclip {
namespace {
constexpr size_t kThisHostModelIndex = 0;
const HostId kThisHostId = {};
}  // namespace

ClipboardModel::ClipboardModel() {
  // We want at least one element for this host clipboard.
  hosts_.resize(1);
  hosts_.front().id = kThisHostId;
  hosts_.front().visible_name = std::string(kThisHostName);
}

void ClipboardModel::OnTextUpdated(const std::string& str) {
  DLOG(INFO) << "[EVENT] ClipboardModel's clipboard text changed: \""
             << str.substr(0, kMaxContentLogSize)
             << (str.size() > kMaxContentLogSize ? "...\"" : "\"");
  ProcessNewTextImpl(kThisHostModelIndex, str);
}

void ClipboardModel::ProcessSyncData(ClipboardData this_host_data,
                                     std::vector<HostData> data) {
  assert(!hosts_.empty());

  if (hosts_[kThisHostModelIndex].data.text.empty()) {
    hosts_[kThisHostModelIndex].data = std::move(this_host_data);
  }

  hosts_.resize(1);
  hosts_.insert(hosts_.end(), std::make_move_iterator(data.begin()),
                std::make_move_iterator(data.end()));

  for (auto* observer : observers_) {
    observer->OnModelReset();
  }
}

void ClipboardModel::ProcessNewHost(const HostId& id, const std::string& name) {
  hosts_.push_back(HostData{id, name, {}});
  for (auto* observer : observers_) {
    observer->OnHostAdded(hosts_.size() - 1);
  }
}

bool ClipboardModel::ProcessNewText(const HostId& id, const std::string& text) {
  DLOG(INFO) << "[EVENT] ClipboardModel received new text from server for host "
                "with id: "
             << id;

  auto it = std::find_if(hosts_.begin(), hosts_.end(),
                         [&](const auto& elem) { return elem.id == id; });
  if (it == hosts_.end()) {
    return false;
  }
  ProcessNewTextImpl(std::distance(hosts_.begin(), it), text);
  return true;
}

void ClipboardModel::ProcessNewTextImpl(size_t index, const std::string& text) {
  assert(index < hosts_.size());
  hosts_[index].data.text.push_front(text);
  for (auto* observer : observers_) {
    observer->OnItemPushed(index);
  }
  while (hosts_[index].data.text.size() > kClipboardSizeMax) {
    hosts_[index].data.text.pop_back();
    for (auto* observer : observers_) {
      observer->OnItemPoped(index);
    }
  }
}

size_t ClipboardModel::GetHostsCount() const { return hosts_.size(); }

const HostData& ClipboardModel::GetHostData(size_t host_index) const {
  assert(host_index < hosts_.size());
  return hosts_[host_index];
}

}  // namespace reclip
