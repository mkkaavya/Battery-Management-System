#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include "fault.h"

typedef enum { STATE_IDLE, STATE_MONITORING, STATE_WARNING,
               STATE_FAULT, STATE_SHUTDOWN } SystemState;

typedef struct { SystemState state; int fault_ticks; } StateMachine;

void        sm_init(StateMachine *sm);
void        sm_start(StateMachine *sm);
SystemState sm_update(StateMachine *sm, const FaultStatus *f);
const char *sm_state_name(SystemState s);
#endif
