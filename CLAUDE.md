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

## Project Retrospective: Key Learnings

This section documents important lessons learned during the development of this Hyprland plugin project, providing valuable context for future AI development sessions.

### 1. **Build System Evolution and Complexity**

**Initial Challenge**: Started with simple GNU Make, evolved into multi-system approach
- **Lesson**: Complex projects benefit from multiple build system options
- **Solution**: Maintained GNU Make for simplicity, added Nix for reproducibility, Just for automation
- **Key Insight**: Each build system serves different use cases - don't try to force one solution

**Specific Issues Encountered**:
- Circular Nix flake dependencies in Hyprland ecosystem (hyprutils/hyprlang chains)
- Complex flake.lock corruption requiring full regeneration
- GitHub Actions compatibility issues with Nix workflows

**Best Practice**: When Nix flake dependencies break:
1. Delete flake.lock completely  
2. Run `nix flake lock` to regenerate from scratch
3. Remove problematic workflows that aren't essential
4. Keep working CI/CD pipelines that don't depend on complex Nix builds

### 2. **Testing Strategy: From Simple to Comprehensive**

**Evolution**: Basic compilation → Unit tests → Integration tests → Mock environments
- **Critical Insight**: Hyprland plugins need mock environments for headless testing
- **Solution**: Created MockHyprland class to simulate compositor environment
- **Result**: Comprehensive test coverage without requiring actual Hyprland instance

**Testing Architecture Lessons**:
- **Component Isolation**: Each class (BezierCurve, PhysicsMotion, etc.) needs independent tests
- **Integration Points**: Focus testing on component interactions, not just individual units  
- **Mock Strategy**: Mock external dependencies (Hyprland API) rather than trying to test against real compositor
- **Memory Management**: Valgrind integration caught several subtle memory issues

### 3. **GitHub Actions: Package Distribution Strategy**

**Key Learning**: Create real distribution packages, not just source bundles
- **Evolution**: Source tarball → Mock packages → Real .deb/.rpm/PKGBUILD packages
- **Critical Decision**: Packages guide users through Nix build process rather than containing pre-built binaries
- **Success Factor**: Post-install scripts provide clear guidance for completing installation

**CI/CD Architecture**:
- **Working Pattern**: Ubuntu-based package creation without Nix dependencies
- **Failed Pattern**: Complex Nix builds in GitHub Actions with flake dependency issues
- **Best Practice**: Keep CI simple - focus on packaging and distribution, not complex builds

### 4. **Code Quality and Standards Evolution**

**Tools Integration Journey**:
- Started with basic compilation
- Added cppcheck for static analysis  
- Integrated clang-format for consistent styling
- Added clang-tidy for advanced static analysis
- **Challenge**: Tool compatibility across different versions in CI environments

**Resolution Strategy**:
- Remove problematic config files (.clang-tidy, .clang-format) when compatibility issues arise
- Focus on essential quality checks (cppcheck, basic compilation warnings)
- **Key Insight**: Tool compatibility is more important than having every possible check

### 5. **C++23 Modern Features and Plugin Development**

**Language Choice Impact**:
- C++23 features improved code quality (explicit constructors, const references)
- **Challenge**: Not all build environments support C++23 fully
- **Solution**: Graceful fallback patterns and explicit feature requirements

**Plugin-Specific Patterns**:
- **Memory Management**: Smart pointers for plugin objects, raw pointers for Hyprland references
- **Event Handling**: Proper registration/cleanup of Hyprland callbacks
- **State Management**: Clear state machines for animation phases

### 6. **Documentation and Communication**

**Multi-Level Documentation Strategy**:
- `README.md`: User-facing installation and usage
- `hyprland_stack3d_plugin.md`: Complete technical specification  
- `CLAUDE.md`: AI development guidance (this file)
- Package documentation: Distribution-specific installation guides

**Key Insight**: Different audiences need different documentation depth and focus

### 7. **Release Management and Version Control**

**Successful Pattern**:
- Real semantic versioning (v1.0.0)
- Comprehensive release notes with features, installation, and checksums
- Multiple distribution package formats
- Clear installation instructions per platform

**Package Distribution Lessons**:
- **Debian**: `.deb` packages with proper control files and post-install scripts
- **Fedora**: `.rpm` packages with spec files and dependency management
- **Arch**: Complete PKGBUILD with makepkg integration
- **Universal**: Nix-based build process with clear user guidance

### 8. **NixOS Integration: Deep but Complex**

**Success Factors**:
- Comprehensive flake.nix with development shell, packages, and system modules
- Both NixOS system modules and Home Manager modules
- Quality checks integration (statix, deadnix)

**Complexity Management**:
- **Challenge**: Hyprland's flake ecosystem has complex, sometimes circular dependencies
- **Solution**: Pin to working versions, be prepared to regenerate flake.lock
- **Best Practice**: Separate "development Nix usage" from "CI/CD Nix usage"

### 9. **User Experience and Installation**

**Key Insight**: Plugin installation is complex - guide users through it
- **Strategy**: Package manager integration + clear post-install instructions
- **Success**: Post-install scripts that show exact commands to run
- **Critical**: Test installation instructions on clean systems

### 10. **AI Development Workflow Integration**

**This Project's Workflow**:
- Detailed specifications before implementation
- Comprehensive testing at each development phase
- Multiple build system support for different user preferences
- Real package distribution across major Linux distributions

**Future AI Development Recommendations**:
1. **Start with clear specifications** - document before implementing
2. **Test incrementally** - don't wait until the end to verify functionality  
3. **Plan for multiple build systems** - users have different preferences
4. **Focus on real distribution** - create packages users can actually install
5. **Document for both humans and AI** - this CLAUDE.md file is crucial for context
6. **Be prepared for dependency complexity** - especially with Nix/flake ecosystems

### 11. **Project Management and Task Tracking**

**Successful Pattern**: TodoWrite tool usage for complex multi-step tasks
- Break large tasks (like "create release") into specific, trackable steps
- Mark progress in real-time rather than batching completions
- Clear task descriptions that can be resumed if interrupted

This retrospective serves as a knowledge base for future development sessions, documenting both successful patterns and challenges encountered during this project's development.

### 12. **Hyprland Plugin Crash Resolution: Critical Debugging Session**

**Problem**: Plugin would compile successfully but crash Hyprland on load
**Root Cause Analysis**: Multiple interrelated issues discovered through systematic debugging

**Key Issues Identified**:

1. **Function Naming Convention**:
   - **Wrong**: Used `PLUGIN_API_VERSION()`, `PLUGIN_INIT()`, `PLUGIN_EXIT()` 
   - **Correct**: Hyprland expects `pluginAPIVersion()`, `pluginInit()`, `pluginExit()` (camelCase)
   - **Detection**: Symbol inspection with `nm plugin.so` revealed correct export names

2. **Header Include Paths**:
   - **Wrong**: `#include <src/plugins/PluginAPI.hpp>`
   - **Correct**: `#include <hyprland/src/plugins/PluginAPI.hpp>` for Nix environments
   - **Impact**: Compilation failures and missing symbol definitions

3. **Build System Approach**:
   - **Wrong**: Manual Makefile with hardcoded compilation flags
   - **Correct**: CMake with pkg-config following official plugin patterns
   - **Research Source**: Official Hyprland plugins repository structure analysis

**Resolution Process**:

1. **Research Phase**: Analyzed official Hyprland plugins repository for build patterns
2. **Minimal Testing**: Created ultra-minimal plugins to isolate crash causes  
3. **Symbol Analysis**: Used `nm` and `objdump` to verify correct symbol exports
4. **Build System Migration**: Switched to CMake with proper pkg-config integration
5. **Version Pinning**: Ensured exact header/runtime version matching

**Critical Research Findings**:
- Official plugins ALL use CMake with pkg-config for dependency resolution
- Function names must match exact camelCase format expected by plugin loader
- Nix development headers require `hyprland/` prefix for proper resolution
- ABI compatibility requires exact version matching between build and runtime

**Best Practice for Plugin Crashes**:
1. **Start with minimal plugin** - test basic loading before adding functionality
2. **Verify symbol exports** - use `nm plugin.so | grep plugin` to check names
3. **Follow official patterns** - research actual plugin repositories, not documentation
4. **Use CMake + pkg-config** - don't try to manually manage compilation flags
5. **Test incrementally** - add functionality only after basic loading works

**Reference Guide Created**: 
`CLAUDE-HYPRLAND-PLUGIN-DEVELOPMENT.md` - Comprehensive guide for all future Hyprland plugin development, incorporating all lessons learned from this debugging session.

**Time Investment vs. Value**: 
Extensive debugging session (multiple attempts over several iterations) that resulted in fundamental understanding of Hyprland plugin architecture. This knowledge is now documented for reuse across projects.

This debugging experience highlights the importance of:
- Following official project patterns rather than trying to create custom approaches
- Systematic troubleshooting (minimal test cases, symbol verification, incremental testing)
- Research-driven development (analyzing working examples rather than just documentation)
- Creating reusable knowledge artifacts (comprehensive guides) from complex debugging sessions

The resolution demonstrates that plugin crashes are often caused by fundamental build system or API contract issues rather than code logic problems.