#pragma once
#include <sstream>

#define LOG(severity) Log(reclip::Severity::_##severity)
#ifdef NDEBUG
#define DLOG(severity) NoOpLog()
#else
#define DLOG(severity) Log(reclip::Severity::_##severity)
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
