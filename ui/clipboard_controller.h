#pragma once
#include "core/clipboard_model.h"

namespace reclip {

class ClipboardModel;
class ContentWindow;

class ClipboardController : public ClipboardModelObserver {
 public:
  ClipboardController(ClipboardModel* model);
  ~ClipboardController();
  void ShowUi();
  void HideUi();
  
  // ClipboardModelObserver overrides
  void OnHostItemAdded() override;
  
 private:
  std::unique_ptr<ContentWindow> content_;
  ClipboardModel* model_;
};

}  // namespace reclip
