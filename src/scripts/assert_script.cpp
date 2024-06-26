#include "assert_script.h"

void assert(bool expression, char* err){
    if(!expression){
        while(1){Serial.println(err);}
    }
}