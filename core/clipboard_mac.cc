#include "core/clipboard_mac.h"

#include <QGuiApplication>
#include <cassert>

#include "base/log.h"
#include "core/clipboard_observer.h"

namespace reclip {

ClipboardMac::ClipboardMac()
    : QObject(nullptr), clipboard_(QGuiApplication::clipboard()) {
  assert(clipboard_);
  DLOG(INFO) << "Using macOS clipboard implementation";
  connect(&check_timer_, &QTimer::timeout, this,
          &ClipboardMac::CheckTimerTimeout);
  prev_text_ = clipboard_->text();
}

void ClipboardMac::WriteText(const std::string& text) {
  prev_text_ = QString::fromStdString(text);
  clipboard_->setText(prev_text_);
}

void ClipboardMac::Start() { check_timer_.start(100); }

void ClipboardMac::Stop() { check_timer_.stop(); }

void ClipboardMac::CheckTimerTimeout() {
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
