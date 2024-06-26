#include "hal/state_machine.h"
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

static const Scene_Transition transitions[] = {
    {.from_state = STATE_WAIT, .event = EVENT_NONE, .to_state = STATE_WAIT},
    {.from_state = STATE_WAIT, .event = EVENT_LINE, .to_state = STATE_SEARCH},

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

static void state_machine_init(State_Machine* state_machine){
    state_machine->state = STATE_WAIT;

    state_machine->global_state_data->enemy.enemy_location = ENEMY_LOC_NONE;
    state_machine->global_state_data->enemy.enemy_range = ENEMY_RANGE_NONE;
    state_machine->global_state_data->line = LINE_NONE;
    state_machine->global_state_data->timer = &state_machine->timer;
    state_machine->global_state_data->input_history = &state_machine->input_histoty;
    timer_hard_reset(&state_machine->timer);

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

static Event state_machine_process_input(State_Machine *state_machine, uint16_t *ranges){
    state_machine->global_state_data->enemy = get_enemy(ranges[0], ranges[1], ranges[2]);
    state_machine->global_state_data->line = get_lines();

    Input input_data = {.enemy = state_machine->global_state_data->enemy, .line = state_machine->global_state_data->line};
    const char *enemy_range = enemy_range_str(input_data.enemy.enemy_range);
    const char *enemy_loc = enemy_loc_str(input_data.enemy.enemy_location);
    Serial.println(String(enemy_range) + " " + String(enemy_loc));
    return EVENT_NONE;
}
static void state_machine_enter(State_Machine *state_machine, State_Type from_state, Event event, State_Type to_state){
    if(from_state != state_machine->state){
        timer_hard_reset(&state_machine->timer);
        state_machine->state = to_state;
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

static void state_machine_process_event(State_Machine *state_machine, Event event){
    for (int i = 0; i < ARRAY_SIZE(transitions); i++) { 
        if (transitions[i].from_state == state_machine->state & transitions[i].event == event){
            state_machine_enter(state_machine, transitions[i].from_state, event, transitions[i].to_state);
        }
    }
}

void state_machine_test_init(struct State_Machine *state_machine, Timer timer){
    state_machine->timer = timer;
    state_machine_init(state_machine);
}

void state_machine_run(struct State_Machine* state_machine, uint16_t *ranges){
    Event event = state_machine_process_input(state_machine, ranges);
    state_machine_process_event(state_machine, event);
}
