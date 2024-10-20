#include "core/clipboard_listener_mac.h"

#include <QGuiApplication>
#include <cassert>

#include "core/clipboard_observer.h"

namespace reclip {

ClipboardListenerMac::ClipboardListenerMac()
    : QObject(nullptr), clipboard_(QGuiApplication::clipboard()) {
  assert(clipboard_);
  connect(&check_timer_, &QTimer::timeout, this,
          &ClipboardListenerMac::CheckTimerTimeout);
}

void ClipboardListenerMac::Start() { check_timer_.start(100); }

void ClipboardListenerMac::Stop() { check_timer_.stop(); }

void ClipboardListenerMac::AddObserver(ClipboardObserver& observer) {
  if (std::find(observers_.begin(), observers_.end(), &observer) ==
      observers_.end()) {
    observers_.push_back(&observer);
  }
}

void ClipboardListenerMac::RemoveObserver(ClipboardObserver& observer) {
  observers_.erase(std::find(observers_.begin(), observers_.end(), &observer));
}

void ClipboardListenerMac::CheckTimerTimeout() {
  const auto text = clipboard_->text();
  if (text != prev_text_) {
    const auto tmp_text = text.toStdString();
    for (auto& observer : observers_) {
      observer->OnTextUpdated(tmp_text);
    }
    prev_text_ = text;
  }
}

}  // namespace reclip
