#pragma once

#define OS_WIN 0
#define OS_LINUX 0
#define OS_MAC 0

#ifdef _WIN32
#define IS_WIN 1
#endif

#ifdef __linux__
#define IS_LINUX 1
#endif

#ifdef __APPLE__
#define IS_MAC 1
#endif
