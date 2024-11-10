#include "ui/content_window.h"
#include <QBoxLayout>

constexpr QSize kMinSize(320, 480);

namespace reclip {

ContentWindow::ContentWindow() {
  text_edit_ = new QTextEdit(this);
  auto* layout = new QVBoxLayout(this);

  // TODO:
  text_edit_->setSizePolicy(
      QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  layout->addWidget(text_edit_);
  setMinimumSize(kMinSize);

  show();
}

}  // namespace reclip
