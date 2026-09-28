internal Timer timer_create(void) {
    Timer timer;

    timer.ticks         = ticks_now();
    timer.delta         = 1.0/60.0;
    timer.ticks_pre_sec = system_info()->microsecond_resolution;
    timer.us_per_tick   = 1000000.0 / (F64)timer.ticks_pre_sec;

    return timer;
}

internal void timer_update(Timer *timer) {
    U64 current_ticks;
    U64 elapsed_ticks;
    F64 us;

    current_ticks = ticks_now();
    elapsed_ticks = current_ticks - timer->ticks;
    us = elapsed_ticks * timer->us_per_tick;

    timer->delta = us;
    timer->ticks = current_ticks;
}

internal U64 timer_get_timestamp_us(Timer* timer) {
    return (U64)(timer->ticks * timer->us_per_tick);
}

internal U32 timer_get_timestamp_ms(Timer* timer) {
    return (U32)(timer_get_timestamp_us(timer) / 1000ULL);
}