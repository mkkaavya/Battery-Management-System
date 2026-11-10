#include <stdlib.h>
#include "hal.h"
#include "config.h"

static float s_cell[NUM_CELLS] = {3.70f, 3.85f, 3.68f, 3.90f};
static float s_temp = 30.0f;
static float s_current = 5.0f;
static int   s_noise = 0;

static float noise(void) { return ((rand() % 1001) - 500) / 100000.0f; } /* +-0.005 */

void hal_init(unsigned seed, int noise_enabled) { srand(seed); s_noise = noise_enabled; }

float read_voltage(int id) {
    if (id < 0 || id >= NUM_CELLS) return 0.0f;
    return s_cell[id] + (s_noise ? noise() : 0.0f);
}
float read_temperature(void) { return s_temp + (s_noise ? noise() * 100 : 0.0f); }
float read_current(void)     { return s_current; }

void hal_sim_set_cell(int id, float v) { if (id >= 0 && id < NUM_CELLS) s_cell[id] = v; }
void hal_sim_bleed(int id, float dv)   { if (id >= 0 && id < NUM_CELLS) s_cell[id] -= dv; }
void hal_sim_set_temperature(float t)  { s_temp = t; }
void hal_sim_set_current(float a)      { s_current = a; }
