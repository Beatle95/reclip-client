#include "core/clipboard_listener_qt.h"

#include <QGuiApplication>
#include <cassert>

#include "base/log.h"
#include "core/clipboard_observer.h"

constexpr size_t kLogLen = 10;

namespace reclip {

ClipboardListenerMac::ClipboardListenerMac()
    : QObject(nullptr), clipboard_(QGuiApplication::clipboard()) {
  assert(clipboard_);
  connect(clipboard_, &QClipboard::dataChanged, this,
          &ClipboardListenerMac::ClipboardTextChanged);
}

void ClipboardListenerMac::AddObserver(ClipboardObserver& observer) {
  if (std::find(observers_.begin(), observers_.end(), &observer) ==
      observers_.end()) {
    observers_.push_back(&observer);
  }
}

void ClipboardListenerMac::RemoveObserver(ClipboardObserver& observer) {
  observers_.erase(std::find(observers_.begin(), observers_.end(), &observer));
}

void ClipboardListenerMac::ClipboardTextChanged() {
  const auto text = clipboard_->text().toStdString();
  DLOG(INFO) << "[EVENT] clipboard text changed: \"" << text.substr(0, kLogLen)
             << (text.size() > kLogLen ? "...\"" : "\"");
  for (auto& observer : observers_) {
    observer->OnTextUpdated(text);
  }
}

}  // namespace reclip
