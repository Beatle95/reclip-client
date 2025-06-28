#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <format>
#include <memory>

import base.constants;
import base.observer_helper;
import core.clipboard_model;
import core.host_types;

using namespace reclip;
using ::testing::_;

class MockClipboardModelObserver : public ClipboardModelObserver {
 public:
  MOCK_METHOD0(OnThisTextPushed, void());
  MOCK_METHOD0(OnThisTextPoped, void());
  MOCK_METHOD1(OnHostUpdated, void(size_t));
  MOCK_METHOD1(OnTextPushed, void(size_t));
  MOCK_METHOD1(OnTextPoped, void(size_t));
  MOCK_METHOD0(OnThisHostDataReset, void());
  MOCK_METHOD0(OnHostsDataReset, void());
};

class ClipboardModelTest : public ::testing::Test {
 public:
  ClipboardModelTest()
      : model_(std::make_unique<ClipboardModel>()),
        observer_(std::make_unique<MockClipboardModelObserver>()) {
    observation_.Reset(*observer_, *model_);
  }

  auto GetThisTextData() {
    return model_->GetThisHostData().data.text;
  }

  auto GetTextData(size_t host_index) {
    assert(host_index < model_->GetHostsCount());
    return model_->GetHostData(host_index).data.text;
  }

  MockClipboardModelObserver& observer() { return *observer_; }

 protected:
  std::unique_ptr<ClipboardModel> model_;
  std::unique_ptr<MockClipboardModelObserver> observer_;
  ScopedObservation<MockClipboardModelObserver, ClipboardModel> observation_;
};

TEST_F(ClipboardModelTest, ThisHostTextUpdating) {
  EXPECT_TRUE(GetThisTextData().empty());
  EXPECT_CALL(observer(), OnThisTextPushed()).Times(kClipboardSizeMax + 1);
  EXPECT_CALL(observer(), OnThisTextPoped()).Times(1);
  for (size_t i = 0; i < kClipboardSizeMax + 1; ++i) {
    model_->OnTextUpdated(std::format("text_{}", i));
    EXPECT_EQ(GetThisTextData().size(), std::min(i + 1, kClipboardSizeMax));
  }
  for (size_t i = 0; i < kClipboardSizeMax; ++i) {
    ASSERT_LT(i, GetThisTextData().size());
    EXPECT_EQ(GetThisTextData()[i], std::format("text_{}", kClipboardSizeMax - i))
        << "Index was: " << i;
  }
  EXPECT_EQ(model_->GetHostsCount(), 0);
}

TEST_F(ClipboardModelTest, ProcessRemoteHostTextUpdate) {
  constexpr HostId kNewHostId("new_host");
  {
    EXPECT_TRUE(GetThisTextData().empty());
    EXPECT_EQ(model_->GetHostsCount(), 0);
    EXPECT_FALSE(model_->AddHostText(kNewHostId, "text"));
    EXPECT_TRUE(GetThisTextData().empty());
  }
  {
    EXPECT_CALL(observer(), OnHostUpdated(_)).Times(1);
    model_->SetHostData(HostData{.id = kNewHostId, .name = {}, .data = {}});
    EXPECT_EQ(model_->GetHostsCount(), 1);
    EXPECT_TRUE(GetThisTextData().empty());
    EXPECT_TRUE(GetTextData(0).empty());
  }
  {
    EXPECT_CALL(observer(), OnTextPushed(0)).Times(1);
    model_->AddHostText(kNewHostId, "text");
    EXPECT_TRUE(GetThisTextData().empty());
    EXPECT_EQ(GetTextData(0).size(), 1u);
  }

  EXPECT_CALL(observer(), OnTextPoped(0)).Times(2);
  for (size_t i = 0; i < kClipboardSizeMax + 1; ++i) {
    EXPECT_CALL(observer(), OnTextPushed(0)).Times(1);
    model_->AddHostText(kNewHostId, std::format("text_{}", i));
    EXPECT_EQ(GetTextData(0).size(), std::min(i + 2, kClipboardSizeMax));
  }

  for (size_t i = 0; i < kClipboardSizeMax; ++i) {
    ASSERT_LT(i, GetTextData(0).size());
    EXPECT_EQ(GetTextData(0)[i], std::format("text_{}", kClipboardSizeMax - i))
        << "Index was: " << i;
  }
  // First must remain empty.
  EXPECT_TRUE(GetThisTextData().empty());
}
