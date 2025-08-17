#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <deque>
#include <format>
#include <memory>

import base.constants;
import base.observer_helper;
import core.hosts_list_model;
import core.host_types;

using namespace reclip;
using ::testing::_;

class MockHostModelObserver : public HostModelObserver {
 public:
  MOCK_METHOD1(OnTextPushed, void(const std::string&));
  MOCK_METHOD0(OnTextPoped, void());
  MOCK_METHOD0(OnReset, void());
};

class ClipboardModelTest : public ::testing::Test {
 public:
  ClipboardModelTest() : model_(std::make_unique<HostsListModel>()) {}

  const std::deque<std::string>& GetLocalHostTextData() {
    return model_->GetLocalHost().GetText();
  }

  const std::deque<std::string>& GetRemoteHostTextData(HostPublicId id) {
    auto* host = model_->GetRemoteHost(id);
    if (!host) {
      throw std::runtime_error("Host not found");
    }
    return host->GetText();
  }

 protected:
  std::unique_ptr<HostsListModel> model_;
};

TEST_F(ClipboardModelTest, ThisHostTextUpdating) {
  MockHostModelObserver observer;
  ScopedObservation<MockHostModelObserver, HostModel> observation_;
  observation_.Reset(observer, model_->GetLocalHost());

  EXPECT_TRUE(GetLocalHostTextData().empty());
  EXPECT_CALL(observer, OnTextPushed(_)).Times(kClipboardSizeMax + 1);
  EXPECT_CALL(observer, OnTextPoped()).Times(1);
  for (size_t i = 0; i < kClipboardSizeMax + 1; ++i) {
    model_->OnTextUpdated(std::format("text_{}", i));
    EXPECT_EQ(GetLocalHostTextData().size(), std::min(i + 1, kClipboardSizeMax));
  }
  for (size_t i = 0; i < kClipboardSizeMax; ++i) {
    ASSERT_LT(i, GetLocalHostTextData().size());
    EXPECT_EQ(GetLocalHostTextData()[i], std::format("text_{}", kClipboardSizeMax - i))
        << "Index was: " << i;
  }
  EXPECT_EQ(model_->GetRemoteHostsCount(), 0);
}

TEST_F(ClipboardModelTest, ProcessRemoteHostTextUpdate) {
  constexpr auto kNewHostId = 200_pubid;
  EXPECT_TRUE(GetLocalHostTextData().empty());
  EXPECT_EQ(model_->GetRemoteHostsCount(), 0);

  model_->ResetLocalHostData(HostData{.id = kNewHostId, .name = {}, .data = {}});
  auto* new_host = model_->GetRemoteHost(kNewHostId);
  ASSERT_NE(new_host, nullptr);

  MockHostModelObserver observer;
  ScopedObservation<MockHostModelObserver, HostModel> observation_;
  observation_.Reset(observer, *new_host);

  {
    EXPECT_CALL(observer, OnTextPushed(_)).Times(1);
    new_host->PushText("text");
    EXPECT_TRUE(GetLocalHostTextData().empty());
  }
  {
    EXPECT_CALL(observer, OnReset()).Times(1);
    model_->ResetLocalHostData(HostData{.id = kNewHostId, .name = {}, .data = {}});
    EXPECT_EQ(model_->GetRemoteHostsCount(), 1);
    EXPECT_TRUE(GetLocalHostTextData().empty());
    EXPECT_TRUE(GetRemoteHostTextData(kNewHostId).empty());
  }
  {
    EXPECT_CALL(observer, OnTextPushed(_)).Times(1);
    new_host->PushText("text");
    EXPECT_TRUE(GetLocalHostTextData().empty());
    EXPECT_EQ(GetRemoteHostTextData(kNewHostId).size(), 1u);
  }

  EXPECT_CALL(observer, OnTextPoped()).Times(2);
  for (size_t i = 0; i < kClipboardSizeMax + 1; ++i) {
    EXPECT_CALL(observer, OnTextPushed(_)).Times(1);
    new_host->PushText(std::format("text_{}", i));
    EXPECT_EQ(GetRemoteHostTextData(kNewHostId).size(), std::min(i + 2, kClipboardSizeMax));
  }

  for (size_t i = 0; i < kClipboardSizeMax; ++i) {
    ASSERT_LT(i, GetRemoteHostTextData(kNewHostId).size());
    EXPECT_EQ(GetRemoteHostTextData(kNewHostId)[i], std::format("text_{}", kClipboardSizeMax - i))
        << "Index was: " << i;
  }
  // First must remain empty.
  EXPECT_TRUE(GetLocalHostTextData().empty());
}
