#pragma once
#include <string>
#include <string_view>

#include "base/host_types.h"

namespace reclip {

class Preferences {
 public:
  ~Preferences() = default;
  static Preferences& GetInstance();

  // Main preferences.
  void SetHostSecret(const HostId& id);
  const HostId& GetHostSecret() const;

 private:
  Preferences() = default;
  HostId id_;
};

}  // namespace reclip
