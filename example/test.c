#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define TEST_DATA "aaa"
#define SIZE 4
#define FILE_PATH "./tmp/test.txt"

int main()
{
    int fd = open(FILE_PATH, O_CREAT | O_RDWR, 0644);

    if (fd == -1) {
        perror("Error when open file");
        return EXIT_FAILURE;
    }

    lseek(fd, 3, 0);

    char *buffer = (char *)malloc(SIZE);
    sprintf(buffer, TEST_DATA);

    buffer = (char *)realloc(buffer, SIZE + 1);

    write(fd, buffer, SIZE);

    char read_data[SIZE];
    read(fd, read_data, SIZE);

    close(fd);
    free(buffer);

    return EXIT_SUCCESS;
}