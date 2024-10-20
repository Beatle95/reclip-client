#pragma once
#include <string>

namespace reclip {

class ClipboardObserver {
  public:
    virtual ~ClipboardObserver() = default;
    virtual void OnTextUpdated(const std::string& value) = 0;
};

}  // namespace reclip
