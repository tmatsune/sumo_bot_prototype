
#ifndef LINE_SENSORS_H
#define LINE_SENSORS_H

#include <Arduino.h>
#include "periph/adc.h"

struct Line_Data {
    int fron_right;
    int front_left;
    int back_right;
    int back_left;
};

void line_sensors_init(void);
void get_lines();


#endif