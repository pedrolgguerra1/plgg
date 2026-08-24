#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "executor.h"
#include "task.h"

int executor_run_single(Task *t) {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Erro: falha ao criar processo para a tarefa '%s'\n", t->name);
        return -1;
    }

    if (pid == 0) {
        execvp(t->argv[0], t->argv);
        fprintf(stderr, "Erro: não foi possível executar o programa '%s'\n", t->argv[0]);
        exit(127);
    }

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0) {
            fprintf(stderr, "Aviso: tarefa '%s' terminou com código de saída %d\n", t->name, exit_code);
        }
        return exit_code;
    }

    return -1;
}

void executor_run_sequential(int count, char *names[]) {
    for (int i = 0; i < count; i++) {
        Task *t = task_find(names[i]);
        if (t == NULL) {
            fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", names[i]);
            continue;
        }
        executor_run_single(t);
    }
}