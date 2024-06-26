
#ifndef LINE_SENSORS_H
#define LINE_SENSORS_H

#include <Arduino.h>
#include "periph/adc.h"

typedef enum {
    LINE_NONE,
    LINE_FRONT,
    LINE_BACK,
    LINE_LEFT,
    LINE_RIGHT,
    LINE_FRONT_LEFT,
    LINE_FRONT_RIGHT,
    LINE_BACK_LEFT,
    LINE_BACK_RIGHT,
    LINE_DIAGONAL_LEFT,
    LINE_DIAGONAL_RIGHT
} Line_Pos;

void line_sensors_init(void);
Line_Pos get_lines();
const char *line_str(Line_Pos line);

#endif