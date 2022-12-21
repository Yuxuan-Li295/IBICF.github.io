/**
 * Copyright (c) TEAM: I believe I can fly
 *
 * UPENN ESE519 PROJECT
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "pwm.pio.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/gpio.h"


# define x_pin 27
# define y_pin 26

#define model_1 3
#define model_2 4
#define model_3 6


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
//118->1.497v  210->2.684v  50->629.6mv
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


int main() {
    stdio_init_all();

    //use to detect message from pico4ml
    gpio_init(model_1);
    gpio_set_dir(model_1, GPIO_IN);
    gpio_init(model_2);
    gpio_set_dir(model_2, GPIO_IN);
    gpio_init(model_3);
    gpio_set_dir(model_3, GPIO_IN);

    //use to output analog
    PIO pio_pwm = pio0;
    int smx = 0;
    int smy = 1;

    uint offset = pio_add_program(pio_pwm, &pwm_program);
    printf("Loaded program at %d\n", offset);

    pwm_program_init(pio_pwm, smx, offset, x_pin);
    pio_pwm_set_period(pio_pwm, smx, 255);

    pwm_program_init(pio_pwm, smy, offset, y_pin);
    pio_pwm_set_period(pio_pwm, smy, 255);

    while (true) {
        int pattern = 0;
        pattern = get_pattern(model_1, model_2, model_3);
        switch(pattern){
            case 0:
                stay(pio_pwm, smx, smy);
                printf("stay\n");
                break;
            case 1:
                move_forward(pio_pwm,smx,smy);
                printf("forward\n");
                break;
            case 2:
                move_back(pio_pwm, smx, smy);
                printf("back\n");
                break;
            case 3:
                move_left(pio_pwm, smx, smy);
                printf("left\n");
                break;
            case 4:
                move_right(pio_pwm, smx, smy);
                printf("right\n");
                break;
        }
    }

}
