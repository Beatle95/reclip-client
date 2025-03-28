#pragma once
#include <deque>
#include <string>

namespace reclip {

using HostId = std::string;
using HostSecret = std::string;
using ClipboardTextContainer = std::deque<std::string>;

// We will create this struct, so later we will populate it with files data.
struct ClipboardData {
  ClipboardTextContainer text;
};

struct HostData {
  HostId id;
  std::string name;
  ClipboardData data;
};

bool operator==(const ClipboardData& lhs, const ClipboardData& rhs);
bool operator!=(const ClipboardData& lhs, const ClipboardData& rhs);

bool operator==(const HostData& lhs, const HostData& rhs);
bool operator!=(const HostData& lhs, const HostData& rhs);

}  // namespace reclip
