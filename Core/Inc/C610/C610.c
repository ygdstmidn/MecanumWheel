#include "C610.h"

void C610_set_current(C610 *target, float current) ///-1.0~1.0
{
  if (current > 1.0)
    current = 1.0;
  if (current < -1.0)
    current = -1.0;
  target->raw_current_ = current * C610_MAX;
}
void C610_set_raw_current(C610 *target, int16_t raw_current) ///-10000~10000
{
  if (raw_current > C610_MAX)
    raw_current = C610_MAX;
  if (raw_current < -1 * C610_MAX)
    raw_current = -1 * C610_MAX;
  target->raw_current_ = raw_current;
}
void C610_parse(C610 *target)
{
  uint8_t *data = target->rx_.raw_data;
  target->rx_.angle = (uint16_t) (data[0] << 8 | data[1]);
  target->rx_.rpm = (int16_t) (data[2] << 8 | data[3]);
  target->rx_.torque = (int16_t) (data[4] << 8 | data[5]);
}

void C610Array_parse_packetAll(C610Array *target)
{
  for (int i = 0; i < 8; i++)
  {
    C610_parse(&(target->arr_[i]));
  }
}
void C610Array_read_packet(C610Array *target,
    const CAN_RxHeaderTypeDef *RxHeader, const uint8_t RxData[8])
{
  if (RxHeader->IDE == CAN_ID_STD && RxHeader->RTR == CAN_RTR_DATA
      && RxHeader->DLC == 8)
  {
    uint32_t id = RxHeader->StdId;     // ID
    if (0x201 <= id && id <= 0x208)
    {
      for (int i = 0; i < 8; i++)
      {
        target->arr_[id - 0x201u].rx_.raw_data[i] = RxData[i];
      }
    }
  }
}
void C610Array_to_msgs(C610Array *target, uint8_t *buf)
{
  for (int i = 0; i < 8; ++i)
  {
    buf[2 * i] = __C610raw_current_(*target,i+1) >> 8;
    buf[2 * i + 1] = __C610raw_current_(*target,i+1) & 0xff;
  }
}
int C610Array_Send(C610Array *target, CAN_HandleTypeDef *hcan)
{
  uint8_t output[16];
  C610Array_to_msgs(target, output);

  if (0 < HAL_CAN_GetTxMailboxesFreeLevel(hcan))
  {
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    TxHeader.StdId = 0x200;                 // CAN ID
    TxHeader.RTR = CAN_RTR_DATA;            // フレームタイプはデータフレーム
    TxHeader.IDE = CAN_ID_STD;              // 標準ID(11ﾋﾞｯﾄ)
    TxHeader.DLC = 8;                       // データ長は8バイトに
    TxHeader.TransmitGlobalTime = DISABLE;  // ???
    HAL_CAN_AddTxMessage(hcan, &TxHeader, output, &TxMailbox);
  } else
  {
    return 1;
  }

  if (0 < HAL_CAN_GetTxMailboxesFreeLevel(hcan))
  {
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    TxHeader.StdId = 0x1FF;                 // CAN ID
    TxHeader.RTR = CAN_RTR_DATA;            // フレームタイプはデータフレーム
    TxHeader.IDE = CAN_ID_STD;              // 標準ID(11ﾋﾞｯﾄ)
    TxHeader.DLC = 8;                       // データ長は8バイトに
    TxHeader.TransmitGlobalTime = DISABLE;  // ???
    HAL_CAN_AddTxMessage(hcan, &TxHeader, output + 8, &TxMailbox);
  } else
  {
    return 2;
  }

  return 0;
}
