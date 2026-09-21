#if !defined(BASE_TIME_H)
#define BASE_TIME_H

typedef struct Timer {
    U64 ticks;
    F32 delta;
    U64 resolution_us;
    F64 inverse_ticks_per_us;
} Timer;

internal Timer timer_start(void);
internal void timer_update(Timer* timer);
internal U64 timer_get_timestamp(Timer* timer);

internal U64 time_now_us(void);

internal U64 ticks_now(void);

#endif /* BASE_TIME_H */