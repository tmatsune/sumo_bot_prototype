#include <Arduino.h>
#include <periph/pwm.h>
#include <periph/ir_remote.h>
#include <periph/pins.h>
#include <periph/i2c.h>
#include <hal/line_sensors.h>
#include <hal/enemy.h>
#include <hal/state_machine.h>
#include <hal/timer.h>

#include <Wire.h>

uint16_t range = 0;
uint16_t range_2 = 0;
uint16_t range_3 = 0;
bool test_mode = false;
bool test_mode_2 = false;
bool test_timer = false;
uint16_t ranges[3] = {0,0,0};

struct Timer timer = {.time = 0, .timeout=0};
struct State_Machine state_machine;

void setup()
{
  Serial.begin(9600);

  Serial.println("init");

  pwm_init();
  ir_remote_init();
  line_sensors_init();
  i2c_init();

  init_timer();
  timer_set_timeout(&timer, 400);

  state_machine_test_init(&state_machine, timer);

  Serial.println("everything connected!");
}

void loop()
{
  
  bool can_read_1 = read_range(LOC1_ADDR, &range);
  bool can_read_2 = read_range(LOC2_ADDR, &range_2);
  bool can_read_3 = read_range(LOC3_ADDR, &range_3);
  ranges[0] = range;
  ranges[1] = range_2;
  ranges[2] = range_3;

  state_machine_run(&state_machine, ranges);
  Serial.println();
  delay(200);

  if(test_mode){

    if (can_read_1){
      Serial.print("range 1: ");
      Serial.println(range);
    } else Serial.println("failed");

    if(can_read_2){
      Serial.print("range 2: ");
      Serial.println(range_2);
    }else Serial.println("failed");

    if(can_read_3){
      Serial.print("range 3: ");
      Serial.println(range_3);
    }else Serial.println("failed");
    Serial.println();
  } else if(test_mode_2) {
    Enemy enemy = get_enemy(range, range_2, range_3);
    Serial.print("Enemy range: ");
    Serial.print(enemy.enemy_range);
    Serial.print(" Enemy location: ");
    Serial.println(enemy.enemy_location);
  }
  
  //delay(200);

  IR_MESSSAGE message = IR_MSG_NONE; //get_message();
  if (message == IR_MSG_0){
    Serial.println("forward, fast");
    drive_set(DRIVE_DIR_FORWARD, DRIVE_SPEED_FAST);
  } else if (message == IR_MSG_1){
    Serial.println("reverse, fast");
    drive_set(DRIVE_DIR_REVERSE, DRIVE_SPEED_FAST);
  } else if(message == IR_MSG_NONE){

  }

  Line_Pos line_pos = get_lines();
  if(line_pos != LINE_NONE){
    switch(line_pos){
      case LINE_FRONT:
        Serial.println("FRONT");
        break;
      case LINE_FRONT_LEFT:
        Serial.println("FRONT_LEFT");
        break;
      case LINE_FRONT_RIGHT:
        Serial.println("FRONT_RIGHT");
        break;
      default:
        break;
    }
  }
  
  uint32_t time1 = get_time();
  timer_tick(&timer, time1);
  bool timed_out = timer_timed_out(timer);
  if(test_timer){
    Serial.println(timer.time);
    Serial.println(timer.timeout);
    if (timed_out) Serial.println(timed_out);
    Serial.println();
  }


}
