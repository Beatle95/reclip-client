#include "ui/text_view.h"

#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>

namespace reclip {

TextView::TextView(const QString& visible_text, QWidget* parent)
    : QWidget(parent) {
  setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
  setObjectName("TextView");

  auto* layout = new QHBoxLayout(this);
  label_ = new QLabel(parent);
  label_->setText(visible_text);
  label_->setWordWrap(false);
  label_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  label_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  layout->addWidget(label_);
}

void TextView::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    emit Clicked();
  }
}

}  // namespace reclip
