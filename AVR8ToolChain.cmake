

 ########################################################
#
# CMake toolchain file for cross-platform build of C/C++ source
# for AVR8  MCUs using CNU avr-gcc and avr-g++ compilers.
#
# Created: 23/01/2024
# Author: Yapu WU
#
########################################################

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
# Specify GNU avr-gcc as AVR8 compiler
set(CMAKE_C_COMPILER   avr-gcc.exe)
set(CMAKE_CXX_COMPILER avr-g++.exe)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

find_program(AVR_OBJCOPY avr-objcopy.exe REQUIRED)
find_program(AVR_SIZE_TOOL avr-size.exe REQUIRED)
find_program(AVR_OBJDUMP avr-objdump.exe REQUIRED)
find_program(AVR_STRIP   avr-strip.exe   REQUIRED)


# Define a list of compiler flags for AVR8 platform
set(FLAGS "-Wall  -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields")


find_path(COMPILERPATH ${CMAKE_C_COMPILER})
get_filename_component(AVR_INC_PATH "${COMPILERPATH}/../avr/include" ABSOLUTE)


# Set the flags for AVR8 platform
set(CMAKE_CXX_FLAGS "${FLAGS}")
set(CMAKE_C_FLAGS "${FLAGS}" )
#add_link_options("-Wl,-gc-sections")

function(add_avr_exec execFile mcuType)
add_executable(${execFile} "")


# 
set_target_properties(${execFile} PROPERTIES OUTPUT_NAME ${execFile}.elf)

# add_custom_command(
#    TARGET ${execFile} 
#    POST_BUILD
#    COMMAND avr-strip ${execFile}.elf)

add_custom_command(
    TARGET ${execFile}
    POST_BUILD
    COMMAND ${AVR_OBJCOPY} -j .text -j .data -O ihex ${execFile}.elf ${execFile}.hex 
)

# create lst file after build
add_custom_command(
      TARGET ${execFile}
      POST_BUILD
      COMMAND
         ${AVR_OBJDUMP} -d ${execFile}.elf > ${execFile}.lst
   )

#create eeprom.hex after build
add_custom_command(
      TARGET ${execFile}
      POST_BUILD
      COMMAND
         ${AVR_OBJCOPY} -j .eeprom --set-section-flags=.eeprom=alloc,load
            --change-section-lma .eeprom=0 --no-change-warnings
            -O ihex ${execFile}.elf ${execFile}-eeprom.hex
   )
#extract FUSE settings
add_custom_command(
      TARGET ${execFile}
      POST_BUILD
      COMMAND
         ${AVR_OBJCOPY} -j .fuse -O ihex ${execFile}.elf ${execFile}.fuse
   )

#display size
 #  add_custom_command(
 #     TARGET ${execFile}
 #     POST_BUILD
 #     COMMAND
 #        ${AVR_SIZE_TOOL} -C; --mcu=${mcuType} ${execFile}.elf
 #  )
#display size
   add_custom_command(
      TARGET ${execFile}
      POST_BUILD
      COMMAND
         ${AVR_SIZE_TOOL}  ${execFile}.elf
   )
target_link_options(${execFile} PUBLIC -mmcu=${mcuType} -Wl,-Map=${execFile}.map)
target_compile_options(${execFile} PUBLIC -mmcu=${mcuType})
target_include_directories (${execFile} PUBLIC ${AVR_INC_PATH})

endfunction()

function(add_avr_library LIBRARY_NAME MCUTYPE)
add_library(${LIBRARY_NAME} STATIC "")
target_compile_options(${LIBRARY_NAME} PRIVATE "-mmcu=${MCUTYPE}")
target_link_options(${LIBRARY_NAME} PRIVATE -mmcu=${mcuType})
target_include_directories (${LIBRARY_NAME} PUBLIC ${AVR_INC_PATH})
endfunction()


function(add_avr_interface LIBRARY_NAME MCUTYPE)
add_library(${LIBRARY_NAME} INTERFACE "")
target_compile_options(${LIBRARY_NAME} INTERFACE "-mmcu=${MCUTYPE}")
target_link_options(${LIBRARY_NAME} INTERFACE "-mmcu=${MCUTYPE}")
target_include_directories (${LIBRARY_NAME} INTERFACE ${AVR_INC_PATH})
endfunction()