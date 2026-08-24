#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
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
        if (t->input_file != NULL) {
            int fd_in = open(t->input_file, O_RDONLY);
            if (fd_in < 0) {
                fprintf(stderr, "Erro: não foi possível abrir o arquivo de entrada '%s'\n", t->input_file);
                exit(126);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (t->output_file != NULL) {
            int flags = O_WRONLY | O_CREAT | (t->append_output ? O_APPEND : O_TRUNC);
            int fd_out = open(t->output_file, flags, 0644);
            if (fd_out < 0) {
                fprintf(stderr, "Erro: não foi possível abrir o arquivo de saída '%s'\n", t->output_file);
                exit(126);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

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

void executor_run_parallel(int count, char *names[]) {
    pid_t pids[MAX_TASKS];
    char *pid_names[MAX_TASKS];
    int started = 0;

    for (int i = 0; i < count; i++) {
        Task *t = task_find(names[i]);
        if (t == NULL) {
            fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", names[i]);
            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Erro: falha ao criar processo para a tarefa '%s'\n", t->name);
            continue;
        }

        if (pid == 0) {
            execvp(t->argv[0], t->argv);
            fprintf(stderr, "Erro: não foi possível executar o programa '%s'\n", t->argv[0]);
            exit(127);
        }

        pids[started] = pid;
        pid_names[started] = t->name;
        started++;
    }

    for (int i = 0; i < started; i++) {
        int status;
        pid_t finished_pid = wait(&status);

        const char *name = "desconhecida";
        for (int j = 0; j < started; j++) {
            if (pids[j] == finished_pid) {
                name = pid_names[j];
                break;
            }
        }

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                fprintf(stderr, "Aviso: tarefa '%s' terminou com código de saída %d\n", name, exit_code);
            }
        }
    }
}

void executor_run_pipe(int count, char *names[]) {
    if (count < 2) {
        fprintf(stderr, "Erro: 'run pipe' precisa de pelo menos 2 tarefas\n");
        return;
    }

    Task *tasks_to_run[MAX_TASKS];
    for (int i = 0; i < count; i++) {
        Task *t = task_find(names[i]);
        if (t == NULL) {
            fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", names[i]);
            return;
        }
        tasks_to_run[i] = t;
    }

    int num_pipes = count - 1;
    int pipes[MAX_TASKS][2];

    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) < 0) {
            fprintf(stderr, "Erro: falha ao criar pipe\n");
            return;
        }
    }

    pid_t pids[MAX_TASKS];

    for (int i = 0; i < count; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Erro: falha ao criar processo para a tarefa '%s'\n", tasks_to_run[i]->name);
            continue;
        }

        if (pid == 0) {
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < num_pipes) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < num_pipes; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(tasks_to_run[i]->argv[0], tasks_to_run[i]->argv);
            fprintf(stderr, "Erro: não foi possível executar o programa '%s'\n", tasks_to_run[i]->argv[0]);
            exit(127);
        }

        pids[i] = pid;
    }

    for (int i = 0; i < num_pipes; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < count; i++) {
        int status;
        waitpid(pids[i], &status, 0);

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                fprintf(stderr, "Aviso: tarefa '%s' terminou com código de saída %d\n", tasks_to_run[i]->name, exit_code);
            }
        }
    }
}