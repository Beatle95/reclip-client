#include "communication/serialization.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <cassert>

namespace reclip {

namespace {

std::optional<HostData> ParseHost(const QJsonObject& obj) {
  (void)obj;
  return HostData{};
}

SerializationTestHelper* g_test_helper = nullptr;

}  // namespace

SerializationTestHelper::SerializationTestHelper() {
  assert(g_test_helper == nullptr);
  g_test_helper = this;
}

SerializationTestHelper::~SerializationTestHelper() {
  assert(g_test_helper == this);
  g_test_helper = nullptr;
}

QByteArray SerializeHostSyncRequest(const HostId& id) {
  // TODO:
  (void)id;
  return {};
}

QByteArray SerializeTextUpdate(const std::string&) {
  // TODO:
  return {};
}

QByteArray SerializeThisHostSync(const HostData& data) {
  // TODO:
  (void)data;
  return {};
}

std::optional<SyncResponse> ParseSyncResponse(const QByteArray& data) {
  if (g_test_helper) {
    return g_test_helper->ParseSyncResponse(data);
  }

  auto doc = QJsonDocument::fromJson(data);
  if (doc.isEmpty()) {
    return {};
  }

  auto root = doc.object();
  if (root.isEmpty()) {
    return {};
  }

  auto this_host_it = root.find("ThisHostData");
  auto hosts_data_it = root.find("HostsData");
  if (this_host_it == root.end() || hosts_data_it == root.end()) {
    return {};
  }

  if (!this_host_it->isObject() || !hosts_data_it->isArray()) {
    return {};
  }

  auto this_host = ParseHost(this_host_it->toObject());
  if (!this_host) {
    return {};
  }
  std::vector<HostData> hosts;
  for (auto obj : hosts_data_it->toArray()) {
    if (!obj.isObject()) {
      return {};
    }
    auto host_obj = ParseHost(obj.toObject());
    if (!host_obj) {
      return {};
    }
    hosts.push_back(std::move(*host_obj));
  }

  return SyncResponse{std::move(*this_host), std::move(hosts)};
}

std::optional<HostId> ParseHostId(const QByteArray& data) {
  if (g_test_helper) {
    return g_test_helper->ParseHostId(data);
  }
  // TODO:
  return {};
}

std::optional<HostData> ParseHostData(const QByteArray& data) {
  if (g_test_helper) {
    return g_test_helper->ParseHostData(data);
  }
  // TODO:
  return {};
}

std::optional<TextUpdateResponse> ParseNewText(const QByteArray& data) {
  if (g_test_helper) {
    return g_test_helper->ParseNewText(data);
  }
  // TODO:
  return {};
}

}  // namespace reclip
