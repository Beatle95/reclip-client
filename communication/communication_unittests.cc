#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cassert>
#include <iostream>

#include "communication/server_impl.h"
#include "core/test_with_event_loop_base.h"

using namespace reclip;
using namespace ::testing;

class MockServerDelegate : public ServerDelegate {
 public:
  MOCK_METHOD2(ProcessSyncData, void(ClipboardData, std::vector<HostData>));
  MOCK_METHOD2(ProcessNewHost, void(const HostId&, const std::string&));
  MOCK_METHOD2(ProcessNewText, bool(const HostId&, const std::string&));
};

class MockServerConnection : public ServerConnection {
 public:
  MOCK_METHOD1(Connect, void(const HostSecret&));
  MOCK_METHOD0(Disconnect, void());
  MOCK_METHOD0(RequestFullSync, void());
  MOCK_METHOD1(SendText, void(const std::string&));

  MockServerConnection() {
    ON_CALL(*this, Connect).WillByDefault([this]() {
      if (delegate_) {
        delegate_->HandleConnect(true);
      }
    });

    ON_CALL(*this, Disconnect).WillByDefault([this]() {
      if (delegate_) {
        delegate_->HandleDisconnected();
      }
    });

    ON_CALL(*this, RequestFullSync).WillByDefault([this]() {
      if (delegate_) {
        delegate_->HandleFullSync({}, {}, true);
      }
    });

    ON_CALL(*this, SendText).WillByDefault([this]() {
      if (delegate_) {
        delegate_->HandleTextSent(true);
      }
    });
  }

  void SetDelegate(ServerConnection::Delegate* delegate) {
    delegate_ = delegate;
  }

 private:
  ServerConnection::Delegate* delegate_ = nullptr;
};

class ServerTestBase : public TestWithEventLoopBase,
                       public ServerImpl::TestHelper {
 public:
  ServerTestBase() {
    connection_owning_ptr_ = std::make_unique<MockServerConnection>();
    connection_ = connection_owning_ptr_.get();
  }

  ~ServerTestBase() override = default;

  void ResetServer(ServerDelegate& delegate) {
    server_ = std::make_unique<ServerImpl>(delegate);
    ASSERT_TRUE(connection_);
    connection_->SetDelegate(server_.get());
  }

  void WaitServerConnected() {
    ASSERT_TRUE(server_);
    while (server_->GetStateForTesting() !=
           ServerImpl::ConnectionState::kConnected) {
      RunEventLoopUntilIdle();
    }
  }

  // ServerImpl::TestHelper overrides.
  std::unique_ptr<ServerConnection> CreateConnection() override {
    assert(!!connection_owning_ptr_);
    return std::move(connection_owning_ptr_);
  }

  ServerImpl* server() {
    assert(server_);
    return server_.get();
  }

  MockServerConnection* connection() { return connection_; }

 private:
  std::unique_ptr<ServerImpl> server_;
  std::unique_ptr<MockServerConnection> connection_owning_ptr_;
  MockServerConnection* connection_ = nullptr;
};

TEST_F(ServerTestBase, AutoReconnection) {
  MockServerDelegate delegate;
  {
    EXPECT_CALL(*connection(), Connect).Times(1);
    EXPECT_CALL(*connection(), RequestFullSync).Times(1);
    EXPECT_CALL(delegate, ProcessSyncData).Times(1);

    ResetServer(delegate);
    ASSERT_TRUE(connection());
    WaitServerConnected();

    Mock::VerifyAndClearExpectations(connection());
    Mock::VerifyAndClearExpectations(&delegate);
  }

  for (uint32_t i = 0u; i < 5u; ++i) {
    EXPECT_CALL(*connection(), Connect).Times(1);
    EXPECT_CALL(*connection(), RequestFullSync).Times(1);
    EXPECT_CALL(delegate, ProcessSyncData).Times(1);

    server()->HandleDisconnected();
    EXPECT_EQ(server()->GetStateForTesting(),
              ServerImpl::ConnectionState::kDisconnected);
    WaitServerConnected();

    Mock::VerifyAndClearExpectations(connection());
    Mock::VerifyAndClearExpectations(&delegate);
  }
}

TEST_F(ServerTestBase, UnknownHostTextReceivedAction) {
  MockServerDelegate delegate;
  {
    EXPECT_CALL(*connection(), Connect).Times(1);
    EXPECT_CALL(*connection(), RequestFullSync).Times(1);
    EXPECT_CALL(delegate, ProcessSyncData).Times(1);

    ResetServer(delegate);
    ASSERT_TRUE(connection());
    WaitServerConnected();

    Mock::VerifyAndClearExpectations(connection());
    Mock::VerifyAndClearExpectations(&delegate);
  }

  const HostId kKnownId = "known_id";
  const HostId kUnknownId = "unknown_id";
  const std::string kKnownHostName = "some_name";
  const std::string kIrrelevantText = "some text";
  {
    EXPECT_CALL(delegate, ProcessNewHost(_, _)).Times(1);
    EXPECT_CALL(delegate, ProcessNewText(_, _)).Times(1).WillOnce(Return(true));
    server()->HandleNewHost(kKnownId, kKnownHostName);
    server()->HandleNewText(kKnownId, kIrrelevantText);
    Mock::VerifyAndClearExpectations(&delegate);
  }
  // Right now we simply disconnecting if unknown host's data was received.
  EXPECT_CALL(*connection(), Disconnect);
  EXPECT_CALL(delegate, ProcessNewText(_, _)).Times(1).WillOnce(Return(false));
  server()->HandleNewText(kUnknownId, kIrrelevantText);
}
