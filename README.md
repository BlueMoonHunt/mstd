# mstd

> A lightweight, cross-platform C library designed to provide a set of standard utilities, data structures, and abstractions.

---

## About

mstd  provides:

* A standard set of tools for C programmers.
* Cross-platform compatibility (Windows, Linux, macOS: in progress).
* Performance optimizations.
  
| Feature | Description |
| :--- | :--- |
| **Memory Management** | Arena allocators, scratch memory, and discereat arrays (`DArray`). |
| **Data Structures** | Linked lists (SLL/DLL), strings (`Str8`, `Str16`, `Str32`). |
| **Platform Abstraction** | Cross-platform support (Windows/Linux/macOS) and compiler detection (MSVC/Clang/GCC). |
| **Unicode Support** | UTF-8/16/32 string handling and conversion. |
| **File I/O** | Basic file operations and memory-mapped file support. |
| **Debugging Tools** | Assertions, traps, and validation macros. |
| **Math Utilities** | Bit manipulation, alignment, and clamping. |
| **Timing** | High-resolution timers and clock utilities. |
| **Thread** | Thread, Mutex, R/W Mutex, Semaphore, Barrier and Condition variable |


## Usage

#### 1) Arena Allocator

```c
#include "mstd.h"

int main() {
    os_state_init();
    Arena* arena = arena_alloc();
    int* numbers = arena_push_array(arena, int, 10);

    for (int i = 0; i < 10; i++)
        numbers[i] = i * 2;

    arena_release(arena);
    return 0;
}

#include "mstd.c"
```
#### 2) String Handling
```c
#include "mstd.h"

int main() {
    os_state_init();
    Arena* arena = arena_alloc(.reserve_size = MB(1));
    Str8 hello = str8_from_cstr("Hello, ");
    Str8 world = str8_from_cstr("world!");
    Str8 greeting = str8_concat(arena, hello, world);

    printf("%.*s\n", (int)greeting.size, greeting.data);
    arena_release(arena);
    return 0;
}

#include "mstd.c"
```

## TODO
* Add custom IO to unify Str8 and Str8View to take away null terminator guarantee in Str8.
* Add Linux layer.
* General Arena with more options. Current one is fix sized does not have customizing options like grow / overflow handeling.
