#pragma once
#include "base/observable.h"

#include <memory>

namespace reclip {

class ClipboardObserver;

class Clipboard : public Observable<ClipboardObserver> {
  public:
    virtual ~Clipboard() = default;
    static std::unique_ptr<Clipboard> Create();

    virtual void WriteText(const std::string& text) = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
};

}  // namespace reclip
