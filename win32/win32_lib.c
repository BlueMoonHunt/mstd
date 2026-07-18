internal Handle os_lib_load(Str8 name) {
    Handle handle;
    Str16 name16;
    Arena* scratch;

    arena_scratch_scope(scratch) {
        name16 = str16_from_8(scratch, name);
        HMODULE mod = LoadLibraryW((LPWSTR)name16.data);
        handle.val[0] = (U64)mod;
    }

    return handle;
}

internal void os_lib_unload(Handle handle) {
    FreeLibrary((HMODULE)handle.val[0]);
}

internal void *os_lib_get_symbol(Handle lib, char* name) {
    return (void *)GetProcAddress((HMODULE)lib.val[0], (LPCSTR)name);
}