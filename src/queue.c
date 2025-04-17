#include "include/queue.h"

void init_log(LogQueue **queue)
{
    #ifdef IS_LOGGER
        disable_log = 1;
    #endif

    int fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0755);
    if (fd == -1) {
        perror("Error when shm_open");
        return;
    }

    // Настройка размера shared memory
    if (ftruncate(fd, sizeof(LogQueue)) == -1) {
        perror("Error when ftruncate");
        close(fd);
        return;
    }

    *queue = mmap(0, sizeof(LogQueue), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (*queue == MAP_FAILED) {
        perror("Error when mmap");
        close(fd);
        return;
    }

    if ((*queue)->is_init == 0) {
        pthread_spin_init(&(*queue)->write_lock, PTHREAD_PROCESS_SHARED);
        (*queue)->is_init = 1;
    }

    // memset(*queue, 0, sizeof(LogQueue));
    // (*queue)->head = 0;
    // (*queue)->tail = 0;

    close(fd);

    #ifdef IS_LOGGER
        disable_log = 0;
    #endif
}

int log_put(LogQueue *queue, LogElement element)
{
    unsigned next;

    pthread_spin_lock(&queue->write_lock);

    next = queue->tail + 1;
    printf("%d\n", next);
    if (next >= QUEUE_VOLUME) {
        next = 0;
    }

    if (next == queue->head) {
        pthread_spin_unlock(&queue->write_lock);
        return -1;
    }

    queue->elements[queue->tail] = element;
    queue->tail = next;
    pthread_spin_unlock(&queue->write_lock);

    return 0;
}

int log_get(LogQueue *queue, LogElement *element)
{
    unsigned next;

    pthread_spin_lock(&queue->write_lock);

    if (queue->head == queue->tail) {
        pthread_spin_unlock(&queue->write_lock);
        return -1;
    }

    next = queue->head + 1;
    if(next >= QUEUE_VOLUME) {
        next = 0;
    }

    *element = queue->elements[queue->head];

    queue->head = next;
    pthread_spin_unlock(&queue->write_lock);

    return 0;
}

int log_is_empty(LogQueue *queue)
{
    return queue->tail == queue->head;
}

void log_reg(LogQueue **queue, char *message, enum LogType type)
{
    #ifdef IS_LOGGER
        disable_log = 1;
    #endif

    if (*queue == NULL) {
        init_log(queue);
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    uint64_t timestamp = (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

    LogElement element;

    memset(&element, '\0', sizeof(LogElement));

    element.timestamp = timestamp;
    strncpy(element.message, message, sizeof(element.message));
    element.type = type;

    char program_name[PROG_NAME_SIZE] = "unknown";

    //get_program_name(program_name, PROG_NAME_SIZE);
    strncpy(element.prog_name, program_name, sizeof(element.prog_name));

    element.pid = getpid();

    printf("head: %d, tail %d --%s\n", (*queue)->head, (*queue)->tail, element.message);

    log_put(*queue, element);

    #ifdef IS_LOGGER
        disable_log = 0;
    #endif
}

static void get_program_name(char *buffer, ssize_t size)
{
    #ifdef IS_LOGGER
        disable_log = 1;
    #endif

    int fd = open("/proc/self/comm", O_RDONLY);
    if (fd == -1) {
        perror("open /proc/self/comm");
        strcpy(buffer, "unknown");
        return; // Не забывайте возвратить, чтобы избежать дальнейшего выполнения
    }

    int bytes_read = read(fd, buffer, size - 1); // Записываем сюда size - 1
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
    } else {
        strcpy(buffer, "unknown");
    }

    close(fd);

    #ifdef IS_LOGGER
        disable_log = 0;
    #endif
}