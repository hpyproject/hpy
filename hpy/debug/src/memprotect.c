#include "debug_internal.h"

// Implements OS dependent abstraction of memory protection

#ifdef _HPY_POSIX

// On UNIX systems we use mmap and mprotect
// On Windows this should be eventually extended to use
// VirtualAlloc and VirtualProtect

#include <string.h>
#include <sys/mman.h>

void *raw_data_copy(const void* data, HPy_ssize_t size, bool write_protect) {
    void* new_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memcpy(new_ptr, data, size);
    if (write_protect) {
        mprotect(new_ptr, size, PROT_READ);
    }
    return new_ptr;
}

void *raw_data_protect(void* data, HPy_ssize_t size) {
    mprotect(data, size, PROT_NONE);
}

#else

// Generic fallback that should work for any OS with decent C compiler: copy
// the memory and then override it with garbage to "protect" it from reading.

#include <string.h>

void *raw_data_copy(const void* data, HPy_ssize_t size, bool write_protect) {
    void *new_data = malloc(size);
    memcpy(new_data, data, size);
    return new_data;
}

void *raw_data_protect(void* data, HPy_ssize_t size) {
    // Override the data with some garbage in hope that the program will
    // eventually crash or give incorrect result if it reads the garbage
    char val = 0;
    for (HPy_ssize_t i = 0; i < size; ++i) {
        ((char*)data)[i] = 143 + val;
        val = (val + 1) % 10;
    }
}

#endif