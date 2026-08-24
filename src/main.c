#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../processflow.h"
#include "parser.h"
#include "task.h"
#include "executor.h"

int dispatch(int argc, char *argv[]) {
    if (strcmp(argv[0], "task") == 0) {
        task_register(argc, argv);
        return 0;
    }

    if (strcmp(argv[0], "run") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Erro: uso correto é 'run sequential|parallel|pipe <tarefa1> [tarefa2...]'\n");
            return 0;
        }

        if (strcmp(argv[1], "sequential") == 0) {
            executor_run_sequential(argc - 2, &argv[2]);
        } else if (strcmp(argv[1], "parallel") == 0) {
            executor_run_parallel(argc - 2, &argv[2]);
        } else if (strcmp(argv[1], "pipe") == 0) {
            executor_run_pipe(argc - 2, &argv[2]);
        } else {
            fprintf(stderr, "Erro: modo de execução '%s' não reconhecido\n", argv[1]);
        }
        return 0;
    }

    fprintf(stderr, "Erro: comando '%s' não reconhecido\n", argv[0]);
    return 0;
}

int process_line(char *line) {
    line[strcspn(line, "\n")] = '\0';

    if (strlen(line) == 0) {
        return 0;
    }

    char *argv[MAX_ARGS];
    char line_copy[MAX_LINE];
    strncpy(line_copy, line, MAX_LINE - 1);
    line_copy[MAX_LINE - 1] = '\0';

    int argc = parse_line(line_copy, argv);

    if (argc == 0) {
        return 0;
    }

    if (strcmp(argv[0], "exit") == 0) {
        return 1;
    }

    dispatch(argc, argv);
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
    task_init();

    if (argc > 2) {
        fprintf(stderr, "Uso: %s [workflowFile]\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        return run_workflow(argv[1]);
    }

    return run_interactive();
}