#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QApplication>

#include "ui/host_clipboard_view.h"

import base.preferences;
import core.clipboard;
import core.clipboard_model;
import core.host_types;
import ui.clipboard_controller;

using namespace reclip;

namespace {
int argc = 0;
char** argv = nullptr;
}  // namespace

class MockClipboard : public Clipboard {
 public:
  ~MockClipboard() override = default;
  MOCK_METHOD1(WriteText, void(const std::string&));
  MOCK_METHOD0(Start, void());
  MOCK_METHOD0(Stop, void());
};

class ModelControllerIntegrationTest : public ::testing::Test {
 public:
  ModelControllerIntegrationTest()
      : app_(argc, argv),
        preferences_guard_(Preferences::InitForTesting()),
        model_(std::make_unique<ClipboardModel>()),
        controller_(std::make_unique<ClipboardController>(model_.get(), &mock_clipboard_)) {
    model_->AddObserver(controller_.get());
  }

  ~ModelControllerIntegrationTest() { model_->RemoveObserver(controller_.get()); }

  MockClipboard& clipboard() { return mock_clipboard_; }
  ClipboardModel& model() { return *model_; }
  ClipboardController& controller() { return *controller_; }

 private:
  QApplication app_;
  MockPreferencesGuard preferences_guard_;
  MockClipboard mock_clipboard_;
  std::unique_ptr<ClipboardModel> model_;
  std::unique_ptr<ClipboardController> controller_;
};

TEST_F(ModelControllerIntegrationTest, ModelReactionTests) {
  EXPECT_EQ(controller().GetContentWindowForTests(), nullptr);
  controller().ShowUi();
  auto* window = controller().GetContentWindowForTests();
  ASSERT_NE(window, nullptr);
  ASSERT_EQ(window->HostsCount(), 1);

  EXPECT_TRUE(model().AdoptThisHostData(0_pubid, "name", {.text = {"text1", "text2"}}));
  ASSERT_EQ(window->HostsCount(), 1);
  EXPECT_EQ(window->GetHostView(0)->GetTextItemsCount(), 2u);

  model().ResetHostsData({
      HostData{.id = 1_pubid, .name = "host1", .data = ClipboardData{.text = {"text0"}}},
      HostData{.id = 2_pubid, .name = "host2", .data = ClipboardData{.text = {"text1", "text2"}}},
  });
  ASSERT_EQ(window->HostsCount(), 3);
  EXPECT_EQ(window->GetHostView(0)->GetTextItemsCount(), 2u);
  EXPECT_EQ(window->GetHostView(1)->GetTextItemsCount(), 1u);
  EXPECT_EQ(window->GetHostView(2)->GetTextItemsCount(), 2u);

  model().ResetHostsData({});
  ASSERT_EQ(window->HostsCount(), 1);

  model().SetHostData(
      HostData{.id = 3_pubid, .name = "host", .data = ClipboardData{.text = {"text1", "text2"}}});
  ASSERT_EQ(window->HostsCount(), 2u);
  EXPECT_EQ(window->GetHostView(0)->GetTextItemsCount(), 2u);
  EXPECT_EQ(window->GetHostView(1)->GetTextItemsCount(), 2u);

  EXPECT_TRUE(model().AddHostText(3_pubid, "text3"));
  ASSERT_EQ(window->HostsCount(), 2u);
  EXPECT_EQ(window->GetHostView(0)->GetTextItemsCount(), 2u);
  EXPECT_EQ(window->GetHostView(1)->GetTextItemsCount(), 3u);

  EXPECT_FALSE(model().AddHostText(323_pubid, "text3"));
  ASSERT_EQ(window->HostsCount(), 2u);
  EXPECT_EQ(window->GetHostView(0)->GetTextItemsCount(), 2u);
  EXPECT_EQ(window->GetHostView(1)->GetTextItemsCount(), 3u);

  controller().HideUi();
  EXPECT_EQ(controller().GetContentWindowForTests(), nullptr);
}

// TODO: Test text position in ui.
