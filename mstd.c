#if !defined(MSTD2_H)
#define MSTD2_H

#include "core/core.c"

#include "arena/arena.c"
#include "arena/arena_temp.c"
#include "arena/arena_scratch.c"

#include "str/unicode.c"
#include "str/str8.c"
#include "str/str16.c"
#include "str/str32.c"

#include "ds/darray.c"

#include "timer/timer.c"

#if OS_WINDOWS

#include "win32/win32_state.c"
#include "win32/win32_memory.c"
#include "win32/win32_thread.c"
#include "win32/win32_cmd.c"
#include "win32/win32_lib.c"
#include "win32/win32_file.c"

#endif

#endif /* MSTD2_H */