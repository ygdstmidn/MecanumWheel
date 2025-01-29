// main code here!!

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "can.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

#define radian(x) ((x) * M_PI / 180.0)
#define degree(x) ((x) * 180.0 / M_PI)
    int mecanumCalc();
    void PcUartRxTbsAfterSwap();

#define PC_UART_RX_BUFFER_SIZE 256
#define MOTOR_MAX_SPEED 40
#define MOTOR1_ADDRESS 0x01
#define MOTOR2_ADDRESS 0x02
#define MOTOR3_ADDRESS 0x03
#define MOTOR4_ADDRESS 0x04

    TripleBufferSystemClass PcUartRxTbs;
    BNO055 bno;
    float robotYaw = 0.0;
    float targetYaw = 0.0;
    float outputRotation;
    float outputDirection;
    float outputSpeed;

    // MARK:setup
    void user_setup(void)
    {
        setbuf(stdout, NULL);
        HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_SET);
        printf("Hello World!!\n");
        PcUartRxTbs.init(PC_UART_RX_BUFFER_SIZE);
        PcUartRxTbs.setFunc(__disable_irq, PcUartRxTbsAfterSwap);
        HAL_UART_Receive_IT(&huart2, PcUartRxTbs.nextWriteBuffer(), 1); // 1byte
        // BNO055_init(&bno, &hi2c1);
        // while (1) // BNO055_setup
        // {
        //     char bno_check = BNO055_check(&bno);
        //     printf("bno_check=%d\n", bno_check);
        //     if (bno_check == 0)
        //     {
        //         HAL_NVIC_SystemReset();
        //     }

        //     BNO055_reset(&bno);

        //     printf("bno_ID=%02X\n", bno.ID.id);
        //     BNO055_set_orientation(&bno, WINDOWS);
        //     BNO055_set_angle_units(&bno, DEGREES);
        //     BNO055_setpowermode(&bno, POWER_MODE_NORMAL);
        //     BNO055_setmode(&bno, OPERATION_MODE_AMG);
        //     //    BNO055_setmode(&bno, OPERATION_MODE_COMPASS);
        //     BNO055_SetExternalCrystal(&bno, true);

        //     char bno_state = BNO055_bno_state(&bno);
        //     printf("bno_state=%02X\n", bno_state);
        //     if (bno_state == 0x01)
        //     {
        //         printf("bno_err_state=%02X\n", BNO055_bno_err_state(&bno));
        //     }
        //     else
        //     {
        //         break;
        //     }
        // }
        // HAL_Delay(1000); // 最初の方はBNOがおかしいらしい(ほんとかよ...)

        HAL_CAN_Start(&hcan1);
        HAL_GPIO_TogglePin(DebugLED_GPIO_Port, DebugLED_Pin);
        outputDirection = 0.0;
        outputSpeed = MOTOR_MAX_SPEED / 2;
        outputRotation = 0.0;
    }

    // MARK:loop
    void user_loop(void)
    {
        const uint32_t now = HAL_GetTick();
        static uint32_t pre = now;
        static uint32_t pre2 = now;

        if (now - pre >= 10)
        {
            HAL_GPIO_TogglePin(DebugLED_GPIO_Port, DebugLED_Pin);
            // printf("now,%lu\n", now);

            // float gyro = BNO055_get_z_gyro(&bno);
            // robotYaw -= gyro * (now - pre) / 1000.0f;

            mecanumCalc();

            pre = now;
        }

        if (now - pre2 >= 5000)
        {
            outputDirection = ((int)outputDirection + 90) % 360;
            pre2 = now;
        }
    }

    // MARK:_write (for printf)
    int _write(int file, char *ptr, int len)
    {
        HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 10);
        return len;
    }

    // MARK:HAL_UART_RxCpltCallback
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    {
        if (huart == &huart2)
        {
            PcUartRxTbs.headMove();
            HAL_UART_Receive_IT(&huart2, PcUartRxTbs.nextWriteBuffer(), 1); // 1byte
        }
    }

    // MARK: mecanumCalc
    int mecanumCalc()
    {
        // メカナムホイールのつき方によって変わる
        int motor1Speed = cos(radian(315 + outputDirection)) * outputSpeed - outputRotation;
        int motor2Speed = cos(radian(45 + outputDirection)) * outputSpeed + outputRotation;
        int motor3Speed = cos(radian(135 + outputDirection)) * outputSpeed - outputRotation;
        int motor4Speed = cos(radian(225 + outputDirection)) * outputSpeed + outputRotation;
        // int motor1Speed = cos(radian(45 + outputDirection)) * outputSpeed - outputRotation;
        // int motor2Speed = cos(radian(135 + outputDirection)) * outputSpeed + outputRotation;
        // int motor3Speed = cos(radian(225 + outputDirection)) * outputSpeed - outputRotation;
        // int motor4Speed = cos(radian(315 + outputDirection)) * outputSpeed + outputRotation;

        int errorCheck = 0;
        errorCheck += DitelMotor(&hcan1, MOTOR1_ADDRESS, motor1Speed);
        errorCheck += DitelMotor(&hcan1, MOTOR2_ADDRESS, motor2Speed);
        errorCheck += DitelMotor(&hcan1, MOTOR3_ADDRESS, motor3Speed);
        HAL_Delay(1);
        errorCheck += DitelMotor(&hcan1, MOTOR4_ADDRESS, motor4Speed);
        return errorCheck;
    }

    // MARK: PcUartRxTbsAfterSwap
    void PcUartRxTbsAfterSwap()
    {
        HAL_UART_AbortReceive_IT(&huart2);
        HAL_UART_Receive_IT(&huart2, PcUartRxTbs.nextWriteBuffer(), 1); // 1byte
        __enable_irq();
    }

#ifdef __cplusplus
}
#endif
