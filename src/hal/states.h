#ifndef STATES_H
#define STATES_H

#include "hal/global_state_data.h"
#include "periph/pwm.h"
#include "hal/timer.h"

// ---------------------------------- SEARCH STATE -------------------------------- //

typedef enum {
    SEARCH_ROTATE,
    SEARCH_FORWARD
} Internal_Search_State;
 
struct Search_State {
    struct Global_State_Data *common_data;
    Internal_Search_State internal_search_state;
};
void search_state_init(struct Search_State* state);
void search_state_enter(struct Search_State* state, State_Type from_state, Event event);


// ---------------------------------- ATTACK STATE -------------------------------- //
typedef enum {
    ATTACK_FORWARD,
    ATTACK_LEFT,
    ATTACK_RIGHT,
} Internal_Attack_State;
struct Attack_State {
    struct Global_State_Data *common_data;
    Internal_Attack_State internal_attack_state;
};
void attack_state_init(struct Attack_State* state);
void attack_state_enter(struct Attack_State* state, State_Type from_state, Event event);

// ---------------------------------- RETREAT STATE -------------------------------- //

typedef enum {
    RETREAT_REVERSE,
    RETREAT_FORWARD,
    RETREAT_ROTATE_LEFT,
    RETREAT_ROTATE_RIGHT,
    RETREAT_ARCTURN_LEFT,
    RETREAT_ARCTURN_RIGHT,
    RETREAT_ALIGN_LEFT,
    RETREAT_ALIGN_RIGHT,
} Internal_Retreat_State;

struct Retreat_State {
    struct Global_State_Data *common_data;
    Internal_Retreat_State internal_retreat_state;
    uint8_t move_index;
};
void retreat_state_init(struct Retreat_State *state);
void retreat_state_enter(struct Retreat_State *state, State_Type from_state, Event event);

// ---------------------------------- WAIT STATE -------------------------------- //

struct Wait_State {
    struct Global_State_Data*common_data;
};
void wait_state_enter(struct Wait_State *state, State_Type from_state, Event event);

#endif