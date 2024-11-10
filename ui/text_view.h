#pragma once
#include <QWidget>

class QLabel;

namespace reclip {

class TextView : public QWidget {
  Q_OBJECT
 public:
  TextView(const QString& visible_text, QWidget* parent);

 private:
  QLabel* label_;
};

}  // namespace reclip
