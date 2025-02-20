// main code here!!

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "can.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#define radian(x) ((x) * M_PI / 180.0)
#define degree(x) ((x) * 180.0 / M_PI)
    int mecanumCalc();
    void PcUartRxTbsAfterSwap();
    void espUartRxTbsAfterSwap();
    void esp32_read();
    void controller_read();
    void BNOSetup();
    void Brake_StopWheel();

#define PC_UART_RX_BUFFER_SIZE 256
#define ESP_UART_RX_BUFFER_SIZE 1024
#define MOTOR_MAX_SPEED 200
#define MAX_ROTATION_SPEED 100
#define ROTATION_KP 5
#define ROTATION_KI 10
#define ROTATION_KD 0
#define MOTOR1_ADDRESS 0x01
#define MOTOR2_ADDRESS 0x02
#define MOTOR3_ADDRESS 0x03
#define MOTOR4_ADDRESS 0x04
#define huartEsp huart3

#define right_button 14 //?
#define left_button 13  //?
#define down_button 12  //?
#define up_button 11    //?
#define brake_button 1
#define ubuntu_ps4_migisenkai_button 5
#define ubuntu_ps4_hidarisenkai_button 4
#define windows_switch_procon_migisenkai_button 10
#define windows_switch_procon_hidarisenkai_button 9

#define move_tate_stick 1
#define move_yoko_stick 0

#define stick_sikii 0.2

    constexpr char BNO_DEFAULT_CALIBRATION[22] = {243, 255, 251, 255, 225, 255, 75, 255, 173, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    //////////////////////////////////////////////243,255,251,255,225,255,72,255,205,253,208,254,000,000,002,000,255,255,255,255,255,255

    TripleBufferSystemClass PcUartRxTbs;
    TripleBufferSystemClass espUartRxTbs;
    Encoder encoder1(&htim1);
    Encoder encoder2(&htim2);
    Encoder encoder3(&htim3);
    Encoder encoder4(&htim4);
    BNO055 bno;
    VelPid rotationPid({{ROTATION_KP, ROTATION_KI, ROTATION_KD}, -MAX_ROTATION_SPEED, MAX_ROTATION_SPEED});
    float robotYaw = 0.0;
    float targetYaw = 0.0;
    float defaultYaw;
    float outputRotation;
    float outputDirection;
    float outputSpeed;

    int input_button[30] = {};
    int input_last_button[30] = {};
    double input_stick[20] = {};
    enum controllerTypes
    {
        ps4_ubuntu,
        ps4_windows,
        switchProCon_windows
    };
    controllerTypes input_controllerType;

    // MARK:setup
    void user_setup(void)
    {
        setbuf(stdout, NULL);
        HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_SET);
        printf("Hello World!!\n");

        PcUartRxTbs.init(PC_UART_RX_BUFFER_SIZE);
        PcUartRxTbs.setFunc(__disable_irq, PcUartRxTbsAfterSwap);
        HAL_UART_Receive_IT(&huart2, PcUartRxTbs.nextWriteBuffer(), 1); // 1byte

        // BNOSetup();//This may take a while

        HAL_CAN_Start(&hcan1);

        espUartRxTbs.init(ESP_UART_RX_BUFFER_SIZE);
        espUartRxTbs.setFunc(__disable_irq, espUartRxTbsAfterSwap);
        HAL_UART_Receive_IT(&huartEsp, espUartRxTbs.nextWriteBuffer(), 1); // 1byte

        encoder1.start();
        encoder2.start();
        encoder3.start();
        encoder4.start();

        outputDirection = 0.0;
        outputSpeed = 0.0;
    }

    // MARK:loop
    void user_loop(void)
    {
        const uint32_t now = HAL_GetTick();
        static uint32_t pre = now;
        // static uint32_t pre2 = now;

        // static int whichPhase = 0;

        if (now - pre >= 10)
        {
            // HAL_GPIO_TogglePin(DebugLED_GPIO_Port, DebugLED_Pin);
            // printf(">now:%lu\n", now);

            // // float gyro = BNO055_get_z_gyro(&bno);
            // // robotYaw -= gyro * (now - pre) / 1000.0f;
            // BNO055_get_angles(&bno);
            // robotYaw = bno.euler.yaw - defaultYaw;
            // // printf(">robotYaw:%f\n", robotYaw);



            int32_t encoder1Speed = encoder1.getSpeed();
            int32_t encoder2Speed = encoder2.getSpeed();
            int32_t encoder3Speed = encoder3.getSpeed();
            int32_t encoder4Speed = encoder4.getSpeed();
            printf(">encoder1Speed:%ld\n", encoder1Speed);
            printf(">encoder2Speed:%ld\n", encoder2Speed);
            printf(">encoder3Speed:%ld\n", encoder3Speed);
            printf(">encoder4Speed:%ld\n", encoder4Speed);

            outputSpeed = MOTOR_MAX_SPEED;
            outputDirection = 0;
            mecanumCalc();

            // esp32_read();
            // controller_read();

            // outputRotation = rotationPid.calc(targetYaw, robotYaw, (now - pre) / 1000.0f);
            // // printf(">outputRotation:%f\n", outputRotation);
            // // printf(">targetYaw:%f\n", targetYaw);
            // // printf(">robotYaw:%f\n", robotYaw);

            // if (input_button[brake_button] == 1)
            // {
            //     outputSpeed = 0;
            //     outputRotation = 0;
            //     targetYaw = robotYaw;
            //     Brake_StopWheel();
            // }
            // else
            // {
            //     mecanumCalc();
            // }
            // // int mecanumError = mecanumCalc();
            // // printf("mecanumError=%d%d%d%d\n", mecanumError & 0x08, mecanumError & 0x04, mecanumError & 0x02, mecanumError & 0x01);

            pre = now;
        }

        // if (now - pre2 >= 3000)
        // {
        //     if (whichPhase == 0)
        //     {
        //         outputDirection = 180;
        //     }
        //     else if (whichPhase == 1)
        //     {
        //         outputDirection = 90;
        //     }
        //     else if (whichPhase == 2)
        //     {
        //         outputDirection = -90;
        //     }
        //     else
        //     {
        //         outputDirection = 0;
        //         whichPhase = -1;
        //     }
        //     whichPhase++;
        //     // targetYaw = ((int)targetYaw + 90) % 180;
        //     pre2 = now;
        // }
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
        else if (huart == &huartEsp)
        {
            espUartRxTbs.headMove();
            HAL_UART_Receive_IT(&huartEsp, espUartRxTbs.nextWriteBuffer(), 1); // 1byte
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

    // MARK: espUartRxTbsAfterSwap
    void espUartRxTbsAfterSwap()
    {
        HAL_UART_AbortReceive_IT(&huartEsp);
        HAL_UART_Receive_IT(&huartEsp, espUartRxTbs.nextWriteBuffer(), 1); // 1byte
        __enable_irq();
    }

    // MARK: esp32_read
    void esp32_read()
    {
        static uint8_t receiveData[ESP_UART_RX_BUFFER_SIZE];
        const uint8_t *data = &receiveData[4];
        static unsigned int index = 0;

        {
            int espUartRxTbsError;
            uint8_t temp;
            while (espUartRxTbs.read(&temp, 1, &espUartRxTbsError))
            {
                receiveData[index] = temp;
                if (temp == '\n')
                    break;
                index++;
            }
        }

        if (receiveData[index] == '\n')
        {
            HAL_GPIO_TogglePin(DebugLED_GPIO_Port, DebugLED_Pin);
            const int DATA_SIZE = index - 5; // 0xFF...\r\n
            receiveData[index - 1] = '\0';   //\rを\0で上書き
            index = 0;
            printf("%d", DATA_SIZE);
            if (DATA_SIZE < 10 || receiveData[1] != 'x') // パケットじゃない
            {
                return;
            }

            int is_button = 1;
            if (DATA_SIZE == 16 || DATA_SIZE == 20 || DATA_SIZE == 13) // ボタンの数
            {
                for (int i = 0; i < DATA_SIZE; i++)
                {
                    if (data[i] != '0' && data[i] != '1')
                    {
                        is_button = 0;
                        break;
                    }
                }
            }
            else
            {
                is_button = 0;
            }

            if (is_button == 1)
            {
                if (DATA_SIZE == 13)
                {
                    input_controllerType = ps4_ubuntu;
                }
                else if (DATA_SIZE == 16)
                {
                    input_controllerType = ps4_windows;
                }
                else if (DATA_SIZE == 20)
                {
                    input_controllerType = switchProCon_windows;
                }
                for (int i = 0; i < DATA_SIZE; i++)
                {
                    input_last_button[i] = input_button[i];
                    input_button[i] = data[i] - '0';
                    putchar(data[i]);
                }
                putchar('\n');
                fflush(NULL);
            }
            else
            {
                // printf("%s\n",data);
                char temp[20]; // 多分小数は20文字以上にはならない気がする
                for (int i = 0; i < 20; i++)
                {
                    temp[i] = '\n';
                }
                size_t data_index = 0;
                int stick_i = 0;
                while (data[data_index] != '\0')
                {
                    int temp_index = 0;
                    for (int j = 0; j <= 10; j++)
                    {
                        if (data[data_index] == '\0')
                        {
                            temp[temp_index] = '\0';
                            break;
                        }
                        if (data[data_index] == '-' && 1 < j)
                        {
                            temp[temp_index] = '\0';
                            break;
                        }
                        if (data[data_index] == '.' && 4 < j) // 次の値の小数点
                        {
                            temp[temp_index - 1] = '\0';
                            data_index--;
                            break;
                        }
                        temp[temp_index] = data[data_index];
                        data_index++;
                        temp_index++;
                    }

                    if (temp[temp_index] != '\0')
                    {
                        // printf("ERROR\nesp32->mbed,data_too_long\n");
                        while (1)
                        {
                            if (data[data_index] == '\0')
                            {
                                break;
                            }
                            if (data[data_index] == '-')
                            {
                                break;
                            }
                            if (data[data_index] == '.') // 次の値の小数点
                            {
                                data_index--;
                                break;
                            }
                            data_index++;
                        }
                    }

                    temp[10] = '\0'; // 文字数制限
                    // printf("%s\n",temp);
                    char *endptr = NULL;
                    errno = 0;
                    double input = strtod(temp, &endptr);
                    if (errno == 0 && *endptr == '\0')
                    {
                        if (-1 * stick_sikii < input && input < stick_sikii)
                        {
                            input_stick[stick_i] = 0.0;
                        }
                        else
                        {
                            if (2.0 < abs(input))
                            {
                                input_stick[stick_i] = 0.0;
                                // printf("ERROR\nesp32->mbed,stick too BIG\n");
                            }
                            else
                            {
                                input_stick[stick_i] = input;
                            }
                        }
                    }
                    // printf("%d,%f\n", stick_i, input_stick[stick_i]);
                    stick_i++;
                }

                input_stick[move_tate_stick] *= -1; // 縦のほうはプラスマイナス逆らしい(意味わからん)
            }
        }

    } // void esp32_read()

    // MARK: controller_read
    void controller_read()
    {
        // iti_PID_ON = 0;
        if (input_controllerType == switchProCon_windows && input_button[right_button] == 1)
        {
            outputDirection = 90;
            outputSpeed = 1;
            if (input_button[up_button] == 1)
                outputDirection -= 45;
            else if (input_button[down_button] == 1)
                outputDirection += 45;
        }
        else if (input_controllerType == switchProCon_windows && input_button[left_button] == 1)
        {
            outputDirection = -90;
            outputSpeed = 1;
            if (input_button[down_button] == 1)
                outputDirection -= 45;
            else if (input_button[up_button] == 1)
                outputDirection += 45;
        }
        else if (input_controllerType == switchProCon_windows && input_button[up_button] == 1)
        {
            outputDirection = 0;
            outputSpeed = 1;
            //        if (input_button[left_button] == 1)
            //          outputDirection += 45;
            //        else if (input_button[right_button] == 1)
            //          outputDirection -= 45;
        }
        else if (input_controllerType == switchProCon_windows && input_button[down_button] == 1)
        {
            outputDirection = 180;
            outputSpeed = 1;
            // if(input_button[right_button] == 1)
            //     outputDirection+=45;
            // else if(input_button[left_button]==1)
            //     outputDirection-=45;
        }
        else if (input_stick[move_tate_stick] != 0.0 || input_stick[move_yoko_stick] != 0.0)
        { // ボタンが押されていないならスティックを読み取る
            outputSpeed = sqrt(
                input_stick[move_tate_stick] * input_stick[move_tate_stick] + input_stick[move_yoko_stick] * input_stick[move_yoko_stick]);
            outputDirection = degree(
                atan2(input_stick[move_yoko_stick], input_stick[move_tate_stick])); // atan2(y,x)
        }
        else
        {
            outputSpeed = 0; // ボタンが押されていないときは停止
        }

        outputSpeed *= MOTOR_MAX_SPEED;

        if (input_controllerType == ps4_ubuntu)
        {
            if (input_button[ubuntu_ps4_migisenkai_button] == 1)
            {
                targetYaw = robotYaw + 30;
            }
            else if (input_button[ubuntu_ps4_hidarisenkai_button] == 1)
            {
                targetYaw = robotYaw - 30;
            }
        }
        else if (input_controllerType == switchProCon_windows)
        {
            if (input_button[windows_switch_procon_migisenkai_button] == 1)
            {
                targetYaw = robotYaw + 30;
            }
            else if (input_button[windows_switch_procon_hidarisenkai_button] == 1)
            {
                targetYaw = robotYaw - 30;
            }
        }
    } // void controller_read()

    // MARK: BNOSetup
    /// BNOのセットアップ-this may take a while
    void BNOSetup()
    {
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
        // for (int i = 0; i < 22; i++)
        // {
        //     bno.calibration[i] = BNO_DEFAULT_CALIBRATION[i];
        // }
        // BNO055_write_calibration_data(&bno);
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
        while (HAL_GPIO_ReadPin(DebugButton_GPIO_Port, DebugButton_Pin) == GPIO_PIN_SET)
        {
            HAL_GPIO_TogglePin(DebugLED_GPIO_Port, DebugLED_Pin);
            HAL_Delay(100);
        }
        BNO055_read_calibration_data(&bno);
        printf("calibration:");
        for (int i = 0; i < 22; i++)
        {
            // printf("calibration[%d]=%d\n", i, bno.calibration[i]);
            printf("%d,", bno.calibration[i]);
        }
        printf("\n");
        for (int i = 0; i < 3; i++)
        {
            HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_SET);
            HAL_Delay(500);
            HAL_GPIO_WritePin(DebugLED_GPIO_Port, DebugLED_Pin, GPIO_PIN_RESET);
            HAL_Delay(500);
        }
        BNO055_get_angles(&bno);
        defaultYaw = bno.euler.yaw;
    }

    // MARK: Brake_StopWheel
    /// DitelMotorDriverRotate(&hcan1, MOTORn_ADDRESS, DITEL_MOTOR_BRAKE, DITEL_NONE);
    void Brake_StopWheel()
    {
        DitelMotorDriverRotate(&hcan1, MOTOR1_ADDRESS, DITEL_MOTOR_BRAKE, DITEL_NONE);
        DitelMotorDriverRotate(&hcan1, MOTOR2_ADDRESS, DITEL_MOTOR_BRAKE, DITEL_NONE);
        DitelMotorDriverRotate(&hcan1, MOTOR3_ADDRESS, DITEL_MOTOR_BRAKE, DITEL_NONE);
        HAL_Delay(1);
        DitelMotorDriverRotate(&hcan1, MOTOR4_ADDRESS, DITEL_MOTOR_BRAKE, DITEL_NONE);
    }

#ifdef __cplusplus
}
#endif
