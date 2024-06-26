#include "enemy.h"

Enemy get_enemy(uint16_t loc1, uint16_t loc2, uint16_t loc3){
    Enemy enemy = {ENEMY_LOC_NONE, ENEMY_RANGE_NONE};
    uint16_t min_distance = min(loc1, loc2);
    min_distance = min(min_distance, loc3);
    Enemy_Range enemy_range = get_enemy_range(min_distance);
    Enemy_Loc enemy_loc = enemy_range != ENEMY_RANGE_NONE ? get_enemy_location(loc1 < 1000, loc2 < 1000, loc3 < 1000) : ENEMY_LOC_NONE;
    enemy.enemy_range = enemy_range;
    enemy.enemy_location = enemy_loc;
    return enemy;
}

Enemy_Range get_enemy_range(uint16_t range){
    Serial.print("Range: ");
    Serial.println(range);
    if(range < 150) return ENEMY_CLOSE;
    else if (range < 500) return ENEMY_MID;
    else if (range < 800) return ENEMY_FAR;
    return ENEMY_RANGE_NONE;
}

Enemy_Loc get_enemy_location(bool left, bool front, bool right){
    if (left & front & right){
        return ENEMY_LOC_FRONT_ALL;
    }else if (left & front){
        return ENEMY_LOC_FRONT_LEFT;
    } else if(right & front){
        return ENEMY_LOC_FRONT_RIGHT;
    } else if(front){
        return ENEMY_LOC_FRONT;
    } else if(left){
        if(right) return ENEMY_LOC_ERROR;
        return ENEMY_LOC_LEFT;
    } else if(right) {
        if(left) return ENEMY_LOC_ERROR;
        return ENEMY_LOC_RIGHT;
    }
    return ENEMY_LOC_NONE;
}

bool enemy_detected(Enemy* enemy){
    return enemy->enemy_location != ENEMY_LOC_NONE;
}
bool enemy_in_front(Enemy* enemy){
    return enemy->enemy_location == ENEMY_LOC_FRONT_ALL || enemy->enemy_location == ENEMY_LOC_FRONT;
}
bool enemy_on_left(Enemy* enemy){
    return enemy->enemy_location == ENEMY_LOC_LEFT || enemy->enemy_location == ENEMY_LOC_FRONT_LEFT;
}
bool enemy_on_right(Enemy* enemy){
    return enemy->enemy_location == ENEMY_LOC_RIGHT || enemy->enemy_location == ENEMY_LOC_FRONT_RIGHT;
}

const char *enemy_loc_str(Enemy_Loc enemy_loc){
    switch(enemy_loc){
        case ENEMY_LOC_NONE:
            return "ENEMY_LOC_NONE";
        case ENEMY_LOC_FRONT:
            return "ENEMY_LOC_FRONT";
        case ENEMY_LOC_FRONT_ALL:
            return "ENEMY_LOC_FRONT_ALL";
        case ENEMY_LOC_FRONT_LEFT:
            return "ENEMY_LOC_FRONT_LEFT";
        case ENEMY_LOC_FRONT_RIGHT:
            return "ENEMY_LOC_FRONT_RIGHT";
        case ENEMY_LOC_LEFT:
            return "ENEMY_LOC_LEFT";
        case ENEMY_LOC_RIGHT:
            return "ENEMY_LOC_RIGHT";
        case ENEMY_LOC_ERROR:
            return "ENEMY_LOC_ERROR";
    }
    return "ENEMY_LOC_NONE";
}

const char *enemy_range_str(Enemy_Range enemy_range){
    switch (enemy_range) {
    case ENEMY_CLOSE:
        return "ENEMY_CLOSE";
    case ENEMY_MID:
        return "ENEMY_MID";
    case ENEMY_FAR:
        return "ENEMY_FAR";
    default:
        return "ENEMY_RANGE_NONE";
    };
    return "ENEMY_RANGE_NONE";
}
