#ifndef ENEMY_H
#define ENEMY_H

#include <Arduino.h>

typedef enum {
    ENEMY_LOC_NONE = 0,
    ENEMY_LOC_FRONT = 1,
    ENEMY_LOC_FRONT_LEFT = 2,
    ENEMY_LOC_FRONT_RIGHT = 3,
    ENEMY_LOC_LEFT = 4,
    ENEMY_LOC_RIGHT = 5,
    ENEMY_LOC_FRONT_ALL = 6,
    ENEMY_LOC_ERROR = 7
} Enemy_Loc;

typedef enum {
    ENEMY_RANGE_NONE = 0,
    ENEMY_CLOSE = 1,
    ENEMY_MID = 2,
    ENEMY_FAR = 3
} Enemy_Range;

typedef struct {
    Enemy_Loc enemy_location;
    Enemy_Range enemy_range;
} Enemy;

Enemy get_enemy(uint16_t loc1, uint16_t loc2, uint16_t loc3);
Enemy_Range get_enemy_range(uint16_t range);
Enemy_Loc get_enemy_location(bool left, bool front, bool right);
bool enemy_detected(Enemy enemy);
bool enemy_on_left(Enemy enemy);
bool enemy_on_right(Enemy enemy);
bool enemy_in_front(Enemy enemy);

//const char* enemy_loc_str(Enemy_Loc enemy_loc);
//const char *enemy_range_str(Enemy_Range enemy_range);

#endif