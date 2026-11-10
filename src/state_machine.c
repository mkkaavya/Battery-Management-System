#include "state_machine.h"
#include "config.h"

void sm_init(StateMachine *sm)  { sm->state = STATE_IDLE; sm->fault_ticks = 0; }
void sm_start(StateMachine *sm) { if (sm->state == STATE_IDLE) sm->state = STATE_MONITORING; }

SystemState sm_update(StateMachine *sm, const FaultStatus *f) {
    switch (sm->state) {
    case STATE_IDLE:
        break;                                   /* waits for sm_start() */
    case STATE_MONITORING:
        if (f->critical)      { sm->state = STATE_FAULT; sm->fault_ticks = 1; }
        else if (f->warning)  sm->state = STATE_WARNING;
        break;
    case STATE_WARNING:
        if (f->critical)      { sm->state = STATE_FAULT; sm->fault_ticks = 1; }
        else if (!f->warning) sm->state = STATE_MONITORING;
        break;
    case STATE_FAULT:
        if (f->critical) {
            if (++sm->fault_ticks >= SHUTDOWN_TICKS) sm->state = STATE_SHUTDOWN;
        } else {
            sm->fault_ticks = 0;
            sm->state = STATE_WARNING;           /* fault cleared */
        }
        break;
    case STATE_SHUTDOWN:
        break;                                   /* latched: needs reset */
    }
    return sm->state;
}

const char *sm_state_name(SystemState s) {
    static const char *names[] = {"IDLE", "MONITORING", "WARNING", "FAULT", "SHUTDOWN"};
    return names[s];
}
