#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// #include "common.h"
// #include "file.h"
// #include "parse.h"

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
        printf("need to create new file!");
    }

    printf("Newfile: %d\n", new_file);
    printf("Filepath: %s\n", filepath);

    return 0;
}
