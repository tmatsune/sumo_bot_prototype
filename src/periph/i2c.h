#ifndef I2C_H
#define I2C_H

#include <Arduino.h>
#include <stdint.h>

// POLLING BASED I2C MASTER DRVIER


typedef enum
{
    I2C_RESULT_OK,
    I2C_RESULT_ERROR_START,
    I2C_RESULT_ERROR_TX,
    I2C_RESULT_ERROR_RX,
    I2C_RESULT_ERROR_STOP,
    I2C_RESULT_ERROR_TIMEOUT,
} i2c_message;


void i2c_init(void);
bool i2c_start(void);
bool i2c_stopp(void);
bool i2c_write(uint8_t data);
void i2c_set_slave_address(uint8_t address);

//i2c_message i2c_write(uint8_t *address, uint8_t address_size, uint8_t *data, uint8_t data_size);
//i2c_message i2c_read(uint8_t *address, uint8_t address_size, uint8_t *data, uint8_t data_size);

// WRAPPER FUNCTIONS
i2c_message i2c_read_address_data8(uint8_t address, uint8_t* data);
i2c_message i2c_read_address_data16(uint8_t address, uint16_t *data);
i2c_message i2c_read_address_data32(uint8_t address, uint32_t *data);
i2c_message i2c_write_address_data8(uint8_t address, uint8_t data);

#endif