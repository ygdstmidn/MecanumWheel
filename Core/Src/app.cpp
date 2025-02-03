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
#define MOTOR_MAX_SPEED 100
#define MAX_ROTATION_SPEED 50
#define ROTATION_KP 5
#define ROTATION_KI 10
#define ROTATION_KD 0
#define MOTOR1_ADDRESS 0x01
#define MOTOR2_ADDRESS 0x02
#define MOTOR3_ADDRESS 0x03
#define MOTOR4_ADDRESS 0x04

constexpr char BNO_DEFAULT_CALIBRATION[22] = {243,255,251,255,225,255,75,255,173,255,255,255,255,255,255,255,255,255,255,255,255,255};
//////////////////////////////////////////////243,255,251,255,225,255,72,255,205,253,208,254,000,000,002,000,255,255,255,255,255,255

    TripleBufferSystemClass PcUartRxTbs;
    BNO055 bno;
    VelPid rotationPid({{ROTATION_KP, ROTATION_KI, ROTATION_KD}, -MAX_ROTATION_SPEED, MAX_ROTATION_SPEED});
    float robotYaw = 0.0;
    float targetYaw = 0.0;
    float defaultYaw;
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
        BNO055_init(&bno, &hi2c1);
        while (1) // BNO055_setup
        {
            char bno_check = BNO055_check(&bno);
            printf("bno_check=%d\n", bno_check);
            if (bno_check == 0)
            {
                HAL_NVIC_SystemReset();
            }

            BNO055_reset(&bno);

            printf("bno_ID=%02X\n", bno.ID.id);
            // BNO055_set_orientation(&bno, WINDOWS);
            BNO055_set_angle_units(&bno, DEGREES);
            BNO055_setpowermode(&bno, POWER_MODE_NORMAL);
            // BNO055_setmode(&bno, OPERATION_MODE_AMG);
            BNO055_setmode(&bno, OPERATION_MODE_NDOF);
            // BNO055_SetExternalCrystal(&bno, true);

            char bno_state = BNO055_bno_state(&bno);
            printf("bno_state=%02X\n", bno_state);
            if (bno_state == 0x01)
            {
                printf("bno_err_state=%02X\n", BNO055_bno_err_state(&bno));
            }
            else
            {
                break;
            }
        }
        for(int i=0;i<22;i++)
        {
            bno.calibration[i] = BNO_DEFAULT_CALIBRATION[i];
        }
        BNO055_write_calibration_data(&bno);
        HAL_GPIO_TogglePin(DebugLED_GPIO_Port, DebugLED_Pin);
        HAL_Delay(1000); // 最初の方はBNOがおかしいらしい(ほんとかよ...)
        while (HAL_GPIO_ReadPin(DebugButton_GPIO_Port, DebugButton_Pin) == GPIO_PIN_SET)
        {
            BNO055_get_calib(&bno);
            printf(">SYS Calib status:%d\n", (bno.calib >> 6) & 0x03);
            // fflush(NULL);
            // for(int i = 0; i< ((bno.calib >>6)&0x03); i++)//SYS Calib status
            // {
            //     HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_SET);
            //     HAL_Delay(100);
            //     HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_RESET);
            //     HAL_Delay(100);
            // }
            // HAL_Delay(1000);
            printf(">GYR Calib status:%d\n", (bno.calib >> 4) & 0x03);
            // fflush(NULL);
            // for(int i = 0; i< ((bno.calib >>4)&0x03); i++)//GYR Calib status
            // {
            //     HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_SET);
            //     HAL_Delay(100);
            //     HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_RESET);
            //     HAL_Delay(100);
            // }
            // HAL_Delay(1000);
            printf(">ACC Calib status:%d\n", (bno.calib >> 2) & 0x03);
            // fflush(NULL);
            // for(int i = 0; i< ((bno.calib >>2)&0x03); i++)//ACC Calib status
            // {
            //     HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_SET);
            //     HAL_Delay(100);
            //     HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_RESET);
            //     HAL_Delay(100);
            // }
            // HAL_Delay(1000);
            printf(">MAG Calib status:%d\n", bno.calib & 0x03);
            fflush(NULL);
            // for(int i = 0; i< (bno.calib & 0x03); i++)//MAG Calib status
            // {
            //     HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_SET);
            //     HAL_Delay(100);
            //     HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_RESET);
            //     HAL_Delay(100);
            // }
            if ((bno.calib & 0xF3) == 0xF3)
            {
                break;
            }
        }
        for (int i = 0; i < 50; i++)
        {
            HAL_GPIO_TogglePin(DebugLED_GPIO_Port, DebugLED_Pin);
            HAL_Delay(100);
        }
        while (HAL_GPIO_ReadPin(DebugButton_GPIO_Port, DebugButton_Pin) == GPIO_PIN_SET)
        {
            HAL_GPIO_TogglePin(DebugLED_GPIO_Port, DebugLED_Pin);
            HAL_Delay(100);
        }
        BNO055_read_calibration_data(&bno);
        printf("calibration:");
        for(int i = 0; i< 22; i++)
        {
            // printf("calibration[%d]=%d\n", i, bno.calibration[i]);
            printf("%d,", bno.calibration[i]);
        }
        printf("\n");
        for(int i = 0; i< 3; i++)
        {
            HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_SET);
            HAL_Delay(500);
            HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_RESET);
            HAL_Delay(500);
        }
        BNO055_get_angles(&bno);
        defaultYaw = bno.euler.yaw;

        HAL_CAN_Start(&hcan1);
        outputDirection = 0.0;
        outputSpeed = MOTOR_MAX_SPEED;
    }

    // MARK:loop
    void user_loop(void)
    {
        const uint32_t now = HAL_GetTick();
        static uint32_t pre = now;
        static uint32_t pre2 = now;

        static int whichPhase = 0;

        if (now - pre >= 10)
        {
            HAL_GPIO_TogglePin(DebugLED_GPIO_Port, DebugLED_Pin);
            // printf(">now:%lu\n", now);

            // float gyro = BNO055_get_z_gyro(&bno);
            // robotYaw -= gyro * (now - pre) / 1000.0f;
            BNO055_get_angles(&bno);
            robotYaw = bno.euler.yaw - defaultYaw;
            // printf(">robotYaw:%f\n", robotYaw);

            outputRotation = rotationPid.calc(targetYaw, robotYaw, (now - pre) / 1000.0f);
            // printf(">outputRotation:%f\n", outputRotation);
            // printf(">targetYaw:%f\n", targetYaw);
            // printf(">robotYaw:%f\n", robotYaw);

            mecanumCalc();
            // int mecanumError = mecanumCalc();
            // printf("mecanumError=%d%d%d%d\n", mecanumError & 0x08, mecanumError & 0x04, mecanumError & 0x02, mecanumError & 0x01);

            pre = now;
        }

        if (now - pre2 >= 3000)
        {
            if (whichPhase == 0)
            {
                outputDirection = 180;
            }
            else if (whichPhase == 1)
            {
                outputDirection = 90;
            }
            else if (whichPhase == 2)
            {
                outputDirection = -90;
            }
            else
            {
                outputDirection = 0;
                whichPhase = -1;
            }
            whichPhase++;
            // targetYaw = ((int)targetYaw + 90) % 180;
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
        // int motor1Speed = cos(radian(outputDirection - 45)) * outputSpeed - outputRotation;
        // int motor2Speed = cos(radian(outputDirection + 45)) * outputSpeed + outputRotation;
        int motor1Speed = cos(radian(outputDirection + 45)) * outputSpeed - outputRotation;
        int motor2Speed = cos(radian(outputDirection - 45)) * outputSpeed + outputRotation;
        int motor3Speed = motor1Speed + 2 * outputRotation;
        int motor4Speed = motor2Speed - 2 * outputRotation;

        int errorCheck = 0;
        errorCheck += DitelMotor(&hcan1, MOTOR1_ADDRESS, motor1Speed);
        errorCheck = errorCheck << 1;
        errorCheck += DitelMotor(&hcan1, MOTOR2_ADDRESS, motor2Speed);
        errorCheck = errorCheck << 1;
        errorCheck += DitelMotor(&hcan1, MOTOR3_ADDRESS, motor3Speed);
        errorCheck = errorCheck << 1;
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
