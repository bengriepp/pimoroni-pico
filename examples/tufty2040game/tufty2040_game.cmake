set(OUTPUT_NAME tufty2040_game)
add_executable(${OUTPUT_NAME} tufty2040_game.cpp)

target_link_libraries(${OUTPUT_NAME}
        tufty2040
        hardware_spi
        pico_graphics
        st7789
        button
)

# enable usb output
pico_enable_stdio_usb(${OUTPUT_NAME} 1)

pico_add_extra_outputs(${OUTPUT_NAME})
