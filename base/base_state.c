internal force_inline void cpu_relax(void) {
#if COMPILER_GCC || COMPILER_CLANG
#if ARCH_X64 || ARCH_X86
    __builtin_ia32_pause();
#elif ARCH_ARM || ARCH_ARM64
    __asm__ __volatile__("yield");
#endif
#elif COMPILER_MSVC
#if ARCH_X64 || ARCH_X86
    _mm_pause();
#elif ARCH_ARM || ARCH_ARM64
    __yield();
#endif
#endif
}