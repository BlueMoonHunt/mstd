#if !defined(BASE_THREAD_H)
#define BASE_THREAD_H

typedef void ThreadEntryFunction(void* data);

internal void thread_entrypoint(ThreadEntryFunction* func, void* data);

internal Handle thread_create(ThreadEntryFunction* func, void* data);
internal void thread_destroy(Handle thread);
internal U32 thread_join(Handle thread, U64 end_time_us);
internal U32 thread_id(void);
internal void thread_sleep(U64 us);

internal Handle mutex_create(void);
internal void mutex_destroy(Handle mutex);
internal void mutex_take(Handle mutex);
internal void mutex_drop(Handle mutex);

internal Handle rw_mutex_create(void);
internal void rw_mutex_destroy(Handle mutex);
internal void rw_mutex_take(Handle mutex, U32 write_mode);
internal void rw_mutex_drop(Handle mutex, U32 write_mode);

internal Handle cond_var_create(void);
internal void cond_var_destroy(Handle cond_var);
internal U32 cond_var_wait(Handle cond_var, Handle rw_mutex, U64 end_us);
internal U32 cond_var_wait_rw(Handle cond_var, Handle rw_mutex, U32 write_mode, U64 end_us);
internal void cond_var_wake(Handle cond_var);
internal void cond_var_wake_all(Handle cond_var);

#define cond_var_wait_r(var, mutex) cond_var_wait_rw((var), (mutex), (0))
#define cond_var_wait_w(var, mutex) cond_var_wait_rw((var), (mutex), (1))

/* Semaphore */
internal Handle semaphore_create(U32 initial_count, U32 max_count, Str8 name);
internal void semaphore_destroy(Handle semaphore);
internal Handle semaphore_open(Str8 name);
internal void semaphore_close(Handle semaphore);
internal U32 semaphore_take(Handle semaphore, U64 end_us);
internal void semaphore_drop(Handle semaphore, U64 count);

/* Barriers */
internal Handle barrier_create(U32 count);
internal void barrier_destroy(Handle barrier);
internal void barrier_wait(Handle barrier);

/* Atomic wait-on-address */
internal void atomic_wait_U32(volatile U32* addr, U32 expected_value, U64 end_us);
internal void atomic_wake(volatile U32* addr);
internal void atomic_wake_all(volatile U32* addr);

/* Spinlock */
internal void spinlock_acquire(volatile U32* lock);
internal void spinlock_release(volatile U32* lock);

#endif /* BASE_THREAD_H */