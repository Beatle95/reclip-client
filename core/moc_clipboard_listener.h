#include <memory>
#include <string>
#include <vector>

#include "core/clipboard.h"
#include "core/clipboard_observer.h"

namespace reclip {

// TODO: unused right now
#if 0
class MocClipboard : public Clipboard {
 public:
  void EmulateTextCopy(const std::string& value);
  void AddObserver(ClipboardObserver& observer) override;
  void RemoveObserver(ClipboardObserver& observer) override;

 private:
  std::vector<ClipboardObserver*> observers_;
};
#endif

}  // namespace reclip
