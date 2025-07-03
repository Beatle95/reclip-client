#include "ui/content_window.h"

#include <QBoxLayout>
#include <QScreen>
#include <QScrollArea>
#include <QStyle>
#include <cassert>

#include "ui/host_clipboard_view.h"

import base.preferences;

constexpr QSize kMinSize(320, 480);
constexpr std::string_view kHeightPref = "ui.remembered_height";
constexpr std::string_view kWidthPref = "ui.remembered_width";

namespace reclip {

ContentWindow::~ContentWindow() = default;

ContentWindow::ContentWindow(Delegate* delegate) : delegate_(delegate) {
  auto& prefs = Preferences::GetInstance();
  prefs.RegisterInt(kHeightPref, kMinSize.height());
  prefs.RegisterInt(kWidthPref, kMinSize.width());

  auto* scroll = new QScrollArea(this);
  main_widget_ = new QWidget;
  main_widget_->setObjectName("ContentWindowWidget");
  scroll->setWidgetResizable(true);
  scroll->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
  scroll->setWidget(main_widget_);
  main_widget_->setLayout(new QHBoxLayout);

  setMinimumSize(kMinSize);
  setCentralWidget(scroll);
  show();
}

void ContentWindow::RemoveHostViews(uint32_t start_index) {
  if (start_index < host_views_.size()) {
    host_views_.resize(start_index);
  }
}

HostClipboardView* ContentWindow::AddHostView(const QString& name) {
  QLayout* layout = main_widget_->layout();
  assert(layout != nullptr);

  auto view = std::make_unique<HostClipboardView>(name);
  layout->addWidget(view.get());
  connect(view.get(), &HostClipboardView::ElementClicked, this, &ContentWindow::HostItemClicked);
  host_views_.push_back(std::move(view));
  return host_views_.back().get();
}

HostClipboardView* ContentWindow::GetHostView(uint32_t index) {
  if (index >= host_views_.size()) {
    return nullptr;
  }
  return host_views_[index].get();
}

size_t ContentWindow::HostsCount() const { return host_views_.size(); }

void ContentWindow::showEvent(QShowEvent*) {
  // Center on screen when shown and try to keep the size of the window.
  auto& prefs = Preferences::GetInstance();
  QRect target_rect = geometry();
  if (const QSize saved_size(prefs.GetInt(kWidthPref), prefs.GetInt(kHeightPref));
      saved_size.isValid()) {
      target_rect.setSize(saved_size);
  }

  auto* current_screen = screen();
  assert(current_screen);
  if (current_screen) {
    if (current_screen->geometry().size().width() > target_rect.size().width() &&
        current_screen->geometry().size().height() > target_rect.size().height()) {
      // Center the window on the screen.
      target_rect.moveCenter(current_screen->geometry().center());
    } else {
      // There is not enough space to fit the window, just maximize it.
      target_rect = current_screen->geometry();
    }
  }

  setGeometry(target_rect);
}

void ContentWindow::closeEvent(QCloseEvent*) {
  const auto cur_size = size();
  auto& prefs = Preferences::GetInstance();
  prefs.SetInt(kHeightPref, cur_size.height());
  prefs.SetInt(kWidthPref, cur_size.width());
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
