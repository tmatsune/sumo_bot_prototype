#ifndef GLOBAL_STATE_DATA_H
#define GLOBAL_STATE_DATA_H

#include "hal/line_sensors.h"
#include "hal/enemy.h"

typedef enum {
    EVENT_NONE,
    EVENT_TIMEOUT,
    EVENT_LINE,
    EVENT_ENEMY,
    EVENT_FINISHED,
    EVENT_COMMAND,
} Event;

typedef enum {
    STATE_WAIT,
    STATE_SEARCH,
    STATE_ATTACK,
    STATE_RETREAT
} State_Type;

struct State_Machine;
struct Timer;
struct Queue;

struct Global_State_Data{
    State_Machine* state_machine;
    Timer* timer;
    Queue* input_history;
    Enemy enemy;
    Line_Pos line;
};

//void post_internal_event(struct State_Machine *state_machine, Event event);

#endif