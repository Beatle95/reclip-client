#pragma once
#include <QByteArray>
#include <QString>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "core/host_types.h"

namespace reclip {

struct Version {
  uint32_t major;
  uint32_t minor;
  uint32_t build_number;
};

struct IntroductionResponse {
  Version server_version;
  QString error;
  bool success;
};

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

  virtual IntroductionResponse ParseIntroductionResponse(
      const QByteArray& data) = 0;
  virtual std::optional<SyncResponse> ParseSyncResponse(
      const QByteArray& data) = 0;
  virtual std::optional<HostId> ParseHostId(const QByteArray& data) = 0;
  virtual std::optional<HostData> ParseHostData(const QByteArray& data) = 0;
  virtual std::optional<TextUpdateResponse> ParseNewText(
      const QByteArray& data) = 0;
};

QByteArray SerializeIntroduction(const HostSecret& id);
QByteArray SerializeHostSyncRequest(const HostId& id);
QByteArray SerializeTextUpdate(const std::string& text);
QByteArray SerializeHostSync(const HostData& data);

IntroductionResponse ParseIntroductionResponse(const QByteArray& data);
std::optional<SyncResponse> ParseSyncResponse(const QByteArray& data);
std::optional<HostId> ParseHostId(const QByteArray& data);
std::optional<HostData> ParseHostData(const QByteArray& data);
std::optional<TextUpdateResponse> ParseNewText(const QByteArray& data);

}  // namespace reclip
