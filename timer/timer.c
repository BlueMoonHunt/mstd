internal Timer timer_start(void) {
    Timer timer;

    timer.ticks = os_ticks_now();
    timer.delta = 16666.6f;
    timer.resolution_us = os_get_system_info()->microsecond_resolution;
    timer.inverse_ticks_per_us = 1000000.0 / (F64)timer.resolution_us;

    return timer;
}

internal void timer_update(Timer *timer) {
    U64 current_ticks;
    U64 elapsed_ticks;
    F64 us;

    current_ticks = os_ticks_now();

    elapsed_ticks = (current_ticks > timer->ticks) ? (current_ticks - timer->ticks) : 0;
    us = (F64)elapsed_ticks * timer->inverse_ticks_per_us;

    timer->delta = (float)us;
    timer->ticks = current_ticks;
}

internal U64 timer_get_timestamp(Timer *timer) {
    return (U64)(timer->ticks * timer->inverse_ticks_per_us);
}