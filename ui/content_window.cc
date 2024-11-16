#include "ui/content_window.h"

#include <QBoxLayout>
#include <QScrollArea>
#include <cassert>

#include "base/log.h"
#include "ui/text_view.h"

constexpr QSize kMinSize(320, 480);
constexpr int kMainWidgetWidth = 300;

namespace reclip {

ContentWindow::ContentWindow(Delegate* delegate) : delegate_(delegate) {
  auto* scroll = new QScrollArea(this);
  main_widget_ = new QWidget;
  main_widget_->setFixedWidth(kMainWidgetWidth);
  scroll->setWidgetResizable(true);
  scroll->setWidget(main_widget_);

  // TODO: set width contraint for main widget
  auto* layout = new QVBoxLayout;
  main_widget_->setLayout(layout);
  layout->setDirection(QBoxLayout::BottomToTop);
  layout->setAlignment(Qt::AlignTop);

  setCentralWidget(scroll);
  setMinimumSize(kMinSize);
  show();
}

void ContentWindow::PushFrontHostText(const QString& text) {
  auto* text_view = new TextView(text, this);
  auto* layout = main_widget_->layout();
  layout->addWidget(text_view);
}

void ContentWindow::PopBackHostText() {
  auto* layout = main_widget_->layout();
  assert(layout->count() != 0);
  delete layout->itemAt(0)->widget();
}

void ContentWindow::TextViewClicked(TextView* child) {
  auto* layout = main_widget_->layout();
  for (int i = 0; i < layout->count(); ++i) {
    if (layout->itemAt(i)->widget() == child) {
      assert(delegate_);
      delegate_->OnItemClicked(static_cast<uint32_t>(layout->count() - i - 1));
      return;
    }
  }
  assert(false && "Unreachable");
}

}  // namespace reclip
