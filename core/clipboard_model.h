#pragma once
#include <vector>

#include "core/clipboard_observer.h"

import core.host_types;
import base.observer_helper;

namespace reclip {

class ClipboardModelObserver : public CheckedObserver {
 public:
  virtual ~ClipboardModelObserver() = default;

  virtual void OnThisTextPushed() = 0;
  virtual void OnThisTextPoped() = 0;

  virtual void OnHostUpdated(size_t host_index) = 0;
  virtual void OnTextPushed(size_t host_index) = 0;
  virtual void OnTextPoped(size_t host_index) = 0;

  virtual void OnThisHostDataReset() = 0;
  virtual void OnHostsDataReset() = 0;
};

class ClipboardModel : public SimpleObservable<ClipboardModelObserver>,
                       public ClipboardObserver {
 public:
  ClipboardModel();
  virtual ~ClipboardModel() = default;

  // ClipboardObserver overrides:
  void OnTextUpdated(const std::string& value) override;

  // Returns true if data was fully adopted (i.e. after this call data inside
  // model is exactly the same as this function's arguments).
  bool AdoptThisHostData(std::string name, ClipboardData data);
  void ResetHostsData(std::vector<HostData> other_hosts_data);
  void SetHostData(HostData data);
  // Returns false, if host with such id was not found.
  bool AddHostText(const HostId& id, const std::string& text);
  bool IsHostExists(const HostId& id) const;

  size_t GetHostsCount() const;
  const HostData& GetHostData(size_t host_index) const;
  const HostData& GetThisHostData() const;

 private:
  void ProcessNewTextImpl(size_t index, const std::string& text);

  HostData this_host_data_;
  std::vector<HostData> hosts_data_;
};

}  // namespace reclip
