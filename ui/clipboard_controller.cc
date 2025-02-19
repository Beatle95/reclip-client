#include "ui/clipboard_controller.h"

#include "base/log.h"
#include "ui/content_window.h"

namespace reclip {

ClipboardController::ClipboardController(ClipboardModel* model,
                                         Clipboard* clipboard)
    : model_(model), clipboard_(clipboard) {}

ClipboardController::~ClipboardController() = default;

void ClipboardController::ShowUi() {
  LOG(INFO) << "Showing ui...";
  content_ = std::make_unique<ContentWindow>(this);

  // Set up ui.
  for (size_t i = 0; i < model_->GetHostsCount(); ++i) {
    for (const auto& elem : model_->GetHostData(i).data.text) {
      content_->PushTopHostText(QString::fromStdString(elem));
    }
  }
}

void ClipboardController::HideUi() {
  LOG(INFO) << "Hiding ui...";
  content_.reset();
}

void ClipboardController::OnItemPushed(size_t host_index) {
  if (!content_) {
    return;
  }
  const auto& data = model_->GetHostData(host_index).data.text;
  content_->PushTopHostText(QString::fromStdString(data.front()));
}

void ClipboardController::OnItemPoped(size_t) {
  if (!content_) {
    return;
  }
  content_->PopBottomHostText();
}

void ClipboardController::OnHostAdded(size_t) {
  if (!content_) {
    return;
  }
  // TODO:
}

void ClipboardController::OnModelReset() {
  if (!content_) {
    return;
  }
  // TODO:
}

void ClipboardController::OnItemClicked(uint32_t host_index,
                                        uint32_t item_index) {
  const auto& data = model_->GetHostData(host_index).data.text;
  if (item_index >= data.size()) {
    assert(false && "Unexpected index, which exeeds data size");
    return;
  }
  clipboard_->WriteText(data[item_index]);
}

}  // namespace reclip
