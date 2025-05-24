#include "communication/message_reassembler.h"

#include "base/byte_swap.h"

namespace reclip {

void MessageReassembler::Process(QByteArray& new_data) {
  for (size_t i = 0; i < static_cast<size_t>(new_data.size()); ++i) {
    // Here must not be a lot of ussless allocations, because QByteArray is
    // COW.
    QByteArray cur_data = new_data.mid(i);
    QByteArray msg = ProcessImpl(cur_data);
    if (!msg.isEmpty()) {
      messages_.push(std::move(msg));
    } else {
      data_.append(std::move(cur_data));
      return;
    }
  }
}

bool MessageReassembler::HasMessage() const { return !messages_.empty(); }

QByteArray MessageReassembler::GetMessage() {
  assert(!messages_.empty());
  auto mes = std::move(messages_.front());
  messages_.pop();
  return mes;
}

QByteArray MessageReassembler::ProcessImpl(QByteArray data) {
  if (!size_.has_value()) {
    if (data_.size() + data.size() < static_cast<int64_t>(sizeof(uint64_t))) {
      return {};
    }
    size_ = ntoh(ConsumeSizeValue(data));
  }

  if (data.size() < static_cast<int64_t>(*size_)) {
    return {};
  }

  QByteArray result = data.mid(*size_);
  size_.reset();
  return result;
}

uint64_t MessageReassembler::ConsumeSizeValue(QByteArray& data) {
  assert(data_.size() < static_cast<int64_t>(sizeof(uint64_t)));
  const int bytes_from_new_data =
      static_cast<int64_t>(sizeof(uint64_t)) - data_.size();
  std::array<char, sizeof(uint64_t)> size_arr;
  std::ranges::copy(data_.begin(), data_.end(), size_arr.begin());
  std::ranges::copy(data.begin(), data.begin() + bytes_from_new_data,
                    size_arr.begin() + data_.size());

  data = data.mid(bytes_from_new_data);
  data_.clear();
  return *reinterpret_cast<uint64_t*>(size_arr.data());
}

}  // namespace reclip
