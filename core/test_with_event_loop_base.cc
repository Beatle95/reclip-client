#include "core/test_with_event_loop_base.h"

#include <QEventLoop>
#include <cassert>

namespace reclip {

TestWithEventLoopBase::~TestWithEventLoopBase() = default;

void TestWithEventLoopBase::SetUp() {
  int argc = 0;
  char** argv = nullptr;
  app_ = std::make_unique<QCoreApplication>(argc, argv);
}

void TestWithEventLoopBase::TearDown() { app_.reset(); }

void TestWithEventLoopBase::RunEventLoopUntilIdle() {
  QCoreApplication::processEvents();
}

void TestWithEventLoopBase::RunEventLoop() {
  assert(app_);
  app_->exec();
}

void TestWithEventLoopBase::ExitEventLoop(int status) {
  app_->exit(status);
}

}  // namespace reclip
