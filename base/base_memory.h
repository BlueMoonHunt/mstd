#if !defined(BASE_MEMORY_H)
#define BASE_MEMORY_H

#if COMPILER_MSVC
#define align_of(T) __alignof(T)
#elif COMPILER_CLANG
#define align_of(T) __alignof(T)
#elif COMPILER_GCC
#define align_of(T) __alignof__(T)
#else
#error align_of not defined for this compiler.
#endif

#if COMPILER_MSVC
#define align_to(x) __declspec(align(x))
#elif COMPILER_CLANG || COMPILER_GCC
#define align_to(x) __attribute__((aligned(x)))
#else
#error align_to not defined for this compiler.
#endif

typedef union Handle {
    U64 val[1];
} Handle;

internal force_inline void* mem_reserve(U64 size, U8 large);
internal force_inline void mem_commit(void* mem, U64 size, U8 large);
internal force_inline void mem_decommit(void* mem, U64 size);
internal force_inline void mem_release(void* mem, U64 size);
internal force_inline U64 mem_page_size(void);

internal force_inline U8 mem_match(void* mem1, void* mem2, U64 size);
internal force_inline void mem_copy(void* dest, void* src, U64 size);
internal force_inline void mem_set(void* mem, U8 byte, U64 size);

#define mem_zero(mem, size)                 mem_set(mem, 0, size)
#define mem_zero_type(mem)                  mem_set(mem, 0, sizeof(*(mem)))
#define mem_zero_array(mem, count)          mem_set(mem, 0, sizeof(*(mem)) * (count))
#define mem_copy_type(dest, src)            mem_copy(dest, src, sizeof(*(dest)))
#define mem_copy_array(dest, src, count)    mem_copy(dest, src, sizeof(*(dest)) * (count))

/* Arena Allocator */

typedef U32 ArenaFlag;

enum {
    ARENA_FLAG_NONE,

    ARENA_FLAG_FORCE_ALIGNMENT_BIT  = 8,
    ARENA_FLAG_FORCE_ALIGNMENT      = bit(ARENA_FLAG_FORCE_ALIGNMENT_BIT),

    ARENA_FLAG_FORCE_ALIGNMENT_1    = ARENA_FLAG_FORCE_ALIGNMENT | bit(0),
    ARENA_FLAG_FORCE_ALIGNMENT_2    = ARENA_FLAG_FORCE_ALIGNMENT | bit(1),
    ARENA_FLAG_FORCE_ALIGNMENT_4    = ARENA_FLAG_FORCE_ALIGNMENT | bit(2),
    ARENA_FLAG_FORCE_ALIGNMENT_8    = ARENA_FLAG_FORCE_ALIGNMENT | bit(3),
    ARENA_FLAG_FORCE_ALIGNMENT_16   = ARENA_FLAG_FORCE_ALIGNMENT | bit(4),
    ARENA_FLAG_FORCE_ALIGNMENT_32   = ARENA_FLAG_FORCE_ALIGNMENT | bit(5),
    ARENA_FLAG_FORCE_ALIGNMENT_64   = ARENA_FLAG_FORCE_ALIGNMENT | bit(6),
    ARENA_FLAG_FORCE_ALIGNMENT_128  = ARENA_FLAG_FORCE_ALIGNMENT | bit(7),

    ARENA_FLAG_LARGE_PAGES          = bit(9),
    ARENA_FLAG_SCRATCH              = bit(10),
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp {
    ArenaTemp* next;
    U64 cursor;
};

typedef struct Arena {
    ArenaFlag flags;
    U32 page_size;
    U64 cursor;
    U64 committed;
    U64 reserved;
    ArenaTemp* temp_stack_tail;
    ArenaTemp* temp_stack_head;
} Arena;

typedef struct ArenaParmas ArenaParmas;
struct ArenaParmas {
    U64 reserve_size;
    U64 commit_size;
    ArenaFlag flags;
};

#define ARENA_HEADER_SIZE           align_up_pow2(sizeof(Arena), 64)

#define ARENA_DEFAULT_RESERVE_SIZE  MB(100)
#define ARENA_DEFAULT_COMMIT_SIZE   KB(40)
#define ARENA_DEFAULT_SCRATCH_COUNT 4

internal Arena* arena_alloc_(ArenaParmas* params);
internal void arena_release(Arena* arena);
internal void arena_reset(Arena* arena);
internal void* arena_push(Arena* arena, U64 size, U64 alignment);

internal void arena_temp_alloc(Arena* arena);
internal void arena_temp_release(Arena* arena);
internal void arena_temp_release_all(Arena* arena);

internal Arena* arena_scratch_alloc_(ArenaParmas* params);
internal void arena_scratch_release(Arena* scratch_arena);

#define arena_alloc(...)                    arena_alloc_(&(ArenaParmas){.reserve_size = ARENA_DEFAULT_RESERVE_SIZE, .commit_size = ARENA_DEFAULT_COMMIT_SIZE, .flags = ARENA_FLAG_NONE, __VA_ARGS__})
#define arena_push_type(arena, T)           arena_push(arena, sizeof(T), align_of(T))
#define arena_push_array(arena, T, count)   arena_push(arena, sizeof(T) * count, align_of(T))

#define arena_scratch_alloc(...)            arena_scratch_alloc_(&(ArenaParmas){.reserve_size = ARENA_DEFAULT_RESERVE_SIZE, .commit_size = ARENA_DEFAULT_COMMIT_SIZE, .flags = ARENA_FLAG_SCRATCH, __VA_ARGS__})
#define arena_scratch_scope(arena)          for (arena = arena_scratch_alloc(); arena; arena_scratch_release(arena), arena = 0)

#endif /* BASE_MEMORY_H */