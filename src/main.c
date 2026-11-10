
#include <stdio.h>
#include <time.h>

#include "hal.h"
#include "battery.h"
#include "fault.h"
#include "state_machine.h"
#include "balancing.h"
#include "scheduler.h"
#include "logger.h"

static Battery g_bat;
static StateMachine g_sm;

static FaultStatus g_prev_faults;
static FaultHistory g_fault_history;

static uint32_t g_now_ms = 0;

/* All possible fault codes */
static const FaultCode ALL_FAULTS[] = {
    FAULT_OVERTEMP,
    FAULT_OVERCURRENT,
    FAULT_OVERVOLTAGE,
    FAULT_UNDERVOLTAGE,
    FAULT_IMBALANCE
};

/* Read battery sensors */
static void task_read_sensors(void) {

    battery_read_sensors(&g_bat);
    battery_calculate(&g_bat);
}

/* Record newly detected faults */
static void record_new_faults(FaultStatus f) {

    uint8_t new_critical =
        f.critical & ~g_prev_faults.critical;

    uint8_t new_warning =
        f.warning & ~g_prev_faults.warning;

    for (unsigned i = 0;
         i < sizeof ALL_FAULTS / sizeof ALL_FAULTS[0];
         i++) {

        FaultCode code = ALL_FAULTS[i];

        if (new_critical & code) {

            fault_history_add(
                &g_fault_history,
                code,
                FAULT_CRITICAL,
                g_now_ms
            );
        }

        if (new_warning & code) {

            fault_history_add(
                &g_fault_history,
                code,
                FAULT_WARNING,
                g_now_ms
            );
        }
    }
}

/* Check battery faults */
static void task_check_faults(void) {

    FaultStatus f = fault_check(&g_bat);

    SystemState old = g_sm.state;

    record_new_faults(f);

    if ((f.critical & ~g_prev_faults.critical) ||
        (f.warning & ~g_prev_faults.warning)) {

        log_event("%s %s DETECTED",
                  fault_name(f.top),
                  f.critical ? "FAULT" : "WARNING");
    }

    sm_update(&g_sm, &f);

    g_prev_faults = f;

    if (g_sm.state != old) {

        log_event("State: %s -> %s",
                  sm_state_name(old),
                  sm_state_name(g_sm.state));

        if (g_sm.state == STATE_SHUTDOWN) {

            hal_sim_set_current(0.0f);

            log_event(
                "SYSTEM SHUTDOWN - pack disconnected"
            );
        }
    }
}

/* Update battery SOC */
static void task_soc(void) {

    if (g_sm.state != STATE_SHUTDOWN)
        battery_update_soc(&g_bat, 1.0f);
}

/* Perform cell balancing */
static void task_balance(void) {

    if (g_sm.state != STATE_MONITORING &&
        g_sm.state != STATE_WARNING)
        return;

    BalanceStatus b = balance_compute(&g_bat);

    if (b.count) {

        balance_apply(&b, 1.0f);

        log_event(
            "Balancing %d cell(s), delta=%.3f V",
            b.count,
            g_bat.delta_v
        );
    }
}

/* Save periodic battery snapshot */
static void task_log(void) {

    log_event(
        "SNAPSHOT state=%s pack=%.2fV avg=%.3fV "
        "dV=%.3fV I=%.1fA T=%.1fC P=%.1fW SOC=%.2f%%",

        sm_state_name(g_sm.state),
        g_bat.pack_v,
        g_bat.avg_v,
        g_bat.delta_v,
        g_bat.current,
        g_bat.temperature,
        g_bat.power,
        g_bat.soc
    );
}

/* Print complete fault history */
static void print_fault_history(void) {

    log_event("========== FAULT HISTORY ==========");

    unsigned count =
        fault_history_count(&g_fault_history);

    if (count == 0) {
        log_event("No faults recorded");
        return;
    }

    for (unsigned i = 0; i < count; i++) {

        FaultRecord record;

        if (fault_history_get(
                &g_fault_history, i, &record)) {

            log_event(
                "Fault: %s | Severity: %s | Time: %u ms",

                fault_name(record.code),

                record.severity == FAULT_CRITICAL
                    ? "CRITICAL"
                    : "WARNING",

                (unsigned)record.timestamp_ms
            );
        }
    }

    log_event("Total fault events: %u", count);
}

/* Main program */
int main(void) {

    hal_init(1, 0);

    log_init("bms_log.txt");

    battery_init(&g_bat, 80.0f);

    sm_init(&g_sm);

    fault_history_init(&g_fault_history);

    Task tasks[] = {

        {"read_sensors", 100, 0, task_read_sensors},

        {"check_faults", 100, 0, task_check_faults},

        {"update_soc", 1000, 0, task_soc},

        {"balance", 1000, 0, task_balance},

        {"save_log", 5000, 0, task_log}
    };

    int n = sizeof tasks / sizeof tasks[0];

    log_event("Battery monitoring started");

    sm_start(&g_sm);

    /* Start performance measurement */
    clock_t start = clock();

    /* Run simulation for 30 seconds */
    for (uint32_t t = 0; t <= 30000; t += 100) {

        g_now_ms = t;

        log_set_time(t);

        /* Temperature rises after 12 seconds */
        if (t >= 12000 &&
            g_sm.state != STATE_SHUTDOWN) {

            hal_sim_set_temperature(
                read_temperature() + 0.5f
            );
        }

        sched_run(tasks, n, t);
    }

    /* Stop performance measurement */
    clock_t end = clock();

    double execution_time =
        (double)(end - start) / CLOCKS_PER_SEC;

    log_event("Simulation finished");

    /* Display recorded fault events */
    print_fault_history();

    /* Display performance */
    log_event(
        "CPU execution time: %.6f seconds",
        execution_time
    );

    log_close();

    return 0;
}
