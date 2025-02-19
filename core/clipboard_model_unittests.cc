#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <format>
#include <memory>

#include "base/constants.h"
#include "base/preferences.h"
#include "base/scoped_observation.h"
#include "core/clipboard_model.h"

using namespace reclip;
using ::testing::_;

class MockClipboardModelObserver : public ClipboardModelObserver {
 public:
  MOCK_METHOD1(OnItemPushed, void(size_t));
  MOCK_METHOD1(OnItemPoped, void(size_t));
  MOCK_METHOD1(OnHostAdded, void(size_t));
  MOCK_METHOD0(OnModelReset, void());
};

class ClipboardModelTest : public ::testing::Test {
 public:
  ClipboardModelTest()
      : model_(std::make_unique<ClipboardModel>()),
        observer_(std::make_unique<MockClipboardModelObserver>()) {
    Preferences::GetInstance().SetHostSecret("host");
    observation_.Reset(*observer_, *model_);
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

TEST_F(ClipboardModelTest, HostTextUpdating) {
  EXPECT_EQ(model_->GetHostsCount(), 1);
  EXPECT_TRUE(GetTextData(0).empty());

  EXPECT_CALL(observer(), OnItemPoped(0)).Times(1);
  for (size_t i = 0; i < kClipboardSizeMax + 1; ++i) {
    EXPECT_CALL(observer(), OnItemPushed(0));
    model_->OnTextUpdated(std::format("text_{}", i));
    EXPECT_EQ(GetTextData(0).size(), std::min(i + 1, kClipboardSizeMax));
  }
  for (size_t i = 0; i < kClipboardSizeMax; ++i) {
    ASSERT_LT(i, GetTextData(0).size());
    EXPECT_EQ(GetTextData(0)[i], std::format("text_{}", kClipboardSizeMax - i))
        << "Index was: " << i;
  }
  EXPECT_EQ(model_->GetHostsCount(), 1);
}

TEST_F(ClipboardModelTest, ProcessRemoteTextUpdate) {
  constexpr auto kNewHostId = "new_host";

  {
    EXPECT_CALL(observer(), OnItemPushed(_)).Times(0);
    EXPECT_EQ(model_->GetHostsCount(), 1);
    EXPECT_TRUE(GetTextData(0).empty());
    model_->ProcessNewText(Preferences::GetInstance().GetHostSecret(), "text");
    // Must be rejected.
    EXPECT_TRUE(GetTextData(0).empty());
  }

  {
    EXPECT_CALL(observer(), OnItemPushed(_)).Times(0);
    model_->ProcessNewText(kNewHostId, "text");
    // Must be rejected.
    EXPECT_EQ(model_->GetHostsCount(), 1);
  }

  {
    EXPECT_CALL(observer(), OnHostAdded(_)).Times(1);
    model_->ProcessNewHost(kNewHostId, "");
    EXPECT_EQ(model_->GetHostsCount(), 2);
    EXPECT_TRUE(GetTextData(0).empty());
    EXPECT_TRUE(GetTextData(1).empty());
  }

  {
    EXPECT_CALL(observer(), OnItemPushed(1)).Times(1);
    model_->ProcessNewText(kNewHostId, "text");
    EXPECT_TRUE(GetTextData(0).empty());
    EXPECT_EQ(GetTextData(1).size(), 1u);
  }

  EXPECT_CALL(observer(), OnItemPoped(1)).Times(2);
  for (size_t i = 0; i < kClipboardSizeMax + 1; ++i) {
    EXPECT_CALL(observer(), OnItemPushed(1)).Times(1);
    model_->ProcessNewText(kNewHostId, std::format("text_{}", i));
    EXPECT_EQ(GetTextData(1).size(), std::min(i + 2, kClipboardSizeMax));
  }
  for (size_t i = 0; i < kClipboardSizeMax; ++i) {
    ASSERT_LT(i, GetTextData(1).size());
    EXPECT_EQ(GetTextData(1)[i], std::format("text_{}", kClipboardSizeMax - i))
        << "Index was: " << i;
  }
  // First must remain empty.
  EXPECT_TRUE(GetTextData(0).empty());
}

TEST_F(ClipboardModelTest, SyncRemoteTest) {
  // TODO:
}
