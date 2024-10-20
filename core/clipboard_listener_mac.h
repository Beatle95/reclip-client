#pragma once
#include <QClipboard>
#include <QObject>
#include <QTimer>
#include <memory>
#include <vector>

#include "core/clipboard_listener.h"

namespace reclip {

class ClipboardListenerMac : public QObject, public ClipboardListener {
  Q_OBJECT
 public:
  ClipboardListenerMac();
  void Start() override;
  void Stop() override;
  void AddObserver(ClipboardObserver& observer) override;
  void RemoveObserver(ClipboardObserver& observer) override;

 private slots:
  void CheckTimerTimeout();

 private:
  std::vector<ClipboardObserver*> observers_;
  QClipboard* clipboard_;
  QTimer check_timer_;
  QString prev_text_;
};

}  // namespace reclip
