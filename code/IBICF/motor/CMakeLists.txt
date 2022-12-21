pico_sdk_init() # PICO SDK (initialization)

add_executable(motor)

pico_generate_pio_header(motor ${CMAKE_CURRENT_LIST_DIR}/pwm.pio)

target_sources(motor PRIVATE pwm.c)

target_link_libraries(motor PRIVATE pico_stdlib hardware_pio hardware_pwm)

pico_enable_stdio_usb(motor 1) # PICO SDK (USB)
pico_enable_stdio_uart(motor 0) # PICO SDK (USB)

pico_add_extra_outputs(motor)