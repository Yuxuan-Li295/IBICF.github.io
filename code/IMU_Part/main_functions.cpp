/**
 * Copyright (c) TEAM: I believe I can fly
 *
 * UPENN ESE519 PROJECT
 */
//==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pwm.pio.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/gpio.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include <hardware/irq.h>
#include <hardware/uart.h>
#include <pico/stdio_usb.h>
#include <ICM42622.h>
#include "imu_provider.h"
#include "magic_wand_model_data.h"
#include "rasterize_stroke.h"

# define x_pin 2
# define y_pin 3

#define SCREEN 1
#if SCREEN
#include "LCD_st7735.h"
#endif
#define PARITY UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1
// Constants for image rasterization
constexpr int raster_width      = 32;
constexpr int raster_height     = 32;
constexpr int raster_channels   = 3;
constexpr int raster_byte_count = raster_height * raster_width * raster_channels;
int8_t        raster_buffer[raster_byte_count];

// Write `period` to the input shift register
void pio_pwm_set_period(PIO pio, uint sm, uint32_t period) {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_put_blocking(pio, sm, period);
    pio_sm_exec(pio, sm, pio_encode_pull(false, false));
    pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
    pio_sm_set_enabled(pio, sm, true);
}

// Write `level` to TX FIFO. State machine will copy this into X.
void pio_pwm_set_level(PIO pio, uint sm, uint32_t level) {
    pio_sm_put_blocking(pio, sm, level);
}

//右(x0)---左(x255), 前(y0)---后(y255)
//partten: 000
void stay(PIO pio, uint sm_x, uint sm_y){
    pio_pwm_set_level(pio, sm_x, 118);
    pio_pwm_set_level(pio, sm_y, 118); 
    // sleep_ms(10000);
}

// partten: 001
void move_forward(PIO pio, uint sm_x, uint sm_y){
    pio_pwm_set_level(pio, sm_x, 125);
    pio_pwm_set_level(pio, sm_y, 77);
    sleep_ms(5000);
}

// partten: 010
void move_back(PIO pio, uint sm_x, uint sm_y){
    pio_pwm_set_level(pio, sm_x, 122);
    pio_pwm_set_level(pio, sm_y,205);
    sleep_ms(5000);
}

//partten: 011
void move_left(PIO pio, uint sm_x, uint sm_y){
    pio_pwm_set_level(pio, sm_x, 85);
    pio_pwm_set_level(pio, sm_y, 120);
    sleep_ms(5000);
}

//partten: 100
void move_right(PIO pio, uint sm_x, uint sm_y){
    pio_pwm_set_level(pio, sm_x, 155);
    pio_pwm_set_level(pio, sm_y, 117);
    sleep_ms(5000);
}

void setup() {
  stdio_init_all();
  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(4, GPIO_FUNC_I2C);
  gpio_set_function(5, GPIO_FUNC_I2C);
  gpio_pull_up(4);
  gpio_pull_up(5);
  sleep_ms(1000);
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  gpio_put(25, !gpio_get(25));
  ICM42622::Icm42622Init();
  gpio_init(26);
  gpio_set_dir(26, GPIO_OUT);
  gpio_init(27);
  gpio_set_dir(27, GPIO_OUT);
  gpio_init(28);
  gpio_set_dir(28, GPIO_OUT);
  

#if SCREEN
  ST7735_Init();
  ST7735_DrawImage(0, 0, 80, 160, arducam_logo);
#endif

#if SCREEN
  ST7735_FillScreen(ST7735_GREEN);
  ST7735_DrawImage(0, 0, 80, 40, (uint8_t *)IMU_ICM20948);

  ST7735_WriteString(5, 45, "I Want", Font_11x18, ST7735_BLACK, ST7735_GREEN);
  ST7735_WriteString(0, 70, "FLY", Font_11x18, ST7735_BLACK, ST7735_GREEN);
  
#endif
  gpio_put(25, !gpio_get(25));
}

//########################################################################################//
//########################################################################################//
//########################################################################################//
//all the function are done here

void loop() {
  stdio_init_all();
  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(4, GPIO_FUNC_I2C);
  gpio_set_function(5, GPIO_FUNC_I2C);
  gpio_pull_up(4);
  gpio_pull_up(5);
  sleep_ms(1000);
  ICM42622::Icm42622Init();
  // Pwm output analog
  // use to output analog
  // PIO pio_pwm = pio1;
  // int smx = 0;
  // int smy = 1;

  // uint offset = pio_add_program(pio_pwm, &pwm_program);
  // printf("Loaded program at %d\n", offset);

  // pwm_program_init(pio_pwm, smx, offset, x_pin);
  // pio_pwm_set_period(pio_pwm, smx, 255);

  // pwm_program_init(pio_pwm, smy, offset, y_pin);
  // pio_pwm_set_period(pio_pwm, smy, 255);
  while(1){
    float x, y, z;
    float accex,accey,accez;
    //printf("Wow");
    ICM42622::Icm42622ReadGyro(&x,&y,&z);
    ICM42622::Icm42622ReadAccel(&accex,&accey,&accez);
    //printf("Gyro: X:%.2f, Y: %.2f\n",x,y);
    printf("Acce: X:%.2f, Y: %.2f,Z:%.2f\n",accex,accey,accez);
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
  }
}





