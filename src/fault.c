
#include "fault.h"

/* Fault priority: highest first */
static const FaultCode PRIORITY[] = {
    FAULT_OVERTEMP,
    FAULT_OVERCURRENT,
    FAULT_OVERVOLTAGE,
    FAULT_UNDERVOLTAGE,
    FAULT_IMBALANCE
};

/* Existing fault detection */
FaultStatus fault_check(const Battery *b) {

    FaultStatus s = {0, 0, FAULT_NONE};

    if (b->temperature > TEMP_CRIT)
        s.critical |= FAULT_OVERTEMP;

    else if (b->temperature > TEMP_WARN)
        s.warning |= FAULT_OVERTEMP;

    if (b->current > CURRENT_CRIT)
        s.critical |= FAULT_OVERCURRENT;

    else if (b->current > CURRENT_WARN)
        s.warning |= FAULT_OVERCURRENT;

    if (b->max_v > CELL_OV_CRIT)
        s.critical |= FAULT_OVERVOLTAGE;

    else if (b->max_v > CELL_OV_WARN)
        s.warning |= FAULT_OVERVOLTAGE;

    if (b->min_v < CELL_UV_CRIT)
        s.critical |= FAULT_UNDERVOLTAGE;

    else if (b->min_v < CELL_UV_WARN)
        s.warning |= FAULT_UNDERVOLTAGE;

    if (b->delta_v > IMBALANCE_WARN)
        s.warning |= FAULT_IMBALANCE;

    uint8_t pick = s.critical ? s.critical : s.warning;

    for (unsigned i = 0;
         i < sizeof PRIORITY / sizeof PRIORITY[0];
         i++) {

        if (pick & PRIORITY[i]) {
            s.top = PRIORITY[i];
            break;
        }
    }

    return s;
}

/* Convert fault code to readable text */
const char *fault_name(FaultCode f) {

    switch (f) {

        case FAULT_OVERTEMP:
            return "OVERHEATING";

        case FAULT_OVERCURRENT:
            return "OVERCURRENT";

        case FAULT_OVERVOLTAGE:
            return "OVERVOLTAGE";

        case FAULT_UNDERVOLTAGE:
            return "UNDERVOLTAGE";

        case FAULT_IMBALANCE:
            return "CELL IMBALANCE";

        default:
            return "NONE";
    }
}

/* Initialise fault history */
void fault_history_init(FaultHistory *history) {

    history->next = 0;
    history->count = 0;
}

/* Add a new fault record */
void fault_history_add(
    FaultHistory *history,
    FaultCode code,
    FaultSeverity severity,
    uint32_t timestamp_ms
) {

    if (history == 0 || code == FAULT_NONE)
        return;

    history->records[history->next].code = code;

    history->records[history->next].severity = severity;

    history->records[history->next].timestamp_ms =
        timestamp_ms;

    history->next =
        (history->next + 1) % FAULT_HISTORY_SIZE;

    if (history->count < FAULT_HISTORY_SIZE)
        history->count++;
}

/* Return number of stored records */
unsigned fault_history_count(const FaultHistory *history) {

    if (history == 0)
        return 0;

    return history->count;
}

/* Retrieve a record in chronological order */
int fault_history_get(
    const FaultHistory *history,
    unsigned index,
    FaultRecord *record
) {

    if (history == 0 || record == 0 ||
        index >= history->count)
        return 0;

    unsigned oldest =
        (history->next + FAULT_HISTORY_SIZE - history->count)
        % FAULT_HISTORY_SIZE;

    unsigned position =
        (oldest + index) % FAULT_HISTORY_SIZE;

    *record = history->records[position];

    return 1;
}
