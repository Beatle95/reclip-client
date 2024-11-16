#pragma once
#include <QWidget>

class QLabel;

namespace reclip {

class ContentWindow;

class TextView : public QWidget {
  Q_OBJECT
 public:
  TextView(const QString& visible_text, ContentWindow* parent);

 protected:
  void mouseReleaseEvent(QMouseEvent *event);

 private:
  ContentWindow* parent_;
  QLabel* label_;
};

}  // namespace reclip
