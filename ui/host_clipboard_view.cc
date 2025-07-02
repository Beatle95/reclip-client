#include "ui/host_clipboard_view.h"

#include <QBoxLayout>
#include <QLabel>
#include <QScrollArea>

#include "ui/text_view.h"

constexpr int kMinWidth = 320;

namespace reclip {

HostClipboardView::HostClipboardView(const QString& visible_name) {
  setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
  setObjectName("HostClipboardView");
  setMinimumWidth(kMinWidth);

  auto* layout = new QVBoxLayout;
  layout->setAlignment(Qt::AlignTop);

  name_ = new QLabel(visible_name, this);
  name_->setAlignment(Qt::AlignHCenter);
  name_->setObjectName("HostName");

  layout->addWidget(name_);
  content_ = new QWidget(this);
  auto* scroll = new QScrollArea(this);
  scroll->setWidgetResizable(true);
  scroll->setWidget(content_);
  layout->addWidget(scroll);

  auto* content_layout = new QVBoxLayout;
  content_layout->setDirection(QBoxLayout::BottomToTop);
  content_layout->setAlignment(Qt::AlignTop);
  content_->setLayout(content_layout);
  setLayout(layout);
}

void HostClipboardView::SetName(const QString& visible_name) {
  name_->setText(visible_name);
}

void HostClipboardView::PushTop(const QString& text) {
  auto* text_view = new TextView(text, this);
  connect(text_view, &TextView::Clicked, this, &HostClipboardView::ItemClicked);
  auto* layout = content_->layout();
  layout->addWidget(text_view);
}

void HostClipboardView::PopBottom() {
  auto* layout = content_->layout();
  assert(layout->count() != 0);
  delete layout->itemAt(0)->widget();
}

void HostClipboardView::Clear() {
  auto* layout = content_->layout();
  while (layout->count() != 0) {
    delete layout->itemAt(0)->widget();
  }
}

size_t HostClipboardView::GetTextItemsCount() const {
  auto* layout = content_->layout();
  return static_cast<size_t>(layout->count());
}

void HostClipboardView::ItemClicked() {
  auto* emitter = sender();
  auto* layout = content_->layout();
  for (int i = 0; i < layout->count(); ++i) {
    if (layout->itemAt(i)->widget() == emitter) {
      emit ElementClicked(static_cast<uint32_t>(layout->count() - i - 1));
      return;
    }
  }
  assert(false && "Unreachable");
}

}  // namespace reclip
