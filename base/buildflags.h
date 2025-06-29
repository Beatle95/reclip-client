#pragma once

#ifdef _WIN32
#define IS_WIN_INTERNAL() (1)
#else
#define IS_WIN_INTERNAL() (0)
#endif

#ifdef __linux__
#define IS_LINUX_INTERNAL() (1)
#else
#define IS_LINUX_INTERNAL() (0)
#endif

#ifdef __APPLE__
#define IS_MAC_INTERNAL() (1)
#else
#define IS_MAC_INTERNAL() (0)
#endif

#ifdef NDEBUG
#define DLOG_ON_INTERNAL() (0)
#else
#define DLOG_ON_INTERNAL() (1)
#endif

#define BUILDFLAG_CAT(a, b) a ## b
#define BUILDFLAG(flag) (BUILDFLAG_CAT(flag, _INTERNAL)())
