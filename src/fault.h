
#ifndef FAULT_H
#define FAULT_H

#include <stdint.h>
#include "battery.h"

#define FAULT_HISTORY_SIZE 32

typedef enum {
    FAULT_NONE         = 0,
    FAULT_OVERTEMP     = 1 << 0,
    FAULT_OVERCURRENT  = 1 << 1,
    FAULT_OVERVOLTAGE  = 1 << 2,
    FAULT_UNDERVOLTAGE = 1 << 3,
    FAULT_IMBALANCE    = 1 << 4
} FaultCode;

typedef enum {
    FAULT_WARNING,
    FAULT_CRITICAL
} FaultSeverity;

typedef struct {
    uint8_t critical;
    uint8_t warning;
    FaultCode top;
} FaultStatus;

/* One historical fault record */
typedef struct {
    FaultCode code;
    FaultSeverity severity;
    uint32_t timestamp_ms;
} FaultRecord;

/* Circular fault history */
typedef struct {
    FaultRecord records[FAULT_HISTORY_SIZE];
    unsigned next;
    unsigned count;
} FaultHistory;

/* Existing fault detection functions */
FaultStatus fault_check(const Battery *b);
const char *fault_name(FaultCode f);

/* New fault history functions */
void fault_history_init(FaultHistory *history);

void fault_history_add(
    FaultHistory *history,
    FaultCode code,
    FaultSeverity severity,
    uint32_t timestamp_ms
);

unsigned fault_history_count(const FaultHistory *history);

int fault_history_get(
    const FaultHistory *history,
    unsigned index,
    FaultRecord *record
);

#endif
