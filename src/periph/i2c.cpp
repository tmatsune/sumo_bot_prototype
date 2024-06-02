#include "i2c.h"

const int MPU6050_ADDR = 0x68; // MPU6050 I2C address when AD0 is low
const int WHO_AM_I = 0x75;

float RateRoll, RatePitch, RateYaw;
float rateCallibrationRoll, rateCallibrationPitch, RateCallibrationYaw;
int rateCaliNum;

#define REG_IDENTIFICATION_MODEL_ID 0xC0 // Register address for device ID
#define VL53L0X_EXPECTED_DEVICE_ID 0xEE  // Expected device ID
#define REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV (0x89)             // 137
#define REG_MSRC_CONFIG_CONTROL (0x60)                          // 96
#define REG_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT (0x44)  // 68
#define REG_SYSTEM_SEQUENCE_CONFIG (0x01)                       // 1
#define REG_DYNAMIC_SPAD_REF_EN_START_OFFSET (0x4F)             // 79
#define REG_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD (0x4E)          // 78 
#define REG_GLOBAL_CONFIG_REF_EN_START_SELECT (0xB6)            // 182
#define REG_SYSTEM_INTERRUPT_CONFIG_GPIO (0x0A)                 // 10
#define REG_GPIO_HV_MUX_ACTIVE_HIGH (0x84)                      // 132
#define REG_SYSTEM_INTERRUPT_CLEAR (0x0B)                       // 11
#define REG_RESULT_INTERRUPT_STATUS (0x13)                      // 19
#define REG_SYSRANGE_START (0x00)                               // 0
#define REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0 (0xB0)             // 176
#define REG_RESULT_RANGE_STATUS (0x14)                          // 20
#define REG_SLAVE_DEVICE_ADDRESS (0x8A)

#define RANGE_SEQUENCE_STEP_TCC (0x10)  /* Target CentreCheck */
#define RANGE_SEQUENCE_STEP_MSRC (0x04) /* Minimum Signal Rate Check */
#define RANGE_SEQUENCE_STEP_DSS (0x28)  /* Dynamic SPAD selection */
#define RANGE_SEQUENCE_STEP_PRE_RANGE (0x40)
#define RANGE_SEQUENCE_STEP_FINAL_RANGE (0x80)


/*
  Primary I2C READ Protocol
    - Wire.beginTransmission( ADDRESS );    // initiates communication with the slave device at the specified address
    - Wire.write( SLAVE REGISTER );         // specifies which register's data you want to read from the slave device
    - Wire.endTransmission();               // ends the transmission
    - Wire.requestFrom(ADDRESS, 1);         // requests 1 byte of data from the slave device at the specified address
    - int data = Wire.read();               // reads the byte of data sent by the slave device in response to the register address

  Primary I2C WRITE Protocol
    - Wire.beginTransmission( ADDRESS );
    - Wire.write( SLAVE REGISTER );
    - Wire.write( DATA );
    - Wire.endTransmission();
*/

// ------------------------------------------ READ / WRITE I2C -----------------------------------------//


bool i2c_read_addr8_data8(uint8_t address, uint8_t reg, uint8_t *data){
  Wire.beginTransmission(address);
  Wire.write(reg);
  if (Wire.endTransmission() != 0) return false;

  Wire.requestFrom(address, 1);
  if (Wire.available() == 1) {
    *data = Wire.read();
  } else return false;

  return true;
};

bool i2c_read_addr8_data16(uint8_t address, uint8_t reg, uint16_t *data){
  Wire.beginTransmission(address);
  Wire.write(reg);
  if (Wire.endTransmission() != 0) return false;
  Wire.requestFrom(address, 2);
  if (Wire.available() == 2) {
    uint8_t high_byte = Wire.read();
    uint8_t low_byte = Wire.read();
    *data = (high_byte << 8) | low_byte;
  } else return false;

  return true;
};

bool i2c_write_addr8_data8(uint8_t address, uint8_t reg, uint8_t data){
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data);
  uint8_t error = Wire.endTransmission();
  return error == 0;
};

static bool sensor_booted(uint8_t address){
  uint8_t device_id = 0;
  if (!i2c_read_addr8_data8( address, REG_IDENTIFICATION_MODEL_ID, &device_id)){
    Serial.println("cannot find device");
    return false;
  } else Serial.println(device_id, HEX);
  return true;
}

// ------------------------------------------------ VL53lOX CONFIG ---------------------------------------------------//

static void config_sensor_pins(void){
  pinMode(XSHUT_LOC1, OUTPUT);
  pinMode(XSHUT_LOC2, OUTPUT);
  pinMode(XSHUT_LOC3, OUTPUT);

  digitalWrite(XSHUT_LOC1, LOW);
  digitalWrite(XSHUT_LOC2, LOW);
  digitalWrite(XSHUT_LOC3, LOW);
}

static void reset_and_unreset_pins(void){
  // all reset
  digitalWrite(XSHUT_LOC1, LOW);
  digitalWrite(XSHUT_LOC2, LOW);
  delay(10);

  digitalWrite(XSHUT_LOC1, HIGH);
  digitalWrite(XSHUT_LOC2, HIGH);
  delay(10);
}

static void i2c_set_slave_address(uint8_t address) {
  TWAR = (address << 1); // Set the address with a left shift because the address is 7-bit
}

static bool config_sensor_address(uint8_t address){
  return i2c_write_addr8_data8(VL53L0X_ADDR, REG_SLAVE_DEVICE_ADDRESS, address & 0x7F);
}

static bool init_address(uint8_t sensor_idx){
  uint8_t address;
  uint8_t pin;
  switch (sensor_idx) {
  case 1:
    address = LOC1_ADDR;
    pin = XSHUT_LOC1;
    break;
  case 2:
    address = LOC2_ADDR;
    pin = XSHUT_LOC2;
    break;
  case 3:
    address = LOC3_ADDR;
    pin = XSHUT_LOC3;
    break;
  default:
    break;
  }
  digitalWrite(pin, HIGH);

  i2c_set_slave_address(VL53L0X_ADDR);
  delay(400);

  if ( !config_sensor_address(address) ) return false;
  return true;
}


// ------------ CONFIG SENSOR SETTINGS ------------- //

static bool configure_interrupt(uint8_t address) {
  if(!i2c_write_addr8_data8(address, REG_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04)) return false; // config an interrupt on GPIO pin when new sample is ready

  // Read gpio HV MUX from MC on sensor, read HV MUX config and store value, clear the 4th bit and write it back to register 
  uint8_t gpio_hv_mux_active_high = 0;
  if (!i2c_read_addr8_data8(address, REG_GPIO_HV_MUX_ACTIVE_HIGH, &gpio_hv_mux_active_high)) return false;
  gpio_hv_mux_active_high &= ~0x10; // This operation clears the 4th bit
  if (!i2c_write_addr8_data8(address, REG_GPIO_HV_MUX_ACTIVE_HIGH, gpio_hv_mux_active_high)) return false;

  if (!i2c_write_addr8_data8(address, REG_SYSTEM_INTERRUPT_CLEAR, 0x01)) return false; // clears any pending interrupts

  return true;
}

static bool set_sequence_steps_enabled(uint8_t address, uint8_t sequence_step) {
  return i2c_write_addr8_data8(address, REG_SYSTEM_SEQUENCE_CONFIG, sequence_step);
}

/*
  Ref calibration is calibratin of two parameters (VHV and phase call) which are temperature dependent
  these are used to set the device sensitivity 
*/
typedef enum {
  CALIBRATION_TYPE_VHV,
  CALIBRATION_TYPE_PHASE
} calibration_type;

static bool perform_single_ref_calibration(uint8_t address, calibration_type calib_type)
{
  uint8_t sysrange_start = 0;
  uint8_t sequence_config = 0;
  switch (calib_type) {
    case CALIBRATION_TYPE_VHV:
      sequence_config = 0x01;
      sysrange_start = 0x01 | 0x40;
      break;
    case CALIBRATION_TYPE_PHASE:
      sequence_config = 0x02;
      sysrange_start = 0x01 | 0x00;
      break;
  }
  if (!i2c_write_addr8_data8(address, REG_SYSTEM_SEQUENCE_CONFIG, sequence_config))
    return false;
  if (!i2c_write_addr8_data8(address, REG_SYSRANGE_START, sysrange_start))
    return false;
  uint8_t interrupt_status = 0;
  bool success = false;
  do {
    success = i2c_read_addr8_data8(address, REG_RESULT_INTERRUPT_STATUS, &interrupt_status);
  } while (success && ((interrupt_status & 0x07) == 0));
  if(!success) return false;

  if (!i2c_write_addr8_data8(address, REG_SYSTEM_INTERRUPT_CLEAR, 0x01)) return false;
  if (!i2c_write_addr8_data8(address, REG_SYSRANGE_START, 0x00)) return false;

  return true;
}

static bool perform_ref_calibration(uint8_t address){
    if (!perform_single_ref_calibration(address, CALIBRATION_TYPE_VHV)) return false;
    if (!perform_single_ref_calibration(address, CALIBRATION_TYPE_PHASE))  return false;

    if (!set_sequence_steps_enabled(address, 0xE8)) return false;
    return true;
}

static bool loc_init(uint8_t address){
  bool booted_sensor = sensor_booted(address);
  if(!booted_sensor) Serial.println("ERROR failed to boot sensor");
  else Serial.println("sensor booted!");

  bool data_initialized = data_init(address);
  if(!data_initialized) Serial.println("ERROR configuring to 2v8");
  else Serial.println("configed to 2v8!");

  bool loaded_default_settings = load_default_settings(address);
  if(!loaded_default_settings) Serial.println("ERROR configuring settings");
  else Serial.println("loaded default settings!");

  bool interrupt_configured = configure_interrupt(address);
  if(!interrupt_configured) Serial.println("ERROR configureing interrupt");
  else Serial.println("interrupt configured!");

  // writeReg(SYSTEM_SEQUENCE_CONFIG, 0xE8); VL53L0 library config is equal to function below
  bool set_sequence_steps = set_sequence_steps_enabled(address, 0xE8); // 0xE8
  if(!set_sequence_steps) Serial.println("ERROR setting up sequence");
  else Serial.println("sequence enabled!");

  bool ref_calibration = perform_ref_calibration(address);
  if(!ref_calibration) Serial.println("ERROR setting up reference calibration");
  else Serial.println("ref calibrated!");

  return booted_sensor & data_initialized & loaded_default_settings & interrupt_configured & set_sequence_steps & ref_calibration;
}

static uint8_t stop_variable = 0;
bool read_range(uint8_t address, uint16_t *range) {
  bool success = i2c_write_addr8_data8(address, 0x80, 0x01);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x01);
  success &= i2c_write_addr8_data8(address, 0x00, 0x00);
  success &= i2c_write_addr8_data8(address, 0x91, stop_variable);
  success &= i2c_write_addr8_data8(address, 0x00, 0x01);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x00);
  success &= i2c_write_addr8_data8(address, 0x80, 0x00);
  if (!success) {
    return false;
  }

  if (!i2c_write_addr8_data8(address, REG_SYSRANGE_START, 0x01)) return false;

  uint8_t sysrange_start = 0;
  do {
      success = i2c_read_addr8_data8(address, REG_SYSRANGE_START, &sysrange_start);
  } while (success && (sysrange_start & 0x01)); // checks busy flag indicating operation is in progress
  if (!success) return false;

  uint8_t interrupt_status = 0;
  do {
      success = i2c_read_addr8_data8(address, REG_RESULT_INTERRUPT_STATUS, &interrupt_status);
  } while (success && ((interrupt_status & 0x07) == 0));
  if (!success) return false; 

  if (!i2c_read_addr8_data16(address, REG_RESULT_RANGE_STATUS + 10, range)) return false;

  if (!i2c_write_addr8_data8(address, REG_SYSTEM_INTERRUPT_CLEAR, 0x01)) return false;

  return true;
}


void i2c_init() {
  Wire.begin();
  // ---- MULTIPLE PINS SETUP ---- // 

  config_sensor_pins();
  reset_and_unreset_pins();

  digitalWrite(XSHUT_LOC2, LOW);
  digitalWrite(XSHUT_LOC3, LOW);
  if(!init_address(1)){
    Serial.println("failed to init address 1");
    while(1);
  }
  if(!loc_init(LOC1_ADDR)){
    Serial.println("Failed to init first sensor");
    while(1);
  }
  
  delay(50);

  if(!init_address(2)){
    Serial.println("failed to init address 2");
    while(1);
  }
  if(!loc_init(LOC2_ADDR)){
    Serial.println("Failed to init second sensor");
    while(1);
  }

  delay(50);
  if(!init_address(3)){
    Serial.println("failed to init address 3");
    while(1);
  }
  if(!loc_init(LOC3_ADDR)){
    Serial.println("Failed to init third sensor");
    while(1);
  }


}


bool data_init(uint8_t address){

  bool success = false;
  uint8_t vhv_config_scl_sda = 0;
  if(!i2c_read_addr8_data8(address, REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV, &vhv_config_scl_sda)){
    return false;
  }
  vhv_config_scl_sda |= 0x01;
  if(!i2c_write_addr8_data8(address, REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV, vhv_config_scl_sda)){
    return false;
  };

  success = i2c_write_addr8_data8(address, 0x88, 0x00);

  success &= i2c_write_addr8_data8(address, 0x80, 0x01);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x01);
  success &= i2c_write_addr8_data8(address, 0x00, 0x00);
  success &= i2c_read_addr8_data8(address, 0x91, &stop_variable);
  success &= i2c_write_addr8_data8(address, 0x00, 0x01);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x00);
  success &= i2c_write_addr8_data8(address, 0x80, 0x00);

  return success;
}

bool load_default_settings(uint8_t address){
  bool success = i2c_write_addr8_data8(address, 0xFF, 0x01);
  success &= i2c_write_addr8_data8(address, 0x00, 0x00);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x00);
  success &= i2c_write_addr8_data8(address, 0x09, 0x00);
  success &= i2c_write_addr8_data8(address, 0x10, 0x00);
  success &= i2c_write_addr8_data8(address, 0x11, 0x00);
  success &= i2c_write_addr8_data8(address, 0x24, 0x01);
  success &= i2c_write_addr8_data8(address, 0x25, 0xFF);
  success &= i2c_write_addr8_data8(address, 0x75, 0x00);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x01);
  success &= i2c_write_addr8_data8(address, 0x4E, 0x2C);
  success &= i2c_write_addr8_data8(address, 0x48, 0x00);
  success &= i2c_write_addr8_data8(address, 0x30, 0x20);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x00);
  success &= i2c_write_addr8_data8(address, 0x30, 0x09);
  success &= i2c_write_addr8_data8(address, 0x54, 0x00);
  success &= i2c_write_addr8_data8(address, 0x31, 0x04);
  success &= i2c_write_addr8_data8(address, 0x32, 0x03);
  success &= i2c_write_addr8_data8(address, 0x40, 0x83);
  success &= i2c_write_addr8_data8(address, 0x46, 0x25);
  success &= i2c_write_addr8_data8(address, 0x60, 0x00);
  success &= i2c_write_addr8_data8(address, 0x27, 0x00);
  success &= i2c_write_addr8_data8(address, 0x50, 0x06);
  success &= i2c_write_addr8_data8(address, 0x51, 0x00);
  success &= i2c_write_addr8_data8(address, 0x52, 0x96);
  success &= i2c_write_addr8_data8(address, 0x56, 0x08);
  success &= i2c_write_addr8_data8(address, 0x57, 0x30);
  success &= i2c_write_addr8_data8(address, 0x61, 0x00);
  success &= i2c_write_addr8_data8(address, 0x62, 0x00);
  success &= i2c_write_addr8_data8(address, 0x64, 0x00);
  success &= i2c_write_addr8_data8(address, 0x65, 0x00);
  success &= i2c_write_addr8_data8(address, 0x66, 0xA0);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x01);
  success &= i2c_write_addr8_data8(address, 0x22, 0x32);
  success &= i2c_write_addr8_data8(address, 0x47, 0x14);
  success &= i2c_write_addr8_data8(address, 0x49, 0xFF);
  success &= i2c_write_addr8_data8(address, 0x4A, 0x00);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x00);
  success &= i2c_write_addr8_data8(address, 0x7A, 0x0A);
  success &= i2c_write_addr8_data8(address, 0x7B, 0x00);
  success &= i2c_write_addr8_data8(address, 0x78, 0x21);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x01);
  success &= i2c_write_addr8_data8(address, 0x23, 0x34);
  success &= i2c_write_addr8_data8(address, 0x42, 0x00);
  success &= i2c_write_addr8_data8(address, 0x44, 0xFF);
  success &= i2c_write_addr8_data8(address, 0x45, 0x26);
  success &= i2c_write_addr8_data8(address, 0x46, 0x05);
  success &= i2c_write_addr8_data8(address, 0x40, 0x40);
  success &= i2c_write_addr8_data8(address, 0x0E, 0x06);
  success &= i2c_write_addr8_data8(address, 0x20, 0x1A);
  success &= i2c_write_addr8_data8(address, 0x43, 0x40);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x00);
  success &= i2c_write_addr8_data8(address, 0x34, 0x03);
  success &= i2c_write_addr8_data8(address, 0x35, 0x44);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x01);
  success &= i2c_write_addr8_data8(address, 0x31, 0x04);
  success &= i2c_write_addr8_data8(address, 0x4B, 0x09);
  success &= i2c_write_addr8_data8(address, 0x4C, 0x05);
  success &= i2c_write_addr8_data8(address, 0x4D, 0x04);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x00);
  success &= i2c_write_addr8_data8(address, 0x44, 0x00);
  success &= i2c_write_addr8_data8(address, 0x45, 0x20);
  success &= i2c_write_addr8_data8(address, 0x47, 0x08);
  success &= i2c_write_addr8_data8(address, 0x48, 0x28);
  success &= i2c_write_addr8_data8(address, 0x67, 0x00);
  success &= i2c_write_addr8_data8(address, 0x70, 0x04);
  success &= i2c_write_addr8_data8(address, 0x71, 0x01);
  success &= i2c_write_addr8_data8(address, 0x72, 0xFE);
  success &= i2c_write_addr8_data8(address, 0x76, 0x00);
  success &= i2c_write_addr8_data8(address, 0x77, 0x00);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x01);
  success &= i2c_write_addr8_data8(address, 0x0D, 0x01);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x00);
  success &= i2c_write_addr8_data8(address, 0x80, 0x01);
  success &= i2c_write_addr8_data8(address, 0x01, 0xF8);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x01);
  success &= i2c_write_addr8_data8(address, 0x8E, 0x01);
  success &= i2c_write_addr8_data8(address, 0x00, 0x01);
  success &= i2c_write_addr8_data8(address, 0xFF, 0x00);
  success &= i2c_write_addr8_data8(address, 0x80, 0x00);
  return success;
}


// ------------ GYRO ----------- //
void mpu_init(void){
  Wire.begin();
  Wire.beginTransmission(MPU6050_ADDR);
  delay(250); // delay

  Wire.write(WHO_AM_I);              // write address to slave
  Wire.endTransmission();            // end write operation

  Wire.requestFrom(MPU6050_ADDR, 1); // read from mpu addresss, 1 byte
  if (Wire.available()) {                  // If data is available
    uint8_t who_am_i = Wire.read();        // Read the data
    Serial.print("WHO_AM_I register: 0x"); // Print label
    Serial.println(who_am_i, HEX);         // Print the value in HEX format
  }
  else
  {
    Serial.println("Failed to read from MPU6050"); // Error message
  }

  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);

  Wire.endTransmission();
}

void read_signals(void){
  Wire.beginTransmission(MPU6050_ADDR); // start connection with slave

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x1A); // write to address register 26
  Wire.write(0x05); // (1 << Bit0) | (1 << Bit1) | (1 << Bit3)
  Wire.endTransmission();

  Wire.beginTransmission(MPU6050_ADDR); // begin eonnectino with slave
  Wire.write(0x1B);                     // Access the GYRO_CONFIG register
  Wire.write(0x8);                      // Set the gyroscope full-scale range to ±500 dps
  Wire.endTransmission();

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x43); // import measurment values located 43 -> 48 and specify the starting register address
  Wire.endTransmission();

  Wire.requestFrom(MPU6050_ADDR, 6); // request 6 bytes

  int16_t gyro_x = Wire.read() << 8 | Wire.read(); // read first 8 bits which is the HIGH byte then shift to the left 1 byte to read LOW byte
  int16_t gyro_y = Wire.read() << 8 | Wire.read();
  int16_t gyro_z = Wire.read() << 8 | Wire.read(); 

  // convert measurments to units in degerees
  RateRoll = float(gyro_x) / 65.5;
  RatePitch = float(gyro_y) / 65.5;
  RateYaw = float(gyro_z) / 65.5;

  Serial.println(RateRoll);
  Serial.println(RatePitch);
  Serial.println(RateYaw);
  Serial.println();
};








