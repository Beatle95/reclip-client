#include <exception>

import app_main.application_main;
import base.log;

using namespace reclip;

int main(int argc, char** argv) {
  try {
    ApplicationMain app(argc, argv);
    return app.exec();
  } catch (const std::exception& err) {
    LogError("Exception in main: {}", err.what());
  } catch (...) {
    LogError("Unknown exception in main");
  }
}
