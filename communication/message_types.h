#pragma once
#include <cstdint>

namespace reclip {

// WARNING: This enums values must be synchronized with server implementation.
enum class ClientMessageType : uint16_t {
  kClientResponse = 0,  // Right now it is not used.
  kFullSyncRequest,
  kHostSyncRequest,
  kHostTextUpdate,
  kSyncThisHost,
  kMax = kSyncThisHost
};

enum class ServerMessageType : uint16_t {
  kServerResponse = 256,
  kHostConnected,
  kHostDisconnected,
  kTextUpdate,
  kHostSynced,
  kMax = kHostSynced
};

}  // namespace reclip
