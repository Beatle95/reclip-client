#pragma once
#include <QString>
#include <QWidget>

class QLabel;

namespace reclip {

class HostClipboardView : public QWidget {
  Q_OBJECT
 public:
  static void RegisterPrefs();

  explicit HostClipboardView(const QString& visible_name = {});
  void SetName(const QString& visible_name);

  void PushText(const QString& text);
  void PopText();
  void ClearText();
  size_t GetTextItemsCount() const;

 private slots:
  void ItemClicked();

 signals:
  void ElementClicked(int index);

 private:
  QWidget* content_;
  QLabel* name_;
};

}  // namespace reclip
