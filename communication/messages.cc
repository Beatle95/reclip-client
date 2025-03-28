#include "communication/messages.h"

#include <cassert>

namespace reclip {
namespace {
uint64_t GenerateId() {
  static uint64_t id_counter = 0;
  return id_counter++;
}
}  // namespace

SerializedMessage SerializeSyncRequest() {
  NetworkHeader data{sizeof(NetworkHeader) - sizeof(NetworkHeader::length),
                     GenerateId(),
                     static_cast<uint16_t>(MessageType::kHostSyncRequest)};
  return {QByteArray(reinterpret_cast<const char*>(&data), sizeof(data)),
          data.id};
}

SerializedMessage SerializeHostSencRequest(const HostId& id) {
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

std::optional<NetworkHeader> ParseNetworkHeader(const QByteArray& data) {
  if (static_cast<size_t>(data.size()) < sizeof(NetworkHeader)) {
    return {};
  }
  NetworkHeader header;
  std::memcpy(reinterpret_cast<char*>(&header), data.data(), sizeof(header));
  assert(header.length == data.size() - sizeof(header.length));
  return header;
}

std::optional<std::tuple<HostData, std::vector<HostData>>> ParseSyncResponse(
    const QByteArray& data) {
  // TODO:
  (void)data;
  return {};
}

std::optional<HostId> ParseHostId(const QByteArray& data) {
  // TODO:
  (void)data;
  return {};
}

std::optional<HostData> ParseHostData(const QByteArray& data) {
  // TODO:
  (void)data;
  return {};
}

std::optional<std::tuple<HostId, std::string>> ParseNewText(
    const QByteArray& data) {
  // TODO:
  (void)data;
  return {};
}

}  // namespace reclip
