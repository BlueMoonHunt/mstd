#if !defined(BASE_TIME_H)
#define BASE_TIME_H

typedef U64 Time; /* micro seconds (unix) */

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

internal U64 ticks_now(void);

internal U64 time_now_ns(void);
internal U64 time_now_us(void);
internal F64 time_now_ms(void);
internal F64 time_now_sec(void);

internal U64 ticks_now_unix(void);

internal U64 time_now_unix_ns(void);
internal U64 time_now_unix_us(void);
internal U64 time_now_unix_ms(void);
internal U64 time_now_unix_sec(void);

#endif /* BASE_TIME_H */