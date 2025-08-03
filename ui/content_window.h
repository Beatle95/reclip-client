#pragma once

#include <QMainWindow>
#include <memory>

class QVBoxLayout;

namespace reclip {

class TextView;
class HostClipboardView;

class ContentWindow : public QMainWindow {
  Q_OBJECT
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void OnClosed() = 0;
  };

  ~ContentWindow();
  explicit ContentWindow(Delegate* delegate);

  void AddHostView(std::unique_ptr<QWidget> host_view);
  void RemoveHostView(int index);
  int GetHostsViewsCount() const;

  void showEvent(QShowEvent *event) override;
  void closeEvent(QCloseEvent* event) override;

  QWidget* GetHostViewForTests(int index) const;

 private:
  Delegate* delegate_ = nullptr;
  QWidget* main_widget_ = nullptr;
};

}  // namespace reclip
