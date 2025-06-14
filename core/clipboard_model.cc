#include "core/clipboard_model.h"

#include <algorithm>
#include <cassert>

#include "base/log.h"

import base.constants;

namespace reclip {

namespace {
const HostId kThisHostId = {};
}  // namespace

ClipboardModel::ClipboardModel() {
  this_host_data_.id = kThisHostId;
  this_host_data_.name = std::string(kThisHostName);
}

void ClipboardModel::OnTextUpdated(const std::string& str) {
  DLOG(INFO) << "[EVENT] ClipboardModel's clipboard text changed: \""
             << str.substr(0, kMaxContentLogSize)
             << (str.size() > kMaxContentLogSize ? "...\"" : "\"");

  auto& text_data = this_host_data_.data.text;
  text_data.push_front(str);
  for (auto* observer : observers_) {
    observer->OnThisTextPushed();
  }
  while (text_data.size() > kClipboardSizeMax) {
    text_data.pop_back();
    for (auto* observer : observers_) {
      observer->OnThisTextPoped();
    }
  }
}

bool ClipboardModel::AdoptThisHostData(std::string name, ClipboardData data) {
  bool result = true;
  if (name != this_host_data_.name) {
    // Our name is the right one, so we don't want to update it.
    result = false;
  }

  auto& this_data = this_host_data_.data;
  if (this_data != data) {
    DLOG(INFO) << "[EVENT] ClipboardModel has received unequal this host data";
    if (this_data.text.empty() && !data.text.empty()) {
      this_data.text.insert(this_data.text.end(),
                            std::make_move_iterator(data.text.begin()),
                            std::make_move_iterator(data.text.end()));
      for (auto* observer : observers_) {
        observer->OnThisHostDataReset();
      }
    } else {
      // TODO: For now we just prefer local data, later we want to adopt them
      // more precisely.
      result = false;
    }
  } else {
    DLOG(INFO) << "[EVENT] ClipboardModel has received equal this host data";
  }
  return result;
}

void ClipboardModel::ResetHostsData(std::vector<HostData> other_hosts_data) {
  DLOG(INFO) << "[EVENT] ClipboardModel received an update to hosts data";

  hosts_data_.clear();
  hosts_data_.insert(hosts_data_.end(),
                     std::make_move_iterator(other_hosts_data.begin()),
                     std::make_move_iterator(other_hosts_data.end()));

  for (auto* observer : observers_) {
    observer->OnHostsDataReset();
  }
}

void ClipboardModel::SetHostData(HostData data) {
  DLOG(INFO) << "[EVENT] ClipboardModel has updated host data with id: "
             << data.id;

  auto it = std::find_if(hosts_data_.begin(), hosts_data_.end(),
                         [&](const auto& elem) { return data.id == elem.id; });
  if (it == hosts_data_.end()) {
    hosts_data_.push_back(std::move(data));
    it = std::prev(hosts_data_.end());
  }

  assert(it != hosts_data_.end());
  for (auto* observer : observers_) {
    observer->OnHostUpdated(std::distance(hosts_data_.begin(), it));
  }
}

bool ClipboardModel::AddHostText(const HostId& id, const std::string& text) {
  DLOG(INFO) << "[EVENT] ClipboardModel received new text from server for host "
                "with id: "
             << id;

  auto it = std::find_if(hosts_data_.begin(), hosts_data_.end(),
                         [&](const auto& elem) { return elem.id == id; });
  if (it == hosts_data_.end()) {
    return false;
  }
  ProcessNewTextImpl(std::distance(hosts_data_.begin(), it), text);
  return true;
}

bool ClipboardModel::IsHostExists(const HostId& id) const {
  return std::find_if(hosts_data_.begin(), hosts_data_.end(),
                      [&](const auto& elem) { return elem.id == id; }) !=
         hosts_data_.end();
}

void ClipboardModel::ProcessNewTextImpl(size_t index, const std::string& text) {
  assert(index < hosts_data_.size());
  hosts_data_[index].data.text.push_front(text);
  for (auto* observer : observers_) {
    observer->OnTextPushed(index);
  }
  while (hosts_data_[index].data.text.size() > kClipboardSizeMax) {
    hosts_data_[index].data.text.pop_back();
    for (auto* observer : observers_) {
      observer->OnTextPoped(index);
    }
  }
}

size_t ClipboardModel::GetHostsCount() const { return hosts_data_.size(); }

const HostData& ClipboardModel::GetHostData(size_t host_index) const {
  assert(host_index < hosts_data_.size());
  return hosts_data_[host_index];
}

const HostData& ClipboardModel::GetThisHostData() const {
  return this_host_data_;
}

}  // namespace reclip
