#pragma once
#include <string>
#include <string_view>

namespace reclip {

class Preferences {
 public:
  ~Preferences() = default;
  static Preferences& GetInstance();

  // Main preferences.
  void SetHostId(std::string_view id);
  const std::string& GetHostId() const;

 private:
  Preferences() = default;
  std::string id_;
};

}  // namespace reclip
