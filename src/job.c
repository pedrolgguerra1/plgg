#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "job.h"
#include "task.h"
#include "executor.h"

static Job jobs[MAX_JOBS];
static int job_count = 0;
static int next_job_id = 1;

void job_init(void) {
    job_count = 0;
    next_job_id = 1;
}

int job_start(const char *task_name) {
    Task *t = task_find(task_name);
    if (t == NULL) {
        fprintf(stderr, "Erro: tarefa '%s' não encontrada\n", task_name);
        return -1;
    }

    if (job_count >= MAX_JOBS) {
        fprintf(stderr, "Erro: número máximo de jobs em background atingido\n");
        return -1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Erro: falha ao criar processo em background para a tarefa '%s'\n", task_name);
        return -1;
    }

    if (pid == 0) {
        execvp(t->argv[0], t->argv);
        fprintf(stderr, "Erro: não foi possível executar o programa '%s'\n", t->argv[0]);
        exit(127);
    }

    Job *j = &jobs[job_count];
    j->job_id = next_job_id;
    j->pid = pid;
    strncpy(j->task_name, task_name, MAX_NAME - 1);
    j->finished = 0;
    j->exit_code = -1;

    printf("[%d] %d\n", j->job_id, j->pid);

    job_count++;
    next_job_id++;
    return 0;
}

void job_list(void) {
    if (job_count == 0) {
        printf("Nenhum job em background.\n");
        return;
    }

    for (int i = 0; i < job_count; i++) {
        Job *j = &jobs[i];

        if (!j->finished) {
            int status;
            pid_t result = waitpid(j->pid, &status, WNOHANG);
            if (result == j->pid) {
                j->finished = 1;
                j->exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
            }
        }

        if (j->finished) {
            printf("[%d] %d  %s  Concluído (código %d)\n", j->job_id, j->pid, j->task_name, j->exit_code);
        } else {
            printf("[%d] %d  %s  Em execução\n", j->job_id, j->pid, j->task_name);
        }
    }
}

int job_wait(int job_id) {
    for (int i = 0; i < job_count; i++) {
        Job *j = &jobs[i];
        if (j->job_id == job_id) {
            if (!j->finished) {
                int status;
                waitpid(j->pid, &status, 0);
                j->finished = 1;
                j->exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
            }
            printf("Job [%d] finalizado com código %d\n", j->job_id, j->exit_code);
            return 0;
        }
    }

    fprintf(stderr, "Erro: job '%d' não encontrado\n", job_id);
    return -1;
}