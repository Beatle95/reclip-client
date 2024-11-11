#include "ui/content_window.h"

#include <QBoxLayout>
#include <QScrollArea>
#include <cassert>

#include "base/log.h"
#include "ui/text_view.h"

constexpr QSize kMinSize(320, 480);

namespace reclip {

ContentWindow::ContentWindow() {
  auto* scroll = new QScrollArea(this);
  main_widget_ = new QWidget;
  scroll->setWidgetResizable(true);
  scroll->setWidget(main_widget_);

  // TODO: set width contraint for main widget
  layout_ = new QVBoxLayout;
  main_widget_->setLayout(layout_);
  layout_->setDirection(QBoxLayout::BottomToTop);
  layout_->setAlignment(Qt::AlignTop);

  setCentralWidget(scroll);
  setMinimumSize(kMinSize);
  show();
}

void ContentWindow::AddHostText(const QString& text) {
  auto* text_view = new TextView(text, main_widget_);
  layout_->addWidget(text_view);
}

void ContentWindow::RemoveLastHostText() {
  assert(layout_->count() != 0);
  delete layout_->itemAt(0)->widget();
}

}  // namespace reclip
