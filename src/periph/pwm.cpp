#include "pwm.h"

/*
    Pulse Width Modulation (PWM) is a square wave where you modulate the amount of timer it
    stays on in ratio to the total period of the signal.
    How long you keep the pin HIGH is the duty cycle
    IMPLEMENTATION:
        PWM LAYER:
            - Use Timer2 and pins 11 & 3 for left and right motor scince these pins are connected to TIMER2 (OC2RB) & (OC2RA)
            - Set Timer2 to FAST PWM MODE for pwm, then set the two compare match values to clear when value is reached
            - Set presclaer to 8 
        MOTOR_DRIVER LAYER:
            - This layer will control the two motor drivers. The motor drivers are h-brides that can change the 
              the motors direction based on which pin is HIGH and LOW, (LOW & LOW) will stop the motor
            - left motor driver will be controlled to pins 12 & 13, right motor driver will be controlled by pins 8 & 9
            - Speed will also can also be adjusted by changes the duty cycle of each motor driver
        DRIVE LAYER:
            - This layer will serve as a drive interface for the user
            - Predetermined directions and speed will be implemented
            - direction and speed will be represented as enums
            - direction and speed will correspond with array of structs that will have instructions for speed
*/

// PINS 11 & 3 for PWM
#define LEFT_PWM_PIN 11
#define RIGHT_PWM_PIN 3

// PINS 12 & 13 for LEFT_MOTOR
#define LEFT_MOTOR_CC1 12
#define LEFT_MOTOR_CC2 13

// PINS 8 & 9 for RIGHT MOTOR
#define RIGHT_MOTOR_CC1 8
#define RIGHT_MOTOR_CC2 9

//-------------------------------------------------PWM-------------------------------------------------//

// PWM CONFIG
struct Pwm_Channel
{
    bool enabled;
    volatile int duty_cycle; // SET MODE OF CAPTURE/COMPARE REGISTER
    int duty_cycle_percent;
};

// CONTROLS PWM
static int left_duty_cycle_percent = 0;
static int right_duty_cycle_percent = 0;

void pwm_init(void)
{
    pinMode(LEFT_PWM_PIN, OUTPUT);
    pinMode(RIGHT_PWM_PIN, OUTPUT);

    // -----____-----____ WHEN DUTY CYCLE IS 50, SET TIMER COMPARE(OCR2A) to 125 //

    // WGM20 & WGM21 SET TIMER2 TO FAST PWM MODE
    // COM2A1 WILL CLEARS OC2RA ON COMAPRE MATCH & SETS AT BOTTOM 
    // COM2B1 WILL CLEARS OC2RB ON COMPARE MATCH & SETS AT BOTTOM 
    TCCR2A = (1 << COM2A1) | (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
    TIMSK2 = (1 << TOIE2); // ENABLE OVERFLOW INTERRUPT FOR TIMER 2

    OCR2A = 0; // OC2RA IS TIMER COMPARE MATCH FOR PIN 11(PB3)
    OCR2B = 0; // OC2RB IS TIMER COMPARE MATCH FOR PIN 3(PD3)
    TCCR2B = (1 << CS20); // SETS PRESCALER TO 8

    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);

    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
}

ISR(TIMER2_OVF_vect)
{
    //OCR2A = (pwm_configs[LEFT_PWM].duty_cycle_percent / 100.0) * 255;
    //OCR2B = (pwm_configs[RIGHT_PWM].duty_cycle_percent / 100.0) * 255;
    OCR2A = left_duty_cycle_percent;
    OCR2B = right_duty_cycle_percent;
}
void pwm_set_duty_cycle(PWM pwm, int duty_cycle) { 
    switch (pwm)
    {
    case LEFT_PWM:
        left_duty_cycle_percent = duty_cycle;
        break;
    case RIGHT_PWM:
        right_duty_cycle_percent = duty_cycle;
    default:
        break;
    }
}

//----------------------------------------------MOTOR DRIVER--------------------------------------------//

struct Motor
{
    volatile uint8_t cc1;
    volatile uint8_t cc2;
};

static struct Motor motors[] = {
    [LEFT_MOTOR] = {.cc1 = LEFT_MOTOR_CC1, .cc2 = LEFT_MOTOR_CC2},
    [RIGHT_MOTOR] = {
        .cc1 = RIGHT_MOTOR_CC1,
        .cc2 = RIGHT_MOTOR_CC2,
    },
};

void motor_driver_set_pwm(Motor_Driver driver, int duty_cycle) { 
    
    pwm_set_duty_cycle((PWM)driver, duty_cycle); // FIX THIS
}

void motor_driver_set_mode(Motor_Driver driver, Motor_Driver_Mode mode)
{

    switch (mode)
    {
    case FORWARD:
        digitalWrite(motors[driver].cc1, LOW);
        digitalWrite(motors[driver].cc2, HIGH);
        break;
    case REVERSE:
        digitalWrite(motors[driver].cc1, HIGH);
        digitalWrite(motors[driver].cc2, LOW);
        break;
    case STOP:
        digitalWrite(motors[driver].cc1, LOW);
        digitalWrite(motors[driver].cc2, LOW);
        break;

    default:
        break;
    }
}

//-------------------------------------------------- DRIVE ----------------------------------------------//

struct Drive_Setting
{
    int8_t left;  // LEFT_MOTOR
    int8_t right; // RIGHT_MOTOR
};

static const struct Drive_Setting drive_settings[][4] = {
    // DRIVE_DIR_FORWARD
    [DRIVE_DIR_FORWARD] = {
        [DRIVE_SPEED_SLOW] = {.left = 25, .right = 25},
        [DRIVE_SPEED_MEDIUM] = {.left = 45, .right = 45},
        [DRIVE_SPEED_FAST] = {.left = 55, .right = 55},
        [DRIVE_SPEED_MAX] = {.left = 100, .right = 100},
    },
    // DRIVE_DIR_REVERSE
    [DRIVE_DIR_REVERSE] = {
        [DRIVE_SPEED_SLOW] = {.left = -25, .right = -25},
        [DRIVE_SPEED_MEDIUM] = {.left = -45, .right = -45},
        [DRIVE_SPEED_FAST] = {.left = -55, .right = -55},
        [DRIVE_SPEED_MAX] = {.left = -100, .right = -100},
    },
    // DRIVE_DIR_ROTATE_LEFT
    [DRIVE_DIR_ROTATE_LEFT] = {
        [DRIVE_SPEED_SLOW] = {.left = -25, .right = 25},
        [DRIVE_SPEED_MEDIUM] = {.left = -50, .right = 50},
        [DRIVE_SPEED_FAST] = {.left = -60, .right = 60},
        [DRIVE_SPEED_MAX] = {.left = -100, .right = 100},
    },
    // DRIVE_DIR_ROTATE_RIGHT
    [DRIVE_DIR_ROTATE_RIGHT] = {
        [DRIVE_SPEED_SLOW] = {.left = 25, .right = -25},
        [DRIVE_SPEED_MEDIUM] = {.left = 50, .right = -50},
        [DRIVE_SPEED_FAST] = {.left = 60, .right = -60},
        [DRIVE_SPEED_MAX] = {.left = 100, .right = -100},
    },
    // DRIVE_DIR_ARCTURN_SHARP_LEFT
    [DRIVE_DIR_ARCTURN_SHARP_LEFT] = {
        [DRIVE_SPEED_SLOW] = {.left = -10, .right = 25},
        [DRIVE_SPEED_MEDIUM] = {.left = -10, .right = 50},
        [DRIVE_SPEED_FAST] = {.left = -25, .right = 75},
        [DRIVE_SPEED_MAX] = {.left = -20, .right = 100},
    },
    // DRIVE_DIR_ARCTURN_SHARP_RIGHT
    [DRIVE_DIR_ARCTURN_SHARP_RIGHT] = {
        [DRIVE_SPEED_SLOW] = {.left = 25, .right = -10},
        [DRIVE_SPEED_MEDIUM] = {.left = 50, .right = -10},
        [DRIVE_SPEED_FAST] = {.left = 75, .right = -25},
        [DRIVE_SPEED_MAX] = {.left = 100, .right = -20},
    },
    // DRIVE_DIR_ARCTURN_MID_LEFT
    [DRIVE_DIR_ARCTURN_MID_LEFT] = {
        [DRIVE_SPEED_SLOW] = {.left = 15, .right = 30},
        [DRIVE_SPEED_MEDIUM] = {.left = 25, .right = 50},
        [DRIVE_SPEED_FAST] = {.left = 35, .right = 70},
        [DRIVE_SPEED_MAX] = {.left = 50, .right = 100},
    },
    // DRIVE_DIR_ARCTURN_MID_RIGHT
    [DRIVE_DIR_ARCTURN_MID_RIGHT] = {
        [DRIVE_SPEED_SLOW] = {.left = 30, .right = 15},
        [DRIVE_SPEED_MEDIUM] = {.left = 50, .right = 25},
        [DRIVE_SPEED_FAST] = {.left = 70, .right = 35},
        [DRIVE_SPEED_MAX] = {.left = 100, .right = 50},
    },
    // DRIVE_DIR_ARCTURN_WIDE_LEFT
    [DRIVE_DIR_ARCTURN_WIDE_LEFT] = {
        [DRIVE_SPEED_SLOW] = {.left = 20, .right = 25},
        [DRIVE_SPEED_MEDIUM] = {.left = 40, .right = 50},
        [DRIVE_SPEED_FAST] = {.left = 60, .right = 70},
        [DRIVE_SPEED_MAX] = {.left = 85, .right = 100},
    },
    // DRIVE_DIR_ARCTURN_WIDE_RIGHT
    [DRIVE_DIR_ARCTURN_WIDE_RIGHT] = {
        [DRIVE_SPEED_SLOW] = {.left = 25, .right = 20},
        [DRIVE_SPEED_MEDIUM] = {.left = 50, .right = 40},
        [DRIVE_SPEED_FAST] = {.left = 70, .right = 60},
        [DRIVE_SPEED_MAX] = {.left = 100, .right = 85},
    },
};

void drive_stop()
{
    left_duty_cycle_percent = 0;
    right_duty_cycle_percent = 0;
    motor_driver_set_mode(LEFT_MOTOR, STOP);
    motor_driver_set_mode(RIGHT_MOTOR, STOP);
}

void drive_set(Drive_Dir direction, Drive_Speed speed)
{
    int8_t left_motor_speed = drive_settings[direction][speed].left;
    int8_t right_motor_speed = drive_settings[direction][speed].right;
    Motor_Driver_Mode left_mode = left_motor_speed > 0 ? FORWARD : REVERSE;
    Motor_Driver_Mode right_mode = right_motor_speed > 0 ? FORWARD : REVERSE;
    if (left_motor_speed < 0)
    {
        left_motor_speed *= -1;
    }
    if (right_motor_speed < 0)
    {
        right_motor_speed *= -1;
    }
    motor_driver_set_pwm(LEFT_MOTOR, left_motor_speed);
    motor_driver_set_pwm(RIGHT_MOTOR, right_motor_speed);
    motor_driver_set_mode(LEFT_MOTOR, left_mode);
    motor_driver_set_mode(RIGHT_MOTOR, right_mode);
}

void test(Drive_Dir direction, Drive_Speed speed)
{
    int left_motor_speed = drive_settings[direction][speed].left;
    int right_motor_speed = drive_settings[direction][speed].right;
    Motor_Driver_Mode left_mode = left_motor_speed > 0 ? FORWARD : REVERSE;
    Motor_Driver_Mode right_mode = right_motor_speed > 0 ? FORWARD : REVERSE;
    if (left_motor_speed < 0) left_motor_speed *= -1;
    if (right_motor_speed < 0) right_motor_speed *= -1;
    motor_driver_set_mode(LEFT_MOTOR, left_mode);
    motor_driver_set_pwm(LEFT_MOTOR, left_motor_speed);
    motor_driver_set_mode(RIGHT_MOTOR, right_mode);
    motor_driver_set_pwm(LEFT_MOTOR, left_motor_speed);
    Serial.println(left_motor_speed);
    Serial.println(right_motor_speed);
    Serial.println();
}
