#include <Arduino.h>
#include <periph/pwm.h>
#include <periph/ir_remote.h>
#include <periph/pins.h>
//#include <periph/adc.h>
#include <hal/line_sensors.h>

void setup() {
  // -------- TEST BOARD --------//

  Serial.begin(9600);
  Serial.println("INIT");

  // ---------- INIT PERIPHERALS --------- //

  pwm_init();
  ir_remote_init();

  line_sensors_init();

  // ------------------------------------ //

}

void loop() {

  get_lines();

  delay(100);
  IR_MESSSAGE message = get_message();
  switch (message)
  {
  case IR_MSG_0:
    Serial.println("FORWARD");
    test(DRIVE_DIR_FORWARD, DRIVE_SPEED_FAST);
    break;
  case IR_MSG_1:
    Serial.println("REVERSE");
    test(DRIVE_DIR_REVERSE, DRIVE_SPEED_FAST);
    break;
  case IR_MSG_2:
    Serial.println("STOP");
    drive_stop();
  default:
    break;
  }

}
