#pragma once
#include <QMainWindow>
#include <cstdint>

#include "core/client_data.h"

class QVBoxLayout;

namespace reclip {

class TextView;

class ContentWindow : public QMainWindow {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void OnItemClicked(uint32_t item_index) = 0;
  };

  explicit ContentWindow(Delegate* delegate);
  void PushFrontHostText(const QString& text);
  void PopBackHostText();
  void TextViewClicked(TextView* child);

 private:
  Delegate* delegate_ = nullptr;
  QWidget* main_widget_ = nullptr;
};

}  // namespace reclip
