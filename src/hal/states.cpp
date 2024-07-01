#include "hal/states.h"
#include "scripts/assert_script.h"
#include "scripts/queue.h"

#define SEARCH_FORWARD_TIMEOUT 200
#define SEARCH_ROTATE_TIMEOUT 100
#define ATTACK_TIMEOUT 4000
#define MAX_MOVES 5


// ---------------------------------- SEARCH STATE -------------------------------- //
void search_state_init(struct Search_State *state){
    state->internal_search_state = SEARCH_FORWARD;
}

static void search_state_run(struct Search_State *state){
    if(state->internal_search_state == SEARCH_ROTATE){
        Enemy last_enemy = queue_last_enemy(state->common_data->input_history);
        if(enemy_on_left(last_enemy)) drive_set(DRIVE_DIR_ROTATE_LEFT, DRIVE_SPEED_FAST);
        else drive_set(DRIVE_DIR_ROTATE_RIGHT, DRIVE_SPEED_FAST);
        timer_set_timeout(state->common_data->timer, SEARCH_ROTATE_TIMEOUT);
    }else if(state->internal_search_state == SEARCH_FORWARD){
        drive_set(DRIVE_DIR_FORWARD, DRIVE_SPEED_FAST);
        timer_set_timeout(state->common_data->timer, SEARCH_FORWARD_TIMEOUT);
    }
}

void search_state_enter(struct Search_State *state, State_Type from_state, Event event){
    Serial.print("SEARCH ");
    Serial.println(state->internal_search_state);
    Serial.println(state->common_data->timer->timeout);
    switch(from_state){
        case STATE_WAIT:
            search_state_run(state);
            break;
        case STATE_ATTACK:
        case STATE_RETREAT:
            switch(event){
                case EVENT_NONE:
                    search_state_run(state);
                    break;
                case EVENT_FINISHED:
                    if(state->internal_search_state == SEARCH_FORWARD)
                        state->internal_search_state = SEARCH_ROTATE;
                    search_state_run(state);
                    break;
                default:
                    assert(0,"ERR: FR ATK | SRCH");
                    break;
            }
            break;
        case STATE_SEARCH:
            switch(event){
                case EVENT_NONE:
                    break;
                case EVENT_TIMEOUT:
                    switch(state->internal_search_state){
                        case SEARCH_FORWARD:
                            state->internal_search_state = SEARCH_ROTATE;
                            break;
                        case SEARCH_ROTATE:
                            state->internal_search_state = SEARCH_FORWARD;
                            break;
                    }
                    search_state_run(state);
                    break;
            }
            break;
        default:
            assert(0,"ERR: all cases covered");
            break;
    }
}



// ---------------------------------- ATTACK STATE -------------------------------- //
void attack_state_init(struct Attack_State *state){
    state->internal_attack_state = ATTACK_FORWARD;
}

static Internal_Attack_State next_attack_state(Enemy enemy){
    if(enemy_in_front(enemy)) return ATTACK_FORWARD;
    else if(enemy_on_right(enemy)) return ATTACK_RIGHT;
    else if(enemy_on_left(enemy)) return ATTACK_LEFT;
    return ATTACK_FORWARD;
};

static void attack_state_run(struct Attack_State *state){
    switch(state->internal_attack_state){
        case ATTACK_FORWARD:
            drive_set(DRIVE_DIR_FORWARD, DRIVE_SPEED_MAX);
            break;
        case ATTACK_LEFT:
            drive_set(DRIVE_DIR_ARCTURN_WIDE_LEFT, DRIVE_SPEED_MAX);
            break;
        case ATTACK_RIGHT:
            drive_set(DRIVE_DIR_ARCTURN_WIDE_RIGHT, DRIVE_SPEED_MAX);
            break;
    };
    timer_set_timeout(state->common_data->timer, ATTACK_TIMEOUT);
}

void attack_state_enter(struct Attack_State *state, State_Type from_state, Event event){
    Serial.println("ATTACK");
    Internal_Attack_State prev_attack_state = state->internal_attack_state;
    state->internal_attack_state = next_attack_state(state->common_data->enemy);

    switch(from_state){
        case STATE_SEARCH:
            switch(event){
                case EVENT_ENEMY:
                    attack_state_run(state);
                    break;
                default:
                    assert(0, "!EVENT FRM SRCH");
            }
            break;
        case STATE_ATTACK:
            switch(event){
                case EVENT_ENEMY:
                    if(prev_attack_state != state->internal_attack_state)
                        attack_state_run(state);
                    break;
                case EVENT_TIMEOUT:
                    assert(0, "!EVENT TIMEOUT");
                    break;
                default:
                    assert(0, "!EVENT FRM ATK");
            }
            break;
        case STATE_RETREAT:
            assert(0,"ATK ERR");
            break;
    }
}


// ---------------------------------- RETREAT STATE -------------------------------- //

struct Move{
    Drive_Dir dir;
    Drive_Speed speed;
    int timeout;
};

struct Maneuver{
    struct Move moves[MAX_MOVES];
    uint8_t move_count;
};

static const Maneuver maneuvers[] = {
    [RETREAT_REVERSE] = {
        .moves = {{.dir = DRIVE_DIR_REVERSE, .speed = DRIVE_SPEED_MAX, .timeout = 80}},
        .move_count = 1,
    },
    [RETREAT_FORWARD] = {
        .moves = {{.dir = DRIVE_DIR_FORWARD, .speed = DRIVE_SPEED_MAX, .timeout = 80}},
        .move_count = 1,
    },
    [RETREAT_ROTATE_LEFT] = {
        .moves = {{.dir = DRIVE_DIR_ROTATE_LEFT, .speed = DRIVE_SPEED_MAX, .timeout = 80}},
        .move_count = 1,
    },
    [RETREAT_ROTATE_RIGHT] = {
        .moves = {{.dir = DRIVE_DIR_ROTATE_RIGHT, .speed = DRIVE_SPEED_MAX, .timeout = 80}},
        .move_count = 1,
    },
    [RETREAT_ARCTURN_LEFT] = {
        .moves = {{.dir = DRIVE_DIR_ARCTURN_WIDE_LEFT, .speed = DRIVE_SPEED_MAX, .timeout = 80}},
        .move_count = 1,
    },
    [RETREAT_ARCTURN_RIGHT] = {
        .moves = {{.dir = DRIVE_DIR_ARCTURN_WIDE_RIGHT, .speed = DRIVE_SPEED_MAX, .timeout = 80}},
        .move_count = 1,
    },
    [RETREAT_ALIGN_LEFT] = {
        .moves = {
            {.dir = DRIVE_DIR_REVERSE, .speed = DRIVE_SPEED_MAX, .timeout = 80},
            {.dir = DRIVE_DIR_ARCTURN_SHARP_LEFT, .speed = DRIVE_SPEED_MAX, .timeout = 60},
            {.dir = DRIVE_DIR_ARCTURN_MID_RIGHT, .speed = DRIVE_SPEED_MAX, .timeout = 60},
        },
        .move_count = 3,
    },
    [RETREAT_ALIGN_RIGHT] = {
        .moves = {
            {.dir = DRIVE_DIR_REVERSE, .speed = DRIVE_SPEED_MAX, .timeout = 80},
            {.dir = DRIVE_DIR_ARCTURN_SHARP_RIGHT, .speed = DRIVE_SPEED_MAX, .timeout = 60},
            {.dir = DRIVE_DIR_ARCTURN_MID_LEFT, .speed = DRIVE_SPEED_MAX, .timeout = 60},
        },
        .move_count = 3,
    },
};

void retreat_state_init(struct Retreat_State *state){
    state->internal_retreat_state = RETREAT_REVERSE;
    state->move_index = 0;
}

static Move current_retreat_move(struct Retreat_State *state){
    return maneuvers[state->internal_retreat_state].moves[state->move_index];
}

static Internal_Retreat_State next_retreat_state(struct Retreat_State *state){
    Line_Pos line = state->common_data->line;
    Enemy enemy = state->common_data->enemy;
    switch(line){
        case LINE_FRONT:
            if(enemy_in_front(enemy))
                return RETREAT_ARCTURN_LEFT;
            else if(enemy_on_right(enemy))
                return RETREAT_ALIGN_RIGHT;
            return RETREAT_REVERSE;

        case LINE_BACK:
            return RETREAT_REVERSE;

        case LINE_LEFT:
            return RETREAT_ARCTURN_RIGHT;
        case LINE_RIGHT:
            return RETREAT_ARCTURN_LEFT;

        case LINE_FRONT_LEFT:
            if (enemy_on_right(enemy) || enemy_in_front(enemy))
                return RETREAT_ALIGN_RIGHT;
            else if (enemy_on_left(enemy))
                return RETREAT_ALIGN_LEFT;
            return RETREAT_REVERSE;

        case LINE_FRONT_RIGHT:
            if (enemy_on_left(enemy) || enemy_in_front(enemy))
                return RETREAT_ALIGN_LEFT;
            else if (enemy_on_right(enemy))
                return RETREAT_ALIGN_RIGHT;
            return RETREAT_REVERSE;

        case LINE_BACK_LEFT:
            if (current_retreat_move(state).dir == DRIVE_DIR_REVERSE)
                return RETREAT_ARCTURN_RIGHT;
            else if (state->internal_retreat_state == RETREAT_ARCTURN_LEFT)
                return RETREAT_ARCTURN_RIGHT;
        return RETREAT_FORWARD;

        case LINE_BACK_RIGHT:
            if (current_retreat_move(state).dir == DRIVE_DIR_REVERSE)
                return RETREAT_ARCTURN_LEFT;
            else if (state->internal_retreat_state == RETREAT_ARCTURN_LEFT)
                return RETREAT_ARCTURN_LEFT;
            return RETREAT_FORWARD;

        case LINE_DIAGONAL_LEFT:
            break;
        case LINE_DIAGONAL_RIGHT:
            break;
    }
    return RETREAT_REVERSE;
}

static bool retreat_state_done(struct Retreat_State *state){
    return state->move_index >= maneuvers[state->internal_retreat_state].move_count;
}

static void start_retreat_move(struct Retreat_State *state){
    struct Move move = current_retreat_move(state);
    timer_set_timeout(state->common_data->timer, move.timeout);
    drive_set(move.dir, move.speed);
}

static void retreat_state_run(struct Retreat_State *state){
    state->move_index = 0;
    state->internal_retreat_state = next_retreat_state(state);
    start_retreat_move(state);
}

void retreat_state_enter(struct Retreat_State *state, State_Type from_state, Event event){
    Serial.println("RETREAT");

    switch(from_state){
        case STATE_ATTACK:
        case STATE_SEARCH:
            switch(event){
                case EVENT_LINE:
                    retreat_state_run(state);
                    break;
            }
            break;
        case STATE_RETREAT:
            switch(event){
                case EVENT_LINE:
                    retreat_state_run(state);
                    break;
                case EVENT_TIMEOUT:
                    state->move_index++;
                    Serial.println(state->move_index);
                    Serial.println(maneuvers[state->internal_retreat_state].move_count);
                    Serial.println();
                    if(retreat_state_done(state)){
                        //post_internal_event(state->common_data->state_machine, EVENT_FINISHED);
                    }else{
                        start_retreat_move(state);
                    }
                    break;
            }
            break;
        
    }

}


// ---------------------------------- WAIT STATE -------------------------------- //
void wait_state_enter(struct Wait_State *state, State_Type from_state, Event event){
    Serial.print("WAIT");
}


