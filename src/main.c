#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/common.h"
#include "../include/file.h"
#include "../include/parse.h"

void print_usage(char* argv[])
{
    printf("Usage: %s -n -f <database_file>\n", argv[0]);
    printf("\t -f  -  (required) path to database file\n");
    printf("\t -n  -  create new database file\n");
    printf("\t -a  -  create new employee\n");
    return;
}

// Flag names:
// -f: filename to our database file
// -n: if a file does  not exist, create a new db file and then perform some action
// -a: to add a new employee
// -d: to delete an employee
// -u: to update an employee

int main(int argc, char* argv[])
{
    char* filepath = NULL;
    bool new_file = false;
    int c = 0;
    int dbfd = -1;
    struct dbheader_t* dbhdr = NULL;
    struct employee_t* employees = NULL;
    char* addstring = NULL;

    while ((c = getopt(argc, argv, "nf:a:")) != -1) {
        switch (c) {
        case 'n':
            new_file = true;
            break;
        case 'f':
            filepath = optarg;
            break;
        case 'a':
            addstring = optarg;
            break;
        case '?':
            printf("Unknown option -%c\n", c);
            break;
        default:
            return -1;
        }
    }

    if (filepath == NULL) {
        printf("Filepath is a required argument\n");
        print_usage(argv);

        return 0;
    }

    if (new_file && filepath != NULL) {
        dbfd = create_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("main::create_db_file\n");
            return -1;
        }

        if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
            printf("main::create_database_header::Failed to create database header\n");
            return -1;
        }
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("main::open_db_file\n");
            return -1;
        }
        if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
            printf("main::validate_database_header::failed to validate database header\n");
            return -1;
        }
    }

    if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
        printf("main::read_employees");
        return 0;
    }

    if (addstring) {
        dbhdr->count++;
        employees = realloc(employees, dbhdr->count * (sizeof(struct employee_t)));
        add_employee(dbhdr, employees, addstring);
    }

    output_file(dbfd, dbhdr, employees);

    printf("\n");
    return 0;
}
