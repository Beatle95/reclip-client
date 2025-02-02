#pragma once
#include <QString>
#include <QWidget>

class QLabel;
class QWidget;

namespace reclip {

class HostClipboardView : public QWidget {
  Q_OBJECT
 public:
  HostClipboardView(const QString& visible_name);
  void PushTop(const QString& text);
  void PopBottom();

 private slots:
  void ItemClicked();

 signals:
  void ElementClicked(uint32_t index);

 private:
  QWidget* content_;
};

}  // namespace reclip
