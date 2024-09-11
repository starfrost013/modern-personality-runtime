# CMakeList.txt : CMake project for dev16, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("dev16")

# Add source to this project's executable.
add_executable (dev16 "dev16.c" "macros.h" "cpu/machine.h" "cpu/8086/8086.h" "devices/bios/bios.h" "dos/dos.h" "util/util.h" "util/util.c" "util/util_console.h" "util/util_console.c" "util/logging.h" "util/logging.c" "binary/binary.h" "cmd/cmd.h" "cmd/cmd.c" "cpu/8086/8086.c" "cpu/machine.c" "dos/dos.c" "binary/binary.c" "cpu/8086/8086_util.c" "dos/dos_int21.c" "cpu/8086/8086_instructions.c" "cpu/8086/8086_instructions_modrm.c" "cpu/8086/8086_instructions_group.c" "cpu/8086/8086_instructions_interrupt.c" "cpu/8086/8086_instructions_string.c")
include_directories(".")

# set debugger directory for Visual Studio to output directory
set_property(TARGET dev16 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dev16 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.