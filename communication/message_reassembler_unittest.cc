#include "communication/message_reassembler.h"

#include <gtest/gtest.h>

#include <limits>

#include "base/byte_swap.h"

using namespace reclip;

namespace {

struct TestData {
  QByteArray data_and_len;
  QByteArray data_only;
};

TestData CreateTestBuffer(uint64_t desired_data_len) {
  QByteArray full_data;
  full_data.resize(desired_data_len + sizeof(uint64_t));
  const uint64_t len = hton(desired_data_len);
  std::memcpy(full_data.data(), reinterpret_cast<const char*>(&len),
              sizeof(len));

  char ch = 0;
  for (int64_t i = sizeof(len); i < full_data.size(); ++i) {
    full_data[i] = ch++;
    if (ch == std::numeric_limits<char>::max()) {
      ch = 0;
    }
  }

  TestData result;
  result.data_only = full_data.mid(sizeof(uint64_t));
  result.data_and_len = std::move(full_data);
  return result;
}

}  // namespace

TEST(MessageReassembler, CorrectMessages) {
  auto test_data = CreateTestBuffer(2048);
  MessageReassembler assembler;

  assembler.Process(test_data.data_and_len);
  ASSERT_TRUE(assembler.HasMessage());
  const auto result_msg = assembler.PopMessage();
  ASSERT_FALSE(assembler.HasMessage());
  EXPECT_EQ(result_msg, test_data.data_only);

  for (int64_t step_size = 1; step_size < 128; step_size += 8) {
    int64_t offset = 0;
    for (; offset + step_size < test_data.data_and_len.size();
         offset += step_size) {
      auto data = test_data.data_and_len.mid(offset, step_size);
      assembler.Process(data);
    }
    auto data_left = test_data.data_and_len.mid(offset);
    assembler.Process(data_left);

    ASSERT_TRUE(assembler.HasMessage());
    const auto result_msg = assembler.PopMessage();
    ASSERT_FALSE(assembler.HasMessage());
    EXPECT_EQ(result_msg, test_data.data_only) << "Step was: " << step_size;
  }

  constexpr int messages_count = 10;
  for (int i = 0; i < messages_count; ++i) {
    int64_t offset = 0;
    for (; offset < test_data.data_and_len.size(); ++offset) {
      assembler.Process(test_data.data_and_len.mid(offset, 1));
    }
  }
  for (int i = 0; i < messages_count; ++i) {
    ASSERT_TRUE(assembler.HasMessage());
    const auto result_msg = assembler.PopMessage();
    EXPECT_EQ(result_msg, test_data.data_only);
  }
  ASSERT_FALSE(assembler.HasMessage());
}

// TODO: Tests for wrong input.
