#include "base/preferences.h"

#include <cassert>

namespace reclip {
Preferences& Preferences::GetInstance() {
  static Preferences instance;
  return instance;
}

void Preferences::SetHostSecret(const HostId& id) {
  assert(!id.empty());
  id_ = id;
}

const HostId& Preferences::GetHostSecret() const { return id_; }

}  // namespace reclip
