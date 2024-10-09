cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(simcontrols)

pico_sdk_init()

add_subdirectory(shifter)
add_subdirectory(shifter-test)
add_subdirectory(pedals)
add_subdirectory(pedals-test)
