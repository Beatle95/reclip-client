#include "ui/text_view.h"

#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>

namespace reclip {

TextView::TextView(const QString& visible_text, QWidget* parent)
    : QWidget(parent) {
  auto* layout = new QHBoxLayout(this);
  label_ = new QLabel(parent);
  label_->setText(visible_text);
  layout->addWidget(label_);
  setMinimumHeight(50);
  setObjectName("TextView");
  setStyleSheet("border: 1px solid red;");
}

void TextView::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    emit Clicked();
  }
}

}  // namespace reclip
