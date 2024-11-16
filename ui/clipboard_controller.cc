#include "ui/clipboard_controller.h"

#include "base/log.h"
#include "ui/content_window.h"

namespace reclip {

ClipboardController::ClipboardController(ClipboardModel* model, Clipboard* clipboard)
    : model_(model), clipboard_(clipboard) {}

ClipboardController::~ClipboardController() = default;

void ClipboardController::ShowUi() {
  LOG(INFO) << "Showing ui...";
  content_ = std::make_unique<ContentWindow>(this);
  
  // Set up ui.
  for (const auto& elem : model_->GetHostContent()) {
    content_->PushFrontHostText(QString::fromStdString(elem));
  }
}

void ClipboardController::HideUi() {
  LOG(INFO) << "Hiding ui...";
  content_.reset();
}

void ClipboardController::OnHostItemPushed() {
  if (!content_) {
    return;
  }
  const auto& data = model_->GetHostContent();
  content_->PushFrontHostText(QString::fromStdString(data.front()));
}

void ClipboardController::OnHostItemPoped() { content_->PopBackHostText(); }

void ClipboardController::OnItemClicked(uint32_t item_index) {
  const auto& data = model_->GetHostContent();
  if (item_index >= data.size()) {
    assert(false && "Unexpected index, which exeeds data size");
    return;
  }
  clipboard_->WriteText(data[item_index]);
}

}  // namespace reclip
