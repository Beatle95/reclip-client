#pragma once
#include <format>
#include <print>
#include <sstream>

#include "base/buildflags.h"

// DEPRECATED: Use module version instead.
#define LOG(severity) reclip::Log(reclip::Severity::_##severity)

#if BUILDFLAG(DLOG_ON)
// DEPRECATED: Use module version instead.
#define DLOG(severity) reclip::Log(reclip::Severity::_##severity)
#else
// DEPRECATED: Use module version instead.
#define DLOG(severity) NoOpLog()
#endif

class QString;

namespace reclip {

// Nono-standard enum elements names, to make logging pritier.
enum class Severity { _INFO, _WARNING, _ERROR, _CRITICAL };

class Log {
 public:
  explicit Log(Severity severity);
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
