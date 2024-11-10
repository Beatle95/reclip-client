#pragma once
#include <QMainWindow>
#include "core/client_data.h"

class QVBoxLayout;

namespace reclip {

class ContentWindow : public QMainWindow {
 public:
  ContentWindow();
  void AddHostText(const QString& text);

 private:
  QWidget* main_widget_ = nullptr;
  QVBoxLayout* layout_ = nullptr;
};

}  // namespace reclip
