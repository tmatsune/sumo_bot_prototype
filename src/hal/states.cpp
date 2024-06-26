#include "hal/states.h"
#include "scripts/assert_script.h"

// ---------------------------------- SEARCH STATE -------------------------------- //
void search_state_init(struct Search_State *state){
    state->internal_search_state = SEARCH_FORWARD;
}

static void search_state_run(struct Search_State state ){

}   

void search_state_enter(struct Search_State *state, State_Type from_state, Event event){
    Serial.println("in SEARCH STATE");
    switch(from_state){
        case STATE_WAIT:
            break;
        case STATE_ATTACK:
            break;
        case STATE_RETREAT:
            break;
        case STATE_SEARCH:
            break;
        default:
            break;
    }

}



// ---------------------------------- ATTACK STATE -------------------------------- //
void attack_state_init(struct Attack_State *state){
    state->internal_attack_state = ATTACK_FORWARD;
}
static void attack_state_run(struct Attack_State *state){

}
void attack_state_enter(struct Attack_State *state, State_Type from_state, Event event){
    Serial.println("IN ATTACK STATE");
}


// ---------------------------------- RETREAT STATE -------------------------------- //
void retreat_state_init(struct Retreat_State *state){
    state->internal_retreat_state = RETREAT_REVERSE;
}
static void retreat_state_run(struct Retreat_State *state){

}

void retreat_state_enter(struct Retreat_State *state, State_Type from_state, Event event){
    Serial.println("in RETREAT STATE");
}


// ---------------------------------- WAIT STATE -------------------------------- //
void wait_state_enter(struct Wait_State *state, State_Type from_state, Event event){
    Serial.println("IN WAIT STATE");
}


