#include "core/clipboard_model.h"

#include <algorithm>

#include "base/constants.h"
#include "base/log.h"
#include "base/preferences.h"

constexpr size_t kThisHostModelIndex = 0;

namespace reclip {

ClipboardModel::ClipboardModel() {
  // We want at least one element for this host clipboard.
  hosts_.resize(1);
  InitThisHostData();
}

void ClipboardModel::OnTextUpdated(const std::string& str) {
  DLOG(INFO) << "[EVENT] ClipboardModel's clipboard text changed: \""
             << str.substr(0, kMaxContentLogSize)
             << (str.size() > kMaxContentLogSize ? "...\"" : "\"");
  ProcessNewTextImpl(kThisHostModelIndex, str);
}

void ClipboardModel::ProcessNewHost(const std::string& id, const std::string& name) {
  hosts_.push_back(HostData{id, name, {}});
  for (auto* observer : observers_) {
    observer->OnHostAdded(hosts_.size() - 1);
  }  
}

void ClipboardModel::ProcessNewText(const std::string& id,
                                    const std::string& text) {
  if (id == Preferences::GetInstance().GetHostId()) {
    LOG(ERROR) << "Server tried to push us our text info";
    return;
  }
  DLOG(INFO) << "[EVENT] ClipboardModel received new text from server for host "
                "with id: "
             << id;

  auto it = std::find_if(hosts_.begin(), hosts_.end(),
                         [&](const auto& elem) { return elem.id == id; });

  if (it == hosts_.end()) {
    LOG(ERROR) << "Server has sent text data for unknown host: " << id;
    return;
  }
  ProcessNewTextImpl(std::distance(hosts_.begin(), it), text);
}

void ClipboardModel::ProcessNewTextImpl(size_t index, const std::string& text) {
  assert(index < hosts_.size());
  hosts_[index].text_data.push_front(text);
  for (auto* observer : observers_) {
    observer->OnItemPushed(index);
  }
  while (hosts_[index].text_data.size() > kClipboardSizeMax) {
    hosts_[index].text_data.pop_back();
    for (auto* observer : observers_) {
      observer->OnItemPoped(index);
    }
  }
}

void ClipboardModel::SyncHosts(std::vector<HostData> data) {
  assert(!hosts_.empty());

  const auto& this_id = Preferences::GetInstance().GetHostId();
  auto it = std::find_if(data.begin(), data.end(),
                         [&](const auto& elem) { return elem.id == this_id; });
  if (it == data.end()) {
    hosts_.resize(1);
    hosts_.insert(hosts_.end(), std::make_move_iterator(data.begin()),
                  std::make_move_iterator(data.end()));
  } else {
    if (data.begin() != it) {
      std::swap(*data.begin(), *it);
    }
    if (hosts_[kThisHostModelIndex].text_data.empty()) {
      hosts_ = std::move(data);
    } else {
      auto& internal_text = hosts_[kThisHostModelIndex].text_data;
      auto& remote_text = data.front().text_data;
      for (size_t i = 0;
           i < remote_text.size() && internal_text.size() < kClipboardSizeMax;
           ++i) {
        internal_text.push_back(std::move(remote_text[i]));
      }
    }
    InitThisHostData();
  }

  for (auto* observer : observers_) {
    observer->OnModelReset();
  }
}

void ClipboardModel::InitThisHostData() {
  assert(!hosts_.empty());
  hosts_.front().id = Preferences::GetInstance().GetHostId();
  hosts_.front().visible_name = std::string(kThisHostName);
}

size_t ClipboardModel::GetHostsCount() const { return hosts_.size(); }

const HostData& ClipboardModel::GetHostData(size_t host_index) const {
  assert(host_index < hosts_.size());
  return hosts_[host_index];
}

}  // namespace reclip
