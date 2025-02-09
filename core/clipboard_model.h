#pragma once
#include <vector>

#include "base/observers_list.h"
#include "core/client_data.h"
#include "core/clipboard_observer.h"

namespace reclip {

class ClipboardModelObserver : public CheckedObserver {
 public:
  virtual ~ClipboardModelObserver() = default;
  virtual void OnItemPushed(size_t host_index) = 0;
  virtual void OnItemPoped(size_t host_index) = 0;
};

class ClipboardModel : public SimpleObservable<ClipboardModelObserver>,
                       public ClipboardObserver {
 public:
  using HostClipboardQueue = std::deque<std::string>;
  ClipboardModel();

  // ClipboardObserver overrides
  void OnTextUpdated(const std::string& value) override;

  size_t GetHostsCount() const;
  const HostClipboardQueue& GetContent(size_t host_index) const;

 private:
  std::vector<HostClipboardQueue> clipboard_content_;
};

}  // namespace reclip
