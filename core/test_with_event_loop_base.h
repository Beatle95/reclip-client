#pragma once

#include <QCoreApplication>

#include "gtest/gtest.h"

class QCoreApplication;

namespace reclip {

class TestWithEventLoopBase : public ::testing::Test {
 public:
  ~TestWithEventLoopBase() override;
  void SetUp() override;
  void TearDown() override;

  void RunEventLoopUntilIdle();
  void RunEventLoop();
  void ExitEventLoop(int status = 0);

 private:
  std::unique_ptr<QCoreApplication> app_;
};

}  // namespace reclip
