#include "include/mem_operations.h"

void *malloc(size_t size)
{
    malloc_orig = dlsym(RTLD_NEXT, "malloc");

    if (disable_log) {
        return malloc_orig(size);
    }

    disable_log = 1;

    void *result = malloc_orig(size);

    char message[1024];

    sprintf(
        message,
        "(malloc) bytes_requested: %d; new_mem_pointer: %p",
        size,
        result
    );

    log_reg(&log_queue, message, MEM_MENEGMENT);

    disable_log = 0;

    return result;
}

void *realloc(void *ptr, size_t size)
{
    realloc_orig = dlsym(RTLD_NEXT, "realloc");

    if (disable_log) {
        return realloc_orig(ptr, size);
    }

    void *result = realloc_orig(ptr, size);

    char message[1024];

    sprintf(
        message,
        "(realloc) bytes_requested: %d; current_mem_pointer: %p; new_mem_pointer: %p",
        size,
        ptr,
        result
    );

    log_reg(&log_queue, message, MEM_MENEGMENT);

    return result;
}

void free(void *ptr)
{
    free_orig = dlsym(RTLD_NEXT, "free");

    if (disable_log) {
        return free_orig(ptr);
    }

    free_orig(ptr);

    char message[1024];

    sprintf(message, "(free) mem_pointer: %p", ptr);

    log_reg(&log_queue, message, MEM_MENEGMENT);
}