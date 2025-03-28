#include "ui/content_window.h"

#include <QBoxLayout>
#include <QScrollArea>
#include <cassert>

#include "base/log.h"
#include "ui/host_clipboard_view.h"

constexpr QSize kMinSize(320, 480);

namespace reclip {

ContentWindow::~ContentWindow() = default;

ContentWindow::ContentWindow(Delegate* delegate) : delegate_(delegate) {
  auto* scroll = new QScrollArea(this);
  main_widget_ = new QWidget;
  scroll->setWidgetResizable(true);
  scroll->setWidget(main_widget_);

  auto* layout = new QHBoxLayout;
  main_widget_->setLayout(layout);

  auto host_view = std::make_unique<HostClipboardView>("This device");
  layout->addWidget(host_view.get());
  connect(host_view.get(), &HostClipboardView::ElementClicked, this,
          &ContentWindow::HostItemClicked);
  host_views_.push_back(std::move(host_view));

  setCentralWidget(scroll);
  setMinimumSize(kMinSize);
  show();
}

void ContentWindow::PushThisHostText(const QString& text) {
  assert(!host_views_.empty());
  host_views_[0]->PushTop(text);
}

void ContentWindow::PopThisHostText() {
  assert(!host_views_.empty());
  host_views_[0]->PopBottom();
}

void ContentWindow::HostItemClicked(uint32_t element_index) {
  auto* emitter = sender();
  for (size_t i = 0; i < host_views_.size(); ++i) {
    if (host_views_[i].get() == emitter) {
      assert(delegate_);
      delegate_->OnItemClicked(i, element_index);
      return;
    }
  }
  assert(false && "Unreachable");
}

}  // namespace reclip
