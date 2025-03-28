#pragma once
#include <QMainWindow>
#include <cstdint>
#include <vector>

class QVBoxLayout;

namespace reclip {

class TextView;
class HostClipboardView;

class ContentWindow : public QMainWindow {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void OnItemClicked(uint32_t host_index, uint32_t item_index) = 0;
  };

  ~ContentWindow();
  explicit ContentWindow(Delegate* delegate);
  void PushThisHostText(const QString& text);
  void PopThisHostText();

 private slots:
  void HostItemClicked(uint32_t element_index);

 private:
  Delegate* delegate_ = nullptr;
  QWidget* main_widget_ = nullptr;
  std::vector<std::unique_ptr<HostClipboardView>> host_views_;
};

}  // namespace reclip
