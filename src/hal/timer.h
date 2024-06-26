#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>
#include <stdint.h>

typedef uint32_t timer_count;

struct Timer {
    uint32_t time;
    uint32_t timeout;
};

void init_timer(void);
void tick(void);
timer_count get_time(void);
void timer_tick(struct Timer* timer, uint32_t main_clock_time);
void timer_set_timeout(struct Timer* timer, uint32_t timeout);
bool timer_timed_out(const struct Timer timer);
void timer_reset(struct Timer* timer);
void timer_hard_reset(struct Timer* timer);

#endif