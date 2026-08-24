#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "../processflow.h"

int executor_run_single(Task *t);

void executor_run_sequential(int count, char *names[]);

#endif