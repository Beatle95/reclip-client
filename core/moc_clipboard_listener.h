#include <memory>
#include <string_view>
#include <vector>

#include "core/clipboard_listener.h"
#include "core/clipboard_observer.h"

namespace reclip {

class MocClipboardListener : public ClipboardListener {
  public:
    void EmulateTextCopy(std::string_view value);
    void AddObserver(
        const std::shared_ptr<ClipboardObserver>& observer) override;
    void RemoveObserver(
        const std::shared_ptr<ClipboardObserver>& observer) override;

  private:
    std::vector<std::shared_ptr<ClipboardObserver>> observers_;
};

}  // namespace reclip
