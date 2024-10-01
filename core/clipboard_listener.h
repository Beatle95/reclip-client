#pragma once
#include <memory>

namespace reclip {

class ClipboardObserver;

class ClipboardListener {
  public:
    static std::unique_ptr<ClipboardListener> Create();

    virtual ~ClipboardListener() = default;
    virtual void Start() {}
    virtual void Stop() {}
    virtual void AddObserver(
        const std::shared_ptr<ClipboardObserver>& observer) = 0;
    virtual void RemoveObserver(
        const std::shared_ptr<ClipboardObserver>& observer) = 0;
};

}  // namespace reclip
