#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include <stdint.h>
#include "hal/enemy.h"
#include "hal/line_sensors.h"
#include "hal/states.h"
#include "hal/global_state_data.h"
#include "hal/timer.h"
#include "scripts/queue.h"

typedef struct {
    State_Type from_state;
    Event event;
    State_Type to_state;
} Scene_Transition;

struct State_Machine{
    State_Type state;
    Global_State_Data *global_state_data;
    struct Search_State search_state;
    struct Attack_State attack_state;
    struct Retreat_State retreat_state;
    struct Wait_State wait_state;
    Event internal_event;
    Timer timer;
    Queue input_histoty;
};

void state_machine_test_init(struct State_Machine *state_machine, Timer timer);
void state_machine_run(struct State_Machine *state_machine, uint16_t *ranges);

#endif