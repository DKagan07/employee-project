#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/common.h"
// #include "../include/file.h"
#include "../include/parse.h"

// void list_employees(struct dbheader_t* dbhdr, struct employee_t* employees) { }
//
// int add_employee(struct dbheader_t* dbhdr, struct employee_t* employees,
//     char* addstring) { }
//
// int read_employees(int fd, struct dbheader_t* dbhdr,
//     struct employee_t** employeesOut) { }

int output_file(int fd, struct dbheader_t* dbhdr, struct employee_t* employees)
{
    if (fd < 0) {
        perror("output_file::Got a bad fd from user\n");
        return STATUS_ERROR;
    }
    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->filesize = htonl(dbhdr->filesize);
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, dbhdr, sizeof(struct dbheader_t));

    return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t** headerOut)
{
    if (fd < 0) {
        perror("validate_db_header::Got a bad fd from user\n");
        return STATUS_ERROR;
    }

    struct dbheader_t* header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        perror("validate_db_header::failed to calloc\n");
        return STATUS_ERROR;
    }

    if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
        perror("validate_db_header::read");
        free(header);
        return STATUS_ERROR;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if (header->version != 1) {
        perror("validate_db_header::Improper header verion\n");
        free(header);
        return STATUS_ERROR;
    }
    if (header->magic != HEADER_MAGIC) {
        perror("validate_db_header::Improper header magic\n");
        free(header);
        return STATUS_ERROR;
    }

    struct stat dbstat = { 0 };
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size) {
        perror("validate_db_header::Corrupted database\n");
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header;

    return STATUS_SUCCESS;
}

int create_db_header(int fd, struct dbheader_t** headerOut)
{
    int header_size = sizeof(struct dbheader_t);
    struct dbheader_t* header = calloc(1, header_size);
    if (header == NULL) {
        perror("create_db_header::failed to calloc\n");
        return STATUS_ERROR;
    }

    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = header_size;

    *headerOut = header;

    return STATUS_SUCCESS;
}
