#pragma once
#include <QMainWindow>
#include <QTextEdit>

namespace reclip {

class ContentWindow : public QMainWindow {
 public:
  ContentWindow();

 private:
  // TODO:
  QTextEdit* text_edit_;
};

}  // namespace reclip
