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
  void OnHostItemPushed() override;
  void OnHostItemPoped() override;

  // ContentWindow::Delegate overrides
  void OnItemClicked(uint32_t host_index, uint32_t item_index) override;

 private:
  std::unique_ptr<ContentWindow> content_;
  ClipboardModel* model_;
  Clipboard* clipboard_;
};

}  // namespace reclip
