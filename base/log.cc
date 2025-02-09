#include "base/log.h"

#include <QCoreApplication>
#include <QDateTime>
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace reclip;

namespace {

std::string_view SeverityToStr(Severity val) {
  switch (val) {
    case Severity::_INFO:
      return "INFO";
    case Severity::_WARNING:
      return "WARNING";
    case Severity::_ERROR:
      return "ERROR";
    case Severity::_CRITICAL:
      return "CRITICAL";
  }
}

// std::filesystem::path GetCurrentLogFilePath() {
//     // TODO:
//     return std::filesystem::current_path() / "log.log";
// }

}  // namespace

namespace reclip {

Log::Log(Severity severity) {
  stream_ << QDateTime::currentDateTime()
                 .toString(QStringLiteral("yyyy.MM.dd hh:mm:ss.zzz "))
                 .toStdString()
          << SeverityToStr(severity) << " ";
}

Log::~Log() {
  // TODO:
  // std::ofstream ofs;
  // ofs.open(GetCurrentLogFilePath(), std::ios::binary | std::ios::app);
  // if (ofs) {
  //     ofs << stream_.str() << '\n';
  // }
  std::cout << stream_.str() << '\n';
}

}  // namespace reclip
