#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <stdint.h>

typedef struct {
    const char *name;
    uint32_t    period_ms;
    uint32_t    last_run_ms;
    void      (*run)(void);
} Task;

void sched_run(Task *tasks, int n, uint32_t now_ms);
#endif
