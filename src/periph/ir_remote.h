#ifndef IR_REMOTE_H
#define IR_REMOTE_H

#include <Arduino.h>
#include <stdint.h>
#include "pins.h"
#include "pwm.h"

typedef enum
{
    IR_MSG_0 = 104,
    IR_MSG_1 = 48,
    IR_MSG_2 = 24,
    IR_MSG_3 = 122,
    IR_MSG_4 = 16,
    IR_MSG_5 = 56,
    IR_MSG_6 = 90,
    IR_MSG_7 = 66,
    IR_MSG_8 = 74,
    IR_MSG_9 = 82,
    IR_MSG_UP = 98,
    IR_MSG_DOWN = 168,
    IR_MSG_LEFT = 34,
    IR_MSG_RIGHT = 194,
    IR_MSG_OK = 38,
    IR_MSG_NONE = 255
} IR_MESSSAGE;

void ir_remote_init(void);
IR_MESSSAGE get_message();

#endif 