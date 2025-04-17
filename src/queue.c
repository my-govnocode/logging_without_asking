/**
 * Logging functionality
 */

#include "include/queue.h"

int disable_log = 0;

void init_log(LogQueue **queue)
{
    disable_log = 1;

    int fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0755);
    if (fd == -1) {
        perror("Error when shm_open");
        goto exit;
    }

    if (ftruncate(fd, sizeof(LogQueue)) == -1) {
        perror("Error when ftruncate");
        goto close;

    }

    *queue = mmap(0, sizeof(LogQueue), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (*queue == MAP_FAILED) {
        perror("Error when mmap");
        goto close;
    }

    if ((*queue)->io_channel.is_init == 0) {
        pthread_spin_init(&(*queue)->io_channel.write_lock, PTHREAD_PROCESS_SHARED);
        (*queue)->io_channel.is_init = 1;
    }

    if ((*queue)->mem_channel.is_init == 0) {
        pthread_spin_init(&(*queue)->mem_channel.write_lock, PTHREAD_PROCESS_SHARED);
        (*queue)->mem_channel.is_init = 1;
    }

    close:
    close(fd);

    exit:
    disable_log = 0;
}

int log_put(LogQueue *queue, LogElement element)
{
    unsigned next;

    LogChannel *channel = get_channel_by_type(queue, element.type);

    if (channel == NULL) {
        return -1;
    }

    pthread_spin_lock(&channel->write_lock);

    next = channel->tail + 1;
    if (next >= QUEUE_VOLUME) {
        next = 0;
    }

    if (next == channel->head) {
        unsigned next_head = channel->head + 1;
        if (next_head >= QUEUE_VOLUME) {
            channel->head = 0;
        }

        channel->head++;
    }

    channel->elements[channel->tail] = element;
    channel->tail = next;
    pthread_spin_unlock(&channel->write_lock);

    return 0;
}

int log_get(LogQueue *queue, LogElement *element, enum LogType type)
{
    LogChannel *channel = get_channel_by_type(queue, type);

    if (channel == NULL) {
        return -1;
    }

    unsigned next;

    pthread_spin_lock(&channel->write_lock);

    if (channel->head == channel->tail) {
        pthread_spin_unlock(&channel->write_lock);
        return -1;
    }

    next = channel->head + 1;
    if(next >= QUEUE_VOLUME) {
        next = 0;
    }

    *element = channel->elements[channel->head];

    channel->head = next;
    pthread_spin_unlock(&channel->write_lock);

    return 0;
}

void log_reg(LogQueue **queue, char *message, enum LogType type)
{
    disable_log = 1;

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

    get_program_name(program_name, sizeof(program_name));
    strncpy(element.prog_name, program_name, sizeof(element.prog_name));

    element.pid = getpid();

    log_put(*queue, element);

    disable_log = 0;
}

static void get_program_name(char *buffer, int size)
{
    int fd = open("/proc/self/comm", O_RDONLY);
    if (fd == -1) {
        perror("open /proc/self/comm");
        strcpy(buffer, "unknown");
        return;
    }

    int bytes_read = read(fd, buffer, size - 1);
    if (bytes_read > 0) {
        buffer[bytes_read - 1] = '\0';
    } else {
        strcpy(buffer, "unknown");
    }

    close(fd);
}

LogChannel *get_channel_by_type(LogQueue *queue, enum LogType type)
{
    if (type== IO_OPERATION) {
        return &queue->io_channel;
    } else if (type == MEM_MENEGMENT) {
        return &queue->mem_channel;
    }

    return NULL;
}