#pragma once
#include <string>
#include <deque>

namespace reclip {

struct HostData {
  using TextQueue = std::deque<std::string>;
  std::string id;
  std::string visible_name;
  TextQueue text_data;
};

}  // namespace reclip
