#pragma once
#include <QClipboard>
#include <QObject>
#include <memory>
#include <vector>

#include "core/clipboard.h"

namespace reclip {

class ClipboardMac : public QObject, public Clipboard {
  Q_OBJECT
 public:
  ClipboardListenerQt();
  void WriteText(const std::string& text) override;
  void AddObserver(ClipboardObserver* observer) override;
  void RemoveObserver(ClipboardObserver* observer) override;

 private slots:
  void ClipboardTextChanged();

 private:
  std::vector<ClipboardObserver*> observers_;
  QClipboard* clipboard_;
};

}  // namespace reclip
