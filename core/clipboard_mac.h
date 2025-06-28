#pragma once
#include <QClipboard>
#include <QObject>
#include <QTimer>

#include "core/clipboard.h"

namespace reclip {

class ClipboardMac : public QObject, public Clipboard {
  Q_OBJECT
 public:
  ClipboardMac();
  void WriteText(const std::string& text) override;
  void Start() override;
  void Stop() override;

 private slots:
  void CheckTimerTimeout();

 private:
  QClipboard* clipboard_;
  QTimer check_timer_;
  QString prev_text_;
};

}  // namespace reclip
