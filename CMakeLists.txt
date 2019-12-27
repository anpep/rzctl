cmake_minimum_required(VERSION 3.15)
project(rzctl C)

set(CMAKE_C_STANDARD )
set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -Werror")

add_executable(rzctl
        3rdparty/argtable3.h 3rdparty/argtable3.c
        util.h
        razer.h razer.c
        list_devices.h list_devices.c
        brightness.h brightness.c
        effect.h effect.c
        color.h color.c
        rzctl.c)
target_link_libraries(rzctl m usb-1.0)
