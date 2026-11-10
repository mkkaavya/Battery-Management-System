#ifndef BATTERY_H
#define BATTERY_H
#include "config.h"

typedef struct {
    float cell_v[NUM_CELLS];
    float pack_v, avg_v, min_v, max_v, delta_v;
    float current, temperature, power, soc;
} Battery;

void battery_init(Battery *b, float initial_soc);
void battery_read_sensors(Battery *b);          /* via HAL */
void battery_calculate(Battery *b);             /* pack/avg/min/max/delta/power */
void battery_update_soc(Battery *b, float dt_s);/* coulomb counting */
#endif
