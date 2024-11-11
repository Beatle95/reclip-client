#include "ui/text_view.h"

#include <QBoxLayout>
#include <QLabel>

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

}  // namespace reclip
