#ifndef ADC_H
#define ADC_H
#include <Arduino.h>

#define ADC_COUNT 4
typedef int adc_input_values[ADC_COUNT];

void adc_init(void);
void start_conversion(void);
void get_adc_input_values(adc_input_values vals);

#endif