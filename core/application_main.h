#pragma once
#include <QCoreApplication>

namespace reclip {

class ClipboardListener;

class ApplicationMain : public QCoreApplication {
  public:
    ApplicationMain(int argc, char** argv);
    ~ApplicationMain();

  private:
    std::unique_ptr<ClipboardListener> clipboard_listener_;
};

}  // namespace reclip
