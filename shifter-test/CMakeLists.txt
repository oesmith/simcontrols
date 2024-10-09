add_executable(shifter_test main.c)

target_link_libraries(shifter_test
  pico_stdlib
  hardware_adc
)

pico_enable_stdio_usb(shifter_test 1)
pico_enable_stdio_uart(shifter_test 0)

pico_add_extra_outputs(shifter_test)
