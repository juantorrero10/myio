cmake_minimum_required(VERSION 3.15)
project(myio C)

# Use C11 and show warnings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if (MSVC)
    add_compile_options(/W4 /permissive- /Zi)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add include directory for all files
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/lib)

add_library(myio SHARED
    lib/obj.c
    lib/user_io.c
    lib/w32_io.c
    lib/free.c
    lib/format_io.c
)

# Define MYIO_EXPORTS inside the DLL so __declspec(dllexport) is active
target_compile_definitions(myio PRIVATE MYIO_EXPORTS)

# Build executable
add_executable(myio_test main.c)

# Link with Windows libs if needed
if (WIN32)
    target_link_libraries(myio_test PRIVATE kernel32)
endif()

# === Link the exe against the DLL ===
target_link_libraries(myio_test PRIVATE myio)
