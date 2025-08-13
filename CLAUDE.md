# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a Hyprland 3D Stack Sliding Animation Plugin project that provides smooth 3D stack animations with sliding transitions between stacked and spread layouts for the Hyprland wayland compositor.

**Current State**: Project is fully implemented with C++ source code, NixOS integration, and build automation. All components are ready for compilation and testing.

## Architecture Overview

The plugin follows a modular architecture with these core components:

- **Stack3DPlugin**: Main plugin class that manages state transitions and coordinates other systems
- **LayoutCalculator**: Handles mathematical calculations for different window layouts (grid, circular, spiral, fibonacci) and 3D transformations  
- **AnimationSystem**: Manages smooth transitions between layouts using Bezier curves and physics-based motion
- **PhysicsMotion**: Implements spring-damping physics for realistic window movement
- **BezierCurve**: Provides easing functions for different animation styles

## Development Commands

### Quick Commands (via Just)
```bash
# Build the plugin
just build

# Development workflow (build + test)
just dev

# Install plugin to user directory  
just install

# Clean build artifacts
just clean

# Run tests
just test

# Format code and run linters
just format && just lint

# Enter development shell
just setup
```

### Build Commands (via Make)
```bash
# Build the plugin
make

# Clean build artifacts
make clean

# Install plugin to user directory
make install
```

### NixOS Commands
```bash
# Enter development environment
nix develop

# Build with Nix
nix build

# Run checks and tests
nix flake check

# Update dependencies
nix flake update
```

### Project Structure
```
hyprland-stack3d/
├── flake.nix                    # Nix flake for development and packaging
├── justfile                     # Build automation and development tasks
├── Makefile                     # GNU Make build configuration
├── main.cpp                     # Plugin entry point
├── CLAUDE.md                    # Development guidance
├── README.md                    # Basic project info
├── hyprland_stack3d_plugin.md   # Complete specification
├── include/                     # Header files
│   ├── Stack3DPlugin.hpp        # Main plugin class
│   ├── LayoutCalculator.hpp     # Layout mathematics and 3D transforms
│   ├── AnimationSystem.hpp      # Animation and transition management
│   ├── PhysicsMotion.hpp        # Physics-based motion calculations
│   └── BezierCurve.hpp          # Easing curve implementations
└── src/                         # Implementation files
    ├── Stack3DPlugin.cpp        # Plugin core functionality
    ├── LayoutCalculator.cpp     # Layout calculation implementations
    ├── AnimationSystem.cpp      # Animation system implementation
    ├── PhysicsMotion.cpp        # Physics motion implementation
    └── BezierCurve.cpp          # Bezier curve mathematics
```

## Technology Stack

- **Language**: C++23
- **Build Systems**: 
  - GNU Make (traditional)
  - Nix Flakes (NixOS integration)
  - Just (development automation)
- **Dependencies**: 
  - Hyprland headers and development libraries
  - pixman-1, libdrm (via pkg-config)
  - GCC compiler with shared library support
- **Development Tools**:
  - cppcheck (static analysis)
  - uncrustify (code formatting)
  - gdb, valgrind (debugging)
  - statix, deadnix (Nix linting)

## Key Technical Concepts

### State Management
The plugin operates in three states:
- `STACKED_3D`: Windows stacked in 3D perspective
- `TRANSITIONING`: Animation in progress between states  
- `SPREAD_LAYOUT`: Windows arranged in selected layout pattern

### Layout Types
- **Grid**: Optimal grid arrangement based on window count and aspect ratio
- **Circular**: Windows arranged in circle around workspace center
- **Spiral**: Expanding spiral pattern with rotation
- **Fibonacci**: Golden ratio-based recursive splitting

### Animation Styles
- `SMOOTH_SLIDE`: Linear transitions with easing
- `BOUNCE_IN`: Bounce effect on arrival
- `ELASTIC_OUT`: Elastic overshoot and settle
- `CASCADE_WAVE`: Staggered wave animation
- Additional physics-based motion patterns

## Plugin Configuration

Configuration options are loaded from Hyprland config under `plugin:stack3d:*`:

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.8
        stagger_delay = 0.05
        transition_style = "smooth_slide"
        stack_depth_step = 100
        spread_padding = 20
        default_layout = "grid"
        spring_strength = 0.8
        damping = 0.92
        motion_blur = true
        perspective = 800
        eye_distance = 1000
    }
}
```

## Default Keybindings
- `SUPER + grave`: Toggle between 3D stack and spread layout
- `SUPER + space`: Temporary peek when in stack mode
- `SUPER SHIFT + grave`: Cycle through layout types

## Implementation Notes

When implementing the actual code:

1. **Follow the documented API exactly** - the specs in `hyprland_stack3d_plugin.md` define precise interfaces
2. **3D Math**: The Transform3D system uses perspective projection to convert 3D positions to 2D screen coordinates
3. **Physics Integration**: Spring-damping forces should be applied per-window with configurable parameters
4. **Memory Management**: Use smart pointers for plugin lifetime management, raw pointers for Hyprland window references
5. **Hook Management**: Register/unregister Hyprland event callbacks properly to avoid memory leaks
6. **Thread Safety**: Ensure animation updates and window state changes are properly synchronized

## Dependencies Installation

```bash
# Arch Linux
sudo pacman -S hyprland-headers gcc make pkg-config

# The actual package names may vary by distribution
# Ensure hyprland development headers are available
```

## NixOS Integration

### Flake Features
- **Development Shell**: Complete development environment with all dependencies
- **Package Build**: Automated compilation and packaging
- **NixOS Module**: System-wide plugin installation and configuration
- **Home Manager Module**: User-level Hyprland plugin configuration
- **Quality Checks**: Automated linting and code quality validation

### Installation on NixOS

#### System-wide (NixOS Module)
```nix
# Add to your system configuration
{
  programs.hyprland.plugins.stack3d = {
    enable = true;
    settings = {
      transition_duration = 0.8;
      default_layout = "grid";
    };
    keybindings = {
      toggle = "SUPER, grave";
      peek = "SUPER, space"; 
    };
  };
}
```

#### User-level (Home Manager)
```nix
# Add to your home.nix
{
  wayland.windowManager.hyprland.plugins.stack3d = {
    enable = true;
    settings = {
      enable = true;
      transition_style = "smooth_slide";
    };
  };
}
```

## Development Workflow

1. **Setup Environment**: `nix develop` or `just setup`
2. **Development Cycle**: `just dev` (build + test)
3. **Code Quality**: `just format && just lint` 
4. **Testing**: `just test` for basic validation
5. **Installation**: `just install` or use NixOS modules
6. **Distribution**: `just package` for release builds