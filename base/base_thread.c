internal void thread_entrypoint(ThreadEntryFunction* func, void* data) {
    func(data);
}

internal void spinlock_acquire(volatile U32* lock) {
    U32 expected;
    for (;;) {
        expected = 0;
        if (u32_atomic_cas_weak(lock, &expected, 1)) {
            break;
        }
        cpu_relax();
    }
}

internal void spinlock_release(volatile U32* lock) {
    u32_atomic_store(lock, 0, 0);
}