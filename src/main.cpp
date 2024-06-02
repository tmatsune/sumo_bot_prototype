#include <Arduino.h>
#include <periph/pwm.h>
#include <periph/ir_remote.h>
#include <periph/pins.h>
#include <hal/line_sensors.h>
#include <periph/i2c.h>
#include <hal/enemy.h>
//#include <scripts/assert_script.h>

#include <Wire.h>

uint16_t range = 0;
uint16_t range_2 = 0;
uint16_t range_3 = 0;
bool test_mode = false;

void setup()
{
  Serial.begin(9600);

  Serial.println("init");
  
  pwm_init();
  ir_remote_init();
  line_sensors_init();
  i2c_init();

  Serial.println("everything connected!");
}

void loop()
{
  
  bool can_read_1 = read_range(LOC1_ADDR, &range);
  bool can_read_2 = read_range(LOC2_ADDR, &range_2);
  bool can_read_3 = read_range(LOC3_ADDR, &range_3);

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
  } else {
    struct Enemy enemy = get_enemy(range, range_2, range_3);
    Serial.print("Enemy range: ");
    Serial.print(enemy.enemy_range);
    Serial.print(" Enemy location: ");
    Serial.println(enemy.enemy_location);
  }
  
  delay(200);

  IR_MESSSAGE message = get_message();
  if (message == IR_MSG_0){
    Serial.println("forward, fast");
    drive_set(DRIVE_DIR_FORWARD, DRIVE_SPEED_FAST);
  } else if (message == IR_MSG_1){
    Serial.println("reverse, fast");
    drive_set(DRIVE_DIR_REVERSE, DRIVE_SPEED_FAST);
  } 

  get_lines();
  Serial.println();

}

/*
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31
#define LOX3_ADDRESS 0x32
int sensor1, sensor2, sensor3;

// set the pins to shutdown
#define SHT_LOX1 7
#define SHT_LOX2 6
#define SHT_LOX3 4

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

void setID()
{
  // all reset
  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);

  // initing LOX1
  if (!lox1.begin(LOX1_ADDRESS))
  {
    Serial.println(F("Failed to boot first VL53L0X"));
    while (1)
      ;
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // initing LOX2
  if (!lox2.begin(LOX2_ADDRESS))
  {
    Serial.println(F("Failed to boot second VL53L0X"));
    while (1)
      ;
  }
}

void read_dual_sensors()
{

  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!

  Serial.print("1: ");
  if (measure1.RangeStatus != 4)
  { // if not out of range
    sensor1 = measure1.RangeMilliMeter;
    Serial.print(sensor1);
    Serial.print("mm");
  }
  else {
    Serial.print("Out of range");
  }

  Serial.print(" 2: ");
  if (measure2.RangeStatus != 4)
  {
    sensor2 = measure2.RangeMilliMeter;
    Serial.print(sensor2);
    Serial.print("mm");
  }
  else{
    Serial.print("Out of range");
  }

  Serial.println();
}

void setup()
{
  Serial.begin(9600);

  Serial.println("init");

  while (!Serial)
  {
    delay(1);
  }

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);

  setID();

  pwm_init();
  ir_remote_init();
  line_sensors_init();
}

*/
