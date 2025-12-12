#ifndef MSTD_GRAPHICS_IMPLEMENTATION
#define MSTD_GRAPHICS_IMPLEMENTATION
#include "mstd.h"

typedef enum EventType EventType;
enum EventType {
    event_type_none,
    event_type_window_resize,
    event_type_window_move,
    event_type_key_pressed,
};

typedef enum KeyState KeyState;
enum KeyState {
    key_state_pressed = 1 << 0,
    key_state_released = 1 << 1,
    key_state_repeat = 1 << 2,
};

typedef struct Event Event;
struct Event {
    enum_class(EventType, u32) type;
    union {
        struct { i32 value1; i32 value2; }window;
        struct { i32 code; enum_class(KeyState, u32) state; }key;
    };
};

typedef struct WindowCreateInfo WindowCreateInfo;

/// @brief use 0 for defaults
struct WindowCreateInfo {
    str8 title;
    struct {i32 width, height; } size;
    struct {i32 x, y; } pos;
};

typedef struct Window Window;

Window* window_create(WindowCreateInfo create_info, Arena* arena);
#endif // MSTD_GRAPHICS_IMPLEMENTATION