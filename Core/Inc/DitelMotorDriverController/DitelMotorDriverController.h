#ifndef DITEL_MOTOR_DRIVER_CONTROLLER_H
#define DITEL_MOTOR_DRIVER_CONTROLLER_H

#include "main.h"

#define DITEL_MOTOR_FORWARD   0x23
#define DITEL_MOTOR_REVERSAL  0x24
#define DITEL_MOTOR_NEUTRAL   0x01
#define DITEL_MOTOR_BRAKE     0x02
#define DITEL_NONE 0

int DitelMotorDriverRotate(CAN_HandleTypeDef *hcan, uint8_t _motorDriverAddress,
    uint8_t _mode, uint16_t speed);
int DitelMotor(CAN_HandleTypeDef *hcan, int motor_address, int speed);

#endif //DITEL_MOTOR_DRIVER_CONTROLLER_H
