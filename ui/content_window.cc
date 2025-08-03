#include "ui/content_window.h"

#include <QBoxLayout>
#include <QScreen>
#include <QScrollArea>
#include <QStyle>
#include <cassert>

#include "base/log.h"

import base.preferences;

constexpr QSize kMinSize(320, 480);
constexpr std::string_view kHeightPref = "ui.remembered_height";
constexpr std::string_view kWidthPref = "ui.remembered_width";

namespace reclip {

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
  raise();
  activateWindow();

  LOG(INFO) << "Showing UI";
}

ContentWindow::~ContentWindow() {
  LOG(INFO) << "Hiding UI";
}

void ContentWindow::AddHostView(std::unique_ptr<QWidget> host_view) {
  QLayout* layout = main_widget_->layout();
  assert(layout != nullptr);
  layout->addWidget(host_view.release());
}

void ContentWindow::RemoveHostView(int index) {
  QLayout* layout = main_widget_->layout();
  assert(layout != nullptr);
  layout->removeItem(layout->itemAt(index));
}

int ContentWindow::GetHostsViewsCount() const {
  QLayout* layout = main_widget_->layout();
  assert(layout != nullptr);
  return layout->count();
}

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

  assert(delegate_);
  delegate_->OnClosed();
}

QWidget* ContentWindow::GetHostViewForTests(int index) const {
  QLayout* layout = main_widget_->layout();
  assert(layout != nullptr);
  return layout->itemAt(index)->widget();
}

}  // namespace reclip
