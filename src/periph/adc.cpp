#include "adc.h"
#include "periph/pins.h"

#define FRONT_LEFT_SENSOR A_FIVE
#define FRONT_RIGHT_SENSOR A_FOUR

volatile adc_input_values adc_values;
int current_channel = 0;

void adc_init() {
    // REFS0 SETS REFERNCE VOLTAGE TO EXTERNAL CAPACITOR
    ADMUX = (1 << REFS0);
    // ADEN ENABLES ADC
    // ADIE ENABLES INTERRUPS
    // ADPS 0, 1, & 2 SETS PRESCALAR TO 128
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    ADMUX = (ADMUX & 0xF0) | 5;

    // test 4 & 5 lights 
    pinMode(FOUR, OUTPUT);
    digitalWrite(FOUR, LOW);
    pinMode(FIVE, OUTPUT);
    digitalWrite(FIVE, LOW);
    start_conversion();
};

void start_conversion(void){ ADCSRA |= (1 << ADSC); }

void get_adc_input_values(adc_input_values vals){
    vals[0] = adc_values[0];
    vals[1] = adc_values[1];
}


ISR(ADC_vect){
    adc_values[current_channel] = ADC;
    current_channel++;
    if(current_channel >= ADC_COUNT){  current_channel = 0; }
    ADMUX = (ADMUX & 0xF0) | (5 - current_channel);
    start_conversion();
    /*
    if(adc_values[0] < 600){ digitalWrite(FIVE, HIGH);
    }else{ digitalWrite(FIVE, LOW); }
    if(adc_values[1] < 600){ digitalWrite(FOUR, HIGH);
    }else{ digitalWrite(FOUR, LOW); }
    */

}
