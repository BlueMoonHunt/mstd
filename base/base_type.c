#if COMPILER_MSVC

internal force_inline U8 u32_count_zerol(U32 x) {
    unsigned long zeros = 0;
    return _BitScanReverse(&zeros, x) ? (U8)(31 - zeros) : 32;
}
internal force_inline U8 u64_count_zerol(U64 x) {
    unsigned long zeros = 0;
    return _BitScanReverse64(&zeros, x) ? (U8)(63 - zeros) : 64;
}

internal force_inline U8 u32_count_zeror(U32 x) {
    unsigned long zeros = 0;
    _BitScanForward(&zeros, x);
    return (U8)zeros;
}
internal force_inline U8 u64_count_zeror(U64 x) {
    unsigned long zeros = 0;
    _BitScanForward64(&zeros, x);
    return (U8)zeros;
}

internal force_inline I8 u32_msb(U32 x) {
    unsigned long where;
    return _BitScanReverse(&where, x) ? (I8)where : -1;
}
internal force_inline I8 u64_msb(U64 x) {
    unsigned long where;
    return _BitScanReverse64(&where, x) ? (I8)where : -1;
}

internal force_inline I8 u32_lsp(U32 x) {
    unsigned long where;
    return _BitScanForward(&where, x) ? (I8)where : -1;
}
internal force_inline I8 u64_lsp(U64 x) {
    unsigned long where;
    return _BitScanForward64(&where, x) ? (I8)where : -1;
}

#endif


internal force_inline void atomic_barrier(void) {
#if COMPILER_GCC || COMPILER_CLANG
    __asm__ __volatile__("" ::: "memory");
#elif COMPILER_MSVC
    _ReadWriteBarrier();
#endif
}

internal force_inline U32 u32_atomic_load(const volatile U32* src, U32 is_relaxed) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_load_n(src, is_relaxed ? __ATOMIC_RELAXED : __ATOMIC_ACQUIRE);
#elif COMPILER_MSVC
    U32 res = *src;
    if (!is_relaxed) {
        atomic_barrier();
    }
    return res;
#endif
}

internal force_inline void u32_atomic_store(volatile U32* dst, U32 val, U32 is_relaxed) {
#if COMPILER_GCC || COMPILER_CLANG
    __atomic_store_n(dst, val, is_relaxed ? __ATOMIC_RELAXED : __ATOMIC_RELEASE);
#elif COMPILER_MSVC
    if (!is_relaxed) {
        atomic_barrier();
    }
    *dst = val;
#endif
}

internal force_inline U32 u32_atomic_add_fetch(volatile U32* dst, U32 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_add_fetch(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return (U32)_InterlockedExchangeAdd((volatile long*)dst, (long)value) + value;
#endif
}

internal force_inline U32 u32_atomic_cas(volatile U32* dst, U32* expected, U32 desired) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_compare_exchange_n(dst, expected, desired, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    U32 old_val = (U32)_InterlockedCompareExchange((volatile long*)dst, (long)desired, (long)(*expected));
    if (old_val == *expected) {
        return 1;
    } else {
        *expected = old_val;
        return 0;
    }
#endif
}

internal force_inline U32 u32_atomic_cas_weak(volatile U32* dst, U32* expected, U32 desired) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_compare_exchange_n(dst, expected, desired, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return u32_atomic_cas(dst, expected, desired);
#endif
}

internal force_inline U32 u32_atomic_fetch_add(volatile U32* dst, U32 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_fetch_add(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return (U32)_InterlockedExchangeAdd((volatile long*)dst, (long)value);
#endif
}

internal force_inline U32 u32_atomic_sub_fetch(volatile U32* dst, U32 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_sub_fetch(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return (U32)_InterlockedExchangeAdd((volatile long*)dst, -(long)value) - value;
#endif
}

internal force_inline U32 u32_atomic_fetch_sub(volatile U32* dst, U32 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_fetch_sub(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return (U32)_InterlockedExchangeAdd((volatile long*)dst, -(long)value);
#endif
}

internal force_inline U32 u32_atomic_fetch_and(volatile U32* dst, U32 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_fetch_and(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return (U32)_InterlockedAnd((volatile long*)dst, (long)value);
#endif
}

internal force_inline U32 u32_atomic_fetch_or(volatile U32* dst, U32 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_fetch_or(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return (U32)_InterlockedOr((volatile long*)dst, (long)value);
#endif
}

internal force_inline U32 u32_atomic_fetch_xor(volatile U32* dst, U32 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_fetch_xor(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return (U32)_InterlockedXor((volatile long*)dst, (long)value);
#endif
}

internal force_inline U32 u32_atomic_exchange(volatile U32* dst, U32 desired) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_exchange_n(dst, desired, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return (U32)_InterlockedExchange((volatile long*)dst, (long)desired);
#endif
}

internal force_inline U64 u64_atomic_load(const volatile U64* src) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_load_n(src, __ATOMIC_RELAXED);
#elif COMPILER_MSVC
#if ARCH_X64 || ARCH_ARM64
    return *src;
#elif ARCH_X86 || ARCH_ARM
    return (U64)_InterlockedCompareExchange64((volatile __int64*)src, 0, 0);
#endif
#endif
}

internal force_inline void u64_atomic_store(volatile U64* dst, U64 val) {
#if COMPILER_GCC || COMPILER_CLANG
    __atomic_store_n(dst, val, __ATOMIC_RELAXED);
#elif COMPILER_MSVC
#if ARCH_X64 || ARCH_ARM64
    *dst = val;
#elif ARCH_X86 || ARCH_ARM
    U64 expected = *dst;
    for (;;) {
        U64 old = (U64)_InterlockedCompareExchange64((volatile __int64*)dst, (__int64)val, (__int64)expected);
        if (old == expected)
            break;
        expected = old;
    }
#endif
#endif
}

internal force_inline U64 u64_atomic_add_fetch(volatile U64* dst, U64 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_add_fetch(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return (U64)_InterlockedExchangeAdd64((volatile __int64*)dst, (__int64)value) + value;
#endif
}

internal force_inline U32 u64_atomic_cas(volatile U64* dst, U64* expected, U64 desired) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_compare_exchange_n(dst, expected, desired, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    U64 old_val = (U64)_InterlockedCompareExchange64((volatile __int64*)dst, (__int64)desired, (__int64)(*expected));
    if (old_val == *expected) {
        return 1;
    } else {
        *expected = old_val;
        return 0;
    }
#endif
}

internal force_inline U32 u64_atomic_cas_weak(volatile U64* dst, U64* expected, U64 desired) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_compare_exchange_n(dst, expected, desired, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return u64_atomic_cas(dst, expected, desired);
#endif
}

internal force_inline U64 u64_atomic_fetch_add(volatile U64* dst, U64 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_fetch_add(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
#if ARCH_X64 || ARCH_ARM64
    return (U64)_InterlockedExchangeAdd64((volatile __int64*)dst, (__int64)value);
#elif ARCH_X86 || ARCH_ARM
    U64 expected = *dst;
    for (;;) {
        U64 old = (U64)_InterlockedCompareExchange64((volatile __int64*)dst, (__int64)(expected + value), (__int64)expected);
        if (old == expected)
            return old;
        expected = old;
    }
#endif
#endif
}

internal force_inline U64 u64_atomic_sub_fetch(volatile U64* dst, U64 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_sub_fetch(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
#if ARCH_X64 || ARCH_ARM64
    return (U64)_InterlockedExchangeAdd64((volatile __int64*)dst, -(__int64)value) - value;
#elif ARCH_X86 || ARCH_ARM
    U64 expected = *dst;
    for (;;) {
        U64 old = (U64)_InterlockedCompareExchange64((volatile __int64*)dst, (__int64)(expected - value), (__int64)expected);
        if (old == expected)
            return old - value;
        expected = old;
    }
#endif
#endif
}

internal force_inline U64 u64_atomic_fetch_sub(volatile U64* dst, U64 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_fetch_sub(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
#if ARCH_X64 || ARCH_ARM64
    return (U64)_InterlockedExchangeAdd64((volatile __int64*)dst, -(__int64)value);
#elif ARCH_X86 || ARCH_ARM
    U64 expected = *dst;
    for (;;) {
        U64 old = (U64)_InterlockedCompareExchange64((volatile __int64*)dst, (__int64)(expected - value), (__int64)expected);
        if (old == expected)
            return old;
        expected = old;
    }
#endif
#endif
}

internal force_inline U64 u64_atomic_fetch_and(volatile U64* dst, U64 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_fetch_and(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
#if ARCH_X64 || ARCH_ARM64
    return (U64)_InterlockedAnd64((volatile __int64*)dst, (__int64)value);
#elif ARCH_X86 || ARCH_ARM
    U64 expected = *dst;
    for (;;) {
        U64 old = (U64)_InterlockedCompareExchange64((volatile __int64*)dst, (__int64)(expected & value), (__int64)expected);
        if (old == expected)
            return old;
        expected = old;
    }
#endif
#endif
}

internal force_inline U64 u64_atomic_fetch_or(volatile U64* dst, U64 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_fetch_or(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
#if ARCH_X64 || ARCH_ARM64
    return (U64)_InterlockedOr64((volatile __int64*)dst, (__int64)value);
#elif ARCH_X86 || ARCH_ARM
    U64 expected = *dst;
    for (;;) {
        U64 old = (U64)_InterlockedCompareExchange64((volatile __int64*)dst, (__int64)(expected | value), (__int64)expected);
        if (old == expected)
            return old;
        expected = old;
    }
#endif
#endif
}

internal force_inline U64 u64_atomic_fetch_xor(volatile U64* dst, U64 value) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_fetch_xor(dst, value, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
#if ARCH_X64 || ARCH_ARM64
    return (U64)_InterlockedXor64((volatile __int64*)dst, (__int64)value);
#elif ARCH_X86 || ARCH_ARM
    U64 expected = *dst;
    for (;;) {
        U64 old = (U64)_InterlockedCompareExchange64((volatile __int64*)dst, (__int64)(expected ^ value), (__int64)expected);
        if (old == expected)
            return old;
        expected = old;
    }
#endif
#endif
}

internal force_inline U64 u64_atomic_exchange(volatile U64* dst, U64 desired) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_exchange_n(dst, desired, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return (U64)_InterlockedExchange64((volatile __int64*)dst, (__int64)desired);
#endif
}

internal force_inline void* ptr_atomic_load(void* volatile* src) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_load_n(src, __ATOMIC_ACQUIRE);
#elif COMPILER_MSVC
    void* res = *src;
    atomic_barrier();
    return res;
#endif
}

internal force_inline void ptr_atomic_store(void* volatile* dst, void* val) {
#if COMPILER_GCC || COMPILER_CLANG
    __atomic_store_n(dst, val, __ATOMIC_RELEASE);
#elif COMPILER_MSVC
    atomic_barrier();
    *dst = val;
#endif
}

internal force_inline void* ptr_atomic_exchange(void* volatile* dst, void* desired) {
#if COMPILER_GCC || COMPILER_CLANG
    return __atomic_exchange_n(dst, desired, __ATOMIC_SEQ_CST);
#elif COMPILER_MSVC
    return _InterlockedExchangePointer(dst, desired);
#endif
}

internal force_inline void* ptr_atomic_cas(void* volatile* dst, void* expected, void* desired) {
#if COMPILER_GCC || COMPILER_CLANG
    void* old_val = expected;
    __atomic_compare_exchange_n(dst, &old_val, desired, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return old_val;
#elif COMPILER_MSVC
    return _InterlockedCompareExchangePointer(dst, desired, expected);
#endif
}

internal force_inline void* ptr_atomic_cas_weak(void* volatile* dst, void* expected, void* desired) {
#if COMPILER_GCC || COMPILER_CLANG
    void* old_val = expected;
    __atomic_compare_exchange_n(dst, &old_val, desired, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return old_val;
#elif COMPILER_MSVC
    return _InterlockedCompareExchangePointer(dst, desired, expected);
#endif
}

internal force_inline U8 type_get_size(Type_Indicator type) {
    return (type & 0x0FU);
}

internal force_inline U8 type_is_void(Type_Indicator type) {
    return (type == TYPE_VOID);
}

internal force_inline U8 type_is_signed(Type_Indicator type) {
    return (type) ? bit_is_set(type, TYPE_SIGN_BIT) : 0;
}

internal force_inline U8 type_is_integer(Type_Indicator type) {
    return (type) ? bit_is_set(type, TYPE_INTEGER_BIT) : 0;
}

internal force_inline U8 type_is_float(Type_Indicator type) {
    return (type) ? !bit_is_set(type, TYPE_INTEGER_BIT) : 0;
}

internal force_inline U8 type_is_unsigned(Type_Indicator type) {
    return (type) ? !bit_is_set(type, TYPE_SIGN_BIT) : 0;
}