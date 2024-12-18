#pragma once
#include <memory>

#include "base/observers_list.h"
#include "core/clipboard_observer.h"

namespace reclip {

class Clipboard : public SimpleObservable<ClipboardObserver> {
 public:
  virtual ~Clipboard() = default;
  static std::unique_ptr<Clipboard> Create();

  virtual void WriteText(const std::string& text) = 0;
  virtual void Start() = 0;
  virtual void Stop() = 0;
};

}  // namespace reclip
