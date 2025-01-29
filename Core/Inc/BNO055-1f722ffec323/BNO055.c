#include <BNO055-1f722ffec323/BNO055.h>

void BNO055_init(BNO055 *target, I2C_HandleTypeDef *i2c)
{
  // Set I2C fast and bring reset line high
  target->i2c = i2c;
  target->address = BNOAddress;
  target->accel_scale = 0.001f;
  target->rate_scale = 1.0f / 16.0f;
  target->angle_scale = 1.0f / 16.0f;
  target->temp_scale = 1;
  target->pre_yaw_NAN = 1; //NANでーす
}

char BNO055_bno_state(BNO055 *target)
{
  BNO055_readchar(target, BNO055_SYS_STAT_ADDR);
  return target->rx;
}

char BNO055_bno_err_state(BNO055 *target)
{
  BNO055_readchar(target, BNO055_SYS_ERR_ADDR);
  return target->rx;
}

float BNO055_get_yaw(BNO055 *target)
{
  BNO055_readstring(target, BNO055_EULER_H_LSB_ADDR, 2);
  target->euler.rawyaw = (target->rawdata[1] << 8 | target->rawdata[0]);
  target->euler.yaw = (float) (target->euler.rawyaw) * target->angle_scale;
  if (target->pre_yaw_NAN == 1)
  {
    target->pre_yaw_NAN = 0;
  } else
  {
    if (target->euler.yaw - target->pre_yaw > 180.0f) //0->360
      target->euler.yaw -= 360.0f;
    if (target->euler.yaw - target->pre_yaw < -180.0f) //360->0
      target->euler.yaw += 360.0f;
  }
  target->pre_yaw = target->euler.yaw;
  return target->euler.yaw;
}

float BNO055_get_z_gyro(BNO055 *target)
{
  BNO055_readstring(target, BNO055_GYRO_DATA_Z_LSB_ADDR, 2);
  target->gyro.rawz = (target->rawdata[1] << 8 | target->rawdata[0]);
  target->gyro.z = (float) (target->gyro.rawz) * target->rate_scale;
  return target->gyro.z;
}

void BNO055_reset(BNO055 *target)
{
  // Perform a power-on-reset
  BNO055_readchar(target, BNO055_SYS_TRIGGER_ADDR);
  target->rx = target->rx | 0x20;
  BNO055_writechar(target, BNO055_SYS_TRIGGER_ADDR, target->rx);
  //Wait for the system to come back up again (datasheet says 650ms)
  HAL_Delay(675);
}

bool BNO055_check(BNO055 *target)
{
  // Check we have communication link with the chip
  BNO055_readchar(target, BNO055_CHIP_ID_ADDR);
  if (target->rx != 0xA0)
    return false;
  // Grab the chip ID and software versions
  BNO055_readstring(target, BNO055_CHIP_ID_ADDR, 7);
  target->ID.id = target->rawdata[0];
  target->ID.accel = target->rawdata[1];
  target->ID.mag = target->rawdata[2];
  target->ID.gyro = target->rawdata[3];
  target->ID.sw[0] = target->rawdata[4];
  target->ID.sw[1] = target->rawdata[5];
  target->ID.bootload = target->rawdata[6];
  BNO055_setpage(target, 1);
  BNO055_readstring(target, BNO055_UNIQUE_ID_ADDR, 16);
  for (int i = 0; i < 16; i++)
  {
    target->ID.serial[i] = target->rawdata[i];
  }
  BNO055_setpage(target, 0);
  return true;
}

void BNO055_SetExternalCrystal(BNO055 *target, bool yn)
{
  // Read the current status from the device
  BNO055_readchar(target, BNO055_SYS_TRIGGER_ADDR);
  if (yn)
    target->rx = target->rx | 0x80;
  else
    target->rx = target->rx & 0x7F;
  BNO055_writechar(target, BNO055_SYS_TRIGGER_ADDR, target->rx);
}

void BNO055_setmode(BNO055 *target, char omode)
{
  BNO055_writechar(target, BNO055_OPR_MODE_ADDR, omode);
  target->op_mode = omode;
}

void BNO055_setpowermode(BNO055 *target, char pmode)
{
  BNO055_writechar(target, BNO055_PWR_MODE_ADDR, pmode);
  target->pwr_mode = pmode;
}

void BNO055_set_accel_units(BNO055 *target, char units)
{
  BNO055_readchar(target, BNO055_UNIT_SEL_ADDR);
  if (units == MPERSPERS)
  {
    target->rx = target->rx & 0xFE;
    target->accel_scale = 0.01f;
  }
  else
  {
    target->rx = target->rx | units;
    target->accel_scale = 0.001f;
  }
  BNO055_writechar(target, BNO055_UNIT_SEL_ADDR, target->rx);
}

void BNO055_set_anglerate_units(BNO055 *target, char units)
{
  BNO055_readchar(target, BNO055_UNIT_SEL_ADDR);
  if (units == DEG_PER_SEC)
  {
    target->rx = target->rx & 0xFD;
    target->rate_scale = 1.0f / 16.0f;
  }
  else
  {
    target->rx = target->rx | units;
    target->rate_scale = 1.0f / 900.0f;
  }
  BNO055_writechar(target, BNO055_UNIT_SEL_ADDR, target->rx);
}

void BNO055_set_angle_units(BNO055 *target, char units)
{
  BNO055_readchar(target, BNO055_UNIT_SEL_ADDR);
  if (units == DEGREES)
  {
    target->rx = target->rx & 0xFB;
    target->angle_scale = 1.0f / 16.0f;
  }
  else
  {
    target->rx = target->rx | units;
    target->rate_scale = 1.0f / 900.0f;
  }
  BNO055_writechar(target, BNO055_UNIT_SEL_ADDR, target->rx);
}

void BNO055_set_temp_units(BNO055 *target, char units)
{
  BNO055_readchar(target, BNO055_UNIT_SEL_ADDR);
  if (units == CENTIGRADE)
  {
    target->rx = target->rx & 0xEF;
    target->temp_scale = 1;
  }
  else
  {
    target->rx = target->rx | units;
    target->temp_scale = 2;
  }
  BNO055_writechar(target, BNO055_UNIT_SEL_ADDR, target->rx);
}

void BNO055_set_orientation(BNO055 *target, char units)
{
  BNO055_readchar(target, BNO055_UNIT_SEL_ADDR);
  if (units == WINDOWS)
    target->rx = target->rx & 0x7F;
  else
    target->rx = target->rx | units;
  BNO055_writechar(target, BNO055_UNIT_SEL_ADDR, target->rx);
}

void BNO055_set_mapping(BNO055 *target, char orient)
{
  switch (orient)
  {
    case 0:
      BNO055_writechar(target, BNO055_AXIS_MAP_CONFIG_ADDR, 0x21);
      BNO055_writechar(target, BNO055_AXIS_MAP_SIGN_ADDR, 0x04);
      break;
    case 1:
      BNO055_writechar(target, BNO055_AXIS_MAP_CONFIG_ADDR, 0x24);
      BNO055_writechar(target, BNO055_AXIS_MAP_SIGN_ADDR, 0x00);
      break;
    case 2:
      BNO055_writechar(target, BNO055_AXIS_MAP_CONFIG_ADDR, 0x24);
      BNO055_writechar(target, BNO055_AXIS_MAP_SIGN_ADDR, 0x00);
      break;
    case 3:
      BNO055_writechar(target, BNO055_AXIS_MAP_CONFIG_ADDR, 0x21);
      BNO055_writechar(target, BNO055_AXIS_MAP_SIGN_ADDR, 0x02);
      break;
    case 4:
      BNO055_writechar(target, BNO055_AXIS_MAP_CONFIG_ADDR, 0x24);
      BNO055_writechar(target, BNO055_AXIS_MAP_SIGN_ADDR, 0x03);
      break;
    case 5:
      BNO055_writechar(target, BNO055_AXIS_MAP_CONFIG_ADDR, 0x21);
      BNO055_writechar(target, BNO055_AXIS_MAP_SIGN_ADDR, 0x01);
      break;
    case 6:
      BNO055_writechar(target, BNO055_AXIS_MAP_CONFIG_ADDR, 0x21);
      BNO055_writechar(target, BNO055_AXIS_MAP_SIGN_ADDR, 0x07);
      break;
    case 7:
      BNO055_writechar(target, BNO055_AXIS_MAP_CONFIG_ADDR, 0x24);
      BNO055_writechar(target, BNO055_AXIS_MAP_SIGN_ADDR, 0x05);
      break;
    default:
      BNO055_writechar(target, BNO055_AXIS_MAP_CONFIG_ADDR, 0x24);
      BNO055_writechar(target, BNO055_AXIS_MAP_SIGN_ADDR, 0x00);
  }
}

void BNO055_get_accel(BNO055 *target)
{
  BNO055_readstring(target, BNO055_ACCEL_DATA_X_LSB_ADDR, 6);
  target->accel.rawx = (target->rawdata[1] << 8 | target->rawdata[0]);
  target->accel.rawy = (target->rawdata[3] << 8 | target->rawdata[2]);
  target->accel.rawz = (target->rawdata[5] << 8 | target->rawdata[4]);
  target->accel.x = (float) (target->accel.rawx) * target->accel_scale;
  target->accel.y = (float) (target->accel.rawy) * target->accel_scale;
  target->accel.z = (float) (target->accel.rawz) * target->accel_scale;
}

void BNO055_get_gyro(BNO055 *target)
{
  BNO055_readstring(target, BNO055_GYRO_DATA_X_LSB_ADDR, 6);
  target->gyro.rawx = (target->rawdata[1] << 8 | target->rawdata[0]);
  target->gyro.rawy = (target->rawdata[3] << 8 | target->rawdata[2]);
  target->gyro.rawz = (target->rawdata[5] << 8 | target->rawdata[4]);
  target->gyro.x = (float) (target->gyro.rawx) * target->rate_scale;
  target->gyro.y = (float) (target->gyro.rawy) * target->rate_scale;
  target->gyro.z = (float) (target->gyro.rawz) * target->rate_scale;
}

void BNO055_get_mag(BNO055 *target)
{
  BNO055_readstring(target, BNO055_MAG_DATA_X_LSB_ADDR, 6);
  target->mag.rawx = (target->rawdata[1] << 8 | target->rawdata[0]);
  target->mag.rawy = (target->rawdata[3] << 8 | target->rawdata[2]);
  target->mag.rawz = (target->rawdata[5] << 8 | target->rawdata[4]);
  target->mag.x = (float) (target->mag.rawx);
  target->mag.y = (float) (target->mag.rawy);
  target->mag.z = (float) (target->mag.rawz);
}

void BNO055_get_lia(BNO055 *target)
{
  BNO055_readstring(target, BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR, 6);
  target->lia.rawx = (target->rawdata[1] << 8 | target->rawdata[0]);
  target->lia.rawy = (target->rawdata[3] << 8 | target->rawdata[2]);
  target->lia.rawz = (target->rawdata[5] << 8 | target->rawdata[4]);
  target->lia.x = (float) (target->lia.rawx) * target->accel_scale;
  target->lia.y = (float) (target->lia.rawy) * target->accel_scale;
  target->lia.z = (float) (target->lia.rawz) * target->accel_scale;
}

void BNO055_get_grv(BNO055 *target)
{
  BNO055_readstring(target, BNO055_GRAVITY_DATA_X_LSB_ADDR, 6);
  target->gravity.rawx = (target->rawdata[1] << 8 | target->rawdata[0]);
  target->gravity.rawy = (target->rawdata[3] << 8 | target->rawdata[2]);
  target->gravity.rawz = (target->rawdata[5] << 8 | target->rawdata[4]);
  target->gravity.x = (float) (target->gravity.rawx) * target->accel_scale;
  target->gravity.y = (float) (target->gravity.rawy) * target->accel_scale;
  target->gravity.z = (float) (target->gravity.rawz) * target->accel_scale;
}

void BNO055_get_quat(BNO055 *target)
{
  BNO055_readstring(target, BNO055_QUATERNION_DATA_W_LSB_ADDR, 8);
  target->quat.raww = (target->rawdata[1] << 8 | target->rawdata[0]);
  target->quat.rawx = (target->rawdata[3] << 8 | target->rawdata[2]);
  target->quat.rawy = (target->rawdata[5] << 8 | target->rawdata[4]);
  target->quat.rawz = (target->rawdata[7] << 8 | target->rawdata[6]);
  target->quat.w = (float) (target->quat.raww) / 16384.0f;
  target->quat.x = (float) (target->quat.rawx) / 16384.0f;
  target->quat.y = (float) (target->quat.rawy) / 16384.0f;
  target->quat.z = (float) (target->quat.rawz) / 16384.0f;
}

void BNO055_get_angles(BNO055 *target)
{
  BNO055_readstring(target, BNO055_EULER_H_LSB_ADDR, 6);
  target->euler.rawyaw = (target->rawdata[1] << 8 | target->rawdata[0]);
  target->euler.rawroll = (target->rawdata[3] << 8 | target->rawdata[2]);
  target->euler.rawpitch = (target->rawdata[5] << 8 | target->rawdata[4]);
  target->euler.yaw = (float) (target->euler.rawyaw) * target->angle_scale;
  target->euler.roll = (float) (target->euler.rawroll) * target->angle_scale;
  target->euler.pitch = (float) (target->euler.rawpitch) * target->angle_scale;
}

void BNO055_get_temp(BNO055 *target)
{
  BNO055_readchar(target, BNO055_TEMP_ADDR);
  target->temperature = target->rx / target->temp_scale;
}

void BNO055_get_calib(BNO055 *target)
{
  BNO055_readchar(target, BNO055_CALIB_STAT_ADDR);
  target->calib = target->rx;
}

void BNO055_read_calibration_data(BNO055 *target)
{
  char tempmode = target->op_mode;
  BNO055_setmode(target, OPERATION_MODE_CONFIG);
  HAL_Delay(20);
  BNO055_readstring(target, ACCEL_OFFSET_X_LSB_ADDR, 22);
  for (int i = 0; i < 22; i++)
  {
    target->calibration[i] = target->rawdata[i];
  }
  BNO055_setmode(target, tempmode);
  HAL_Delay(10);
}

void BNO055_write_calibration_data(BNO055 *target)
{
  char tempmode = target->op_mode;
  BNO055_setmode(target, OPERATION_MODE_CONFIG);
  HAL_Delay(20);
  BNO055_writestring(target, ACCEL_OFFSET_X_LSB_ADDR, target->calibration, 22);
  BNO055_setmode(target, tempmode);
  HAL_Delay(10);
}

void BNO055_readchar(BNO055 *target, uint8_t location)
{
  HAL_I2C_Mem_Read(target->i2c, target->address | 1, location, 1,
      &target->rx, 1,
      HAL_MAX_DELAY);
}

void BNO055_readstring(BNO055 *target, uint8_t location, uint16_t size)
{
  HAL_I2C_Mem_Read(target->i2c, target->address | 1, location, 1,
      target->rawdata, size,
      HAL_MAX_DELAY);
}

void BNO055_writechar(BNO055 *target, uint8_t location, uint8_t value)
{
  HAL_I2C_Mem_Write(target->i2c, target->address | 1, location, 1,
      &value, 1, HAL_MAX_DELAY);
}

void BNO055_writestring(BNO055 *target, uint8_t location, uint8_t *values,
    uint8_t size)
{
  HAL_I2C_Mem_Write(target->i2c, target->address | 1, location, 1,
      values, size, HAL_MAX_DELAY);
}

void BNO055_setpage(BNO055 *target, uint8_t value)
{
  BNO055_writechar(target, BNO055_PAGE_ID_ADDR, value);
}
