#pragma once
#include <QWidget>

class QLabel;

namespace reclip {

class TextView : public QWidget {
  Q_OBJECT
 public:
  TextView(const QString& visible_text, QWidget* parent);

 protected:
  void mouseReleaseEvent(QMouseEvent* event);

 signals:
  void Clicked();

 private:
  QLabel* label_;
};

}  // namespace reclip
