#pragma once
#include <QString>
#include <QWidget>

class QLabel;
class QWidget;

namespace reclip {

class HostClipboardView : public QWidget {
  Q_OBJECT
 public:
  explicit HostClipboardView(const QString& visible_name = {});
  void SetName(const QString& visible_name);
  
  void PushTop(const QString& text);
  void PopBottom();
  void Clear();

 private slots:
  void ItemClicked();

 signals:
  void ElementClicked(uint32_t index);

 private:
  QWidget* content_;
  QLabel* name_;
};

}  // namespace reclip
