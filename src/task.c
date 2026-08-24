#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"

static Task tasks[MAX_TASKS];
static int task_count = 0;

void task_init(void) {
    task_count = 0;
}

int task_register(int argc, char *argv_tokens[]) {
    if (argc < 3) {
        fprintf(stderr, "Erro: uso correto é 'task <nome> <programa> [argumentos...]'\n");
        return -1;
    }

    if (task_count >= MAX_TASKS) {
        fprintf(stderr, "Erro: número máximo de tarefas atingido\n");
        return -1;
    }

    Task *t = &tasks[task_count];
    memset(t, 0, sizeof(Task));

    strncpy(t->name, argv_tokens[1], MAX_NAME - 1);

    int prog_argc = 0;
    for (int i = 2; i < argc; i++) {
        t->argv[prog_argc] = strdup(argv_tokens[i]);
        prog_argc++;
    }
    t->argv[prog_argc] = NULL;
    t->argc = prog_argc;

    t->input_file = NULL;
    t->output_file = NULL;
    t->append_output = 0;
    t->workdir = NULL;

    task_count++;
    printf("Tarefa '%s' cadastrada com sucesso.\n", t->name);
    return 0;
}

Task *task_find(const char *name) {
    for (int i = 0; i < task_count; i++) {
        if (strcmp(tasks[i].name, name) == 0) {
            return &tasks[i];
        }
    }
    return NULL;
}

int task_set_input(int argc, char *argv_tokens[]) {
    if (argc != 3) {
        fprintf(stderr, "Erro: uso correto é 'input <tarefa> <arquivo>'\n");
        return -1;
    }

    Task *t = task_find(argv_tokens[1]);
    if (t == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", argv_tokens[1]);
        return -1;
    }

    if (t->input_file != NULL) {
        free(t->input_file);
    }
    t->input_file = strdup(argv_tokens[2]);
    return 0;
}

int task_set_output(int argc, char *argv_tokens[]) {
    if (argc != 3) {
        fprintf(stderr, "Erro: uso correto é 'output <tarefa> <arquivo>'\n");
        return -1;
    }

    Task *t = task_find(argv_tokens[1]);
    if (t == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", argv_tokens[1]);
        return -1;
    }

    if (t->output_file != NULL) {
        free(t->output_file);
    }
    t->output_file = strdup(argv_tokens[2]);
    t->append_output = 0;
    return 0;
}

int task_set_append(int argc, char *argv_tokens[]) {
    if (argc != 3) {
        fprintf(stderr, "Erro: uso correto é 'append <tarefa> <arquivo>'\n");
        return -1;
    }

    Task *t = task_find(argv_tokens[1]);
    if (t == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", argv_tokens[1]);
        return -1;
    }

    if (t->output_file != NULL) {
        free(t->output_file);
    }
    t->output_file = strdup(argv_tokens[2]);
    t->append_output = 1;
    return 0;
}