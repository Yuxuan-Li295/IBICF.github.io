/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "pwm.pio.h"

# define motor_A1 27
# define motor_B1 3
# define motor_B2 26
# define motor_A2 4

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

int main() {
    stdio_init_all();

    PIO pio = pio0;
    int sm1 = 0;
    int sm2 = 1;
    int sm3 = 2;
    int sm4 = 3;

    uint offset = pio_add_program(pio, &pwm_program);
    printf("Loaded program at %d\n", offset);

    pwm_program_init(pio, sm1, offset, motor_A1);
    pio_pwm_set_period(pio, sm1, (1u << 16) - 1);

    pwm_program_init(pio, sm2, offset, motor_B1);
    pio_pwm_set_period(pio, sm2, (1u << 16) - 1);

    pwm_program_init(pio, sm3, offset, motor_B2);
    pio_pwm_set_period(pio, sm3, (1u << 16) - 1);

    pwm_program_init(pio, sm4, offset, motor_A2); 
    pio_pwm_set_period(pio, sm4, (1u << 16) - 1);

    int level = 0;
    while (true) {
        printf("Level = %d\n", level);
        pio_pwm_set_level(pio, sm1, level * level);
        pio_pwm_set_level(pio, sm3, level * level);
        pio_pwm_set_level(pio, sm2, level * level);
        pio_pwm_set_level(pio, sm4, level * level);
        level = (level + 1) % 256;
        while(level == 255){
            pio_pwm_set_level(pio, sm1, level * level);
            pio_pwm_set_level(pio, sm3, level * level);
            pio_pwm_set_level(pio, sm2, level * level);
            pio_pwm_set_level(pio, sm4, level * level);
        }
        sleep_ms(10);
    }

}

/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Fade an LED between low and high brightness. An interrupt handler updates
// the PWM slice's output level each time the counter wraps.

// #include "pico/stdlib.h"
// #include <stdio.h>
// #include "pico/time.h"
// #include "hardware/irq.h"
// #include "hardware/pwm.h"

// # define motor_pin 27

// #ifdef motor_pin
// void on_pwm_wrap() {
//     static int fade = 0;
//     static bool going_up = true;
//     // Clear the interrupt flag that brought us here
//     pwm_clear_irq(pwm_gpio_to_slice_num(motor_pin));

//     if (going_up) {
//         ++fade;
//         if (fade > 255) {
//             fade = 255;
//             going_up = false;
//         }
//     } else {
//         --fade;
//         if (fade < 0) {
//             fade = 0;
//             going_up = true;
//         }
//     }
//     // Square the fade value to make the LED's brightness appear more linear
//     // Note this range matches with the wrap value
//     pwm_set_gpio_level(motor_pin, fade * fade);
// }
// #endif

// int main() {
// #ifndef motor_pin
// #warning pwm/led_fade example requires a board with a regular LED
// #else
//     // Tell the LED pin that the PWM is in charge of its value.
//     gpio_set_function(motor_pin, GPIO_FUNC_PWM);
//     // Figure out which slice we just connected to the LED pin
//     uint slice_num = pwm_gpio_to_slice_num(motor_pin);

//     // Mask our slice's IRQ output into the PWM block's single interrupt line,
//     // and register our interrupt handler
//     pwm_clear_irq(slice_num);
//     pwm_set_irq_enabled(slice_num, true);
//     irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
//     irq_set_enabled(PWM_IRQ_WRAP, true);

//     // Get some sensible defaults for the slice configuration. By default, the
//     // counter is allowed to wrap over its maximum range (0 to 2**16-1)
//     pwm_config config = pwm_get_default_config();
//     // Set divider, reduces counter clock to sysclock/this value
//     pwm_config_set_clkdiv(&config, 4.f);
//     // Load the configuration into our PWM slice, and set it running.
//     pwm_init(slice_num, &config, true);

//     // Everything after this point happens in the PWM interrupt handler, so we
//     // can twiddle our thumbs
//     while (1)
//         tight_loop_contents();
// #endif
// }
