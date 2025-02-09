#pragma once
#include <functional>
#include <string>
#include <vector>

#include "core/clipboard_observer.h"
#include "core/host_data.h"

namespace reclip {

class Server : public ClipboardObserver {
 public:
  using SyncCallback = std::function<void(std::vector<HostData> data)>;
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void ProcessNewHost(const std::string& id,
                                const std::string& name) = 0;
    virtual void ProcessNewText(const std::string& id,
                                const std::string& text) = 0;
  };

  explicit Server(Delegate& delegate);
  void OnTextUpdated(const std::string& value) override;

  void RequestFullSync(SyncCallback callback);

 private:
  Delegate* delegate_;
};

}  // namespace reclip
