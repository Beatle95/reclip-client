#pragma once
#include "core/clipboard_observer.h"

namespace reclip {

class Server : public ClipboardObserver {
  public:
    void OnTextUpdated(const std::string& value) override;
};

}
