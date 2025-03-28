#pragma once
#include <memory>

#include "base/scoped_observation.h"
#include "communication/client_server_types.h"
#include "core/clipboard_model.h"

namespace reclip {

class CommunicationHost : public Client, public ClipboardModelObserver {
 public:
  explicit CommunicationHost(ClipboardModel& model);
  ~CommunicationHost() override = default;

  // Client overrides:
  void OnFullSync(HostData, std::vector<HostData>) override;
  void HostConnected(const HostId& id) override;
  void HostDisconnected(const HostId& id) override;
  void HostTextAdded(const HostId& id, const std::string& text) override;
  void HostSynced(HostData data) override;

  // ClipboardModelObserver overrides:
  void OnThisItemPushed() override;
  void OnThisItemPoped() override {}
  void OnHostUpdated(size_t) override {}
  void OnItemPushed(size_t) override {}
  void OnItemPoped(size_t) override {}
  void OnThisHostDataUpated() override {}
  void OnHostsDataUpdated() override {}

 private:
  void OnHostSynced(std::optional<HostData> data);

  ClipboardModel* model_;
  std::unique_ptr<Server> server_;
  ScopedObservation<CommunicationHost, ClipboardModel> model_observation_;
};

}  // namespace reclip
