#if !defined(BASE_TYPE_H)
#define BASE_TYPE_H

#define global static

#if COMPILER_CLANG || COMPILER_GCC
#define internal static __attribute__((unused))
#elif COMPILER_MSVC
#define internal __pragma(warning(suppress : 4505)) static
#else
#define internal static
#endif

#if OS_WINDOWS
#define export extern __declspec(dllexport)
#else
#define export extern
#endif

#if COMPILER_MSVC
#pragma section(".ro", read)
#define read_only __declspec(allocate(".ro")) const
#elif COMPILER_GCC || COMPILER_CLANG
#define read_only __attribute__((section(".rodata"))) const
#else
#define read_only const
#endif

#if COMPILER_MSVC
#define thread_var __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
#define thread_var __thread
#else
#define thread_var
#endif

#if COMPILER_MSVC
#define force_inline __forceinline
#elif COMPILER_CLANG || COMPILER_GCC
#define force_inline __attribute__((always_inline))
#else
#define force_inline
#endif

#if COMPILER_MSVC
#define no_inline __declspec(noinline)
#elif COMPILER_CLANG || COMPILER_GCC
#define no_inline __attribute__((noinline))
#else
#define no_inline
#endif

#if COMPILER_MSVC
#define unreachable __assume(0)
#elif COMPILER_CLANG || COMPILER_GCC
#define unreachable __builtin_unreachable()
#else
#define unreachable
#endif

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef float F32;
typedef double F64;

#define U8_MIN 0
#define U16_MIN 0
#define U32_MIN 0
#define U64_MIN 0

#define u8_min 0
#define u16_min 0
#define U32_min 0
#define U64_min 0

#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#define I8_MIN ((-__INT8_MAX__) - 1)
#define I8_MAX __INT8_MAX__
#define U8_MAX (__INT8_MAX__ * 2U + 1U)

#define I16_MIN ((-__INT16_MAX__) - 1)
#define I16_MAX __INT16_MAX__
#define U16_MAX (__INT16_MAX__ * 2U + 1U)

#define I32_MIN ((-__INT32_MAX__) - 1)
#define I32_MAX __INT32_MAX__
#define U32_MAX (__INT32_MAX__ * 2U + 1U)

#define I64_MIN ((-__INT64_MAX__) - 1LL)
#define I64_MAX __INT64_MAX__
#define U64_MAX (__INT64_MAX__ * 2ULL + 1ULL)

#define F32_MIN __FLT_MIN__
#define F32_MAX __FLT_MAX__

#define F64_MIN __DBL_MIN__
#define F64_MAX __DBL_MAX__

#else
#define I8_MIN ((I8)(-127 - 1))
#define I8_MAX ((I8)127)
#define U8_MAX ((U8)255U)

#define I16_MIN ((I16)(-32767 - 1))
#define I16_MAX ((I16)32767)
#define U16_MAX ((U16)65535U)

#define I32_MIN (-2147483647 - 1)
#define I32_MAX 2147483647
#define U32_MAX 4294967295U

#define I64_MIN (-9223372036854775807LL - 1LL)
#define I64_MAX 9223372036854775807LL
#define U64_MAX 18446744073709551615ULL

#define F32_MAX (*(const float *)(const unsigned int[]){0x7F7FFFFF})
#define F32_MIN (*(const float *)(const unsigned int[]){0x00800000})

#define F64_MAX (*(const double *)(const unsigned long long[]){0x7FEFFFFFFFFFFFFFULL})
#define F64_MIN (*(const double *)(const unsigned long long[]){0x0010000000000000ULL})
#endif

#define i8_min I8_MIN
#define i8_max I8_MAX
#define u8_max U8_MAX

#define i16_min I16_MIN
#define i16_max I16_MAX
#define u16_max U16_MAX

#define i32_min I32_MIN
#define i32_max I32_MAX
#define U32_max U32_MAX

#define i64_min I64_MIN
#define i64_max I64_MAX
#define U64_max U64_MAX

#define f32_min F32_MIN
#define f32_max F32_MAX

#define f64_min F64_MIN
#define f64_max F64_MAX

#if COMPILER_MSVC

internal force_inline I8 u32_count_zerol(U32 x);
internal force_inline I8 u64_count_zerol(U64 x);
internal force_inline I8 u32_count_zeror(U32 x);
internal force_inline I8 u64_count_zeror(U64 x);

internal force_inline I8 u32_msb(U32 x);
internal force_inline I8 u64_msb(U64 x);
internal force_inline I8 u32_lsp(U32 x);
internal force_inline I8 u64_lsp(U64 x);

#define u32_count_set_bits __popcnt
#define u64_count_set_bits __popcnt64

#elif COMPILER_CLANG || COMPILER_GCC

#define u32_msb(x) ((x) == 0 ? -1 : 31 - __builtin_clz(x))
#define u64_msb(x) ((x) == 0 ? -1 : 63 - __builtin_clzll(x))

#define u32_lsp(x) ((x) == 0 ? -1 : __builtin_ctz(x))
#define u64_lsp(x) ((x) == 0 ? -1 : __builtin_ctzll(x))

#define u32_count_set_bits __builtin_popcount
#define u64_count_set_bits __builtin_popcountll

#define u32_count_zerol __builtin_clz
#define u64_count_zerol __builtin_clzll
#define u32_count_zeror __builtin_ctz
#define u64_count_zeror __builtin_ctzll

#endif

/* Atomic */

#if defined(COMPILER_MSVC)
    #if defined(OS_WINDOWS)
        #pragma intrinsic(_InterlockedExchangeAdd)
        #pragma intrinsic(_InterlockedExchangeAdd64)
        #pragma intrinsic(_InterlockedCompareExchange)
        #pragma intrinsic(_InterlockedCompareExchange64)
        #pragma intrinsic(_InterlockedExchange)
        #pragma intrinsic(_InterlockedExchange64)
        #pragma intrinsic(_ReadWriteBarrier)
    #endif
#endif

internal force_inline void atomic_barrier(void);
internal force_inline void atomic_barrier_full(void);

internal force_inline U32 u32_atomic_load(const volatile U32* src, U32 is_relaxed);
internal force_inline void u32_atomic_store(volatile U32* dst, U32 val, U32 is_relaxed);
internal force_inline U32 u32_atomic_add_fetch(volatile U32* dst, U32 value);
internal force_inline U32 u32_atomic_sub_fetch(volatile U32* dst, U32 value);
internal force_inline U32 u32_atomic_fetch_add(volatile U32* dst, U32 value);
internal force_inline U32 u32_atomic_fetch_sub(volatile U32* dst, U32 value);
internal force_inline U32 u32_atomic_fetch_and(volatile U32* dst, U32 value);
internal force_inline U32 u32_atomic_fetch_or(volatile U32* dst, U32 value);
internal force_inline U32 u32_atomic_fetch_xor(volatile U32* dst, U32 value);
internal force_inline U32 u32_atomic_cas(volatile U32* dst, U32* expected, U32 desired);
internal force_inline U32 u32_atomic_cas_weak(volatile U32* dst, U32* expected, U32 desired);
internal force_inline U32 u32_atomic_exchange(volatile U32* dst, U32 desired);

internal force_inline U64 u64_atomic_load(const volatile U64* src);
internal force_inline void u64_atomic_store(volatile U64* dst, U64 val);
internal force_inline U64 u64_atomic_add_fetch(volatile U64* dst, U64 value);
internal force_inline U64 u64_atomic_sub_fetch(volatile U64* dst, U64 value);
internal force_inline U64 u64_atomic_fetch_add(volatile U64* dst, U64 value);
internal force_inline U64 u64_atomic_fetch_sub(volatile U64* dst, U64 value);
internal force_inline U64 u64_atomic_fetch_and(volatile U64* dst, U64 value);
internal force_inline U64 u64_atomic_fetch_or(volatile U64* dst, U64 value);
internal force_inline U64 u64_atomic_fetch_xor(volatile U64* dst, U64 value);
internal force_inline U32 u64_atomic_cas(volatile U64* dst, U64* expected, U64 desired);
internal force_inline U32 u64_atomic_cas_weak(volatile U64* dst, U64* expected, U64 desired);
internal force_inline U64 u64_atomic_exchange(volatile U64* dst, U64 desired);

internal force_inline void* ptr_atomic_load(void* volatile* src);
internal force_inline void ptr_atomic_store(void* volatile* dst, void* val);
internal force_inline void* ptr_atomic_exchange(void* volatile* dst, void* desired);
internal force_inline void* ptr_atomic_cas(void* volatile* dst, void* expected, void* desired);
internal force_inline void* ptr_atomic_cas_weak(void* volatile* dst, void* expected, void* desired);

/* Type Indicator */

typedef U8 Type_Indicator;

enum {
    TYPE_SIGN_BIT       = 6,
    TYPE_INTEGER_BIT    = 7,
};

enum {
    TYPE_VOID       = 0,

    TYPE_INTEGER    = bit(TYPE_INTEGER_BIT),
    TYPE_FLOAT      = !bit(TYPE_INTEGER_BIT),

    TYPE_SIGNED     = bit(TYPE_SIGN_BIT),
    TYPE_UNSIGNED   = !bit(TYPE_SIGN_BIT),
};

enum {
    TYPE_U8     = TYPE_INTEGER | TYPE_UNSIGNED | sizeof(U8),
    TYPE_U16    = TYPE_INTEGER | TYPE_UNSIGNED | sizeof(U16),
    TYPE_U32    = TYPE_INTEGER | TYPE_UNSIGNED | sizeof(U32),
    TYPE_U64    = TYPE_INTEGER | TYPE_UNSIGNED | sizeof(U64),

    TYPE_I8     = TYPE_INTEGER | TYPE_SIGNED | sizeof(I8),
    TYPE_I16    = TYPE_INTEGER | TYPE_SIGNED | sizeof(I16),
    TYPE_I32    = TYPE_INTEGER | TYPE_SIGNED | sizeof(I32),
    TYPE_I64    = TYPE_INTEGER | TYPE_SIGNED | sizeof(I64),

    TYPE_F32    = TYPE_FLOAT | TYPE_SIGNED | sizeof(F32),
    TYPE_F64    = TYPE_FLOAT | TYPE_SIGNED | sizeof(F64),
};

internal force_inline U8 type_get_size(Type_Indicator type);
internal force_inline U8 type_is_void(Type_Indicator type);
internal force_inline U8 type_is_signed(Type_Indicator type);
internal force_inline U8 type_is_integer(Type_Indicator type);
internal force_inline U8 type_is_float(Type_Indicator type);
internal force_inline U8 type_is_unsigned(Type_Indicator type);

#endif /* BASE_TYPE_H */