#include "communication/communication_host.h"

#include <cassert>

#include "base/log.h"
#include "communication/server_impl.h"
#include "core/clipboard_model.h"

using namespace std::placeholders;

namespace reclip {

CommunicationHost::CommunicationHost(ClipboardModel& model) : model_(&model) {
  server_ = std::make_unique<ServerImpl>(*this);
  model_observation_.Reset(*this, model);
}

void CommunicationHost::OnFullSync(HostData this_host_data,
                                   std::vector<HostData> data) {
  model_->ResetHostsData(std::move(data));
  if (!model_->AdoptThisHostData(std::move(this_host_data.name),
                                 std::move(this_host_data.data))) {
    server_->SyncThisHost(model_->GetThisHostData());
  }
}

void CommunicationHost::HostConnected(const HostId& id) {
  if (!model_->IsHostExists(id)) {
    server_->RequestHostSync(
        id, std::bind(&CommunicationHost::OnHostSynced, this, _1));
  }
}

void CommunicationHost::HostDisconnected(const HostId& id) {
  (void)id;
  // Right now it is not used, but may be used to display state of the host.
}

void CommunicationHost::HostTextAdded(const HostId& id,
                                      const std::string& text) {
  if (!model_->AddHostText(id, text)) {
    server_->RequestHostSync(
        id, std::bind(&CommunicationHost::OnHostSynced, this, _1));
  }
}

void CommunicationHost::HostSynced(HostData data) {
  model_->SetHostData(std::move(data));
}

void CommunicationHost::OnThisTextPushed() {
  const auto& text_data = model_->GetThisHostData().data.text;
  assert(!text_data.empty());
  server_->AddThisHostText(text_data.front());
}

void CommunicationHost::OnHostSynced(std::optional<HostData> data) {
  if (data) {
    HostSynced(std::move(data.value()));
  } else {
    LOG(ERROR) << "Unable to sync host";
  }
}

}  // namespace reclip
