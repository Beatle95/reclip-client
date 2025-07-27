#include <exception>

#include "base/log.h"

import app_main.application_main;

using namespace reclip;

int main(int argc, char** argv) {
  try {
    ApplicationMain app(argc, argv);
    return app.exec();
  } catch (const std::exception& err) {
    LOG(ERROR) << "Exception in main: " << err.what();
  } catch (...) {
    LOG(ERROR) << "Unknown exception in main";
  }
}
