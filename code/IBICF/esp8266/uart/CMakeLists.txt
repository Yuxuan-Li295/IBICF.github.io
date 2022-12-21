pico_sdk_init() # PICO SDK (initialization)

add_executable(uart uart.c)

# pull in common dependencies
target_link_libraries(uart pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(uart)
