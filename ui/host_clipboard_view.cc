#include "ui/host_clipboard_view.h"

#include <QBoxLayout>
#include <QLabel>

#include "ui/text_view.h"

namespace reclip {

HostClipboardView::HostClipboardView(const QString& visible_name) {
  auto* layout = new QVBoxLayout;
  layout->setAlignment(Qt::AlignTop);

  auto* host = new QLabel(visible_name, this);
  host->setAlignment(Qt::AlignHCenter);
  layout->addWidget(host);
  content_ = new QWidget(this);
  layout->addWidget(content_);

  auto* content_layout = new QVBoxLayout;
  content_layout->setDirection(QBoxLayout::BottomToTop);
  content_layout->setAlignment(Qt::AlignTop);
  content_->setLayout(content_layout);

  setFixedWidth(300);
  setLayout(layout);
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
