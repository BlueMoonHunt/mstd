#if !defined(MSTD2_H)
#define MSTD2_H

#include "core/base_type.c"
#include "core/arena.c"

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