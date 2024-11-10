#pragma once
#include <cassert>
#include <vector>

namespace reclip {

template <typename Observer>
class Observable {
 public:
  virtual ~Observable() = default;

  void AddObserver(Observer* target) {
    if (std::find(observers_.begin(), observers_.end(), target) ==
        observers_.end()) {
      observers_.push_back(target);
    }
  }

  void RemoveObserver(Observer* target) {
    observers_.erase(std::find(observers_.begin(), observers_.end(), target));
    assert(std::find(observers_.begin(), observers_.end(), target) ==
           observers_.end());
  }

 protected:
  std::vector<Observer*> observers_;
};

}  // namespace reclip
