#ifndef IO_LISTENER_H
#define IO_LISTENER_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"

#define IO_PID_FILE "/var/run/io_logd.pid"
#define MEM_PID_FILE "/var/run/mem_logd.pid"

extern int running;

typedef struct {
    char log_file[256];
    unsigned frequency_of_checking;
    enum LogType type;
} ListenerConf;

pid_t daemonize(char *pid_file);
void *listen(void *conf);
int file_exists(char *filename);

#endif