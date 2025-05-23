/**
 * Overriding standard memory management operations from libc
 */

#include "include/mem_operations.h"

void *malloc(size_t size)
{
    malloc_orig = malloc_orig ? malloc_orig : dlsym(RTLD_NEXT, "malloc");

    if (is_disable_log()) {
        return malloc_orig(size);
    }

    disable_log();

    void *result = malloc_orig(size);

    char message[1024];

    sprintf(
        message,
        "(malloc) bytes_requested: %ld; new_mem_pointer: %p",
        size,
        result
    );

    log_reg(&log_queue, message, MEM_MENEGMENT);

    enable_log();

    return result;
}

void *realloc(void *ptr, size_t size)
{
    realloc_orig = realloc_orig ? realloc_orig : dlsym(RTLD_NEXT, "realloc");

    if (is_disable_log()) {
        return realloc_orig(ptr, size);
    }

    void *result = realloc_orig(ptr, size);

    char message[1024];

    sprintf(
        message,
        "(realloc) bytes_requested: %ld; current_mem_pointer: %p; new_mem_pointer: %p",
        size,
        ptr,
        result
    );

    log_reg(&log_queue, message, MEM_MENEGMENT);

    return result;
}

void free(void *ptr)
{
    free_orig = free_orig ? free_orig : dlsym(RTLD_NEXT, "free");

    if (is_disable_log()) {
        return free_orig(ptr);
    }

    free_orig(ptr);

    char message[1024];

    sprintf(message, "(free) mem_pointer: %p", ptr);

    log_reg(&log_queue, message, MEM_MENEGMENT);
}