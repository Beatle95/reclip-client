#include "core/host_types.h"

namespace reclip {

bool operator==(const ClipboardData& lhs, const ClipboardData& rhs) {
  return lhs.text == rhs.text;
}

bool operator!=(const ClipboardData& lhs, const ClipboardData& rhs) {
  return lhs.text != rhs.text;
}

bool operator==(const HostData& lhs, const HostData& rhs) {
  return lhs.id == rhs.id && lhs.name == rhs.name && lhs.data == rhs.data;
}

bool operator!=(const HostData& lhs, const HostData& rhs) {
  return !operator==(lhs, rhs);
}

}  // namespace reclip
