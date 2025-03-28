#pragma once
#include <cstdint>

namespace reclip {

// WARNING: This enums values must be synchronized with server implementation.
enum class ClientMessageType : uint16_t {
  kClientsResponse = 0,  // Right now it is not used.
  kFullSyncRequest,
  kHostSyncRequest,
  kHostTextUpdate,
  kSyncThisHost,
};

enum class ServerMessageType {
  kServerResponse = 256,
  kHostConnected,
  kHostDisconnected,
  kTextUpdate,
  kHostSynced,
};

}  // namespace reclip
