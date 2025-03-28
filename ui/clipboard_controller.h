#pragma once
#include <cstdint>

#include "core/clipboard.h"
#include "core/clipboard_model.h"
#include "ui/content_window.h"

namespace reclip {

class ClipboardModel;

class ClipboardController : public ClipboardModelObserver,
                            public ContentWindow::Delegate {
 public:
  ClipboardController(ClipboardModel* model, Clipboard* clipboard);
  ~ClipboardController();
  void ShowUi();
  void HideUi();

  // ClipboardModelObserver overrides
  void OnThisTextPushed() override;
  void OnThisTextPoped() override;
  void OnTextPushed(size_t host_index) override;
  void OnTextPoped(size_t host_index) override;
  void OnHostUpdated(size_t host_index) override;
  void OnThisHostDataReset() override;
  void OnHostsDataReset() override;

  // ContentWindow::Delegate overrides
  void OnItemClicked(uint32_t host_index, uint32_t item_index) override;

 private:
  void PopulateData(const HostData& model_data, HostClipboardView& view);
  void ShowHostsModelData();
  void OnTextPushedImpl(size_t index, const QString& text);
  void OnTextPopedImpl(size_t index);

  std::unique_ptr<ContentWindow> content_;
  ClipboardModel* model_;
  Clipboard* clipboard_;
};

}  // namespace reclip
