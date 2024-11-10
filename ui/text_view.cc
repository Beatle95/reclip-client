#include "ui/text_view.h"

#include <QLabel>

namespace reclip {

TextView::TextView(const QString& visible_text, QWidget* parent)
    : QWidget(parent) {
  label_ = new QLabel(parent);
  label_->setText(visible_text);
}

}  // namespace reclip
