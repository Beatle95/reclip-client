#pragma once
#include <string>

import base.observer_helper;

namespace reclip {

class ClipboardObserver : public CheckedObserver {
  public:
    virtual ~ClipboardObserver() = default;
    virtual void OnTextUpdated(const std::string& value) = 0;
};

}  // namespace reclip
