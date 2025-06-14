#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QApplication>

#include "core/clipboard.h"
#include "ui/host_clipboard_view.h"

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
        model_(std::make_unique<ClipboardModel>()),
        controller_(std::make_unique<ClipboardController>(model_.get(),
                                                          &mock_clipboard_)) {
    model_->AddObserver(controller_.get());
  }

  ~ModelControllerIntegrationTest() {
    model_->RemoveObserver(controller_.get());
  }

  MockClipboard& clipboard() { return mock_clipboard_; }
  ClipboardModel& model() { return *model_; }
  ClipboardController& controller() { return *controller_; }

 private:
  QApplication app_;
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

  const auto name = model().GetThisHostData().name;
  EXPECT_TRUE(model().AdoptThisHostData(name, {.text = {"text1", "text2"}}));
  ASSERT_EQ(window->HostsCount(), 1);
  EXPECT_EQ(window->GetHostView(0)->GetTextItemsCount(), 2u);

  model().ResetHostsData({
      HostData{.id = "host_id1",
               .name = "host1",
               .data = ClipboardData{.text = {"text0"}}},
      HostData{.id = "host_id2",
               .name = "host2",
               .data = ClipboardData{.text = {"text1", "text2"}}},
  });
  ASSERT_EQ(window->HostsCount(), 3);
  EXPECT_EQ(window->GetHostView(0)->GetTextItemsCount(), 2u);
  EXPECT_EQ(window->GetHostView(1)->GetTextItemsCount(), 1u);
  EXPECT_EQ(window->GetHostView(2)->GetTextItemsCount(), 2u);

  model().ResetHostsData({});
  ASSERT_EQ(window->HostsCount(), 1);

  model().SetHostData(
      HostData{.id = "host_id3",
               .name = "host",
               .data = ClipboardData{.text = {"text1", "text2"}}});
  ASSERT_EQ(window->HostsCount(), 2u);
  EXPECT_EQ(window->GetHostView(0)->GetTextItemsCount(), 2u);
  EXPECT_EQ(window->GetHostView(1)->GetTextItemsCount(), 2u);

  EXPECT_TRUE(model().AddHostText("host_id3", "text3"));
  ASSERT_EQ(window->HostsCount(), 2u);
  EXPECT_EQ(window->GetHostView(0)->GetTextItemsCount(), 2u);
  EXPECT_EQ(window->GetHostView(1)->GetTextItemsCount(), 3u);

  EXPECT_FALSE(model().AddHostText("host_unknown", "text3"));
  ASSERT_EQ(window->HostsCount(), 2u);
  EXPECT_EQ(window->GetHostView(0)->GetTextItemsCount(), 2u);
  EXPECT_EQ(window->GetHostView(1)->GetTextItemsCount(), 3u);

  controller().HideUi();
  EXPECT_EQ(controller().GetContentWindowForTests(), nullptr);
}
