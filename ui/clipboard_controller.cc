#include "ui/clipboard_controller.h"

#include "base/log.h"
#include "ui/content_window.h"
#include "ui/host_clipboard_view.h"

import core.clipboard_model;
import core.host_types;

namespace reclip {
namespace {
constexpr uint32_t kThisHostIndex = 0;
constexpr uint32_t kThisHostOffset = 1;
}

ClipboardController::ClipboardController(ClipboardModel* model,
                                         Clipboard* clipboard)
    : model_(model), clipboard_(clipboard) {}

ClipboardController::~ClipboardController() = default;

void ClipboardController::ShowUi() {
  LOG(INFO) << "Showing ui...";
  content_ = std::make_unique<ContentWindow>(this);

  auto* this_view = content_->AddHostView();
  assert(this_view);
  PopulateData(model_->GetThisHostData(), *this_view);

  ShowHostsModelData();
}

void ClipboardController::HideUi() {
  LOG(INFO) << "Hiding ui...";
  content_.reset();
}

void ClipboardController::OnThisTextPushed() {
  OnTextPushedImpl(
      kThisHostIndex,
      QString::fromStdString(model_->GetThisHostData().data.text.front()));
}

void ClipboardController::OnThisTextPoped() {
  OnTextPopedImpl(kThisHostIndex);
}

void ClipboardController::OnTextPushed(size_t host_index) {
  OnTextPushedImpl(kThisHostOffset + host_index,
                   QString::fromStdString(
                       model_->GetHostData(host_index).data.text.front()));
}

void ClipboardController::OnTextPoped(size_t host_index) {
  OnTextPopedImpl(kThisHostOffset + host_index);
}

void ClipboardController::OnHostUpdated(size_t host_index) {
  const auto view_index = kThisHostOffset + host_index;
  assert(view_index <= content_->HostsCount());
  if (view_index == content_->HostsCount()) {
    content_->AddHostView();
  }

  auto* view = content_->GetHostView(kThisHostOffset + host_index);
  assert(view);
  PopulateData(model_->GetHostData(host_index), *view);
}

void ClipboardController::OnThisHostDataReset() {
  auto* this_view = content_->GetHostView(kThisHostIndex);
  assert(this_view);
  PopulateData(model_->GetThisHostData(), *this_view);
}

void ClipboardController::OnHostsDataReset() {
  content_->RemoveHostViews(kThisHostIndex + kThisHostOffset);
  ShowHostsModelData();
}

void ClipboardController::OnItemClicked(uint32_t host_index,
                                        uint32_t item_index) {
  const HostData* data = nullptr;
  if (host_index == kThisHostIndex) {
    data = &model_->GetThisHostData();
  } else {
    data = &model_->GetHostData(host_index - kThisHostOffset);
  }

  if (item_index >= data->data.text.size()) {
    assert(false && "Unexpected index, which exeeds data size");
    return;
  }
  clipboard_->WriteText(data->data.text[item_index]);
}

void ClipboardController::PopulateData(const HostData& model_data,
                                       HostClipboardView& view) {
  view.SetName(QString::fromStdString(model_data.name));
  view.Clear();
  for (const auto& elem : model_data.data.text) {
    view.PushTop(QString::fromStdString(elem));
  }
}

void ClipboardController::ShowHostsModelData() {
  for (size_t i = 0; i < model_->GetHostsCount(); ++i) {
    auto& model_data = model_->GetHostData(i);
    auto* view = content_->AddHostView(QString::fromStdString(model_data.name));
    assert(view);
    PopulateData(model_data, *view);
  }  
}

void ClipboardController::OnTextPushedImpl(size_t index, const QString& text) {
  if (!content_) {
    return;
  }
  HostClipboardView* view = content_->GetHostView(index);
  assert(view);
  view->PushTop(text);
}

void ClipboardController::OnTextPopedImpl(size_t index) {
  if (!content_) {
    return;
  }
  HostClipboardView* view = content_->GetHostView(index);
  assert(view);
  view->PopBottom();  
}

}  // namespace reclip
