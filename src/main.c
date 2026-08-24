#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../processflow.h"

int process_line(char *line) {
    line[strcspn(line, "\n")] = '\0';

    if (strcmp(line, "exit") == 0) {
        return 1;
    }

    if (strlen(line) == 0) {
        return 0;
    }

    printf("[debug] comando recebido: %s\n", line);

    return 0;
}

int run_interactive(void) {
    char line[MAX_LINE];

    while (1) {
        printf("processflow> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }

        if (process_line(line) == 1) {
            break;
        }
    }

    return 0;
}

int run_workflow(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Erro: não foi possível abrir o arquivo workflow '%s'\n", filename);
        return 1;
    }

    char line[MAX_LINE];
    int should_exit = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
        if (line[strlen(line) - 1] != '\n') {
            printf("\n");
        }

        if (process_line(line) == 1) {
            should_exit = 1;
            break;
        }
    }

    if (!should_exit) {
        fprintf(stderr, "Aviso: arquivo workflow terminou sem o comando 'exit'\n");
    }

    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Uso: %s [workflowFile]\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        return run_workflow(argv[1]);
    }

    return run_interactive();
}