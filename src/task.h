#ifndef TASK_H
#define TASK_H

#include "../processflow.h"

void task_init(void);
int task_register(int argc, char *argv_tokens[]);
Task *task_find(const char *name);

int task_set_input(int argc, char *argv_tokens[]);
int task_set_output(int argc, char *argv_tokens[]);
int task_set_append(int argc, char *argv_tokens[]);

#endif