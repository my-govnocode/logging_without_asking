#include "include/listener.h"

pid_t daemonize(char *pid_file)
{
    if (file_exists(pid_file) == 1) {
        printf("Демон уже запущен\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid;

    pid = fork();
    if (pid < 0 || pid > 0)
    {
        return pid;
    }

    if (setsid() < 0)
    {
        exit(EXIT_FAILURE);
    }

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--)
    {
        close(fd);
    }

    int fd = open("/dev/null", O_RDWR);
    if (fd >= 0)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO)
        {
            close(fd);
        }
    }

    int pid_file_fd = open(pid_file, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (pid_file_fd < 0)
    {
        exit(EXIT_FAILURE);
    }

    if (lockf(pid_file_fd, F_TLOCK, 0) < 0)
    {
        exit(EXIT_FAILURE);
    }

    char pid_str[10];
    snprintf(pid_str, sizeof(pid_str), "%d\n", getpid());
    write(pid_file_fd, pid_str, strlen(pid_str));
    close(pid_file_fd);

    return pid;
}

void *listen(void *arg_ptr)
{
    ListenerConf *conf = (ListenerConf *)arg_ptr;

    LogQueue *log_queue;
    memset(log_queue, 0, sizeof(LogQueue));
    init_log(&log_queue);

    int io_log_fd = open(conf->log_file, O_CREAT | O_APPEND | O_WRONLY, 0755);

    if (io_log_fd == -1) {
        perror("Open log file");
        munmap(log_queue, sizeof(LogQueue));
        close(io_log_fd);
        exit(EXIT_FAILURE);
    }

    while (running) {
        LogElement element;
        log_get(log_queue, &element);

        if (element.timestamp != 0 && element.type == conf->type) {
            char log_messsage[2056];

            memset(log_messsage, '\0', sizeof(log_messsage));
            sprintf(log_messsage, "[%lu] pid: %d; program name: \"%s\" %s\n", element.timestamp, element.pid, element.prog_name, element.message);
            int write_len = write(io_log_fd, log_messsage, strlen(log_messsage));
        }

        sleep(conf->frequency_of_checking);
    }

    munmap(log_queue, sizeof(LogQueue));
    close(io_log_fd);

    return NULL;
}

int file_exists(char *filename)
{
    struct stat buffer;   
    return stat(filename, &buffer) == 0;
}