#ifndef PWM_H
#define PWM_H
#include <Arduino.h>
#include <stdint.h>

typedef enum
{
    LEFT_PWM,
    RIGHT_PWM,
} PWM;

typedef enum
{
    LEFT_MOTOR,
    RIGHT_MOTOR,
} Motor_Driver;

typedef enum
{
    FORWARD,
    REVERSE,
    STOP,
} Motor_Driver_Mode;

typedef enum
{
    DRIVE_DIR_FORWARD,
    DRIVE_DIR_REVERSE,
    DRIVE_DIR_ROTATE_LEFT,
    DRIVE_DIR_ROTATE_RIGHT,
    DRIVE_DIR_ARCTURN_SHARP_LEFT,
    DRIVE_DIR_ARCTURN_SHARP_RIGHT,
    DRIVE_DIR_ARCTURN_MID_LEFT,
    DRIVE_DIR_ARCTURN_MID_RIGHT,
    DRIVE_DIR_ARCTURN_WIDE_LEFT,
    DRIVE_DIR_ARCTURN_WIDE_RIGHT,
} Drive_Dir;

typedef enum
{
    DRIVE_SPEED_SLOW,
    DRIVE_SPEED_MEDIUM,
    DRIVE_SPEED_FAST,
    DRIVE_SPEED_MAX
} Drive_Speed;

void test(Drive_Dir direction, Drive_Speed speed);

// ----------------- PWM LAYER ---------------- //
void pwm_init(void);
void pwm_set_duty_cycle(PWM pwm, int duty_cycle);

// --------------- MOTOR DRIVER -------------- //
void motor_driver_set_mode(Motor_Driver driver, Motor_Driver_Mode mode);
void motor_driver_set_pwm(Motor_Driver driver, int duty_cycle);

// --------------- DRIVE INTERFACE ----------- //
void drive_stop(void);
void drive_set(Drive_Dir direction, Drive_Speed speed);

#endif
