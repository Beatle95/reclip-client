#pragma once
#include <cstdint>
#include <vector>

namespace reclip {

class ObserversListBase;

class Observer {
 public:
  Observer() = default;
  virtual ~Observer() = default;
  Observer(const Observer&) = delete;
  Observer(Observer&&) = delete;
  Observer& operator=(const Observer&) = delete;
  Observer& operator=(Observer&&) = delete;

 protected:
  virtual void OnSubscribe() = 0;
  virtual void OnUnsibscribe() = 0;
};

class CheckedObserver : public Observer {
 public:
  ~CheckedObserver() override;

 private:
  template <typename T>
  friend class ObserversList;
  friend class TestObserver;

  void OnSubscribe() override;
  void OnUnsibscribe() override;

  uint32_t subscriptions_count_ = 0;
};

}  // namespace reclip
