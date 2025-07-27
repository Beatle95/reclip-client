#pragma once

#include <sstream>

#include "base/buildflags.h"

#define LOG(severity) \
  reclip::Log(reclip::Severity::_##severity, reclip::PathToFilename(__FILE__), __LINE__)

#if BUILDFLAG(DLOG_ON)
#define DLOG(severity) \
  reclip::Log(reclip::Severity::_##severity, reclip::PathToFilename(__FILE__), __LINE__)
#else
#define DLOG(severity) NoOpLog()
#endif

class QString;

namespace reclip {

consteval std::string_view PathToFilename(std::string_view path) {
  return path.substr(path.find_last_of('/') + 1);
}

// Nono-standard enum elements names, to make logging pritier.
enum class Severity { _INFO, _WARNING, _ERROR, _CRITICAL };

class Log {
 public:
  explicit Log(Severity severity, std::string_view file, int line);
  ~Log();

  Log& operator<<(const QString& val);

  template <typename T>
  Log& operator<<(const T& val) {
    stream_ << val;
    return *this;
  }

 private:
  std::stringstream stream_;
};

class NoOpLog {
 public:
  template <typename T>
  NoOpLog& operator<<(T&&) {
    return *this;
  }
};

}  // namespace reclip
