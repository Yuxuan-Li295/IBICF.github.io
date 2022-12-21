pico_sdk_init() # PICO SDK (initialization)

add_executable(pio_rx)

pico_generate_pio_header(pio_rx ${CMAKE_CURRENT_LIST_DIR}/uart_rx.pio)

target_sources(pio_rx PRIVATE uart_rx.c)

target_link_libraries(pio_rx PRIVATE
        pico_stdlib
        pico_multicore
        hardware_pio
        )

pico_add_extra_outputs(pio_rx)