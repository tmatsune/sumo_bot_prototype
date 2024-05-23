#ifndef PINS_H
#define PINS_H
#include <Arduino.h>

// only 2 external interrupt pins pin 2 / 3
// INT0 = TWO, INT1 = THREE
 
typedef enum {
    ZERO = PD0, ONE = PD1, TWO = PD2, THREE = PD3, FOUR = PD4, FIVE = PD5,
    SIX = PD6, SEVEN = PD7, EIGHT = PB0, NINE = PB1, TEN = PB2, ELEVIN = PB3,
    TWELVE = PB4, THIRTEEN = PB5, A_ZERO = PC0, A_ONE = PC1, A_TWO = PC2,
    A_THREE = PC3, A_FOUR = PC4, A_FIVE = PC5,
} Pin_Type;

typedef enum {
  RISING_EDGE,
  FALLING_EDGE,
} Pin_Trigger;

typedef void (*isr_function)(void);
void config_interrupt_trigger(Pin_Type pin, Pin_Trigger trigger);
void register_isr(Pin_Type pin, isr_function isr);
void unregister_isr(Pin_Type pin);

void enable_interrupt(byte pin);
void disable_interrupt(byte pin);

uint8_t get_pin_port(Pin_Type pin);

#endif