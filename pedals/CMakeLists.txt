add_executable(pedals main.c usb_descriptors.c)

target_include_directories(pedals PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(pedals
  pico_stdlib
  pico_unique_id
  hardware_adc
  tinyusb_device
  tinyusb_board
)

pico_add_extra_outputs(pedals)
