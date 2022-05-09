add_executable(tic_tac_toe
        main.c
        painting.c
        logic.c
        lib/fonts.c
        lib/st7735.c
        lib/DEV_Config.c
        lib/ICM20948.c
        )

# pull in common dependencies
target_link_libraries(
  tic_tac_toe
  hardware_pwm
  hardware_pio
  hardware_spi
  hardware_i2c
  pico_stdlib
)

pico_enable_stdio_usb(tic_tac_toe 1)
pico_enable_stdio_uart(tic_tac_toe 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(tic_tac_toe)

# add url via pico_set_program_url
example_auto_set_url(tic_tac_toe)
