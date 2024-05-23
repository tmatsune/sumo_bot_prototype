#include "line_sensors.h"


void line_sensors_init(void){
    adc_init();
}

void get_lines(){
    adc_input_values adc_vals;
    get_adc_input_values(adc_vals);
    if(adc_vals[0] < 600){ digitalWrite(5, HIGH);
    }else{ digitalWrite(5, LOW); }
    if(adc_vals[1] < 600){ digitalWrite(4, HIGH);
    }else{ digitalWrite(4, LOW); }
}