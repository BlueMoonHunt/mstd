#if !defined(BASE_RING_BUFFER_H)
#define BASE_RING_BUFFER_H

typedef struct RingBuffer RingBuffer;
struct RingBuffer {
    U8* base;
    U64 size;
    U64 read_pos;
    U64 write_pos;
};

typedef struct TS_RingBuffer TS_RingBuffer;
struct TS_RingBuffer {
    RingBuffer* ring_buffer;
    Handle mutex;
    Handle cond_var;
};

internal RingBuffer* ring_buffer_create(Arena* arena, U64 size);
internal U32 ring_buffer_try_read(RingBuffer* ring_buffer, void* data, U64 size);
internal U32 ring_buffer_try_write(RingBuffer* ring_buffer, void* data, U64 size);

#define ring_buffer_try_read_struct(ring_buffer, ptr) ring_buffer_try_read((ring_buffer), sizeof(*(ptr)), (ptr))
#define ring_buffer_try_write_struct(ring_buffer, ptr) ring_buffer_try_write((ring_buffer), sizeof(*(ptr)), (ptr))


internal TS_RingBuffer* ts_ring_buffer_create(Arena* arena, U64 size);
internal void ts_ring_buffer_destroy(TS_RingBuffer* ring_buffer);

internal RingBuffer* ts_ring_buffer_open(TS_RingBuffer* rb);
internal void ts_ring_buffer_close(TS_RingBuffer* rb);
internal U32 ts_ring_buffer_try_read(TS_RingBuffer* ring_buffer, void* data, U64 size);
internal U32 ts_ring_buffer_try_write(TS_RingBuffer* ring_buffer, void* data, U64 size);
internal U32 ts_ring_buffer_read_or_wait(TS_RingBuffer* rb, void* data, U64 size, U64 end_us);
internal U32 ts_ring_buffer_write_or_wait(TS_RingBuffer* rb, void* data, U64 size, U64 end_us);

#endif /* BASE_RING_BUFFER_H */