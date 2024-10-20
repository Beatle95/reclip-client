#pragma once
#include <memory>

namespace reclip {

class ClipboardObserver;

// TODO: change to Clipboard (listener is a part of its work, it must put
// something in clipboard too)
class ClipboardListener {
  public:
    static std::unique_ptr<ClipboardListener> Create();

    virtual ~ClipboardListener() = default;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void AddObserver(ClipboardObserver& observer) = 0;
    virtual void RemoveObserver(ClipboardObserver& observer) = 0;
};

}  // namespace reclip
