#ifndef IO_OPERATIONS_H
#define IO_OPERATIONS_H

#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>
#include "queue.h"

#define IS_LOGGER

extern LogQueue *log_queue;
extern int disable_log;

static int (*open_orig)(const char *pathname, int flags, ...);
static ssize_t (*read_orig)(int fd, void *buf, size_t count);
static ssize_t (*write_orig)(int fd, const void *buf, size_t count);
static int (*close_orig)(int fd);
static off_t (*lseek_orig)(int fd, off_t offset, int whence);

int open(const char *pathname, int flags, ...);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int close(int fd);
off_t lseek(int fd, off_t offset, int whence);

#endif
