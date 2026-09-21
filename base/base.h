#if !defined(BASE_H)
#define BASE_H

#include "base_env.h"
#include "base_core.h"

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>

#if COMPILER_MSVC
#include <intrin.h>
#endif

#include "base_type.h"
#include "base_memory.h"
#include "base_array.h"
#include "base_list.h"
#include "base_string.h"
#include "base_state.h"
#include "base_thread.h"
#include "base_file.h"
#include "base_time.h"

#if OS_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <win32/w32_base.h>
#endif

#endif /* BASE_H */