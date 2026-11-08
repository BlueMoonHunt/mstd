#include "base_type.c"
#include "base_memory.c"
#include "base_varray.c"
#include "base_string.c"
#include "base_state.c"
#include "base_thread.c"
#include "base_time.c"
#include "base_ring_buffer.c"

#if OS_WINDOWS
#include "../win32/base/w32_base.c"
#endif