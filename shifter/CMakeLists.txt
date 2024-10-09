add_executable(shifter main.c usb_descriptors.c)

target_include_directories(shifter PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(shifter
  pico_stdlib
  pico_unique_id
  hardware_adc
  tinyusb_device
  tinyusb_board
)

pico_add_extra_outputs(shifter)
