#include "hal/state_machine.h"
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

static const Scene_Transition transitions[] = {
    {.from_state = STATE_WAIT, .event = EVENT_NONE, .to_state = STATE_WAIT},
    {.from_state = STATE_WAIT, .event = EVENT_LINE, .to_state = STATE_SEARCH},
    {.from_state = STATE_WAIT, .event = EVENT_ENEMY, .to_state = STATE_SEARCH},

    {.from_state = STATE_SEARCH, .event = EVENT_NONE, .to_state = STATE_SEARCH},
    {.from_state = STATE_SEARCH, .event = EVENT_TIMEOUT, .to_state = STATE_SEARCH},
    {.from_state = STATE_SEARCH, .event = EVENT_ENEMY, .to_state = STATE_ATTACK},
    {.from_state = STATE_SEARCH, .event = EVENT_LINE, .to_state = STATE_RETREAT},

    {.from_state = STATE_ATTACK, .event = EVENT_NONE, .to_state = STATE_SEARCH},
    {.from_state = STATE_ATTACK, .event = EVENT_ENEMY, .to_state = STATE_ATTACK},
    {.from_state = STATE_ATTACK, .event = EVENT_LINE, .to_state = STATE_RETREAT},
    {.from_state = STATE_ATTACK, .event = EVENT_TIMEOUT, .to_state = STATE_ATTACK},
    {.from_state = STATE_ATTACK, .event = EVENT_NONE, .to_state = STATE_SEARCH},

    {.from_state = STATE_RETREAT, .event = EVENT_NONE, .to_state = STATE_RETREAT},
    {.from_state = STATE_RETREAT, .event = EVENT_LINE, .to_state = STATE_RETREAT},
    {.from_state = STATE_RETREAT, .event = EVENT_TIMEOUT, .to_state = STATE_RETREAT},
    {.from_state = STATE_RETREAT, .event = EVENT_FINISHED, .to_state = STATE_SEARCH},
    {.from_state = STATE_RETREAT, .event = EVENT_ENEMY, .to_state = STATE_RETREAT},
};

/*
static void get_state_machine_status(State_Machine *state_machine){
    Serial.println(state_machine->state);
    switch(state_machine->state){
        case STATE_ATTACK:
            Serial.print("ATTACK: ");
            switch(state_machine->attack_state.internal_attack_state){
                case ATTACK_FORWARD:
                    Serial.print("ATTACK FORWARD: ");
                    break;
                case ATTACK_LEFT:
                    Serial.print("ATTACK LEFT");
                    break;
                case ATTACK_RIGHT:
                    Serial.print("ATTACK RIGHT");
                    break;
            };
            break;
        case STATE_RETREAT:
            Serial.print("RETREAT: ");
            break;
        case STATE_SEARCH:
            Serial.print("SEARCH: ");
            switch(state_machine->search_state.internal_search_state){
                case SEARCH_FORWARD:
                    Serial.print("SEARCH FORWARD");
                    break;
                case SEARCH_ROTATE:
                    Serial.print("SEARCH ROTATE");
                    break;
            };
            break;
        case STATE_WAIT:
            Serial.print("WAIT: ");
            break;
    }
    if(state_machine->timer->timeout > 0){
        Serial.println();
        Serial.print("Time: ");
        Serial.println(state_machine->timer->time);
    }else Serial.println();
}
*/

void state_machine_init(State_Machine* state_machine, Timer *timer){
    state_machine->state = STATE_WAIT;

    state_machine->timer = timer;
    
    //state_machine->global_state_data->state_machine = state_machine;
    state_machine->global_state_data->enemy.enemy_location = ENEMY_LOC_NONE;
    state_machine->global_state_data->enemy.enemy_range = ENEMY_RANGE_NONE;
    state_machine->global_state_data->line = LINE_NONE;
    state_machine->global_state_data->timer = state_machine->timer;
    state_machine->global_state_data->input_history = &state_machine->input_histoty;
    //timer_hard_reset(state_machine->timer);

    state_machine->internal_event = EVENT_NONE;
    
    // states
    state_machine->search_state.common_data = state_machine->global_state_data;
    state_machine->attack_state.common_data = state_machine->global_state_data;
    state_machine->retreat_state.common_data = state_machine->global_state_data;
    state_machine->wait_state.common_data = state_machine->global_state_data;
    search_state_init(&state_machine->search_state);
    attack_state_init(&state_machine->attack_state);
    retreat_state_init(&state_machine->retreat_state);

}

void post_internal_event(struct State_Machine *state_machine, Event event){
    Serial.print("POSTED");
    Serial.println(event);
    state_machine->internal_event = event;
}

static bool has_internal_event(struct State_Machine *state_machine){
    return state_machine->internal_event != EVENT_NONE;
}
static Event get_internal_event(struct State_Machine *state_machine){
    if(!has_internal_event(state_machine)) assert(0,"no internal");
    Event event = state_machine->internal_event;
    state_machine->internal_event = EVENT_NONE;
    return event;
}

static Event state_machine_process_input(State_Machine *state_machine, uint16_t *ranges){
    state_machine->global_state_data->enemy = get_enemy(ranges[0], ranges[1], ranges[2]);
    state_machine->global_state_data->line = get_lines();

    Input input_data = {.enemy = state_machine->global_state_data->enemy, .line = state_machine->global_state_data->line};
    queue_push(&state_machine->input_histoty, input_data);
    Serial.print("Event: ");
    if(has_internal_event(state_machine)){
        Serial.print("INTERNAL ");
        Serial.println(state_machine->internal_event);
        return get_internal_event(state_machine);
    }else if(timer_timed_out(state_machine->timer)){
        Serial.println("TIMEOUT");
        return EVENT_TIMEOUT;
    }else if(state_machine->global_state_data->line != LINE_NONE){
        Serial.println("LINE");
        return EVENT_LINE;
    }else if(enemy_detected(state_machine->global_state_data->enemy)){
        Serial.println("ENEMY");
        return EVENT_ENEMY;
    }
    Serial.println("NONE");
    return EVENT_NONE;
}

static void state_machine_enter(State_Machine *state_machine, State_Type from_state, Event event, State_Type to_state){
    if(from_state != to_state){
        timer_hard_reset(state_machine->timer);
        state_machine->state = to_state;
        Serial.print("State: ");
        Serial.println(state_machine->state);
    }
    switch(to_state){
        case STATE_WAIT:
            wait_state_enter(&state_machine->wait_state, from_state, event);
            return;
        case STATE_SEARCH:
            search_state_enter(&state_machine->search_state, from_state, event);
            return;
        case STATE_ATTACK:
            attack_state_enter(&state_machine->attack_state, from_state, event);
            return;
        case STATE_RETREAT:
            retreat_state_enter(&state_machine->retreat_state, from_state, event);
            return;
        default:
            break;
    }

}
// 0: wait, 1: search, 2: attack 3: retreat
// 0:timeout  1:line  2:enemy  3:finsihed  4:command  5:None
static void state_machine_process_event(State_Machine *state_machine, Event event){
    for (int i = 0; i < ARRAY_SIZE(transitions); i++) { 
        if (transitions[i].from_state == state_machine->state & transitions[i].event == event){
            /*
            Serial.println(state_machine->state);
            Serial.println(transitions[i].from_state);
            Serial.println(transitions[i].event);
            Serial.println(transitions[i].to_state);
            Serial.println(i);
            */
            state_machine_enter(state_machine, transitions[i].from_state, event, transitions[i].to_state);
            return;
        }
    }
    assert(0, "transition not found");
}

void state_machine_run(struct State_Machine *state_machine, uint16_t *ranges){
    Event event = state_machine_process_input(state_machine, ranges);
    state_machine_process_event(state_machine, event);
}


