#if !defined(BASE_TIME_H)
#define BASE_TIME_H

typedef struct Timer {
    U64 ticks;
    F64 delta;
    U64 ticks_pre_sec;
    F64 us_per_tick;
} Timer;

internal Timer timer_create(void);
internal void timer_update(Timer* timer);
internal U64 timer_get_timestamp_us(Timer* timer);
internal U32 timer_get_timestamp_ms(Timer* timer);

internal U64 time_now_us(void);

internal U64 ticks_now(void);

#endif /* BASE_TIME_H */