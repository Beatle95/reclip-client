#pragma once
#include <QByteArray>
#include <cassert>
#include <optional>
#include <queue>

namespace reclip {

class MessageReassembler {
 public:
  void Clear();
  void Process(const QByteArray& new_data);
  bool HasMessage() const;
  QByteArray PopMessage();

 private:
  size_t ProcessLen(const QByteArray& data);
  size_t ProcessMsg(const QByteArray& data);

  std::queue<QByteArray> messages_;
  QByteArray data_leftover_;
  std::optional<uint64_t> size_;
};

}  // namespace reclip
