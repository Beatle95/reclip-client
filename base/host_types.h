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
  std::string visible_name;
  ClipboardData data;
};

}  // namespace reclip
