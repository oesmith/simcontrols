add_executable(pedals_test main.c)

target_link_libraries(pedals_test
  pico_stdlib
  hardware_adc
)

pico_enable_stdio_usb(pedals_test 1)
pico_enable_stdio_uart(pedals_test 0)

pico_add_extra_outputs(pedals_test)
