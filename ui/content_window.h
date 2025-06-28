#pragma once
#include <QMainWindow>
#include <cstdint>
#include <vector>

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
    virtual void OnItemClicked(uint32_t host_index, uint32_t item_index) = 0;
  };

  ~ContentWindow();
  explicit ContentWindow(Delegate* delegate);

  void RemoveHostViews(uint32_t start_index = 0);
  HostClipboardView* AddHostView(const QString& name = {});
  HostClipboardView* GetHostView(uint32_t index);
  size_t HostsCount() const;

 private slots:
  void HostItemClicked(uint32_t element_index);

 private:
  Delegate* delegate_ = nullptr;
  QWidget* main_widget_ = nullptr;
  std::vector<std::unique_ptr<HostClipboardView>> host_views_;
};

}  // namespace reclip
