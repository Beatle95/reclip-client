#pragma once
#include <vector>

#include "base/observers_list.h"
#include "core/client_data.h"
#include "core/clipboard_observer.h"

namespace reclip {

class ClipboardModelObserver : public CheckedObserver {
 public:
  virtual ~ClipboardModelObserver() = default;
  virtual void OnHostItemPushed() = 0;
  virtual void OnHostItemPoped() = 0;
};

class ClipboardModel : public SimpleObservable<ClipboardModelObserver>,
                       public ClipboardObserver {
 public:
  // ClipboardObserver overrides
  void OnTextUpdated(const std::string& value) override;

  const std::deque<std::string>& GetHostContent() const;

 private:
  std::deque<std::string> host_clipboard_content_;
};

}  // namespace reclip
