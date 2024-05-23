#include "pins.h"
#define PORT_COUNT (3u)
#define ISR_PIN_COUNT (2u)

typedef enum {
  INTERRUPT_0,
  INTERRUPT_1,
} interrupt_pin; 

volatile uint8_t *port_direction_register[PORT_COUNT] = {&DDRB, &DDRC, &DDRD};
volatile uint8_t *port_on_register[PORT_COUNT] = {&PORTB, &PORTC, &PORTD};

static isr_function interrupt_functions[ISR_PIN_COUNT][1] = {
    [INTERRUPT_0] = {NULL},
    [INTERRUPT_1] = {NULL},
};

uint8_t get_pin_port(Pin_Type pin){
    if(pin == ONE || pin == TWO || pin == THREE || pin == FOUR || pin == FIVE || pin == SIX || pin == SEVEN){ return 0; }
    else if(pin == EIGHT || pin == NINE || pin == TEN || pin == ELEVIN || pin == TWELVE || pin == THIRTEEN){ return 1; }
    else return 2;  
}

void config_interrupt_trigger(Pin_Type pin, Pin_Trigger trigger){
    switch (pin)
    {
    case TWO:
        switch (trigger)
        {
        case FALLING_EDGE:
            EICRA = (0 << ISC00) | (1 << ISC01);
            break;
        case RISING_EDGE:
            EICRA |= (1 << ISC01) | (1 << ISC00);
            break;
        default:
            break;
        }
        break;
    case THREE:
        switch (trigger)
        {
        case FALLING_EDGE:
            EICRA |= (0 << ISC10) | (1 << ISC11);
            break;
        case RISING_EDGE:
            EICRA |= (1 << ISC10) | (1 << ISC11);
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void register_isr(Pin_Type pin, isr_function isr){
    switch (pin)
    {
    case TWO:
        interrupt_functions[INTERRUPT_0][0] = isr;
        break;
    case THREE:
        interrupt_functions[INTERRUPT_1][0] = isr;
    default:
        break;
    }
}

void unregister_isr(Pin_Type pin){
    switch (pin)
    { 
    case TWO:
        interrupt_functions[INTERRUPT_0][0] = NULL;
        break;
    case THREE:
        interrupt_functions[INTERRUPT_1][0] = NULL;
    default:
        break;
    }
}

void enable_interrupt(byte pin){ EIMSK |= (1 << pin); }
void disable_interrupt(byte pin) { EIMSK |= (0 << pin); }

ISR(INT0_vect){
    if(interrupt_functions[INTERRUPT_0][0]){
        interrupt_functions[INTERRUPT_0][0]();
    }
}
ISR(INT1_vect){
    if(interrupt_functions[INTERRUPT_1][0]){
        interrupt_functions[INTERRUPT_1][0]();
    }
}
