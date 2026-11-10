#include <string.h>
#include "battery.h"
#include "hal.h"

void battery_init(Battery *b, float initial_soc) {
    memset(b, 0, sizeof *b);
    b->soc = initial_soc;
}

void battery_read_sensors(Battery *b) {
    for (int i = 0; i < NUM_CELLS; i++) b->cell_v[i] = read_voltage(i);
    b->temperature = read_temperature();
    b->current     = read_current();
}

void battery_calculate(Battery *b) {
    float sum = 0.0f;
    b->min_v = b->max_v = b->cell_v[0];
    for (int i = 0; i < NUM_CELLS; i++) {
        sum += b->cell_v[i];
        if (b->cell_v[i] < b->min_v) b->min_v = b->cell_v[i];
        if (b->cell_v[i] > b->max_v) b->max_v = b->cell_v[i];
    }
    b->pack_v  = sum;
    b->avg_v   = sum / NUM_CELLS;
    b->delta_v = b->max_v - b->min_v;
    b->power   = b->pack_v * b->current;
}

void battery_update_soc(Battery *b, float dt_s) {
    /* positive current = discharge */
    b->soc -= (b->current * dt_s) / (CAPACITY_AH * 3600.0f) * 100.0f;
    if (b->soc < 0.0f)   b->soc = 0.0f;
    if (b->soc > 100.0f) b->soc = 100.0f;
}
