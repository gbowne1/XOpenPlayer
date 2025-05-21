cmake_minimum_required(VERSION 3.25.1)

set(IS_OS_WINDOWS FALSE)
set(IS_OS_LINUX FALSE)
set(IS_OS_MAC FALSE)

set(IS_ARCH_AMD64 FALSE)
set(IS_ARCH_X86 FALSE)
set(IS_ARCH_ARM64 FALSE)
set(IS_ARCH_ARM FALSE)

project(XOpenPlayer C)

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(IS_OS_WINDOWS TRUE)
    set(USER_HOME_DIRECTORY "$ENV{USERPROFILE}")
    string(REPLACE "\\" "/" USER_HOME_DIRECTORY "${USER_HOME_DIRECTORY}")
    message("Building for the Windows operating system.")
elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(IS_OS_LINUX TRUE)
    set(USER_HOME_DIRECTORY $ENV{HOME})
    add_compile_options(-Wno-stringop-overflow)
    message("Building for the Linux operating system.")
elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(IS_OS_MAC TRUE)
    set(USER_HOME_DIRECTORY $ENV{HOME})
    message("Building for the Mac operating system.")
endif ()

if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
    set(IS_ARCH_AMD64 TRUE)
    enable_language(ASM_NASM)
    message("Building for the AMD64 architecture.")
elseif ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64")
    set(IS_ARCH_ARM64 TRUE)
    enable_language(ASM)
    message("Building for the ARM64 architecture.")
endif ()

set(CMAKE_CXX_STANDARD 20)

add_executable(XOpenPlayer
        include/player.h src/player.c
        src/main.c
)

target_link_libraries(XOpenPlayer PRIVATE X11 m)