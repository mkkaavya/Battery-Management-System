#ifndef BALANCING_H
#define BALANCING_H
#include <stdbool.h>
#include "battery.h"

typedef struct { bool active[NUM_CELLS]; int count; } BalanceStatus;

BalanceStatus balance_compute(const Battery *b);
void          balance_apply(const BalanceStatus *s, float dt_s);
#endif
