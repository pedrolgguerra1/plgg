#ifndef JOB_H
#define JOB_H

#include "../processflow.h"

void job_init(void);

int job_start(const char *task_name);

void job_list(void);

int job_wait(int job_id);

#endif