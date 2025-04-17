#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_PATH "test.txt"

int main()
{
    int fd = open(FILE_PATH, O_CREAT | O_WRONLY, 0644);

    if (fd == -1) {
        perror("Error when open file");
        return EXIT_FAILURE;
    }

    char *buffer = (char *)malloc(4);
    sprintf(buffer, "aaa");

    write(fd, buffer, 4);

    close(fd);
    free(buffer);

    return EXIT_SUCCESS;
}