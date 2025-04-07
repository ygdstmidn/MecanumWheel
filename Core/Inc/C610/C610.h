#ifndef RCT_C610_H
#define RCT_C610_H
/// @file
/// @brief Provides the C610 class for controlling the motor driver for M3508.
/// @copyright Copyright (c) 2024 Yoshikawa Teru
/// @license This project is released under the MIT License.
/// @note This file is renewed by me

#include "main.h"

#define C610_MAX 10000

/// @brief The packet structure of the C610 motor driver.
typedef struct C610Packet
{
  uint16_t angle;
  int16_t rpm;
  int16_t torque;
  uint8_t raw_data[8];
} C610Packet;

/// @brief The C610 motor driver class for M3508.
typedef struct C610
{
  int16_t raw_current_;
  C610Packet rx_;
} C610;

/// @brief The C610 motor driver array for M3508.
typedef struct C610Array
{
  C610 arr_[8];
} C610Array;

void C610_set_current(C610 *target, float current);
void C610_set_raw_current(C610 *target, int16_t raw_current);
void C610_parse(C610 *target);

void C610Array_parse_packetAll(C610Array *target);
void C610Array_read_packet(C610Array *target,
    const CAN_RxHeaderTypeDef *RxHeader, const uint8_t RxData[8]);
void C610Array_to_msgs(C610Array *target, uint8_t *buf);
int C610Array_Send(C610Array *target, CAN_HandleTypeDef *hcan);

#define __C610angle(c610arr,motor) ((c610arr).arr_[(motor)-1].rx_.angle)
#define __C610rpm(c610arr,motor) ((c610arr).arr_[(motor)-1].rx_.rpm)
#define __C610torque(c610arr,motor) ((c610arr).arr_[(motor)-1].rx_.torque)
#define __C610raw_current_(c610arr,motor) ((c610arr).arr_[(motor)-1].raw_current_)
#define __C610_parse(c610arr,motor) C610_parse(&((c610arr).arr_[(motor)-1]))
#define __C610_set_current(c610arr,motor,output) C610_set_current(&((c610arr).arr_[(motor)-1]),(output))
#define __C610_set_raw_current(c610arr,motor,output) C610_set_raw_current(&((c610arr).arr_[(motor)-1]),(output))

#endif /// RCT_C610_H
