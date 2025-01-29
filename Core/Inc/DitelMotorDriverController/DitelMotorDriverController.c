#include "DitelMotorDriverController/DitelMotorDriverController.h"

int DitelMotorDriverRotate(CAN_HandleTypeDef *hcan, uint8_t _motorDriverAddress,
    uint8_t _mode, uint16_t speed)
{
  uint8_t canSendData[3];

  canSendData[0] = _mode;
  canSendData[1] = speed >> 8;
  canSendData[2] = speed & 0x00FF;

  if (0 < HAL_CAN_GetTxMailboxesFreeLevel(hcan))
  {
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    TxHeader.StdId = _motorDriverAddress;                 // CAN ID
    TxHeader.RTR = CAN_RTR_DATA;            // フレームタイプはデータフレーム
    TxHeader.IDE = CAN_ID_STD;              // 標準ID(11ﾋﾞｯﾄ)
    TxHeader.DLC = 3;                       // データ長は8バイトに
    TxHeader.TransmitGlobalTime = DISABLE;  // ???
    HAL_CAN_AddTxMessage(hcan, &TxHeader, canSendData, &TxMailbox);

    return 0;
  } else
  {
    return 1;
  }
}

int DitelMotor(CAN_HandleTypeDef *hcan, int motor_address, int speed)
{
  if (speed > 199)
    speed = 199;
  if (speed < -199)
    speed = -199;

  if (speed > 2)
    return DitelMotorDriverRotate(hcan,motor_address, DITEL_MOTOR_FORWARD, speed);
  else if (speed < -2)
    return DitelMotorDriverRotate(hcan,motor_address, DITEL_MOTOR_REVERSAL, -1 * speed);
  else
    return DitelMotorDriverRotate(hcan,motor_address, DITEL_MOTOR_BRAKE,DITEL_NONE);

  return 1;
}
