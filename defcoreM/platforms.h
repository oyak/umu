#ifndef PLATFORMS_H
#define PLATFORMS_H

#include "systemdetection.h"

#if defined(_MSC_VER)
#define DEFCORE_CC_MSVC
#define DEFCORE_FUNC_INFO __FUNCSIG__
#define DEFCORE_ALIGNOF(type) __alignof(type)
#define DEFCORE_DECL_ALIGN(n) __declspec(align(n))
#define DEFCORE_ASSUME_IMPL(expr) __assume(expr)
#define DEFCORE_UNREACHABLE_IMPL() __assume(0)
#define DEFCORE_NORETURN __declspec(noreturn)
#define DEFCORE_DECL_DEPRECATED __declspec(deprecated)
#define DEFCORE_DECL_EXPORT __declspec(dllexport)
#define DEFCORE_DECL_IMPORT __declspec(dllimport)
#elif defined(__BORLANDC__) || defined(__TURBOC__)
#define DEFCORE_NORETURN
#define DEFCORE_CC_BOR
#elif defined(__GNUC__)
#define DEFCORE_CC_GNU
#if defined(__MINGW32__)
#define DEFCORE_CC_MINGW
#endif
#if defined(DEFCORE_OS_WIN)
#define DEFCORE_DECL_EXPORT __declspec(dllexport)
#define DEFCORE_DECL_IMPORT __declspec(dllimport)
#elif defined(DEFCORE_VISIBILITY_AVAILABLE)
#define DEFCORE_DECL_EXPORT __attribute__((visibility("default")))
#define DEFCORE_DECL_IMPORT __attribute__((visibility("default")))
#define DEFCORE_DECL_HIDDEN __attribute__((visibility("hidden")))
#endif
#define DEFCORE_FUNC_INFO __PRETTY_FUNCTION__
#define DEFCORE_ALIGNOF(type) __alignof__(type)
#define DEFCORE_TYPEOF(expr) __typeof__(expr)
#define DEFCORE_DECL_DEPRECATED __attribute__((__deprecated__))
#define DEFCORE_DECL_ALIGN(n) __attribute__((__aligned__(n)))
#define DEFCORE_DECL_UNUSED __attribute__((__unused__))
#define DEFCORE_LIKELY(expr) __builtin_expect(!!(expr), true)
#define DEFCORE_UNLIKELY(expr) __builtin_expect(!!(expr), false)
#define DEFCORE_NORETURN __attribute__((__noreturn__))
#define DEFCORE_REQUIRED_RESULT __attribute__((__warn_unused_result__))
#endif

#if defined(__i386__) || defined(_WIN32) || defined(_WIN32_WCE)
#if defined(DEFCORE_CC_GNU)
#define DEFCORE_FASTCALL __attribute__((regparm(3)))
#elif defined(DEFCORE_CC_MSVC) || defined(DEFCORE_CC_BOR)
#define DEFCORE_FASTCALL __fastcall
#else
#define DEFCORE_FASTCALL
#endif
#else
#define DEFCORE_FASTCALL
#endif

inline void defcore_noop(void) {}
void defcore_assert(const char* assertion, const char* file, int line) DEFCORE_NORETURN;

#if !defined(DEFCORE_NO_DEBUG) && !defined(DEFCORE_DEBUG)
#define DEFCORE_DEBUG
#endif

#if defined(DEFCORE_DEBUG)
#define DEFCORE_ASSERT(cond) ((!(cond)) ? defcore_assert(#cond, __FILE__, __LINE__) : defcore_noop())
#else
#define DEFCORE_ASSERT(cond) (defcore_noop())
#endif

#if defined(DEFCORE_OS_WIN) && !defined(DEFCORE_CC_GNU)
#define SLEEP(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP(ms) usleep(ms * 1000);
#endif

#if !defined(DEFCORE_CC_BOR)
typedef double TDateTime;
#define SIZE_OF_TYPE(ptr) sizeof(ptr)
#else
#include <System.hpp>
#define SIZE_OF_TYPE(ptr) 4
#endif
TDateTime CurrentDateTime();

#define UNUSED(x) (void) x

#endif  // PLATFORMS_H
