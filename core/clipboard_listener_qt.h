#pragma once
#include <QClipboard>
#include <QObject>
#include <memory>
#include <vector>

#include "core/clipboard_listener.h"

namespace reclip {

class ClipboardListenerMac : public QObject, public ClipboardListener {
  Q_OBJECT
 public:
  ClipboardListenerQt();
  void AddObserver(ClipboardObserver& observer) override;
  void RemoveObserver(ClipboardObserver& observer) override;

 private slots:
  void ClipboardTextChanged();

 private:
  std::vector<ClipboardObserver*> observers_;
  QClipboard* clipboard_;
};

}  // namespace reclip
