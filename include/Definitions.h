#ifndef Definitions_h
#define Definitions_h

#include "Arduino.h"

#define COM Serial
#define BAUDRATE 115200

// EEPROM directions.
#define LASER1_TIME_DIRECTION 8
#define LASER1_POWER_DIRECTION 10
#define LASER1_DELAY_DIRECTION 12
#define LASER2_TIME_DIRECTION 16
#define LASER2_POWER_DIRECTION 18
#define LASER2_DELAY_DIRECTION 20

// Pin definitions.
#define PWM1_SGNL 0
#define POWER1_SGNL 4
#define PWM2_SGNL 1
#define POWER2_SGNL 5

#define DOOR_INPUT 8

// Typedef
typedef enum{
    NO_PRESSED,
    LASER1_TIME,
    LASER1_POWER,
    LASER1_DELAY,
    LASER2_TIME,
    LASER2_POWER,
    LASER2_DELAY
} buttonModes;

#endif