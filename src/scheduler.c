#include "scheduler.h"

void sched_run(Task *tasks, int n, uint32_t now_ms) {
    for (int i = 0; i < n; i++) {
        if (now_ms - tasks[i].last_run_ms >= tasks[i].period_ms) {
            tasks[i].last_run_ms = now_ms;
            tasks[i].run();
        }
    }
}
