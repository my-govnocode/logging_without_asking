#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include "include/queue.h"
#include "include/listener.h"

#define IO_LOGS_DEFAULT "logs/io.log"
#define MEM_LOGS_DEFAULT "logs/mem.log"

int running = 1;

void sig_handler(int sig)  { 
    running = 0; 
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Не достаточно аргументов: частота проверки (сек), файл лог io, файл логов mem\n");
        return EXIT_FAILURE;
    }

    ListenerConf io_conf, mem_conf;

    io_conf.type = IO_OPERATION;
    io_conf.frequency_of_checking = (unsigned)(argv[1] - '0');
    strcpy(io_conf.log_file, argv[2]);

    mem_conf.type = MEM_MENEGMENT;
    mem_conf.frequency_of_checking = (unsigned)(argv[1] - '0');
    strcpy(mem_conf.log_file, argv[3]);

    printf("%s %s\n", io_conf.log_file, mem_conf.log_file);

    if (daemonize(IO_PID_FILE) == 0) {
        signal(SIGINT, sig_handler);
        signal(SIGTERM, sig_handler);

        pthread_t io_listener_pthread_id = 0;

        if (pthread_create(&io_listener_pthread_id, NULL, listen, &io_conf) != 0) {
            perror("Ошибка при создании потока");
            return EXIT_FAILURE;
        }

        pthread_join(io_listener_pthread_id, NULL);

        if (remove(IO_PID_FILE) != 0) {
            perror("Ошибка при удалении файла для хранения pid\n");
            return EXIT_FAILURE;
        };

        return EXIT_SUCCESS;
    }

    if (daemonize(MEM_PID_FILE) == 0) {
        signal(SIGINT, sig_handler);
        signal(SIGTERM, sig_handler);

        pthread_t io_listener_pthread_id = 0;

        if (pthread_create(&io_listener_pthread_id, NULL, listen, &mem_conf) != 0) {
            perror("Ошибка при создании потока");
            return EXIT_FAILURE;
        }

        pthread_join(io_listener_pthread_id, NULL);

        if (remove(MEM_PID_FILE) != 0) {
            perror("Ошибка при удалении файла для хранения pid\n");
            return EXIT_FAILURE;
        };

        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}