#include "ui/content_window.h"

#include <QBoxLayout>
#include <QScrollArea>

#include "base/log.h"
#include "ui/text_view.h"

constexpr QSize kMinSize(320, 480);
constexpr int kVisibleItemsLimit = 10;

namespace reclip {

ContentWindow::ContentWindow() {
  main_widget_ = new QScrollArea(this);
  layout_ = new QVBoxLayout(main_widget_);
  layout_->setDirection(QBoxLayout::BottomToTop);
  setMinimumSize(kMinSize);
  show();
}

void ContentWindow::AddHostText(const QString& text) {
  // TODO:
  auto* text_view = new TextView(text, main_widget_);
  layout_->addWidget(text_view);

  auto& children = layout_->children();
  while (children.size() >= kVisibleItemsLimit) {
    delete children.front();
  }
}

}  // namespace reclip
