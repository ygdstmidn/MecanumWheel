#ifndef DITEL_MOTOR_DRIVER_CONTROLLER_H
#define DITEL_MOTOR_DRIVER_CONTROLLER_H

#include "main.h"

#define DITEL_MOTOR_FORWARD   0x11//本当は逆だが，とりあえず(逆でもいいやろ(昔のプロトコルと反転してるのがおかしいねん！！))
#define DITEL_MOTOR_REVERSAL  0x10
#define DITEL_MOTOR_NEUTRAL   0x12
#define DITEL_MOTOR_BRAKE     0x13
#define DITEL_NONE 0

int DitelMotorDriverRotate(CAN_HandleTypeDef *hcan, uint8_t _motorDriverAddress,
    uint8_t _mode, uint16_t speed);
int DitelMotor(CAN_HandleTypeDef *hcan, int motor_address, int speed);

#endif //DITEL_MOTOR_DRIVER_CONTROLLER_H
