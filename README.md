# MecanumWheel
## 概要
メカナムホイールを制御する．

| 項目         | 仕様              |
| ------------ | ----------------- |
| 使用マイコン | NUCLEO_F446RE     |
| 使用IDE      | CubeIDE,VSCode    |
| 使用言語等   | C++, STM32CubeHAL |

## 今後の目標
全方向移動\
ホイールのPID制御\
角度制御

## 何を実現できたのか

## ピン設定
| ピン | 設定        | ラベル              | 備考           |
| ---- | ----------- | ------------------- | -------------- |
| PA5  | GPIO_Output | DebugLED            | ラベルのみ変更 |
| PC13 | GPIO_EXTI13 | DebugButton         | ラベルのみ変更 |
| PB9  | I2C1_SDA    | I2C1_SDA_BNO        |                |
| PB8  | I2C1_SCL    | I2C1_SCL_BNO        |                |
| PA12 | CAN1_TX     | CAN1_TX_motorDriver |                |
| PA11 | CAN1_RX     | CAN1_RX_motorDriver |                |
| PC10 | USART3_TX   | USART3_TX_ESP       |                |
| PC11 | USART3_RX   | USART3_RX_ESP       |                |
| PA8  | TIM1_CH1    | Encoder1_1          |                |
| PA9  | TIM1_CH2    | Encoder1_2          |                |
| PA0  | TIM2_CH1    | Encoder2_1          |                |
| PA1  | TIM2_CH2    | Encoder2_2          |                |
| PA6  | TIM3_CH1    | Encoder3_1          |                |
| PA7  | TIM3_CH2    | Encoder3_2          |                |
| PB6  | TIM4_CH1    | Encoder4_1          |                |
| PB7  | TIM4_CH2    | Encoder4_2          |                |
| PC6  | USART6_TX   | USART6_TX_Arduino   |                |
| PC7  | USART6_RX   | USART6_RX_Arduino   |                |

## クロック設定
| 項目                   | 内容 | 備考               |
| ---------------------- | ---- | ------------------ |
| APB1 peripheral clocks | 30   | CAN1を使いたかった |

## 変更した設定
### Project Manager
Code Generator, Generated files\
✅Generate peripheral initialization as a pair of '.c/.h' files per peripheral
### USART2
NVIC Settings\
✅USART2 global interrupt
### USART3
MODE: Asynchronous\
NVIC Settings\
✅USART3 global interrupt
### USART6
MODE: Asynchronous\
NVIC Settings\
✅USART6 global interrupt
### I2C1
MODE: I2C
#### Parameter Settings
| 項目            | 設定      | 備考                        |
| --------------- | --------- | --------------------------- |
| I2C Speed Mode  | Fast Mode | Standard Modeじゃ無理だった |
| I2C Clock Speed | 400000Hz  | BNO055                      |

その他は変えていない
### CAN1
MODE: ✅Activated\
CAN1で通信する．
#### Parameter Settings
[参考文献](https://hsdev.co.jp/stm32-can/)の通りに設定した．

| 項目                       | 内容  |
| -------------------------- | ----- |
| 転送速度 Baud Rate         | 1Mbps |
| サンプリングポイント       | 80%   |
| Bit Time(Time for one Bit) | 1μs   |
| CANのクロック源            | 30MHz |
| Prescaler                  | 3     |
| Time Quantum               | 100ns |
| TSeg1                      | 7     |
| TSeg2                      | 2     |
| SJW                        | 1     |

### TIM1
Combined Channels : Encoder Mode\
ロータリーエンコーダを使うための設定[参考文献](https://www.shujima.work/entry/2019/05/28/221629#%E3%82%A8%E3%83%B3%E3%82%B3%E3%83%BC%E3%83%80%E3%81%AE%E3%83%94%E3%83%B3%E8%A8%AD%E5%AE%9A)
#### Parameter Settings
##### Encoder
Encoder Mode : Encoder Mode TI1 and TI2
### TIM2
設定はTIM1と同じ
### TIM3
設定はTIM1と同じ
### TIM4
設定はTIM1と同じ

## プログラム概要
### setup()
デバッグLEDを点灯\
Hello WorldとPCに送信\
PCからのデータを受信割り込み設定\
//BNO055の初期化\
CA1の初期化\
デバッグLEDを消灯\
outputDirectionを0°\
outputSpeedは最大値の半分\
outputRotationは0°
### loop()
now = HAL_GetTick()
#### 0.01秒ごとに
デバッグLEDをトグル\
//BNO055からZ軸の角加速度を取得，現在の角度の導出\
mecanumCalc()
### mecanumCalc()
モーターのスピード計算\
DitelMotor出力\
エラーがいくつあったか返す
