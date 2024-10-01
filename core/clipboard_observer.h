#pragma once
#include <string_view>

namespace reclip {

class ClipboardObserver {
  public:
    virtual ~ClipboardObserver() = default;
    virtual void OnTextUpdated(std::string_view value) = 0;
};

}  // namespace reclip
