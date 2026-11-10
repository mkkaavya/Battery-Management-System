#ifndef HAL_H
#define HAL_H
/* Hardware Abstraction Layer: the rest of the code only calls read_*().
   Replace hal.c with real ADC drivers later (e.g., on a TI MCU). */
void  hal_init(unsigned seed, int noise_enabled);
float read_voltage(int cell_id);
float read_temperature(void);
float read_current(void);

/* Simulation-only hooks (not part of a real HAL) */
void  hal_sim_set_cell(int cell_id, float v);
void  hal_sim_bleed(int cell_id, float dv);
void  hal_sim_set_temperature(float t);
void  hal_sim_set_current(float a);
#endif
