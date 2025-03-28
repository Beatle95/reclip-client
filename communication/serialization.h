#pragma once
#include <QByteArray>
#include <optional>
#include <string>
#include <vector>

#include "core/host_types.h"

namespace reclip {

struct SyncResponse {
  HostData this_host_data;
  std::vector<HostData> hosts_data;
};

struct TextUpdateResponse {
  HostId id;
  std::string text;
};

class SerializationTestHelper {
 public:
  SerializationTestHelper();
  virtual ~SerializationTestHelper();

  virtual std::optional<SyncResponse> ParseSyncResponse(
      const QByteArray& data) = 0;
  virtual std::optional<HostId> ParseHostId(const QByteArray& data) = 0;
  virtual std::optional<HostData> ParseHostData(const QByteArray& data) = 0;
  virtual std::optional<TextUpdateResponse> ParseNewText(
      const QByteArray& data) = 0;
};

QByteArray SerializeHostSencRequest(const HostId& id);
QByteArray SerializeTextUpdate(const std::string& text);
QByteArray SerializeThisHostSync(const HostData& data);

std::optional<SyncResponse> ParseSyncResponse(const QByteArray& data);
std::optional<HostId> ParseHostId(const QByteArray& data);
std::optional<HostData> ParseHostData(const QByteArray& data);
std::optional<TextUpdateResponse> ParseNewText(const QByteArray& data);

}  // namespace reclip
