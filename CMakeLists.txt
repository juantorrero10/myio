cmake_minimum_required(VERSION 3.15)
project(myio C)

# Use C11 and show warnings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add include directory for all files
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/lib)

# List your sources
set(SOURCES
    main.c
    lib/obj.c
    lib/user_io.c
    lib/w32_io.c
)

# Build executable
add_executable(myio_test ${SOURCES})

# Link with Windows libs if needed
if (WIN32)
    target_link_libraries(myio_test PRIVATE kernel32)
endif()
