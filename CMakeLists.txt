cmake_minimum_required(VERSION 3.20)


#set(MCUTYPE attiny13a)
set(MCUTYPE atmega328p)
include(./AVR8ToolChain.cmake)
project(LeakageDetector)


set(CMAKE_BUILD_TYPE Release)

add_compile_definitions(F_CPU=16000000L)
add_compile_definitions(BAUD=9600)
include_directories("F:\\temp\\avr-libstdcpp-master\\include")

add_avr_exec(detector ${MCUTYPE})
target_sources(detector PRIVATE main.cpp)

#target_link_libraries(first PRIVATE GPIO_AVR  IOEXPANDER LCD  SPI_DEF )
#add_subdirectory(driver)
#add_subdirectory(Utils)
