pico_sdk_init() # PICO SDK (initialization)

add_executable(pio_tx)

pico_generate_pio_header(pio_tx ${CMAKE_CURRENT_LIST_DIR}/uart_tx.pio)

target_sources(pio_tx PRIVATE uart_tx.c)

target_link_libraries(pio_tx PRIVATE pico_stdlib hardware_pio)

pico_add_extra_outputs(pio_tx)

