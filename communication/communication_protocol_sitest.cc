#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>
#include <cassert>

#include "base/buildflags.h"
#include "base/log.h"
#include "communication/connection.h"
#include "communication/connection_impl.h"

import core.test_with_event_loop_base;
import communication.message_types;

using namespace reclip;
using namespace std::chrono_literals;

constexpr size_t kMaxSize = 2 * 1024 * 1024;
constexpr size_t kStepSize = 10 * 1024;
constexpr size_t kExpectedPacketCount = kMaxSize / kStepSize;
constexpr uint16_t kDefaultPort = 8367;

namespace {

class ServerProcess {
 public:
  ServerProcess(const QString& path) : path_(path) {
    QObject::connect(&proc_, &QProcess::readyReadStandardOutput,
                     [this]() { OutReady(); });
    QObject::connect(&proc_, &QProcess::readyReadStandardError,
                     [this]() { ErrorReady(); });
  }

  bool Start() {
    assert(proc_.state() == QProcess::NotRunning);
    proc_.start(path_, {QString::number(kDefaultPort)});
    return proc_.waitForStarted(5000);
  }

  bool WaitFinished() { return proc_.waitForFinished(5000); }

 private:
  void OutReady() {
    const auto strings =
        QString(proc_.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);
    for (const auto& str : strings) {
      LOG(INFO) << "[server out]: " << str.toStdString();
    }
  }

  void ErrorReady() {
    const auto strings =
        QString(proc_.readAllStandardError()).split('\n', Qt::SkipEmptyParts);
    for (const auto& str : strings) {
      LOG(INFO) << "[server err]: " << str.toStdString();
    }
  }

  QProcess proc_;
  QString path_;
};

template <typename T>
T Increment(T val, T default_val) {
  auto integral_val = std::to_underlying(val);
  integral_val += 1;
  if (integral_val > std::to_underlying(T::kMax)) {
    return default_val;
  }
  return static_cast<T>(integral_val);
}

QString GetServerPartTestPath() {
  const auto dir_path = QCoreApplication::instance()->applicationDirPath();
  auto result_path =
      dir_path + "/../../../server/out/communication_protocol_test";
#if BUILDFLAG(IS_WIN)
  result_path += ".exe";
#endif
  if (!QFileInfo::exists(result_path)) {
    return {};
  }
  return result_path;
}

std::vector<char> GetDataBuffer() {
  std::vector<char> data;
  data.resize(kMaxSize);
  char ch = 'a';
  for (auto& elem : data) {
    elem = ch;
    if (++ch > 'z') {
      ch = 'a';
    }
  }
  return data;
}

bool IsDataCorrect(const QByteArray& data) {
  char expected_char = 'a';
  for (int i = 0; i < data.size(); ++i) {
    if (data[i] != expected_char) {
      return false;
    }
    if (++expected_char > 'z') {
      expected_char = 'a';
    }
  }
  return true;
}

}  // namespace

class CommunicationProtocolTest : public Connection::Delegate,
                                  public TestWithEventLoopBase {
 public:
  void Run() {
    test_connection_ = std::make_unique<ServerConnectionImpl>(
        *this, "127.0.0.1", kDefaultPort);
    test_connection_->Connect();
    first_connect_time_ = std::chrono::steady_clock::now();
  }

  void SendAsync() {
    QTimer::singleShot(0,
                       std::bind(&CommunicationProtocolTest::SendImpl, this));
  }

  void HandleConnected(bool is_connected) override {
    if (is_connected) {
      LOG(INFO) << "Connection success\n";
      SendAsync();
      return;
    }

    LOG(INFO) << "Connect error\n";
    if (std::chrono::steady_clock::now() - first_connect_time_ > 10s) {
      ExitEventLoop(1);
      ASSERT_TRUE(false) << "Unable to connect to server test part";
    } else {
      QTimer::singleShot(100ms, [this]() {
        test_connection_->Connect();
      });
    }
  }

  void HandleDisconnected() override { ExitEventLoop(); }

  void HandleReceieved(uint64_t id, ServerMessageType type,
                       const QByteArray& data) override {
    const bool is_message_correct =
        id == packets_recv_ && type == server_msg_type_ &&
        (static_cast<size_t>(data.size()) == (packets_recv_ + 1) * kStepSize) &&
        IsDataCorrect(data);
    if (!is_message_correct) {
      EXPECT_TRUE(false) << "Got incorrect packet, id: " << id
                         << " message type: " << static_cast<int>(type)
                         << " data size: " << data.size();
      test_connection_->Disconnect();
      ExitEventLoop(1);
      return;
    }

    ++packets_recv_;
    server_msg_type_ =
        Increment(server_msg_type_, ServerMessageType::kServerResponse);

    if (packets_recv_ == kExpectedPacketCount) {
      test_connection_->Disconnect();
    }
  }

  bool IsAllDataSentAndReceived() {
    return packets_sent_ == kExpectedPacketCount &&
           packets_recv_ == kExpectedPacketCount;
  }

 private:
  void SendImpl() {
    const auto send_size = kStepSize * (packets_sent_ + 1);
    assert(send_size <= data_.size());
    if (!test_connection_->SendMessage(
            packets_sent_, client_msg_type_,
            QByteArray::fromRawData(data_.data(), send_size))) {
      ExitEventLoop();
    }

    ++packets_sent_;
    client_msg_type_ =
        Increment(client_msg_type_, ClientMessageType::kClientResponse);

    if (packets_sent_ != kExpectedPacketCount) {
      SendAsync();
    }
  }

  std::unique_ptr<Connection> test_connection_;
  std::chrono::steady_clock::time_point first_connect_time_;
  std::vector<char> data_ = GetDataBuffer();

  size_t packets_sent_ = 0;
  size_t packets_recv_ = 0;
  ClientMessageType client_msg_type_ = ClientMessageType::kClientResponse;
  ServerMessageType server_msg_type_ = ServerMessageType::kServerResponse;
};

TEST_F(CommunicationProtocolTest, SimpleSendAndReceive) {
  const auto path = GetServerPartTestPath();
  ASSERT_FALSE(path.isEmpty())
      << "Test was unable to find server part test binary. It expected to be "
         "located at this path (expecting that current directory is "
         "application dir path) "
         "./../../../server/out/communication_protocol_test";

  ServerProcess process(path);
  ASSERT_TRUE(process.Start());
  QTimer::singleShot(0, std::bind(&CommunicationProtocolTest::Run, this));
  RunEventLoop();

  EXPECT_TRUE(IsAllDataSentAndReceived());
  EXPECT_TRUE(process.WaitFinished());
}
