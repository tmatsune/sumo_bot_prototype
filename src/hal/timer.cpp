#include "hal/timer.h"
#define MAX_TIME (6000u)

static volatile timer_count time = 0;
static volatile int seconds = 0;
static bool test_time = false;

static void reset_main_time(void){ time = 0; }

void init_timer(void){
    WDTCSR = (1 << WDIE);
}

timer_count get_time(void){
    return time;
}

void tick(){
    time++;
    if(time > MAX_TIME) time = 0;
    if(test_time){
        if (time > 62) {
            seconds++;
            Serial.println(seconds);
            time = 0;
        }
    }
}
void timer_tick(struct Timer* timer, uint32_t main_clock_time){
    timer->time = main_clock_time;
}

void timer_set_timeout(struct Timer* timer, uint32_t timeout){
    timer->timeout = timeout;
}

bool timer_timed_out(const struct Timer timer){
    if(timer.time > timer.timeout & timer.timeout > 0) reset_main_time();
    return timer.time > timer.timeout & timer.timeout > 0;
}

void timer_reset(struct Timer* timer){
    timer->time = 0;
}
void timer_hard_reset(struct Timer* timer){
    timer->time = 0;
    timer->timeout = 0;
}

ISR(WDT_vect) {
    tick();
}