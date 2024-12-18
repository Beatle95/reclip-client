#pragma once
#include <algorithm>
#include <cassert>
#include <list>

#include "base/observer.h"

namespace reclip {

// This class is needed to check emptines of subscribed observers upon
// destruction and to ensure that adding and removing elements is safe upon
// iteration.
template <typename T>
class ObserversList {
 public:
  using ContainerType = std::list<T*>;

  // TODO: save pointer inside it, so that after removing element we will still
  // dereference to correct location.
  class SafeIterator {
   public:
    using internal_iterator = ContainerType::iterator;
    using value_type = ContainerType::iterator::value_type;
    using difference_type = ContainerType::iterator::difference_type;
    using pointer = ContainerType::iterator::pointer;
    using reference = ContainerType::iterator::reference;
    using iterator_category = std::forward_iterator_tag;

    SafeIterator() = default;

    SafeIterator(ContainerType::iterator it, ObserversList& list)
        : it_(it), observers_list_(&list) {
      observers_list_->IncrementLock();
      MoveForwardWhilePossible();
    }

    SafeIterator(const SafeIterator& other)
        : SafeIterator(other.it_, *other.observers_list_) {}

    SafeIterator(SafeIterator&& other)
        : it_(std::move(other.it_)),
          observers_list_(std::move(other.observers_list_)) {
      other.observers_list_ = nullptr;
    }

    SafeIterator& operator=(const SafeIterator& other) {
      it_ = other.it_;
      if (observers_list_ == other.observers_list_) {
        return *this;
      }

      if (observers_list_) {
        observers_list_->DecrementLock();
      }
      observers_list_ = other.observers_list_;
      if (observers_list_) {
        observers_list_->IncrementLock();
      }

      return *this;
    }

    SafeIterator& operator=(SafeIterator&& other) {
      it_ = std::move(other.it_);
      observers_list_ = std::move(other.observers_list_);
      observers_list_ = nullptr;
      return *this;
    }

    ~SafeIterator() {
      if (observers_list_) {
        observers_list_->DecrementLock();
      }
    }

    bool operator==(const SafeIterator& other) const {
      return it_ == other.it_;
    }
    bool operator!=(const SafeIterator& other) const {
      return it_ != other.it_;
    }

    SafeIterator& operator++() {
      ++it_;
      MoveForwardWhilePossible();
      return *this;
    }

    SafeIterator operator++(int) {
      SafeIterator result(it_, *observers_list_);
      ++it_;
      MoveForwardWhilePossible();
      return result;
    }

    reference operator*() {
      assert(observers_list_ != nullptr);
      assert(it_ != observers_list_->observers_.end());
      return *it_;
    }

    pointer operator->() {
      assert(observers_list_ != nullptr);
      assert(it_ != observers_list_->observers_.end());
      return &(*it_);
    }

   private:
    void MoveForwardWhilePossible() {
      assert(observers_list_ != nullptr);
      while (it_ != observers_list_->observers_.end() && *it_ == nullptr) {
        ++it_;
      }
    }

    ContainerType::iterator it_;
    ObserversList* observers_list_ = nullptr;
  };

  ~ObserversList() {
    assert(lock_count_ == 0 &&
           "Destructor was called while iterating ObserversList");
    assert(observers_.empty() &&
           "Observers must be removed before destruction");
  }

  void AddObserver(T* target) {
    assert(target != nullptr);
    if (std::find(observers_.begin(), observers_.end(), target) !=
        observers_.end()) {
      assert(false && "Adding the same element twice is forbidden");
      return;
    }

    observers_.push_back(target);
    // It is actually not obvious do we need to notify here, or when pushing to
    // the main container.
    target->OnSubscribe();
    ++size_;
  }

  void RemoveObserver(T* target) {
    assert(target != nullptr);
    const auto it = std::find(observers_.begin(), observers_.end(), target);
    if (it == observers_.end()) {
      // NOTE: Maybe we want an assert here?
      return;
    }

    if (lock_count_ == 0) {
      observers_.erase(it);
    } else {
      *it = nullptr;
    }

    target->OnUnsibscribe();
    --size_;
    assert(std::find(observers_.begin(), observers_.end(), target) ==
           observers_.end());
  }

  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }
  SafeIterator begin() { return SafeIterator(observers_.begin(), *this); }
  SafeIterator end() { return SafeIterator(observers_.end(), *this); }

 private:
  friend class SafeIterator;

  void IncrementLock() { ++lock_count_; }

  void DecrementLock() {
    assert(lock_count_ != 0);
    if (--lock_count_ == 0) {
      ProceedLockRelease();
    }
  }

  void ProceedLockRelease() {
    // Remove all nullptr's.
    observers_.erase(
        std::remove_if(observers_.begin(), observers_.end(),
                       [](const T* elem) { return elem == nullptr; }),
        observers_.end());
  }

  ContainerType observers_;
  size_t size_ = 0;
  uint32_t lock_count_ = 0;
};

// This is simple class to eliminate some boilerplates.
template <typename T>
class SimpleObservable {
 public:
  void AddObserver(T* target) { observers_.AddObserver(target); }
  void RemoveObserver(T* target) { observers_.RemoveObserver(target); }

 protected:
  ObserversList<T> observers_;
};

}  // namespace reclip
