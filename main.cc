#include <exception>

#include "base/log.h"
#include "core/application_main.cc"

using namespace reclip;

int main(int argc, char** argv) {
  try {
    ApplicationMain app(argc, argv);
    return app.exec();
  } catch (const std::exception& err) {
    LOG(CRITICAL) << "Exception in main: " << err.what();
  } catch (...) {
    LOG(CRITICAL) << "Unknown exception in main";
  }
}
