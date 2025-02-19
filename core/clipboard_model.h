#pragma once
#include <vector>

#include "base/host_types.h"
#include "base/observers_list.h"
#include "core/client_data.h"
#include "core/clipboard_observer.h"
#include "core/server.h"

namespace reclip {

class ClipboardModelObserver : public CheckedObserver {
 public:
  virtual ~ClipboardModelObserver() = default;
  virtual void OnItemPushed(size_t host_index) = 0;
  virtual void OnItemPoped(size_t host_index) = 0;
  virtual void OnHostAdded(size_t host_index) = 0;
  virtual void OnModelReset() = 0;
};

class ClipboardModel : public SimpleObservable<ClipboardModelObserver>,
                       public ClipboardObserver,
                       public Server::Delegate {
 public:
  ClipboardModel();
  virtual ~ClipboardModel() = default;
  void SyncHosts(ClipboardData this_host_data, std::vector<HostData> data);

  // ClipboardObserver overrides
  void OnTextUpdated(const std::string& value) override;

  // Server::Delegate overrides
  void ProcessNewHost(const HostId& id, const std::string& name) override;
  bool ProcessNewText(const HostId& id, const std::string& text) override;

  size_t GetHostsCount() const;
  const HostData& GetHostData(size_t host_index) const;

 private:
  void ProcessNewTextImpl(size_t index, const std::string& text);

  std::vector<HostData> hosts_;
};

}  // namespace reclip
