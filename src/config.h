#ifndef CONFIG_H
#define CONFIG_H

#define NUM_CELLS            4
#define CAPACITY_AH          2.5f

/* Illustrative thresholds - real limits come from the cell datasheet */
#define CELL_OV_CRIT         4.25f
#define CELL_OV_WARN         4.20f
#define CELL_UV_CRIT         2.80f
#define CELL_UV_WARN         3.00f
#define TEMP_CRIT            60.0f
#define TEMP_WARN            50.0f
#define CURRENT_CRIT         30.0f
#define CURRENT_WARN         20.0f
#define IMBALANCE_WARN       0.10f   /* max-min cell voltage (V) */

#define BALANCE_THRESHOLD    0.03f   /* V above average -> bleed  */
#define BALANCE_RATE         0.02f   /* V per second per cell     */
#define SHUTDOWN_TICKS       3       /* fault ticks before SHUTDOWN */

#endif
