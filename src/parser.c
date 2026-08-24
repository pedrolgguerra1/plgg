#include <string.h>
#include "parser.h"

int parse_line(char *line, char *argv[MAX_ARGS]) {
    int argc = 0;
    char *token = strtok(line, " \t");

    while (token != NULL && argc < MAX_ARGS - 1) {
        argv[argc] = token;
        argc++;
        token = strtok(NULL, " \t");
    }

    argv[argc] = NULL;
    return argc;
}