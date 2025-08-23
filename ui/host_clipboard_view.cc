#include "ui/host_clipboard_view.h"

#include <QBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <string_view>

#include "ui/text_view.h"

import base.preferences;

constexpr std::string_view kMinWidthPrefName = "ui.host_clipboard_view.min_width";
constexpr int kDefaultMinWidth = 380;

namespace reclip {

// static
void HostClipboardView::RegisterPrefs() {
  Preferences::GetInstance().RegisterInt(kMinWidthPrefName, kDefaultMinWidth);
}

HostClipboardView::HostClipboardView(const QString& visible_name) {
  setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
  setObjectName("HostClipboardView");
  setMinimumWidth(Preferences::GetInstance().GetInt(kMinWidthPrefName));

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

void HostClipboardView::PushText(const QString& text) {
  auto* text_view = new TextView(text, this);
  connect(text_view, &TextView::Clicked, this, &HostClipboardView::ItemClicked);
  auto* layout = content_->layout();
  layout->addWidget(text_view);
}

void HostClipboardView::PopText() {
  auto* layout = content_->layout();
  assert(layout->count() != 0);
  delete layout->itemAt(0)->widget();
}

void HostClipboardView::ClearText() {
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
  // We are pushing elements in the reverse order, so we have to take this into account when
  // reporting clicked index.
  auto* emitter = sender();
  auto* layout = content_->layout();
  for (int i = 0; i < layout->count(); ++i) {
    if (layout->itemAt(i)->widget() == emitter) {
      emit ElementClicked(layout->count() - i - 1);
      return;
    }
  }
  assert(false && "Unreachable");
}

}  // namespace reclip
