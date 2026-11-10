#include "balancing.h"
#include "hal.h"

BalanceStatus balance_compute(const Battery *b) {
    BalanceStatus s = {{false}, 0};
    for (int i = 0; i < NUM_CELLS; i++) {
        if (b->cell_v[i] - b->avg_v > BALANCE_THRESHOLD) {
            s.active[i] = true;
            s.count++;
        }
    }
    return s;
}

void balance_apply(const BalanceStatus *s, float dt_s) {
    for (int i = 0; i < NUM_CELLS; i++)
        if (s->active[i]) hal_sim_bleed(i, BALANCE_RATE * dt_s);
}
