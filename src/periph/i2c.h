#ifndef I2C_H
#define I2C_H

#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>

// POLLING BASED I2C MASTER DRVIER

#define XSHUT_LOC1 7
#define XSHUT_LOC2 6
#define XSHUT_LOC3 4
#define LOC1_ADDR (0x30)
#define LOC2_ADDR (0x31)
#define LOC3_ADDR (0x32)

#define VL53L0X_ADDR 0x29 // The I2C address of the VL53L0X sensor

typedef enum
{
    I2C_RESULT_OK,
    I2C_RESULT_ERROR_START,
    I2C_RESULT_ERROR_TX,
    I2C_RESULT_ERROR_RX,
    I2C_RESULT_ERROR_STOP,
    I2C_RESULT_ERROR_TIMEOUT,
} i2c_message;

void i2c_init(void); // sets up 
bool i2c_start(void);
bool i2c_stopp(void);
bool i2c_write(uint8_t data);
void i2c_set_slave_address(uint8_t address);

bool i2c_read_addr8_data8(uint8_t address, uint8_t reg, uint8_t *data);
bool i2c_read_addr8_data16(uint8_t address, uint8_t reg, uint8_t *data);
bool i2c_write_addr8_data8(uint8_t address, uint8_t reg, uint8_t data);

bool data_init(uint8_t address);
bool load_default_settings(uint8_t address);
bool read_range(uint8_t address, uint16_t *range);

void mpu_init(void);
void read_signals(void);

#endif