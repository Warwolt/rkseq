cmake_minimum_required(VERSION 3.24)
set(AVR_UPLOADTOOL avrdude)
set(AVR_PROGRAMMER arduino)
option(AVR_UPLOADTOOL_PORT "USB port that device is connected to")
set(AVR_MCU atmega328p)
set(MCU_SPEED 16000000UL)
project(rkseq)
enable_language(C)

# ------------------------- Cross-compilation setup -------------------------- #
if(AVR)
    if (AVR_UPLOADTOOL_PORT MATCHES "OFF")
        message(FATAL_ERROR
            "AVR_UPLOADTOOL_PORT is required but not defined. "
            "Please define it using -DAVR_UPLOADTOOL_PORT=<usb port>, "
            "where <usb port> is the port your device is connected to: "
            "e.g. com5 (windows), /dev/ttyUSB0 (linux)"
        )
    endif()

    if(DEFINED ENV{AVR_FIND_ROOT_PATH})
        set(CMAKE_FIND_ROOT_PATH $ENV{AVR_FIND_ROOT_PATH})
    else(DEFINED ENV{AVR_FIND_ROOT_PATH})
        if(EXISTS "/opt/local/avr")
          set(CMAKE_FIND_ROOT_PATH "/opt/local/avr")
        elseif(EXISTS "/usr/avr")
          set(CMAKE_FIND_ROOT_PATH "/usr/avr")
        elseif(EXISTS "/usr/lib/avr")
          set(CMAKE_FIND_ROOT_PATH "/usr/lib/avr")
        elseif(EXISTS "/usr/local/CrossPack-AVR")
          set(CMAKE_FIND_ROOT_PATH "/usr/local/CrossPack-AVR")
        else(EXISTS "/opt/local/avr")
          message(FATAL_ERROR "Please set AVR_FIND_ROOT_PATH in your environment.")
        endif(EXISTS "/opt/local/avr")
    endif(DEFINED ENV{AVR_FIND_ROOT_PATH})

    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    # not added automatically, since CMAKE_SYSTEM_NAME is "generic"
    set(CMAKE_SYSTEM_INCLUDE_PATH "${CMAKE_FIND_ROOT_PATH}/include")
    set(CMAKE_SYSTEM_LIBRARY_PATH "${CMAKE_FIND_ROOT_PATH}/lib")


    if(CMAKE_BUILD_TYPE MATCHES Release)
        set(CMAKE_C_FLAGS_RELEASE "-Os")
        set(CMAKE_CXX_FLAGS_RELEASE "-Os")
    endif(CMAKE_BUILD_TYPE MATCHES Release)

    if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Os -save-temps -g -gdwarf-3 -gstrict-dwarf")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os -save-temps -g -gdwarf-3 -gstrict-dwarf")
    endif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_C_FLAGS_DEBUG "-O0 -save-temps -g -gdwarf-3 -gstrict-dwarf")
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -save-temps -g -gdwarf-3 -gstrict-dwarf")
    endif(CMAKE_BUILD_TYPE MATCHES Debug)
endif(AVR)

# --------------------------- Report configuration --------------------------- #
if(AVR)
    message(STATUS "Current uploadtool is: ${AVR_UPLOADTOOL}")
    message(STATUS "Current programmer is: ${AVR_PROGRAMMER}")
    message(STATUS "Current upload port is: ${AVR_UPLOADTOOL_PORT}")
    message(STATUS "Current uploadtool options are: ${AVR_UPLOADTOOL_OPTIONS}")
    message(STATUS "Current MCU is set to: ${AVR_MCU}")
    message(STATUS "Set CMAKE_FIND_ROOT_PATH to ${CMAKE_FIND_ROOT_PATH}")
    message(STATUS "Set CMAKE_SYSTEM_INCLUDE_PATH to ${CMAKE_SYSTEM_INCLUDE_PATH}")
    message(STATUS "Set CMAKE_SYSTEM_LIBRARY_PATH to ${CMAKE_SYSTEM_LIBRARY_PATH}")
endif(AVR)

# ---------------------------- Program executable ---------------------------- #
if(AVR)
add_compile_options(-Os -Wall -Werror -Wextra)
add_avr_executable("${PROJECT_NAME}"
  src/main.c
  src/logging.c
  src/data/button.c
  src/hardware/gpio.c
  src/hardware/hw_serial.c
  src/hardware/rotary_encoder.c
  src/hardware/segment_display.c
  src/hardware/sw_serial.c
  src/hardware/timer0.c
  src/sequencer/beat_clock.c
  src/user_interface/playback_control.c
  src/util/usec_timer.c
)
avr_target_include_directories("${PROJECT_NAME}" PRIVATE src)
add_definitions("-DF_CPU=${MCU_SPEED}")
else()
# -------------------------------- Unit tests -------------------------------- #
add_subdirectory(external/rktest)
add_executable(tests
  src/data/button.c
  tests/button_tests.c
  tests/ring_buffer_tests.c
)
target_link_libraries(tests rktest)
target_include_directories(tests PRIVATE src)
endif()
