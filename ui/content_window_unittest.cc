#include "ui/content_window.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QApplication>

#include "ui/host_clipboard_view.h"

import base.preferences;
import core.clipboard;
import core.hosts_list_model;
import core.host_types;
import ui.content_window_controller;

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
        model_(std::make_unique<HostsListModel>()),
        controller_(std::make_unique<ContentWindowController>(model_.get(), &mock_clipboard_)) {
  }

  MockClipboard& clipboard() { return mock_clipboard_; }
  HostsListModel& model() { return *model_; }
  ContentWindowController& controller() { return *controller_; }

 private:
  QApplication app_;
  MockPreferencesGuard preferences_guard_;
  MockClipboard mock_clipboard_;
  std::unique_ptr<HostsListModel> model_;
  std::unique_ptr<ContentWindowController> controller_;
};

TEST_F(ModelControllerIntegrationTest, ModelReactionTests) {
  EXPECT_EQ(controller().GetContentWindowForTests(), nullptr);
  controller().ShowUi();
  auto* window = controller().GetContentWindowForTests();
  ASSERT_NE(window, nullptr);
  ASSERT_EQ(window->GetHostsViewsCount(), 1);

  auto get_items_count = [window](int index) -> int {
    auto* view = dynamic_cast<HostClipboardView*>(window->GetHostViewForTests(index));
    if (!view) {
      return -1;
    }
    return view->GetTextItemsCount();
  };

  EXPECT_TRUE(model().GetLocalHost().AdoptData("name", {.text = {"text1", "text2"}}));
  ASSERT_EQ(window->GetHostsViewsCount(), 1);
  EXPECT_EQ(get_items_count(0), 2);

  model().ResetRemoteHostsData({
      HostData{.id = 1_pubid, .name = "host1", .data = ClipboardData{.text = {"text0"}}},
      HostData{.id = 2_pubid, .name = "host2", .data = ClipboardData{.text = {"text1", "text2"}}},
  });
  ASSERT_EQ(window->GetHostsViewsCount(), 3);
  EXPECT_EQ(get_items_count(0), 2);
  EXPECT_EQ(get_items_count(1), 1);
  EXPECT_EQ(get_items_count(2), 2);

  model().ResetRemoteHostsData({});
  ASSERT_EQ(window->GetHostsViewsCount(), 1);

  model().ResetLocalHostData(
      HostData{.id = 3_pubid, .name = "host", .data = ClipboardData{.text = {"text1", "text2"}}});
  ASSERT_EQ(window->GetHostsViewsCount(), 2u);
  EXPECT_EQ(get_items_count(0), 2u);
  EXPECT_EQ(get_items_count(1), 2u);

  model().GetRemoteHost(3_pubid)->PushText("text3");
  ASSERT_EQ(window->GetHostsViewsCount(), 2u);
  EXPECT_EQ(get_items_count(0), 2u);
  EXPECT_EQ(get_items_count(1), 3u);

  controller().HideUi();
  EXPECT_EQ(controller().GetContentWindowForTests(), nullptr);
}

// TODO: Test text position in ui.
