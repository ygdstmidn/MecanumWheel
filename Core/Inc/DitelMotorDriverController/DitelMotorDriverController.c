#include "DitelMotorDriverController/DitelMotorDriverController.h"

int DitelMotorDriverRotate(CAN_HandleTypeDef *hcan, uint8_t _motorDriverAddress,
    uint8_t _mode, uint16_t speed)
{
  static uint8_t lastMode[16];//0000～1111
  if(lastMode[_motorDriverAddress] != _mode && lastMode[_motorDriverAddress] != DITEL_MOTOR_NEUTRAL)
  {
    _mode = DITEL_MOTOR_NEUTRAL;
    speed = 0;
  }
  lastMode[_motorDriverAddress] = _mode;

  uint8_t canSendData[8] = {};

  if (speed > 50)
    speed = 50;

  canSendData[0] = _mode;
  canSendData[1] = speed >> 8;
  canSendData[2] = speed & 0x00FF;
  canSendData[3] = 0;
  canSendData[4] = 0;
  canSendData[5] = 0;
  canSendData[6] = 0;
  canSendData[7] = 0;

  if (0 < HAL_CAN_GetTxMailboxesFreeLevel(hcan))
  {
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    TxHeader.StdId = _motorDriverAddress;                 // CAN ID
    TxHeader.RTR = CAN_RTR_DATA;            // フレームタイプはデータフレーム
    TxHeader.IDE = CAN_ID_STD;              // 標準ID(11ﾋﾞｯﾄ)
    TxHeader.DLC = 8;                       // データ長は8バイトに
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
  speed *= 1;
  if (speed > 50)
    speed = 50;
  if (speed < -50)
    speed = -50;

  if (speed > 0)
    return DitelMotorDriverRotate(hcan,motor_address, DITEL_MOTOR_FORWARD, 1 * speed);
  else if (speed < 0)
    return DitelMotorDriverRotate(hcan,motor_address, DITEL_MOTOR_REVERSAL, -1 * speed);
  else
    // return DitelMotorDriverRotate(hcan,motor_address, DITEL_MOTOR_BRAKE,DITEL_NONE);
    return DitelMotorDriverRotate(hcan,motor_address, DITEL_MOTOR_NEUTRAL, DITEL_NONE);

  return 1;
}
