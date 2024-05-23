#include "ir_remote.h"
#include "scripts/queue.h"

#define REMOTE_PIN TWO
#define TIMER_DIVIDER (8u)
#define TIMER_PER_ms (16000000u / TIMER_DIVIDER / 1000u)
static struct Queue *remote_messages = (struct Queue *)malloc(sizeof(struct Queue));

static union {
	struct{
		uint8_t cmd_inverted;
		uint8_t cmd;
		uint8_t addr_inverted;
		uint8_t addr;
	} decoded;
	uint32_t raw;
} ir_message;

static int timer_ms = 0;
static int pulse_count = 0;


// ---------- TIMER1 CONFIG ---------- //
static void timer_init(void){
    TCCR1A = 0;              // CLEAR TIMER1 CONTROL REGISTER
    TCCR1B = (1 << WGM12);   // SET TIMER1 TO CTC MODE 
    OCR1A = TIMER_PER_ms;    // SETS THE VALUE COMPARE MATCH VALUE, WHEN TCNT1 REACHES OCR1A, GOES BACK TO 0
    TCCR1B |= (1 << CS11);   // SETS PRESCALER TO 8
    TIMSK1 |= (1 << OCIE1A); // ENABLE INTERRUPTS ON COMPARE MATCH WITH OCR1A
}

static void ir_stop_timer(void) { TCCR1B = (TCCR1B & ~0x18) | 0x00; }
static void ir_start_timer(void) {
    TCNT1 = 0;
    TCCR1A = 0;            
    TCCR1B = (1 << WGM12); 
    OCR1A = TIMER_PER_ms - 1;
    TCCR1B |= (1 << CS11);
    TIMSK1 |= (1 << OCIE1A);
    timer_ms = 0;
} 
// ----------------------------------- //


// ------------------------------- CHECK BIT PULSE --------------------- //

static inline bool is_bit_pulse(uint16_t pulse)
{
    return 3 <= pulse && pulse <= 34; // CHECK TO MAKE SURE PULSE IS WITHIN 32 BIT signal
}

static inline bool is_message_pulse(uint16_t pulse)
{
    return pulse == 34 || (pulse > 36 && pulse % 2 == 1); // check to see if pulse is withing message bits
}
// -------------------------------------------------------------------- //

static void isr_pulse(){

    ir_stop_timer();

    pulse_count++;
    if(is_bit_pulse(pulse_count)){
        ir_message.raw <<= 1; // SHIFT EXISTING BITS BY 1 TO THE LEFT TO MAKE ROOM FORO THER BITS
        ir_message.raw += (timer_ms >= 2) ? 1 : 0; // IF TIMER BETWEEN PULSES IS > 2 THEN 1 SHOULD BE ADDED ELSE 0
    }
    if(is_message_pulse(pulse_count)){
        queue_push(remote_messages, ir_message.decoded.cmd);
    }

    ir_start_timer();

}

static void receiver_pin_init() {
    pinMode(REMOTE_PIN, INPUT);
    digitalWrite(REMOTE_PIN, LOW);
    enable_interrupt(INT0);
    config_interrupt_trigger(REMOTE_PIN, FALLING_EDGE);
    register_isr(REMOTE_PIN, isr_pulse);
}

void ir_remote_init(void){
    timer_init();
    receiver_pin_init();
}

IR_MESSSAGE get_message(){
    disable_interrupt(INT0);

    IR_MESSSAGE message = IR_MSG_NONE;

    if(!queue_empty(remote_messages)){
        int decoded_message = queue_pop(remote_messages);
        switch (decoded_message)
        {
        case IR_MSG_0:
            message = IR_MSG_0;
            break;
        case IR_MSG_1:
            message = IR_MSG_1;
            break;
        case IR_MSG_2:
            message = IR_MSG_2;
            break;
        default:
            break;
        }
    }

    enable_interrupt(INT0);

    return message;
}


ISR(TIMER1_COMPA_vect){
    if(timer_ms < 150){
        timer_ms++;
    }else{
        ir_stop_timer();
        timer_ms = 0;
        pulse_count = 0;
        ir_message.raw = 0;
    }
}