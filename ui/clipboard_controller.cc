#include "ui/clipboard_controller.h"

#include "base/log.h"
#include "ui/content_window.h"

namespace reclip {

ClipboardController::ClipboardController(ClipboardModel* model)
    : model_(model) {}

ClipboardController::~ClipboardController() = default;

void ClipboardController::ShowUi() {
  DLOG(INFO) << "Showing ui...";
  content_ = std::make_unique<ContentWindow>();
}

void ClipboardController::HideUi() {
  DLOG(INFO) << "Hiding ui...";
  content_.reset();
}

void ClipboardController::OnHostItemAdded() {
  if (!content_) {
    return;
  }
  const auto& data = model_->GetHostContent();
  content_->AddHostText(QString::fromStdString(data.back()));
}

}  // namespace reclip
