#pragma once
#include <deque>
#include <string>

namespace reclip {

using ClientId = uint64_t;

struct ClientData {
  ClientId client_id;
  std::string internal_name;
  std::string overriden_name;
  std::deque<std::string> clipboard_content;
};

}  // namespace reclip
