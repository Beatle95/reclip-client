#include "base/log.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QString>
#include <format>
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

}  // namespace

namespace reclip {

Log::Log(Severity severity, std::string_view file, int line) {
  const auto time_str = QDateTime::currentDateTime()
                            .toString(QStringLiteral("yyyy.MM.dd hh:mm:ss.zzz"))
                            .toStdString();
  stream_ << std::format("{} [{}] [{}:{}]: ", time_str, SeverityToStr(severity), file, line);
}

Log::~Log() { std::cout << stream_.str() << '\n'; }

Log& Log::operator<<(const QString& val) {
  stream_ << val.toStdString();
  return *this;
}

}  // namespace reclip
