#include "base/preferences.h"

#include <cassert>

namespace reclip {
Preferences& Preferences::GetInstance() {
  static Preferences instance;
  return instance;
}

void Preferences::SetHostId(std::string_view id) {
  assert(!id.empty());
  id_ = id;
}

const std::string& Preferences::GetHostId() const { return id_; }

}  // namespace reclip
