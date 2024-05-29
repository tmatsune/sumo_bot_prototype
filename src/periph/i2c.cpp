#include "i2c.h"

/*
_delay_ms(10);                      // Wait for the MPU6000 to stabilize
   uint8_t who_am_i = MPU6000_read_register(0x75); // Read WHO_AM_I register
   Serial.println(who_am_i);
  

#define F_CPU 16000000UL  // Define CPU frequency
#define SCL_CLOCK 100000L // Define SCL frequency (100kHz)
#define MPU6000_ADDRESS 0x68


float RateRoll, RatePitch, RateYaw;
float rateCallibrationRoll, rateCallibrationPitch, RateCallibrationYaw;
int rateCaliNum;


void I2C_init(void)
{
  // Set SCL frequency
  TWSR = 0x00;                           // Set prescaler to 1
  TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2; // Set bit rate register

  // Enable TWI
  TWCR = (1 << TWEN);
}

void I2C_start(void)
{
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Send start condition
  while (!(TWCR & (1 << TWINT)))
    ; // Wait for TWINT flag set
}

void I2C_stop(void)
{
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Send stop condition
  while (TWCR & (1 << TWSTO))
    ; // Wait for stop condition to be executed
}

void I2C_write(uint8_t data)
{
  TWDR = data;                       // Load data into TWDR register
  TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission of data
  while (!(TWCR & (1 << TWINT)))
    ; // Wait for TWINT flag set
}

uint8_t I2C_read_ack(void)
{
  TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN); // Enable ACK generation
  while (!(TWCR & (1 << TWINT)))
    ;          // Wait for TWINT flag set
  return TWDR; // Return received data
}

uint8_t I2C_read_nack(void)
{
  TWCR = (1 << TWINT) | (1 << TWEN); // Enable NACK generation
  while (!(TWCR & (1 << TWINT)))
    ;          // Wait for TWINT flag set
  return TWDR; // Return received data
}

void MPU6000_write_register(uint8_t reg, uint8_t data)
{
  I2C_start();                     // Send start condition
  I2C_write(MPU6000_ADDRESS << 1); // Send MPU6000 address with write flag
  I2C_write(reg);                  // Send register address
  I2C_write(data);                 // Send data
  I2C_stop();                      // Send stop condition
}

uint8_t MPU6000_read_register(uint8_t reg)
{
  uint8_t data;
  I2C_start();                           // Send start condition
  I2C_write(MPU6000_ADDRESS << 1);       // Send MPU6000 address with write flag
  I2C_write(reg);                        // Send register address
  I2C_start();                           // Send repeated start condition
  I2C_write((MPU6000_ADDRESS << 1) | 1); // Send MPU6000 address with read flag
  data = I2C_read_nack();                // Read data with NACK
  I2C_stop();                            // Send stop condition
  return data;
}

void MPU6000_read_multiple_registers(uint8_t start_reg, uint8_t *buffer, uint8_t length)
{
  I2C_start();                           // Send start condition
  I2C_write(MPU6000_ADDRESS << 1);       // Send MPU6000 address with write flag
  I2C_write(start_reg);                  // Send start register address
  I2C_start();                           // Send repeated start condition
  I2C_write((MPU6000_ADDRESS << 1) | 1); // Send MPU6000 address with read flag
  for (uint8_t i = 0; i < length - 1; i++)
  {
    buffer[i] = I2C_read_ack(); // Read data with ACK
  }
  buffer[length - 1] = I2C_read_nack(); // Read last data with NACK
  I2C_stop();                           // Send stop condition
}

void gyro_signals(void)
{

  // Switch on the low pass filter
  MPU6000_write_register(0x1A, 0x05); // Choose low pass filter with frequency of 10 Hz

  // Set the sensitivity scale factor
  MPU6000_write_register(0x1B, 0x08); // LSB sensitivity of 65.5

  // Access the register storing the gyro measurements
  uint8_t buffer[6];
  MPU6000_read_multiple_registers(0x43, buffer, 6); // Read 6 bytes from the gyro registers

  // Merge the two bytes to form a 16-bit integer
  int16_t gyro_x = (buffer[0] << 8) | buffer[1];
  int16_t gyro_y = (buffer[2] << 8) | buffer[3];
  int16_t gyro_z = (buffer[4] << 8) | buffer[5];

  // Convert measurements to units in degrees per second
  RateRoll = (float)gyro_x / 65.5;
  RatePitch = (float)gyro_y / 65.5;
  RateYaw = (float)gyro_z / 65.5;

  Serial.println(RateRoll);
  Serial.println(RatePitch);
  Serial.println(RateYaw);
  Serial.println();
}
*/