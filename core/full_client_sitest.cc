#include <QtCore/qcoreapplication.h>
#include <QtCore/qeventloop.h>
#include <core/clipboard.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <format>
#include <memory>

import base.constants;
import base.observer_helper;
import core.clipboard_model;
import core.host_types;
import core.test_with_event_loop_base;
import core.test_server_process;
import communication.communication_manager;
import communication.connection_info_provider;
import communication.server_impl;

using namespace reclip;
using namespace std::chrono_literals;

namespace {

class ServerTestHelper : public ServerImpl::TestHelper {
 public:
  ~ServerTestHelper() override = default;
  std::chrono::milliseconds GetReconnectDuration() override { return 100ms; }
};

class TestConnInfoProvider : public ConnectionInfoProvider {
 public:
  static constexpr std::string kTestIp = "127.0.0.1";
  static constexpr uint16_t kTestPort = 6783;

  explicit TestConnInfoProvider(const HostSecret& secret) : secret_(secret) {}
  ~TestConnInfoProvider() override = default;

  HostSecret GetSecret() const override { return secret_; }

  const std::string& GetIp() const override { return kTestIp; }

  uint16_t GetPort() const override { return kTestPort; }

 private:
  HostSecret secret_;
};

class TestClient {
 public:
  explicit TestClient(HostSecret secret) {
    model_ = std::make_unique<ClipboardModel>();
    communication_manager_ = std::make_unique<CommunicationManager>(
        *model_, std::make_unique<TestConnInfoProvider>(secret));
  }

  void AddText(const std::string& text) { model_->OnTextUpdated(text); }
  void AddModelObserver(ClipboardModelObserver* observer) { model_->AddObserver(observer); }
  void RemoveModelObserver(ClipboardModelObserver* observer) { model_->RemoveObserver(observer); }

  void WaitConnected() {
    Server* server = communication_manager_->GetServerForTesting();
    ASSERT_NE(server, nullptr);
    ServerImpl* server_impl = dynamic_cast<ServerImpl*>(server);
    ASSERT_NE(server_impl, nullptr);
    while (server_impl->GetStateForTesting() != ServerImpl::ConnectionState::kConnected) {
      QCoreApplication::processEvents();
    }
  }

  void Disconnect() {
    bool callback_called = false;
    communication_manager_->DisconnectFromServer(
        [&callback_called](bool) { callback_called = true; }, 250ms);
    while (!callback_called) {
      QCoreApplication::processEvents();
    }
  }

  const ClipboardModel& GetModel() const { return *model_; }

 private:
  std::unique_ptr<ClipboardModel> model_;
  std::unique_ptr<CommunicationManager> communication_manager_;
};

class ModelUpdatesWaiter : public ClipboardModelObserver {
 public:
  void WaitForCalls(uint32_t expected_count) {
    assert(!event_loop_.isRunning());
    if (calls_ < expected_count) {
      expected_calls_ = expected_count;
      event_loop_.exec();
    }
  }

  // ClipboardModelObserver overrides:
  void OnTextPushed(size_t) override {
    ++calls_;
    if (event_loop_.isRunning() && calls_ == expected_calls_) {
      event_loop_.quit();
    }
  }

  void OnThisTextPushed() override {}
  void OnThisTextPoped() override {}
  void OnHostUpdated(size_t) override {}
  void OnTextPoped(size_t) override {}
  void OnThisHostDataReset() override {}
  void OnHostsDataReset() override {}

 private:
  QEventLoop event_loop_;
  uint32_t expected_calls_ = 0;
  uint32_t calls_ = 0;
};

}  // namespace

using ClientsGroupIntegration = TestWithEventLoopBase;

TEST_F(ClientsGroupIntegration, CoupleClientsCommunication) {
  ServerTestHelper helper;
  ServerProcess process;
  ASSERT_TRUE(process.Start("simple_communication_test", TestConnInfoProvider::kTestPort));

  std::vector<TestClient> clients;
  clients.emplace_back("secret1");
  clients.emplace_back("secret2");
  clients.emplace_back("secret3");

  ModelUpdatesWaiter waiter;
  std::for_each(clients.begin(), clients.end(),
                [&](TestClient& client) { client.AddModelObserver(&waiter); });

  std::for_each(clients.begin(), clients.end(),
                [&](TestClient& client) { client.WaitConnected(); });

  static constexpr uint32_t kUpdatesCount = 20;
  ASSERT_GT(kUpdatesCount, kClipboardSizeMax);
  for (uint32_t i = 1; i <= kUpdatesCount; ++i) {
    clients[0].AddText(std::format("client_{}_text_{}", 1, i));
    clients[1].AddText(std::format("client_{}_text_{}", 2, i));
    clients[2].AddText(std::format("client_{}_text_{}", 3, i));
    RunEventLoopUntilIdle();
  }
  waiter.WaitForCalls(clients.size() * (clients.size() - 1) * kUpdatesCount);

  std::for_each(clients.begin(), clients.end(),
                [&](TestClient& client) { client.RemoveModelObserver(&waiter); });
  std::for_each(clients.begin(), clients.end(), [&](TestClient& client) { client.Disconnect(); });

  for (uint32_t main_client = 0; main_client < clients.size(); ++main_client) {
    const auto& this_data = clients[main_client].GetModel().GetThisHostData();
    // This is set by server.
    EXPECT_EQ(this_data.id, std::format("public{}", main_client + 1));
    EXPECT_EQ(this_data.name, std::format("name{}", main_client + 1));

    EXPECT_EQ(this_data.data.text.size(), kClipboardSizeMax);
    for (uint32_t text_index = 0; text_index < kClipboardSizeMax; ++text_index) {
      EXPECT_EQ(this_data.data.text[text_index],
                std::format("client_{}_text_{}", main_client + 1, kUpdatesCount - text_index));
    }

    EXPECT_EQ(clients[main_client].GetModel().GetHostsCount(), clients.size() - 1);
    for (uint32_t client_counter = 0; client_counter < clients.size(); ++client_counter) {
      if (client_counter == main_client) {
        continue;
      }
      // Hosts position inside model is not defined, so we will find each expected element.
      const HostData* other_data =
          clients[main_client].GetModel().GetHostData(std::format("public{}", client_counter + 1));
      ASSERT_NE(other_data, nullptr);
      EXPECT_EQ(other_data->name, std::format("name{}", client_counter + 1));

      EXPECT_EQ(other_data->data.text.size(), kClipboardSizeMax);
      for (uint32_t text_index = 0; text_index < kClipboardSizeMax; ++text_index) {
        EXPECT_EQ(other_data->data.text[text_index],
                  std::format("client_{}_text_{}", client_counter + 1, kUpdatesCount - text_index));
      }
    }
  }

  process.Stop();
  EXPECT_TRUE(process.WaitFinished(3000));
}
