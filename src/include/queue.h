#ifndef QUEUE_H
#define QUEUE_H

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
#include <sys/types.h>
#include <pthread.h>

#define SHARED_MEM_NAME "/log_buffer"
#define QUEUE_VOLUME 1024
#define MESSAGE_SIZE 1024
#define PROG_NAME_SIZE 256

enum LogType {
    IO_OPERATION, MEM_MENEGMENT
};

typedef struct {
    u_int64_t timestamp;
    pid_t pid;
    char prog_name[PROG_NAME_SIZE];
    char message[MESSAGE_SIZE];
    enum LogType type;
} LogElement;

typedef struct {
    LogElement elements[QUEUE_VOLUME];
    unsigned head;
    unsigned tail;
    pthread_spinlock_t write_lock;
    int is_init;
} LogChannel;

typedef struct {
    LogChannel io_channel;
    LogChannel mem_channel;
} LogQueue;

void init_log(LogQueue **queue);
int log_put(LogQueue *queue, LogElement element);
int log_get(LogQueue *queue, LogElement *element, enum LogType type);
void log_reg(LogQueue **queue, char *message, enum LogType type);
void get_program_name(char *buffer, int size);
LogChannel *get_channel_by_type(LogQueue *queue, enum LogType type);
int is_disable_log();
void disable_log();
void enable_log();

#endif