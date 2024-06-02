#include "adc.h"
#include "periph/pins.h"

/*
    ADC (Analogue to Digital Converter) sampling an analog signal at regular intervals and converting
    each sample into a digital value that represents the magnitude of the analog signal at that moment.
    Sampling -> Quantization -> Conversion -> Output
    IMPLEMENTATION:
        ADC LAYER:
            - This layer initializes the ADC configurations
            - set reference voltage to external capacitor
            - enable adc, interrups, and set pre scalar to 128
            - NOTE: atmega32 cannot read adc all at once, each interrupt increment index of adc array 
                    to and store each value of adc in correspoding array index. 
            - Create typdef array that will store values of adc inputs, then create a volatile version of
                    this array that can be accessed by higher layer
*/
   
#define FRONT_LEFT_SENSOR A_ZERO
#define FRONT_RIGHT_SENSOR A_ONE

volatile adc_input_values adc_values;
int current_channel = 0;

void adc_init() {
    // REFS0 SETS REFERNCE VOLTAGE TO EXTERNAL CAPACITOR
    ADMUX = (1 << REFS0);
    // ADEN ENABLES ADC
    // ADIE ENABLES INTERRUPS
    // ADPS 0, 1, & 2 SETS PRESCALAR TO 128
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    ADMUX = (ADMUX & 0xF0) | FRONT_LEFT_SENSOR;
    
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
    ADMUX = (ADMUX & 0xF0) | (FRONT_LEFT_SENSOR + current_channel);
    start_conversion();
}
