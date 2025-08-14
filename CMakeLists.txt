cmake_minimum_required(VERSION 3.16)
project(hyprland-stack3d VERSION 1.0.0 DESCRIPTION "3D stack sliding animations plugin for Hyprland")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

# Find required dependencies using pkg-config like official plugins
pkg_check_modules(deps REQUIRED IMPORTED_TARGET
    hyprland
    libdrm
    libinput
    libudev  
    pangocairo
    pixman-1
    wayland-server
    xkbcommon
)

# Create the plugin shared library
add_library(stack3d SHARED
    main.cpp
    src/Stack3DPlugin.cpp
    src/BezierCurve.cpp
    src/PhysicsMotion.cpp
    src/LayoutCalculator.cpp
    src/AnimationSystem.cpp
)

# Set output name to match expected plugin naming
set_target_properties(stack3d PROPERTIES
    OUTPUT_NAME "stack3d"
    PREFIX ""
)

# Link against Hyprland and dependencies (following official plugin pattern)
target_link_libraries(stack3d PRIVATE rt PkgConfig::deps)

# Set compile flags
target_compile_options(stack3d PRIVATE -Wall -Wextra -fPIC)

# Include directories
target_include_directories(stack3d PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install target
install(TARGETS stack3d
    LIBRARY DESTINATION lib
)