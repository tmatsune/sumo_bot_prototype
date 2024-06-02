#include "vl53lox.h"

const int MPU6050_ADDR = 0x68; // MPU6050 I2C address when AD0 is low
const int WHO_AM_I = 0x75;

#define VL53L0X_ADDR 0x29                                      // The I2C address of the VL53L0X sensor
#define REG_IDENTIFICATION_MODEL_ID 0xC0                       // Register address for device ID
#define VL53L0X_EXPECTED_DEVICE_ID 0xEE                        // Expected device ID
#define REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV (0x89)            // 137
#define REG_MSRC_CONFIG_CONTROL (0x60)                         // 96
#define REG_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT (0x44) // 68
#define REG_SYSTEM_SEQUENCE_CONFIG (0x01)                      // 1
#define REG_DYNAMIC_SPAD_REF_EN_START_OFFSET (0x4F)            // 79
#define REG_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD (0x4E)         // 78
#define REG_GLOBAL_CONFIG_REF_EN_START_SELECT (0xB6)           // 182
#define REG_SYSTEM_INTERRUPT_CONFIG_GPIO (0x0A)                // 10
#define REG_GPIO_HV_MUX_ACTIVE_HIGH (0x84)                     // 132
#define REG_SYSTEM_INTERRUPT_CLEAR (0x0B)                      // 11
#define REG_RESULT_INTERRUPT_STATUS (0x13)                     // 19
#define REG_SYSRANGE_START (0x00)                              // 0
#define REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0 (0xB0)            // 176
#define REG_RESULT_RANGE_STATUS (0x14)                         // 20

#define RANGE_SEQUENCE_STEP_TCC (0x10)  /* Target CentreCheck */
#define RANGE_SEQUENCE_STEP_MSRC (0x04) /* Minimum Signal Rate Check */
#define RANGE_SEQUENCE_STEP_DSS (0x28)  /* Dynamic SPAD selection */
#define RANGE_SEQUENCE_STEP_PRE_RANGE (0x40)
#define RANGE_SEQUENCE_STEP_FINAL_RANGE (0x80)

#define XSHUT_LOC1 7
#define XSHUT_LOC2 6
#define XSHUT_LOC3 4
#define LOC1_ADDR (0x30)
#define LOC2_ADDR (0x31)
#define LOC3_ADDR (0x32)