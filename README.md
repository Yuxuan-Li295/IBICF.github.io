# ESE519_Final

For ESE 519 Final project. balanced board controled Drone and Panel kit Camera.

**TEAM - I believe I can fly:**

Yuxuan Li: https://github.com/Yuxuan-Li295

Xingqi Pan: https://github.com/anniepan8215

Yuxin Wang: https://github.com/Ariiees


## What it does?

You coud using a balanced board to control the fly of the drone in real time(mimic the behavior of the joystick) which is also the main part to control the drone. At the same time, we also create a camera panel which can track the drone's trajectory by search its front LED and rotate with it.

### Demo

https://www.youtube.com/watch?v=_GHgrHEjJ3k

## How we made it

### Circuit connection 【Hardware】

**PIN MAP**

| RP2040 | Controller | PICO4ML |
| :--| :--  |:-- |
| A2 | X | N/A |
|  A3  | Y | N/A |
| MO | N/A | GP28 |
| MI | N/A | GP27 |
| SCK | N/A | GP26 |
| GND | GND | GND |

|PICO4ML|PANEL KIT|
| :--| :--  |
|VBUS| 5V|
|G|GND|
|GP0|BOT SERVO PWM INPUT|
|GP1|UP SERVO PWM INPUT|

##Hardware Circuit

Amplifier: Scale 3V to 5V

<img width="400" alt="amplifier" src=media/hardware_citcuit/amplifier.jpg>

Controller: Original Controller PCB for drone

<img width="400" alt="controller" src=media/hardware_citcuit/controller.jpg>

Indicator and filter: indicate the circuit connection and filt the PWM ourtput

<img width="400" alt="indicator_and_filter" src=media/hardware_citcuit/indicator_and_filter.jpg>

Tracking: track the LED in drone

<img width="400" alt="tracking" src=media/hardware_citcuit/tracking.jpg>

### Control theory 【Code for each MCU】


**RP2040_Drone**

We use PIO to write PWM that out put a DC voltage which will mimic the output the joystick, and this is the main part that we used to control the drone.

```c
// partten: 001
void move_forward(PIO pio, uint sm_x, uint sm_y){
    pio_pwm_set_level(pio, sm_x, 125);
    pio_pwm_set_level(pio, sm_y, 77);
}

// partten: 010
void move_back(PIO pio, uint sm_x, uint sm_y){
    pio_pwm_set_level(pio, sm_x, 122);
    pio_pwm_set_level(pio, sm_y, 205);
}

//partten: 011
void move_left(PIO pio, uint sm_x, uint sm_y){
    pio_pwm_set_level(pio, sm_x, 85);
    pio_pwm_set_level(pio, sm_y, 120);
}

//partten: 100
void move_right(PIO pio, uint sm_x, uint sm_y){
    pio_pwm_set_level(pio, sm_x, 155);
    pio_pwm_set_level(pio, sm_y, 117);
}
```

**PICO4ML_IMU**

Initialize three ports for output and communication between RP2040 and Pico4ML:
```c
  ICM42622::Icm42622Init();
  gpio_init(26);
  gpio_set_dir(26, GPIO_OUT);
  gpio_init(27);
  gpio_set_dir(27, GPIO_OUT);
  gpio_init(28);
  gpio_set_dir(28, GPIO_OUT);
``` 
Get the accelerometer value for the IMU
```c
ICM42622::Icm42622ReadGyro(&x,&y,&z);
ICM42622::Icm42622ReadAccel(&accex,&accey,&accez);
//printf("Gyro: X:%.2f, Y: %.2f\n",x,y);
printf("Acce: X:%.2f, Y: %.2f,Z:%.2f\n",accex,accey,accez);
``` 
Then we judge the degree of tilt of the drone and assign different signal indicator that and pass it to RP2040 to control the fly direction of it.

```c
if(accex >= -1.1 && accex <= -0.7)
    {
      gpio_put(26,0);
      gpio_put(27,0);
      gpio_put(28,1);
      printf("Acce: X:%.2f, Y: %.2f\n",accex,accey);
      // move_forward(pio_pwm,smx,smy);
      printf("forward\n");
    }
    else if(accex >= 0.7 && accex <= 1.1)
    {
      gpio_put(26,0);
      gpio_put(27,1);
      gpio_put(28,0);
      printf("Acce: X:%.2f, Y: %.2f\n",accex,accey);
      // move_back(pio_pwm, smx, smy);
      printf("back\n");
    }
    else if(accey >= 0.6 && accey <= 1.1)
    {
      gpio_put(26,0);
      gpio_put(27,1);
      gpio_put(28,1);
      printf("Acce: X:%.2f, Y: %.2f\n",accex,accey);
      //move_left (pio_pwm, smx, smy);
      printf("left\n");
    }
    else if(accey >= -1.1 && accey <= -0.6)
    {
      gpio_put(26,1);
      gpio_put(27,0);
      gpio_put(28,0);
      printf("Acce: X:%.2f, Y: %.2f\n",accex,accey);
      // move_right(pio_pwm, smx, smy);
      printf("right\n");
    }
    else{
      gpio_put(26,0);
      gpio_put(27,0);
      gpio_put(28,0);
      printf("Acce: X:%.2f, Y: %.2f\n",accex,accey);
      // stay(pio_pwm, smx, smy);
      printf("stay\n");
    }
```

**PICO4ML_Camera_LED_TRACKING**
Tracking each pixels captured by camera, find the location of the pixel with highest luminous and records it as the tracking path.
```C
while (true) {
    memset(pre_displayBuf, 0, sizeof(pre_displayBuf)); // reset draft
    // ST7735_DrawImage(0, 0, 80, 160, arducam_logo);
    pre_max_x = 0; // x index of max value t-1 time
    pre_max_y = 0; // y index of max value t-1 time
    for(track_period = 0; track_period < 100; track_period++){
        // gpio_put(PIN_LED, !gpio_get(PIN_LED));
        arducam_capture_frame(&config);

        uint16_t index = 0;
        int max_x = 0;
        int max_y = 0;
        uint16_t max = 0;
        for (int y = 0; y < 160; y++) {
            for (int x = 0; x < 80; x++) {
                uint8_t c = image_buf[(2+320-2*y)*324+(2+40+2*x)];
                uint16_t imageRGB   = ST7735_COLOR565(c, c, c);
                if(imageRGB > max && imageRGB > 0xAAAA){
		    // record the pixel with highest luminous
                    max = imageRGB;
	            max_x = x;
	            max_y = y;
                }
                displayBuf[index++] = (uint8_t)(imageRGB >> 8) & 0xFF;
                displayBuf[index++] = (uint8_t)(imageRGB)&0xFF;
            }
        }
        // pre_displayBuf[max_x*max_y+1] = 0xFF;
	// display the tracking path on the LCD screen
        pre_displayBuf[max_x*max_y] = 0xFF;
        pre_displayBuf[max_x*max_y+1] = 0xFF;
        ST7735_DrawImage(0, 0, 80, 160, pre_displayBuf);
}
```

**PICO4ML_Servo_Control**
```C
gpio_set_function(PIN_OUT_LR, GPIO_FUNC_PWM);  /// Set the pin 0 to be PWM for bot servo
gpio_set_function(PIN_OUT_UD, GPIO_FUNC_PWM);  /// Set the pin 1 to be PWM for top servo
uint slice_LR   = pwm_gpio_to_slice_num(PIN_OUT_LR);
uint channel_LR = pwm_gpio_to_channel(PIN_OUT_LR);
uint slice_UD   = pwm_gpio_to_slice_num(PIN_OUT_UD);
uint channel_UD = pwm_gpio_to_channel(PIN_OUT_UD);
...
pwm_set_clkdiv(slice_LR, 256.0f);  /// Setting the divider to slow down the clock
pwm_set_wrap(slice_LR, 9804);      /// setting the Wrap time to 9764 (20 ms)
pwm_set_enabled(slice_LR, true);

pwm_set_clkdiv(slice_UD, 256.0f);  /// Setting the divider to slow down the clock
pwm_set_wrap(slice_UD, 9804);      /// setting the Wrap time to 9764 (20 ms)
pwm_set_enabled(slice_UD, true);
... \\\inside loop
    if(pre_max_x < max_x){
	// match rotation degree with distance between t and t-1
	pwm_set_chan_level(slice_LR, channel_LR, (735-((735-490)*(max_x-pre_max_x)/(90))));
    }else{
	pwm_set_chan_level(slice_LR, channel_LR,  (735+((980-735)*(pre_max_x-max_x)/(90))));

    }
    if(pre_max_y < max_y){
	pwm_set_chan_level(slice_UD, channel_UD,  (735-((735-490)*(max_y-pre_max_y)/(90))));
    }else{
	pwm_set_chan_level(slice_UD, channel_UD,  (735+(980-735)*((pre_max_y-max_y)/(90))));
    }
    pre_max_x = max_x;
    pre_max_y = max_y;
```

### Communication protocol 【Code between each MCU】

**RP2040_Drone with PICO4ML_IMU**

We use 3 pins to indict the pattern of drone's direction. `000` means stay, '001': forward, `010`: backward, `011`: left, `100`: right.

For RP2040, it will get the digital value of pin, and distinguish the pattern.

```C
int get_pattern(uint pin1, uint pin2, uint pin3){
    int a, b, c;
    sleep_us(50);
    if(gpio_get(pin1)==0){
        a = 0;
    }else{
        a = 1;
    }

    if(gpio_get(pin2)==0){
        b = 0;
    }else{
        b = 1;
    }

    if(gpio_get(pin3)==0){
        c = 0;
    }else{
        c = 1;
    }
    int pattern = c << 2 | b << 1 | a;
    return pattern;
}
```
After got the integer value of the pattern by applying the 'left shift' operation combined with the 'OR' operation we can directly return the pattern value.


## File description

【File_name】: What is it

【README_midpoint】: Our midpoint project report.

【code/IBICF/motor/pwm_new.c】: Main code for RP2040_Drone.

【code/IMU_Part】: The main code for enable the IMU and using the accelerometer value to control the direction of the drone's flying.

【code/cam_tracking】: The main code for camera tracking and servo controlling



