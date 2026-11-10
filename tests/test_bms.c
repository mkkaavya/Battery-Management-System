
#include <stdio.h>
#include <math.h>

#include "../src/battery.h"
#include "../src/fault.h"
#include "../src/state_machine.h"
#include "../src/balancing.h"
#include "../src/hal.h"

static int pass = 0;
static int fail = 0;

#define CHECK(c) do {                         \
    if (c) {                                  \
        pass++;                               \
    } else {                                  \
        fail++;                               \
        printf("FAIL line %d: %s\n",          \
               __LINE__, #c);                 \
    }                                         \
} while (0)

/* Create a test battery */
static Battery make(float v, float t, float i) {

    Battery b;

    battery_init(&b, 50);

    for (int k = 0; k < NUM_CELLS; k++)
        b.cell_v[k] = v;

    b.temperature = t;
    b.current = i;

    battery_calculate(&b);

    return b;
}

/* Battery calculation tests */
static void test_calc(void) {

    Battery b;

    battery_init(&b, 50);

    float v[4] = {
        3.70f, 3.72f, 3.68f, 3.71f
    };

    for (int i = 0; i < 4; i++)
        b.cell_v[i] = v[i];

    b.current = 5;

    battery_calculate(&b);

    CHECK(fabsf(b.pack_v - 14.81f) < 1e-3f);

    CHECK(fabsf(b.avg_v - 3.7025f) < 1e-3f);

    CHECK(fabsf(b.min_v - 3.68f) < 1e-3f);

    CHECK(fabsf(b.max_v - 3.72f) < 1e-3f);

    CHECK(fabsf(b.delta_v - 0.04f) < 1e-3f);

    CHECK(fabsf(b.power - 74.05f) < 1e-2f);
}

/* Fault boundary tests */
static void test_boundaries(void) {

    Battery b;

    b = make(3.7f, 50.0f, 5);
    CHECK(fault_check(&b).warning == 0);

    b = make(3.7f, 50.1f, 5);
    CHECK(fault_check(&b).warning & FAULT_OVERTEMP);

    b = make(3.7f, 60.0f, 5);
    CHECK(!(fault_check(&b).critical & FAULT_OVERTEMP));

    b = make(3.7f, 60.1f, 5);
    CHECK(fault_check(&b).critical & FAULT_OVERTEMP);

    b = make(4.20f, 30, 5);
    CHECK(!(fault_check(&b).warning & FAULT_OVERVOLTAGE));

    b = make(4.21f, 30, 5);
    CHECK(fault_check(&b).warning & FAULT_OVERVOLTAGE);

    b = make(4.25f, 30, 5);
    CHECK(!(fault_check(&b).critical & FAULT_OVERVOLTAGE));

    b = make(4.26f, 30, 5);
    CHECK(fault_check(&b).critical & FAULT_OVERVOLTAGE);

    b = make(2.79f, 30, 5);
    CHECK(fault_check(&b).critical & FAULT_UNDERVOLTAGE);

    b = make(3.7f, 30, 30.1f);
    CHECK(fault_check(&b).critical & FAULT_OVERCURRENT);

    b = make(3.7f, 30, 5);
    CHECK(fault_check(&b).top == FAULT_NONE);
}

/* Fault priority tests */
static void test_priority(void) {

    Battery b = make(4.3f, 65, 35);

    CHECK(
        fault_check(&b).top == FAULT_OVERTEMP
    );
}

/* State machine tests */
static void test_state_machine(void) {

    StateMachine sm;

    sm_init(&sm);

    FaultStatus ok = {0, 0, FAULT_NONE};

    FaultStatus warn = {
        0, FAULT_OVERTEMP, FAULT_OVERTEMP
    };

    FaultStatus crit = {
        FAULT_OVERTEMP, 0, FAULT_OVERTEMP
    };

    sm_update(&sm, &ok);
    CHECK(sm.state == STATE_IDLE);

    sm_start(&sm);
    CHECK(sm.state == STATE_MONITORING);

    sm_update(&sm, &warn);
    CHECK(sm.state == STATE_WARNING);

    sm_update(&sm, &ok);
    CHECK(sm.state == STATE_MONITORING);

    sm_update(&sm, &crit);
    CHECK(sm.state == STATE_FAULT);

    sm_update(&sm, &ok);
    CHECK(sm.state == STATE_WARNING);

    sm_update(&sm, &crit);
    sm_update(&sm, &crit);
    sm_update(&sm, &crit);

    CHECK(sm.state == STATE_SHUTDOWN);

    sm_update(&sm, &ok);
    CHECK(sm.state == STATE_SHUTDOWN);
}

/* Balancing tests */
static void test_balancing(void) {

    Battery b;

    battery_init(&b, 50);

    float v[4] = {
        3.70f, 3.85f, 3.68f, 3.90f
    };

    for (int i = 0; i < 4; i++) {

        b.cell_v[i] = v[i];

        hal_sim_set_cell(i, v[i]);
    }

    battery_calculate(&b);

    BalanceStatus s = balance_compute(&b);

    CHECK(s.count == 2);

    CHECK(s.active[1] && s.active[3]);

    float before = b.delta_v;

    balance_apply(&s, 1.0f);

    battery_read_sensors(&b);

    battery_calculate(&b);

    CHECK(b.delta_v < before);
}

/* SOC tests */
static void test_soc(void) {

    Battery b = make(3.7f, 30, 5);

    b.soc = 50;

    battery_update_soc(&b, 3600);

    CHECK(b.soc == 0.0f);

    b.soc = 50;

    b.current = 0;

    battery_update_soc(&b, 3600);

    CHECK(b.soc == 50.0f);
}

/* Fault history tests */
static void test_fault_history(void) {

    FaultHistory history;

    FaultRecord record;

    fault_history_init(&history);

    CHECK(fault_history_count(&history) == 0);

    fault_history_add(
        &history,
        FAULT_OVERTEMP,
        FAULT_WARNING,
        1000
    );

    fault_history_add(
        &history,
        FAULT_OVERCURRENT,
        FAULT_CRITICAL,
        2000
    );

    CHECK(fault_history_count(&history) == 2);

    CHECK(fault_history_get(&history, 0, &record));

    CHECK(record.code == FAULT_OVERTEMP);

    CHECK(record.severity == FAULT_WARNING);

    CHECK(record.timestamp_ms == 1000);

    CHECK(fault_history_get(&history, 1, &record));

    CHECK(record.code == FAULT_OVERCURRENT);

    CHECK(record.severity == FAULT_CRITICAL);

    CHECK(record.timestamp_ms == 2000);

    CHECK(!fault_history_get(&history, 2, &record));

    fault_history_add(
        &history,
        FAULT_NONE,
        FAULT_WARNING,
        3000
    );

    CHECK(fault_history_count(&history) == 2);

    CHECK(!fault_history_get(NULL, 0, &record));

    CHECK(!fault_history_get(&history, 0, NULL));
}

/* Circular buffer overwrite test */
static void test_fault_history_capacity(void) {

    FaultHistory history;

    FaultRecord record;

    fault_history_init(&history);

    for (unsigned i = 0; i < FAULT_HISTORY_SIZE + 5; i++) {

        fault_history_add(
            &history,
            FAULT_OVERTEMP,
            FAULT_WARNING,
            i * 1000
        );
    }

    CHECK(
        fault_history_count(&history) ==
        FAULT_HISTORY_SIZE
    );

    CHECK(fault_history_get(&history, 0, &record));

    CHECK(
        record.timestamp_ms == 5000
    );

    CHECK(fault_history_get(
        &history,
        FAULT_HISTORY_SIZE - 1,
        &record
    ));

    CHECK(
        record.timestamp_ms ==
        (FAULT_HISTORY_SIZE + 4) * 1000
    );
}

/* Run all tests */
int main(void) {

    test_calc();

    test_boundaries();

    test_priority();

    test_state_machine();

    test_balancing();

    test_soc();

    test_fault_history();

    test_fault_history_capacity();

    printf("\n========== BMS TEST REPORT ==========\n");

    printf("Tests passed: %d\n", pass);

    printf("Tests failed: %d\n", fail);

    printf("Total tests:  %d\n", pass + fail);

    printf("=====================================\n");

    return fail ? 1 : 0;
}
