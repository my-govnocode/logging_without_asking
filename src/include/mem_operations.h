#ifndef MEM_OPERATIONS_H
#define MEM_OPERATIONS_H

#define _GNU_SOURCE
#define IS_LOGGER

#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>
#include "queue.h"

extern LogQueue *log_queue;
extern int disable_log;

static void *(*malloc_orig)(size_t size);
static void (*free_orig)(void *ptr);
static void *(*realloc_orig)(void *ptr, size_t size);

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

#endif
