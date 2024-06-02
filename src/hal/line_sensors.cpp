#include "line_sensors.h"


void line_sensors_init(void){
    adc_init();
}
 
void get_lines(){
    adc_input_values adc_vals;
    get_adc_input_values(adc_vals); // adc_vals[2] test
    if(adc_vals[0] < 700) Serial.println("front_left");
    if(adc_vals[1] < 700) Serial.println("front_right");
}