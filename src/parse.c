#include "../include/parse.h"
#include "../include/common.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void list_employees(struct dbheader_t* dbhdr, struct employee_t* employees)
{
    int i = 0;
    for (; i < dbhdr->count; i++) {
        printf("Employee %d\n", i + 1);
        printf("\tName: %s\n", employees[i].name);
        printf("\tAddress: %s\n", employees[i].address);
        printf("\tHours worked: %d\n", employees[i].hours);
    }
    return;
}

int add_employee(struct dbheader_t* dbhdr, struct employee_t* employees, char* addstring)
{
    char* name = strtok(addstring, ",");
    char* addr = strtok(NULL, ",");
    char* hours = strtok(NULL, ",");

    strncpy(employees[dbhdr->count - 1].name, name, sizeof(employees[dbhdr->count - 1].name));
    strncpy(employees[dbhdr->count - 1].address, addr, sizeof(employees[dbhdr->count - 1].address));
    employees[dbhdr->count - 1].hours = atoi(hours);

    return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t* dbhdr, struct employee_t** employeesOut)
{
    if (fd < 0) {
        perror("read_employees::Got a bad fd from user\n");
        return STATUS_ERROR;
    }

    int count = dbhdr->count;

    struct employee_t* employees = calloc(count, sizeof(struct employee_t));
    if (employees == NULL) {
        printf("read_employees::calloc");
        return STATUS_ERROR;
    }

    read(fd, employees, count * sizeof(struct employee_t));

    int i = 0;
    for (; i < count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;
    return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t* dbhdr, struct employee_t* employees)
{
    if (fd < 0) {
        perror("output_file::Got a bad fd from user\n");
        return STATUS_ERROR;
    }

    int size_count = 0;
    int realcount = dbhdr->count;

    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    lseek(fd, 0, SEEK_SET);

    size_count += sizeof(struct dbheader_t);
    write(fd, dbhdr, sizeof(struct dbheader_t));

    int i = 0;
    for (; i < realcount; i++) {
        employees[i].hours = htonl(employees[i].hours);
        size_count += sizeof(struct employee_t);
        write(fd, &employees[i], sizeof(struct employee_t));
    }

    ftruncate(fd, size_count);

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
    struct dbheader_t* header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        perror("create_db_header::failed to calloc\n");
        return STATUS_ERROR;
    }

    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;

    return STATUS_SUCCESS;
}

int update_employee(int fd, struct dbheader_t* dbhdr, char* updatestring, struct employee_t* employees)
{
    if (fd < 0) {
        printf("update_employee::bad fd\n");
        return STATUS_ERROR;
    }

    bool updated = false;

    char* old_name = strtok(updatestring, ",");
    char* update_name = strtok(NULL, ",");
    char* update_addr = strtok(NULL, ",");
    char* update_hours = strtok(NULL, ",");

    if (old_name == NULL || update_name == NULL || update_addr == NULL || update_hours == NULL) {
        printf("update_employee::not formatted properly\n");
        return STATUS_ERROR;
    }

    int i = 0;
    for (; i < dbhdr->count; i++) {
        if (strcmp(employees[i].name, old_name) == 0) {
            int num_hours = atoi(update_hours);
            strncpy(employees[i].name, update_name, sizeof(employees[i].name));
            strncpy(employees[i].address, update_addr, sizeof(employees[i].address));
            employees[i].hours = atoi(update_hours);
            updated = true;
        }
    }

    if (!updated) {
        printf("no employee to update");
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

int delete_employee(int fd, struct dbheader_t* dbhdr, char* deletestring, struct employee_t* employees)
{
    if (fd < 0) {
        printf("update_employee::bad fd\n");
        return STATUS_ERROR;
    }

    int i = 0;
    int delete_index = -1;

    for (; i < dbhdr->count; i++) {
        if (strcmp(deletestring, employees[i].name) == 0) {
            delete_index = i;
            break;
        }
    }

    if (delete_index == -1) {
        printf("delete_employee::did not find employee\n");
        return STATUS_ERROR;
    }

    // shifting the employees down to the left, overwriting
    for (i = delete_index; i < dbhdr->count - 1; i++) {
        employees[i] = employees[i + 1];
    }

    // the last entry should be a duplicate, we want to free that memory
    if (memset(&employees[dbhdr->count - 1], 0, sizeof(struct employee_t)) == NULL) {
        printf("delete_employee::memset failed\n");
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}
