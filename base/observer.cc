#include "base/observer.h"

#include <algorithm>
#include <cassert>

namespace reclip {

CheckedObserver::~CheckedObserver() {
  assert(subscriptions_count_ == 0 &&
         "Destructor of the subsceibed observer was called");
}

void CheckedObserver::OnSubscribe() { ++subscriptions_count_; }

void CheckedObserver::OnUnsibscribe() {
  assert(subscriptions_count_ != 0);
  --subscriptions_count_;
}

}  // namespace reclip
