#pragma once
#include <QByteArray>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "core/host_types.h"

namespace reclip {

// WARNING: This enum values must be synchronized with server implementation.
enum class MessageType : uint16_t {
  kClientsResponseToServer = 0,  // Right now it is not used.
  kFullSyncRequest,
  kHostSyncRequest,
  kHostTextUpdate,
  kHostSyncData,

  kServerResponseToClient = 256,
  kServerHostConnected,
  kServerHostDisconnected,
  kServerTextUpdate,
  kServerHostSynced,
};

struct NetworkHeader {
  uint64_t length;  // Length of the message without len field.
  uint64_t id;      // Unique id of the message.
  uint16_t type;    // Value of MessageType enum.
};

struct SerializedMessage {
  QByteArray message;
  uint64_t id;
};

// Serialization function returns data to be sent and message id used for this
// message.
SerializedMessage SerializeSyncRequest();
SerializedMessage SerializeHostSencRequest(const HostId& id);
QByteArray SerializeTextUpdate(const std::string& text);
QByteArray SerializeThisHostSync(const HostData& data);

std::optional<NetworkHeader> ParseNetworkHeader(const QByteArray& data);
std::optional<std::tuple<HostData, std::vector<HostData>>> ParseSyncResponse(
    const QByteArray& data);
std::optional<HostId> ParseHostId(const QByteArray& data);
std::optional<HostData> ParseHostData(const QByteArray& data);
std::optional<std::tuple<HostId, std::string>> ParseNewText(
    const QByteArray& data);

}  // namespace reclip
