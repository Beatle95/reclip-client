#pragma once
#include <QByteArray>
#include <cassert>
#include <optional>
#include <queue>

namespace reclip {

class MessageReassembler {
 public:
  void Process(QByteArray& new_data);

  bool HasMessage() const;
  QByteArray GetMessage();

 private:
  QByteArray ProcessImpl(QByteArray data);
  uint64_t ConsumeSizeValue(QByteArray& data);

  std::queue<QByteArray> messages_;
  QByteArray data_;
  std::optional<uint64_t> size_;
};

}  // namespace reclip
