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
BNO055の初期化\
CA1の初期化\
ESP32からのデータを受信割り込み設定\
arduinoからのデータを受信割り込み設定\
outputDirectionを0°\
outputSpeedは0

### loop()
now = HAL_GetTick()

#### 10msごとに
BNO055からヨーを取得し，ロボットの現在の角度の導出\
esp32_read()により，espからコントローラの状態を取得\
controller_read()により，コントローラの状態を解析\
arduino_read()により，arduinoからの信号を読む\
現在の角度と目標角度からPIDでoutputRotationを計算\
コントローラのブレーキボタンが押された，input_arduinoが0である場合\
　　ブレーキをかける．\
　　目標角度を現在の角度に設定\
　　outputSpeedを0に設定\
　　outputRotationを0に設定\
それ以外は\
　　mecanumCalc()でメカナムを動かす

### mecanumCalc()
outputDirection，outputSpeed，outputRotationから各モーターのスピード計算\
DitelMotor出力\
エラーがいくつあったか返す

### esp32_read()
espUartRxTbsに入ったデータを取り出す．\
\nを取得したとき，パケットの末尾としてみなす．\
\nを所得した後，そのパケットを解析する．\
input_buttonにボタンの状態を，input_stickにスティックの状態を格納する．\
パケットがボタンデータだった場合，ボタンの数からコントローラの種類を予測して設定する．

### controller_read()
コントローラの状態を解析する．\
マクロで定義されたボタンが押されている時に対応するoutputDirection，outputSpeed，targetYawを設定する．\
この関数では，コントローラの種類によって分岐する場所がある．

### arduino_read()
arduinoUartRxTbsに入ったデータを取り出す．\
0x02だった場合，arduinoにoutputSpeedを0～99の値で送信する．\
他の場合，ブレーキon/offの信号であるため，フラグを立てる．(input_arduino)

### BNOSetup()
BNO055関係\
初期化．\
チェックする．→PCに出力\
チェック結果が良くない場合，ソフトウェアリセット\
リセットする．\
BNOのIDをPCに出力\
角度の単位をdegreeに設定\
電源モードをNormalに設定\
使用モードをNDOFに設定(9軸フュージョンモード)\
ステータスを所得．→PCに出力\
ステータスがエラーだった場合\
　　エラーを取得．→PCに出力\
問題ない場合はbreak;，エラーだった場合はチェックからやりなおし．\
デバッグLED消灯．\
1秒待つ．\
ここから，キャリブレーションに入る．\
キャリブレーションの状況を取得．\
SYS GYR ACC MAGのキャリブレーション状況をPCに出力．\
キャリブレーションのステータスがACC以外3になるまで繰り返す．\
このキャリブレーションはデバッグボタンを押すことでスキップできる．\
キャリブレーションが終わった後，100msごとにデバッグLEDを点滅させて知らせる．\
デバッグボタンを押すと3回長く点滅し，次の処理へ移行する．\
この時，PCに今回のキャリブレーションデータを送信する．\
現在の角度を取得する．\
取得した角度をdefaultYawに設定する．

よって，LEDが点灯中はBNO設定中→LEDが消灯することでキャリブレーションへ移行
→LEDが点滅するまでキャリブレーション→望みの角度に合わせてデバッグボタンを押す\
という流れになる．

### Brake_StopWheel()
ホイールを止める．

### PcUartRxTbsAfterSwap()
TripleBufferSystemに必要な関数．割り込みを設定しなおし，割り込みを有効にする．
### espUartRxTbsAfterSwap()
TripleBufferSystemに必要な関数．割り込みを設定しなおし，割り込みを有効にする．
### arduinoUartRxTbsAfterSwap()
TripleBufferSystemに必要な関数．割り込みを設定しなおし，割り込みを有効にする．

### HAL_UART_RxCpltCallback()
割り込み処理関数．\
TripleBufferSystemを使っている\
対応するtbsをheadMove()し，新しい割り込みを設定する．
