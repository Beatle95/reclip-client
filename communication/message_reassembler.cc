#include "communication/message_reassembler.h"

#include "base/byte_swap.h"
#include "base/log.h"

constexpr uint64_t kMaxMessageSize = 1 * 1024 * 1024 * 1024;

namespace reclip {

void MessageReassembler::Process(const QByteArray& new_data) {
  for (size_t i = 0; i < static_cast<size_t>(new_data.size());) {
    // Here must not be a lot of ussless allocations, because QByteArray is
    // COW.
    QByteArray data_left_to_process = new_data.mid(i);
    size_t bytes_processed = size_.has_value()
                                 ? ProcessMsg(data_left_to_process)
                                 : ProcessLen(data_left_to_process);
    if (bytes_processed == 0) {
      // TODO: Keep track of allocated memory here too.
      data_leftover_.append(std::move(data_left_to_process));
      return;
    }
    i += bytes_processed;
  }
}

bool MessageReassembler::HasMessage() const { return !messages_.empty(); }

QByteArray MessageReassembler::PopMessage() {
  assert(!messages_.empty());
  auto mes = std::move(messages_.front());
  messages_.pop();
  return mes;
}

size_t MessageReassembler::ProcessLen(const QByteArray& data) {
  if (data_leftover_.size() + data.size() <
      static_cast<int64_t>(sizeof(uint64_t))) {
    return 0;
  }

  assert(data_leftover_.size() < static_cast<int64_t>(sizeof(uint64_t)));
  const int bytes_from_new_data =
      static_cast<int64_t>(sizeof(uint64_t)) - data_leftover_.size();
  std::array<char, sizeof(uint64_t)> size_arr;
  std::ranges::copy(data_leftover_.begin(), data_leftover_.end(),
                    size_arr.begin());
  std::ranges::copy(data.cbegin(), data.cbegin() + bytes_from_new_data,
                    size_arr.begin() + data_leftover_.size());

  data_leftover_.clear();
  size_ = ntoh(*reinterpret_cast<uint64_t*>(size_arr.data()));
  if (size_ > kMaxMessageSize) {
    // TODO: Enter broken stream state.
    assert(false);
  }

  return sizeof(uint64_t);
}

size_t MessageReassembler::ProcessMsg(const QByteArray& data) {
  const auto cur_size = static_cast<int64_t>(*size_);
  if (data_leftover_.size() + data.size() < cur_size) {
    return 0;
  }

  assert(data_leftover_.size() < cur_size);
  QByteArray result;
  std::swap(result, data_leftover_);
  size_.reset();

  size_t data_to_process = cur_size - result.size();
  result.append(data.mid(0, data_to_process));
  // TODO: Keep track of full allocated memory in messages.
  messages_.push(std::move(result));
  return data_to_process;
}

}  // namespace reclip
