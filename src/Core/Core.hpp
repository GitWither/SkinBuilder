#pragma once

#include <memory>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
#define SKINBUILDER_PLATFORM_WINDOWS
#elif __linux__
#define SKINBUILDER_PLATFORM_LINUX
#endif

#if !defined(NDEBUG) && !defined(SKINBUILDER_DEBUG)
#define SKINBUILDER_DEBUG
#endif

#if SKINBUILDER_DEBUG
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define SB_DEBUG_BREAK() __debugbreak()
#elif defined(_ARMCC_VERSION)
#define SB_DEBUG_BREAK() __breakpoint(42)
#else
#include <signal.h>
#if defined(SIGTRAP)
#define SB_DEBUG_BREAK raise(SIGTRAP)
#endif
#endif
#endif

#if SKINBUILDER_DEBUG
#define SB_ASSERT(check, ...) { if (!check) { SB_DEBUG_BREAK(); }}
#else
#define SB_ASSERT(check, ...)
#endif

template <typename T>
using Shared = std::shared_ptr<T>;

template <typename T, typename ... Args>
constexpr Shared<T> MakeShared(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
using Unique = std::unique_ptr<T>;

template <typename T, typename ... Args>
constexpr Unique<T> MakeUnique(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}