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
      content_->PushThisHostText(QString::fromStdString(elem));
    }
  }
}

void ClipboardController::HideUi() {
  LOG(INFO) << "Hiding ui...";
  content_.reset();
}

void ClipboardController::OnThisItemPushed() {
  if (!content_) {
    return;
  }
  const auto& data = model_->GetThisHostData().data.text;
  content_->PushThisHostText(QString::fromStdString(data.front()));
}

void ClipboardController::OnThisItemPoped() {
  if (!content_) {
    return;
  }
  content_->PopThisHostText();
}

void ClipboardController::OnItemPushed(size_t host_index) {
  // TODO:
  (void)host_index;
}

void ClipboardController::OnItemPoped(size_t host_index) {
  // TODO:
  (void)host_index;
}

void ClipboardController::OnHostUpdated(size_t host_index) {
  // TODO:
  (void)host_index;
}

void ClipboardController::OnThisHostDataUpated() {
  // TODO:
}

void ClipboardController::OnHostsDataUpdated() {
  // TODO:
}

void ClipboardController::OnItemClicked(uint32_t host_index,
                                        uint32_t item_index) {
  const HostData* data = nullptr;
  // TODO:
  if (host_index == 0) {
    data = &model_->GetThisHostData();
  } else {
    data = &model_->GetHostData(host_index - 1);
  }

  if (item_index >= data->data.text.size()) {
    assert(false && "Unexpected index, which exeeds data size");
    return;
  }
  clipboard_->WriteText(data->data.text[item_index]);
}

}  // namespace reclip
