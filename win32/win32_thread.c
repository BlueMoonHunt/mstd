typedef struct Win32ThreadCtx {
    HANDLE handle;
    DWORD id;
    ThreadEntryPointFn *user_func;
    void *user_data;
} Win32ThreadCtx;

internal DWORD WINAPI os_ThreadTrampoline(LPVOID param) {
    Win32ThreadCtx *ctx = (Win32ThreadCtx *)param;
    if (ctx && ctx->user_func) {
        ctx->user_func(ctx->user_data);
    }
    return 0;
}

internal void os_thread_attach(Thread *thread, ThreadEntryPointFn *func, void *data) {
    Win32ThreadCtx *ctx = (Win32ThreadCtx *)&thread->reserved;
    ctx->user_func = func;
    ctx->user_data = data;
    ctx->handle = CreateThread(NULL, 0, os_ThreadTrampoline, ctx, 0, &ctx->id);
}

internal U32 os_thread_join(Thread *thread) {
    Win32ThreadCtx *ctx = (Win32ThreadCtx *)thread->reserved;
    DWORD result = WaitForSingleObject(ctx->handle, INFINITE);
    CloseHandle(ctx->handle);
    return (result == WAIT_OBJECT_0);
}

internal void os_thread_detach(Thread *thread) {
    Win32ThreadCtx *ctx = (Win32ThreadCtx *)thread->reserved;
    CloseHandle(ctx->handle);
}

internal U32 os_thread_id(void) { return (U32)GetCurrentThreadId(); }

internal void os_thread_sleep(U32 ms) {
    if (ms == 0) {
        SwitchToThread();
        return;
    }
    static HANDLE s_timer = NULL;

    if (!s_timer)
        s_timer = CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (s_timer) {
        LARGE_INTEGER due;
        due.QuadPart = -(LONGLONG)ms * 10000;
        SetWaitableTimer(s_timer, &due, 0, NULL, NULL, FALSE);
        WaitForSingleObject(s_timer, INFINITE);
    }
}

internal void  os_mutex_init(Mutex *mutex) {
    InitializeCriticalSection((LPCRITICAL_SECTION)mutex->reserved);
}

internal void  os_mutex_take(Mutex *mutex) {
    EnterCriticalSection((LPCRITICAL_SECTION)mutex->reserved);
}

internal void  os_mutex_drop(Mutex *mutex) {
    LeaveCriticalSection((LPCRITICAL_SECTION)mutex->reserved);
}

internal void  os_mutex_destroy(Mutex *mutex) {
    DeleteCriticalSection((LPCRITICAL_SECTION)mutex->reserved);
}

internal void os_rw_mutex_init(RWMutex *mutex) {
    InitializeSRWLock((PSRWLOCK)mutex->reserved);
}

internal void os_rw_mutex_take(RWMutex *mutex, U32 write_mode) {
    if (write_mode) {
        AcquireSRWLockExclusive((PSRWLOCK)mutex->reserved);
    } else {
        AcquireSRWLockShared((PSRWLOCK)mutex->reserved);
    }
}

internal void os_rw_mutex_drop(RWMutex *mutex, U32 write_mode) {
    if (write_mode) {
        ReleaseSRWLockExclusive((PSRWLOCK)mutex->reserved);
    } else {
        ReleaseSRWLockShared((PSRWLOCK)mutex->reserved);
    }
}

/* SRWLOCK does not require explicit destruction on Windows. */
internal void os_rw_mutex_destroy(RWMutex *mutex) {
    (void)mutex;
}

internal void os_cond_var_init(CondVar *var) {
    InitializeConditionVariable((PCONDITION_VARIABLE)var->reserved);
}

internal U32 os_cond_var_wait(CondVar *var, Mutex *mutex) {
    BOOL result =
        SleepConditionVariableCS((PCONDITION_VARIABLE)var->reserved, (PCRITICAL_SECTION)mutex->reserved, INFINITE);
    return (U32)result;
}

internal U32 os_cond_var_wait_rw(CondVar *var, RWMutex *mutex, U32 write_mode) {
    ULONG flags = write_mode ? 0 : CONDITION_VARIABLE_LOCKMODE_SHARED;
    BOOL result =
        SleepConditionVariableSRW((PCONDITION_VARIABLE)var->reserved, (PSRWLOCK)mutex->reserved, INFINITE, flags);
    return (U32)result;
}

internal void os_cond_var_signal(CondVar *var) {
    WakeConditionVariable((PCONDITION_VARIABLE)var->reserved);
}

internal void os_cond_var_broadcast(CondVar *var) {
    WakeAllConditionVariable((PCONDITION_VARIABLE)var->reserved);
}

/* CONDITION_VARIABLE does not require explicit destruction on Windows. */
internal void os_cond_var_destroy(CondVar *var) {
    (void)var;
}

internal void  os_semaphore_init(Semaphore *semaphore, U32 initial_count, U32 max_count) {
    HANDLE handle = CreateSemaphoreW(NULL, (LONG)initial_count, (LONG)max_count, NULL);
    *((HANDLE *)semaphore->reserved) = handle;
}

internal U32  os_semaphore_take(Semaphore *semaphore) {
    HANDLE handle = *((HANDLE *)semaphore->reserved);
    DWORD result = WaitForSingleObject(handle, INFINITE);
    return (result == WAIT_OBJECT_0);
}

internal void  os_semaphore_drop(Semaphore *semaphore) {
    HANDLE handle = *((HANDLE *)semaphore->reserved);
    ReleaseSemaphore(handle, 1, NULL);
}

internal void  os_semaphore_destroy(Semaphore *semaphore) {
    HANDLE handle = *((HANDLE *)semaphore->reserved);
    CloseHandle(handle);
}

internal void os_barrier_init(Barrier *barrier, U32 count) {
    InitializeSynchronizationBarrier((LPSYNCHRONIZATION_BARRIER)barrier->reserved, (LONG)count, -1);
}

internal void os_barrier_wait(Barrier *barrier) {
    EnterSynchronizationBarrier((LPSYNCHRONIZATION_BARRIER)barrier->reserved, 0);
}

internal void os_barrier_destroy(Barrier *barrier) {
    DeleteSynchronizationBarrier((LPSYNCHRONIZATION_BARRIER)barrier->reserved);
}

internal void os_atomic_wait_u32(atomic_u32 *addr, U32 expected_value) {
    WaitOnAddress((volatile PVOID)addr, (PVOID)&expected_value, sizeof(U32), INFINITE);
}

internal void os_atomic_wake_single(atomic_u32 *addr) {
    WakeByAddressSingle((PVOID)addr);
}

internal void os_atomic_wake_all(atomic_u32 *addr) {
    WakeByAddressAll((PVOID)addr);
}