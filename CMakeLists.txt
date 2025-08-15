cmake_minimum_required(VERSION 3.16)
project(hyprland-stack3d VERSION 1.0.0 DESCRIPTION "3D stack sliding animations plugin for Hyprland")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

# Find required dependencies using pkg-config - minimal set for 3D animations
pkg_check_modules(deps REQUIRED IMPORTED_TARGET
    hyprland
    libdrm
    pixman-1
    wayland-server
)

# Create the plugin shared library
add_library(stack3d SHARED main.cpp)

# Set output name to match expected plugin naming
set_target_properties(stack3d PROPERTIES
    OUTPUT_NAME "stack3d"
    PREFIX ""
)

# Link against Hyprland and dependencies
target_link_libraries(stack3d PRIVATE rt PkgConfig::deps)

# Set compile flags
target_compile_options(stack3d PRIVATE -Wall -Wextra -fPIC)

# Install target
install(TARGETS stack3d
    LIBRARY DESTINATION lib
)