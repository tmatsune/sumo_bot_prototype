#ifndef STATES_H
#define STATES_H

#include "hal/global_state_data.h"

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
//void search_state_run(struct Search_State* search_state);

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
//void attack_state_run(struct Attack_State* attack_state);
// ---------------------------------- RETREAT STATE -------------------------------- //
typedef enum {
    RETREAT_REVERSE,
} Internal_Retreat_State;
struct Retreat_State {
    struct Global_State_Data *common_data;
    Internal_Retreat_State internal_retreat_state;
};
void retreat_state_init(struct Retreat_State *state);
void retreat_state_enter(struct Retreat_State *state, State_Type from_state, Event event);
//void retreat_state_run(struct Retreat_State* retreat_state);
// ---------------------------------- WAIT STATE -------------------------------- //
struct Wait_State {
    struct Global_State_Data*common_data;
};
void wait_state_enter(struct Wait_State *state, State_Type from_state, Event event);

#endif