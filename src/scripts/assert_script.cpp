#include "assert_script.h"

void assert_script(void){
    pinMode(13, OUTPUT);
    Serial.println("error");
    while (1)
    {
        digitalWrite(13, !digitalRead(13));
        delay(100);
    }
    
}