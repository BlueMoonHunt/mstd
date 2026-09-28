#include "base_type.c"
#include "base_memory.c"
#include "base_varray.c"
#include "base_string.c"
#include "base_state.c"
#include "base_thread.c"
#include "base_file.c"
#include "base_file_watcher.c"
#include "base_time.c"

#if OS_WINDOWS
#include <win32/base/w32_base.c>
#endif