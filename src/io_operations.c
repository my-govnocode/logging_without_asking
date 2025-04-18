/**
 * Overriding standard I/O operations from libc
 */

#include "include/io_operations.h"

int open(const char *pathname, int flags, ...)
{
    open_orig = open_orig ? open_orig : dlsym(RTLD_NEXT, "open");

    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }

    if (is_disable_log()) {
        return open_orig(pathname, flags, mode);
    }

    int result = open_orig(pathname, flags, mode);

    char message[MESSAGE_SIZE];

    sprintf(message, "(open) pathname: %s; flags: %d; result: %d", pathname, flags, result);

    log_reg(&log_queue, message, IO_OPERATION);
    
    return result;
}


ssize_t read(int fd, void *buf, size_t count)
{
    read_orig = read_orig ? read_orig : dlsym(RTLD_NEXT, "read");

    if (is_disable_log()) {
        return read_orig(fd, buf, count);
    }

    ssize_t result = read_orig(fd, buf, count);

    char message[MESSAGE_SIZE];

    sprintf(
        message,
        "(read) file_descriptor: %d; buffer_pointer: %p; count: %ld; bytes_read: %ld",
        fd,
        buf,
        count,
        result
    );

    log_reg(&log_queue, message, IO_OPERATION);

    return result;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    write_orig = write_orig ? write_orig : dlsym(RTLD_NEXT, "write");

    if (is_disable_log()) {
        return write_orig(fd, buf, count);
    }

    ssize_t result = write_orig(fd, buf, count);

    char message[MESSAGE_SIZE];

    sprintf(
        message,
        "(write) file_descriptor: %d; buffer_pointer: %p; count: %ld; bytes_written: %ld",
        fd,
        buf,
        count,
        result
    );

    log_reg(&log_queue, message, IO_OPERATION);

    return result;
}

int close(int fd)
{
    close_orig = close_orig ? close_orig : dlsym(RTLD_NEXT, "close");

    if (is_disable_log()) {
        return close_orig(fd);
    }

    int result = close_orig(fd);

    char message[MESSAGE_SIZE];

    sprintf(
        message,
        "(close) file_descriptor: %d; return_code: %d",
        fd,
        result
    );

    log_reg(&log_queue, message, IO_OPERATION);

    return result;
}

off_t lseek(int fd, off_t offset, int whence)
{
    lseek_orig = lseek_orig ? lseek_orig : dlsym(RTLD_NEXT, "lseek");

    if (is_disable_log()) {
        return lseek_orig(fd, offset, whence);
    }

    off_t result = lseek_orig(fd, offset, whence);

    char message[MESSAGE_SIZE];

    sprintf(
        message,
        "(lseek) file_descriptor: %d; requested_offset: %ld; whence: %d; resulted_offset: %ld",
        fd,
        offset,
        whence,
        result
    );

    log_reg(&log_queue, message, IO_OPERATION);

    return result;
}