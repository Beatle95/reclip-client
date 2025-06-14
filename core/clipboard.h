#pragma once
#include <memory>

#include "core/clipboard_observer.h"

import base.observer_helper;

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
