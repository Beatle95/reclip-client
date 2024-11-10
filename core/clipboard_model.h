#pragma once
#include <vector>

#include "core/client_data.h"
#include "core/clipboard_observer.h"

namespace reclip {

class ClipboardModel : public ClipboardObserver {
 public:
  // Right now we will simply notify about any updates.
  class Observer {
   public:
    virtual void OnModelUpdated() = 0;
  };

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

  void OnTextUpdated(const std::string& value) override;

  const std::deque<std::string>& GetHostContent() const;

 private:
  std::vector<Observer*> observers_;
  std::deque<std::string> host_clipboard_content_;
};

}  // namespace reclip
