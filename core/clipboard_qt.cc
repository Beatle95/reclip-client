#include "core/clipboard_qt.h"

#include <QGuiApplication>
#include <cassert>

#include "base/constants.h"
#include "base/log.h"
#include "core/clipboard_observer.h"

namespace reclip {
void ClipboardMac::WriteText(const std::string&) {
  // TODO:
}

ClipboardMac::ClipboardMac()
    : QObject(nullptr), clipboard_(QGuiApplication::clipboard()) {
  assert(clipboard_);
  connect(clipboard_, &QClipboard::dataChanged, this,
          &ClipboardMac::ClipboardTextChanged);
}

void ClipboardMac::AddObserver(ClipboardObserver* observer) {
  if (std::find(observers_.begin(), observers_.end(), observer) ==
      observers_.end()) {
    observers_.push_back(observer);
  }
}

void ClipboardMac::RemoveObserver(ClipboardObserver* observer) {
  observers_.erase(std::find(observers_.begin(), observers_.end(), observer));
}

void ClipboardMac::ClipboardTextChanged() {
  const auto text = clipboard_->text().toStdString();
  DLOG(INFO) << "[EVENT] ClipboardModel's clipboard text changed: \""
             << str.substr(0, kMaxContentLogSize)
             << (str.size() > kMaxContentLogSize ? "...\"" : "\"");
  for (auto& observer : observers_) {
    observer->OnTextUpdated(text);
  }
}

}  // namespace reclip
