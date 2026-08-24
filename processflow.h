#ifndef PROCESSFLOW_H
#define PROCESSFLOW_H

#define MAX_LINE 1024
#define MAX_ARGS 64
#define MAX_TASKS 128
#define MAX_JOBS 128
#define MAX_NAME 64

typedef struct {
    char name[MAX_NAME];
    char *argv[MAX_ARGS];
    int argc;
    char *input_file;
    char *output_file;
    int append_output;
    char *workdir;
} Task;

typedef struct {
    int job_id;
    pid_t pid;
    char task_name[MAX_NAME];
    int finished;
    int exit_code;
} Job;

#endif