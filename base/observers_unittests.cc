#include <gtest/gtest.h>

#include <memory>
#include <span>

import base.observer_helper;

using namespace reclip;
using namespace ::testing;

namespace reclip {
class TestObserver : public CheckedObserver {
 public:
  uint32_t GetSubscriptions() { return subscriptions_count_; }
};
}  // namespace reclip

class ObservationSystemTest : public Test {
 public:
  TestObserver* CreateAndAddObserver() {
    auto* result = CreateObserver();
    observers_.AddObserver(result);
    return result;
  }

  TestObserver* CreateObserver() {
    return observers_storage_.emplace_back(std::make_unique<TestObserver>())
        .get();
  }

  ObserversList<TestObserver>& observers() { return observers_; }

  std::span<const std::unique_ptr<TestObserver>> storage() {
    return observers_storage_;
  }

 private:
  ObserversList<TestObserver> observers_;
  std::vector<std::unique_ptr<TestObserver>> observers_storage_;
};

TEST_F(ObservationSystemTest, ObserverSubscriptions) {
  auto* elem = CreateObserver();
  EXPECT_EQ(elem->GetSubscriptions(), 0);

  observers().AddObserver(elem);
  EXPECT_EQ(elem->GetSubscriptions(), 1);

  ObserversList<TestObserver> another_list;
  another_list.AddObserver(elem);
  EXPECT_EQ(elem->GetSubscriptions(), 2);

  observers().RemoveObserver(elem);
  EXPECT_EQ(elem->GetSubscriptions(), 1);
  // Removing twice does not affects subscriptions count.
  observers().RemoveObserver(elem);
  EXPECT_EQ(elem->GetSubscriptions(), 1);

  another_list.RemoveObserver(elem);
  EXPECT_EQ(elem->GetSubscriptions(), 0);
}

TEST_F(ObservationSystemTest, ObserversListModification) {
  ASSERT_EQ(observers().size(), 0);
  auto* first_elem = CreateAndAddObserver();
  ASSERT_EQ(observers().size(), 1);

  {
    auto it = observers().begin();
    ASSERT_NE(it, observers().end());
    auto* new_observer = CreateAndAddObserver();
    auto old_it = it++;
    EXPECT_EQ(*old_it, first_elem);
    EXPECT_EQ(*it, new_observer);

    ASSERT_NE(it, old_it);
    ASSERT_EQ(new_observer, *it);
    ASSERT_EQ(++old_it, it);
    ASSERT_EQ(++it, observers().end());
    ASSERT_EQ(observers().size(), 2);

    for (auto* elem : observers()) {
      ASSERT_EQ(elem->GetSubscriptions(), 1);
    }
  }

  {
    EXPECT_EQ(observers().size(), 2);
    auto it = observers().begin();
    ASSERT_NE(it, observers().end());

    first_elem = *it;
    ASSERT_NE(first_elem, nullptr);
    observers().RemoveObserver(first_elem);
    ASSERT_EQ(*it, nullptr);
    ASSERT_EQ(observers().size(), 1);

    first_elem = *(observers().begin());
    auto* second_elem = CreateAndAddObserver();
    ASSERT_EQ(observers().size(), 2);
    observers().RemoveObserver(first_elem);
    ASSERT_EQ(observers().size(), 1);
    ++it;
    ASSERT_EQ(*it, second_elem);

    observers().RemoveObserver(second_elem);
  }
  ASSERT_EQ(observers().size(), 0);
}

TEST_F(ObservationSystemTest, ObservationListParallelIteration) {
  for (int i = 0; i < 200; ++i) {
    CreateAndAddObserver();
  }
  for (const auto& elem : storage()) {
    EXPECT_EQ(elem->GetSubscriptions(), 1);
  }

  TestObserver* last = nullptr;
  {
    auto even_it = observers().begin();
    auto odd_it = observers().begin();
    for (int i = 1; i <= 200; ++i) {
      if (i == 100) {
        last = CreateAndAddObserver();
      }
      if (i % 2 == 0) {
        observers().RemoveObserver(*odd_it);
        ++even_it;
      } else {
        observers().RemoveObserver(*even_it);
        ++odd_it;
      }
    }

    EXPECT_EQ(observers().size(), 1);
    EXPECT_EQ(*(observers().begin()), last);
  }
  EXPECT_EQ(*(observers().begin()), last);
  observers().RemoveObserver(last);

  for (const auto& elem : storage()) {
    EXPECT_EQ(elem->GetSubscriptions(), 0);
  }
}
