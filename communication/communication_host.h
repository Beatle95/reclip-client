#pragma once
#include <memory>

#include "base/scoped_observation.h"
#include "communication/server.h"
#include "core/clipboard_model.h"

namespace reclip {

class CommunicationHost : public ServerDelegate, public ClipboardModelObserver {
 public:
  explicit CommunicationHost(ClipboardModel& model);
  ~CommunicationHost() override = default;

  // ServerDelegate overrides:
  void OnFullSync(HostData, std::vector<HostData>) override;
  void HostConnected(const HostId& id) override;
  void HostDisconnected(const HostId& id) override;
  void HostTextAdded(const HostId& id, const std::string& text) override;
  void HostSynced(HostData data) override;

  // ClipboardModelObserver overrides:
  void OnThisTextPushed() override;
  void OnThisTextPoped() override {}
  void OnHostUpdated(size_t) override {}
  void OnTextPushed(size_t) override {}
  void OnTextPoped(size_t) override {}
  void OnThisHostDataReset() override {}
  void OnHostsDataReset() override {}

 private:
  void OnHostSynced(std::optional<HostData> data);

  ClipboardModel* model_;
  std::unique_ptr<Server> server_;
  ScopedObservation<CommunicationHost, ClipboardModel> model_observation_;
};

}  // namespace reclip
