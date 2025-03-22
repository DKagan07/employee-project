#include <stdio.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/file.h"

// create_db_file checks to see if a file already exists, if it does, we fail and return a STATUS_ERROR.
// If the file does not exist, we create the file and return the file descriptor.
int create_db_file(char* filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd != -1) {
        close(fd);
        printf("create_db_file::file already exists\n");
        return STATUS_ERROR;
    }

    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        close(fd);
        perror("create_db_file::open:create\n");
        return STATUS_ERROR;
    }
    return fd;
}

// open_db_file opens an already-existing db file.
int open_db_file(char* filename)
{
    int fd = open(filename, O_RDWR, 0644);
    if (fd == -1) {
        close(fd);
        perror("open_db_file::open\n");
    }

    return fd;
}
