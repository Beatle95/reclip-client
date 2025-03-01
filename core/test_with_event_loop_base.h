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

 private:
  std::unique_ptr<QCoreApplication> app_;
};

}  // namespace reclip
