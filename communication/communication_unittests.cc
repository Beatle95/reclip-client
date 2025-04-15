#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QTimer>
#include <cassert>
#include <chrono>
#include <iostream>

#include "communication/message_types.h"
#include "communication/serialization.h"
#include "communication/server.h"
#include "communication/server_impl.h"
#include "core/clipboard_model.h"
#include "core/test_with_event_loop_base.h"

using namespace reclip;
using namespace ::testing;
using namespace std::chrono_literals;

class SerializationTestHelperImpl : public SerializationTestHelper {
 public:
  ~SerializationTestHelperImpl() override = default;

  void SetSyncResult(auto&& val) { 
    sync_ = std::forward<decltype(val)>(val); 
  }
  void SetHostIdResult(auto&& val) {
    host_id_ = std::forward<decltype(val)>(val);
  }
  void SetHostDataResult(auto&& val) {
    host_data_ = std::forward<decltype(val)>(val);
  }
  void SetNewTextResult(auto&& val) {
    new_text_ = std::forward<decltype(val)>(val);
  }

  std::optional<SyncResponse> ParseSyncResponse(const QByteArray&) override {
    return sync_;
  }
  std::optional<HostId> ParseHostId(const QByteArray&) override {
    return host_id_;
  }
  std::optional<HostData> ParseHostData(const QByteArray&) override {
    return host_data_;
  }
  std::optional<TextUpdateResponse> ParseNewText(const QByteArray&) override {
    return new_text_;
  }

 private:
  std::optional<SyncResponse> sync_;
  std::optional<HostId> host_id_;
  std::optional<HostData> host_data_;
  std::optional<TextUpdateResponse> new_text_;
};

class MockServerDelegate : public ServerDelegate {
 public:
  MOCK_METHOD2(OnFullSync, void(HostData, std::vector<HostData>));
  MOCK_METHOD1(HostConnected, void(const HostId& id));
  MOCK_METHOD1(HostDisconnected, void(const HostId& id));
  MOCK_METHOD2(HostTextAdded, void(const HostId& id, const std::string& text));
  MOCK_METHOD1(HostSynced, void(HostData data));
};

class MockConnection : public Connection {
 public:
  MOCK_METHOD0(Connect, void());
  MOCK_METHOD0(Disconnect, void());
  MOCK_METHOD3(SendMessage,
               bool(uint64_t, ClientMessageType, const QByteArray&));

  MockConnection() {
    ON_CALL(*this, Connect).WillByDefault([this]() {
      if (delegate_) {
        QTimer::singleShot(1ms, [&]() { delegate_->HandleConnected(true); });
      }
    });

    ON_CALL(*this, Disconnect).WillByDefault([this]() {
      if (delegate_) {
        QTimer::singleShot(1ms, [&]() { delegate_->HandleDisconnected(); });
      }
    });

    ON_CALL(*this, SendMessage)
        .WillByDefault([this](uint64_t id, ClientMessageType type,
                              const QByteArray& data) -> bool {
          if (delegate_) {
            QTimer::singleShot(1ms, [=, this]() {
              if (type == ClientMessageType::kFullSyncRequest) {
                delegate_->HandleReceieved(
                    id, ServerMessageType::kServerResponse, data);
              }
            });
          }
          return true;
        });
  }

  void SetDelegate(Connection::Delegate* delegate) { delegate_ = delegate; }

 private:
  Connection::Delegate* delegate_ = nullptr;
};

class ServerTestBase : public TestWithEventLoopBase,
                       public ServerImpl::TestHelper {
 public:
  ~ServerTestBase() override = default;

  void ResetServer() {
    server_delegate_ = std::make_unique<MockServerDelegate>();
    server_ = std::make_unique<ServerImpl>(*server_delegate_);
    ASSERT_TRUE(connection_);
    connection_->SetDelegate(server_.get());
  }

  void WaitServerConnected() {
    ASSERT_TRUE(server_);
    ASSERT_TRUE(connection_);
    while (server_->GetStateForTesting() !=
           ServerImpl::ConnectionState::kConnected) {
      RunEventLoopUntilIdle();
    }
  }

  // ServerImpl::TestHelper overrides.
  std::unique_ptr<Connection> CreateConnection() override {
    auto ptr = std::make_unique<MockConnection>();
    connection_ = ptr.get();
    return std::move(ptr);
  }

  ServerImpl* server() { return server_.get(); }
  MockServerDelegate* server_delegate() { return server_delegate_.get(); }
  MockConnection* connection() { return connection_; }

 private:
  std::unique_ptr<ServerImpl> server_;
  std::unique_ptr<MockServerDelegate> server_delegate_;
  MockConnection* connection_ = nullptr;
};

namespace {
SerializationTestHelperImpl CreateCommonSerializationHelper() {
  SerializationTestHelperImpl result;
  result.SetSyncResult(SyncResponse{});
  result.SetHostIdResult(HostId{});
  result.SetHostDataResult(HostData{});
  result.SetNewTextResult(TextUpdateResponse{});
  return result;
}
}  // namespace

TEST_F(ServerTestBase, AutoReconnection) {
  const auto serializer_helper = CreateCommonSerializationHelper();
  ResetServer();
  ASSERT_NE(connection(), nullptr);
  EXPECT_CALL(*connection(), Connect).Times(1);
  EXPECT_CALL(*connection(), SendMessage).Times(1);
  EXPECT_CALL(*server_delegate(), OnFullSync).Times(1);

  WaitServerConnected();
  Mock::VerifyAndClearExpectations(connection());
  Mock::VerifyAndClearExpectations(server_delegate());

  for (uint32_t i = 0u; i < 5u; ++i) {
    EXPECT_CALL(*connection(), Connect).Times(1);
    EXPECT_CALL(*connection(), SendMessage).Times(1);
    EXPECT_CALL(*server_delegate(), OnFullSync).Times(1);

    server()->HandleDisconnected();
    EXPECT_EQ(server()->GetStateForTesting(),
              ServerImpl::ConnectionState::kDisconnected);
    WaitServerConnected();
    Mock::VerifyAndClearExpectations(connection());
    Mock::VerifyAndClearExpectations(server_delegate());
  }
}

TEST_F(ServerTestBase, ServerClientCommunication) {
  auto serializer_helper = CreateCommonSerializationHelper();
  ResetServer();
  ASSERT_NE(connection(), nullptr);
  EXPECT_CALL(*connection(), Connect).Times(1);
  EXPECT_CALL(*connection(), SendMessage).Times(1);
  EXPECT_CALL(*server_delegate(), OnFullSync).Times(1);
  ASSERT_NE(connection(), nullptr);
  WaitServerConnected();

  const HostId kKnownId = "known_id";
  const std::string kIrrelevantText = "some text";
  constexpr uint64_t kIrrelevantMsgId = 0;

  serializer_helper.SetHostIdResult(kKnownId);
  EXPECT_CALL(*server_delegate(), HostConnected(kKnownId)).Times(1);
  server()->HandleReceieved(kIrrelevantMsgId, ServerMessageType::kHostConnected,
                            {});
  Mock::VerifyAndClearExpectations(server_delegate());

  EXPECT_CALL(*server_delegate(), HostDisconnected(kKnownId)).Times(1);
  server()->HandleReceieved(kIrrelevantMsgId,
                            ServerMessageType::kHostDisconnected, {});
  Mock::VerifyAndClearExpectations(server_delegate());

  serializer_helper.SetNewTextResult(
      TextUpdateResponse{.id = kKnownId, .text = kIrrelevantText});
  EXPECT_CALL(*server_delegate(), HostTextAdded(kKnownId, kIrrelevantText)).Times(1);
  server()->HandleReceieved(kIrrelevantMsgId, ServerMessageType::kTextUpdate,
                            {});
  Mock::VerifyAndClearExpectations(server_delegate());

  HostData synced_host{
      .id = kKnownId,
      .name = "name",
      .data = {.text = ClipboardTextContainer{kIrrelevantText}}};
  serializer_helper.SetHostDataResult(synced_host);
  EXPECT_CALL(*server_delegate(), HostSynced(synced_host)).Times(1);
  server()->HandleReceieved(kIrrelevantMsgId, ServerMessageType::kHostSynced,
                            {});
  Mock::VerifyAndClearExpectations(server_delegate());
}
