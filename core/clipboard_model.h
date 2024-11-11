#pragma once
#include <vector>

#include "base/observable.h"
#include "core/client_data.h"
#include "core/clipboard_observer.h"

namespace reclip {

class ClipboardModelObserver {
 public:
  virtual ~ClipboardModelObserver() = default;
  virtual void OnHostItemAdded() = 0;
  virtual void OnLastHostItemRemoved() = 0;
};

class ClipboardModel : public Observable<ClipboardModelObserver>,
                       public ClipboardObserver {
 public:
  // ClipboardObserver overrides
  void OnTextUpdated(const std::string& value) override;

  const std::deque<std::string>& GetHostContent() const;

 private:
  std::deque<std::string> host_clipboard_content_;
};

}  // namespace reclip
