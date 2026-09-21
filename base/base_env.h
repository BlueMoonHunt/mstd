#if !defined(BASE_ENV_H)
#define BASE_ENV_H

/* Compiler */
#if defined(__clang__)

#define COMPILER_CLANG 1
#define COMPILER_MSVC 0
#define COMPILER_GCC 0

#elif defined(_MSC_VER)

#define COMPILER_CLANG 0
#define COMPILER_MSVC 1
#define COMPILER_GCC 0

#elif defined(__GNUC__)

#define COMPILER_CLANG 0
#define COMPILER_MSVC 0
#define COMPILER_GCC 1

#else

#define COMPILER_CLANG 0
#define COMPILER_MSVC 0
#define COMPILER_GCC 0
#error "Unknown compiler!"

#endif

/* OS */
#if defined(_WIN32)
#define OS_WINDOWS 1
#else
#define OS_WINDOWS 0
#endif

#if defined(__linux__)
#define OS_LINUX 1
#else
#define OS_LINUX 0
#endif

#if defined(__ANDROID__)
#define OS_ANDROID 1
#else
#define OS_ANDROID 0
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#define OS_MAC 0
#endif

#if defined(__APPLE_CC__)
#define OS_MACINTOSH 1
#else
#define OS_MACINTOSH 0
#endif

/* Architecture */
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#define ARCH_X64 1
#else
#define ARCH_X64 0
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define ARCH_ARM64 1
#else
#define ARCH_ARM64 0
#endif

#if defined(_M_IX86) || defined(i386) || defined(__i386) || defined(__i386__)
#define ARCH_X86 1
#else
#define ARCH_X86 0
#endif

#if defined(__arm__) || defined(_M_ARM)
#define ARCH_ARM 1
#else
#define ARCH_ARM 0
#endif

/* Language */
#if defined(__cplusplus)
#define LANG_CPP 1
#define LANG_C 0
#else
#define LANG_CPP 0
#define LANG_C 1
#endif

#endif /* BASE_ENV_H */