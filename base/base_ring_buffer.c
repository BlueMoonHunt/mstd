internal RingBuffer* ring_buffer_create(Arena* arena, U64 size) {
    RingBuffer* rb;

    rb = arena_push_type(arena, RingBuffer);
    rb->base = arena_push_array(arena, U8, size);
    rb->size = size;

    return rb;
}

internal U32 ring_buffer_try_read(RingBuffer* ring_buffer, void* data, U64 size) {
    U64 available;
    I32 result;

    available = ring_buffer->write_pos - ring_buffer->read_pos;
    result    = 0;

    if (available >= size) {
        result = 1;

        if (data) {
            ring_buffer->read_pos += mem_read_wrapped(ring_buffer->base, ring_buffer->size, ring_buffer->read_pos, data, size);
        }
    }

    return result;
}

internal U32 ring_buffer_try_write(RingBuffer* ring_buffer, void* data, U64 size) {
    U64 free_space;
    I32 result;

    free_space = ring_buffer->size - (ring_buffer->write_pos - ring_buffer->read_pos);
    result     = 0;

    if (free_space >= size) {
        result = 1;

        if (data) {
            ring_buffer->write_pos += mem_write_wrapped(ring_buffer->base, ring_buffer->size, ring_buffer->write_pos, data, size);
        }
    }

    return result;
}

internal TS_RingBuffer* ts_ring_buffer_create(Arena* arena, U64 size) {
    TS_RingBuffer* rb;

    rb = arena_push_type(arena, TS_RingBuffer);

    rb->ring_buffer = ring_buffer_create(arena, size);
    rb->mutex       = mutex_create();
    rb->cond_var    = cond_var_create();

    return rb;
}

internal void ts_ring_buffer_destroy(TS_RingBuffer* ring_buffer) {
    mutex_destroy(ring_buffer->mutex);
    cond_var_destroy(ring_buffer->cond_var);
}

internal RingBuffer* ts_ring_buffer_open(TS_RingBuffer* rb) {
    mutex_take(rb->mutex);
    return rb->ring_buffer;
}

internal void ts_ring_buffer_close(TS_RingBuffer* rb) {
    mutex_drop(rb->mutex);
}

internal U32 ts_ring_buffer_try_read(TS_RingBuffer* rb, void* data, U64 size) {
    U32 bytes_read;

    bytes_read = ring_buffer_try_read(rb->ring_buffer, data, size);

    if (bytes_read && data) {
        cond_var_wake_all(rb->cond_var);
    }

    return bytes_read;
}

internal U32 ts_ring_buffer_try_write(TS_RingBuffer* rb, void* data, U64 size) {
    U32 bytes_written;

    bytes_written = ring_buffer_try_write(rb->ring_buffer, data, size);

    if (bytes_written && data) {
        cond_var_wake_all(rb->cond_var);
    }

    return bytes_written;
}

internal U32 ts_ring_buffer_read_or_wait(TS_RingBuffer* rb, void* data, U64 size, U64 end_us) {
    U32 done;

    for (done = 0; !done; ) {
        done = ts_ring_buffer_try_read(rb, data, size);

        if (done || time_now_us() >= end_us) {
            break;
        }

        cond_var_wait(rb->cond_var, rb->mutex, end_us);
    }

    return done;
}

internal U32 ts_ring_buffer_write_or_wait(TS_RingBuffer* rb, void* data, U64 size, U64 end_us) {
    U32 done;

    for (done = 0; !done; ) {
        done = ts_ring_buffer_try_write(rb, data, size);

        if (done || time_now_us() >= end_us) {
            break;
        }

        cond_var_wait(rb->cond_var, rb->mutex, end_us);
    }

    return done;
}