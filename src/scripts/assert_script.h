
#ifndef ASSERT_SCRIPT_H
#define ASSERT_SCRIPT_H
#include <Arduino.h>

#define ASSERT(expression) \
    do { \
        if(!expression){ \
            assert_script(); \
        } \
    } while(0)

void assert_script(void);

#endif