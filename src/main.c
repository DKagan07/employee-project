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
    struct dbheader_t* header = NULL;

    while ((c = getopt(argc, argv, "nf:a:")) != -1) {
        switch (c) {
        case 'n':
            new_file = true;
            break;
        case 'f':
            filepath = optarg;
            break;
        case 'a':
            printf("arg %s\n", optarg);
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
            perror("main::create_db_file\n");
            return -1;
        }

        if (create_db_header(dbfd, &header) == STATUS_ERROR) {
            perror("main::create_database_header::Failed to create database header\n");
            return -1;
        }
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            perror("main::open_db_file\n");
            return -1;
        }
        if (validate_db_header(dbfd, &header) == STATUS_ERROR) {
            perror("main::validate_database_header::failed to validate database header\n");
            return -1;
        }
    }

    output_file(dbfd, header, NULL);

    printf("\n");
    return 0;
}
