#include "assert_script.h"

void assert(bool expression, String err){
    if(!expression){
        while(1){Serial.println(err);}
    }
}