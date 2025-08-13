# Development Guide

Complete guide for developing and contributing to the Hyprland Stack3D plugin.

## 🛠️ Development Environment Setup

### NixOS Development (Recommended)

The project includes a comprehensive Nix flake with all dependencies:

```bash
# Clone the repository
git clone https://github.com/yourusername/hyprland-stack3d.git
cd hyprland-stack3d

# Enter development environment
nix develop

# You now have access to all development tools
just --list  # See available commands
```

The development shell includes:
- **Build Tools**: GCC 14+, Make, pkg-config
- **Dependencies**: Hyprland development headers, pixman, libdrm
- **Debug Tools**: gdb, valgrind, address sanitizer
- **Code Quality**: cppcheck, clang-tools, uncrustify
- **Nix Tools**: nixd, statix, deadnix, nix-tree
- **Development**: just, pre-commit, gitlint

### Traditional Development Setup

**Arch Linux:**
```bash
sudo pacman -S hyprland-dev gcc make pkg-config gdb valgrind cppcheck
```

**Ubuntu/Debian:**
```bash
sudo apt install build-essential pkg-config libpixman-1-dev libdrm-dev \
    gdb valgrind cppcheck uncrustify
# Install hyprland-dev from source or PPA
```

**Fedora:**
```bash
sudo dnf install gcc-c++ make pkg-config pixman-devel libdrm-devel \
    gdb valgrind cppcheck
```

## 🚀 Quick Start

### Development Workflow

```bash
# Enter development environment (NixOS)
nix develop

# Development cycle
just dev        # Build + test
just format     # Format code
just lint       # Run linters
just install    # Install locally

# Or with make
make clean
make debug      # Debug build
make test       # Run tests
```

### Project Structure

```
hyprland-stack3d/
├── include/                 # Header files (.hpp)
│   ├── Stack3DPlugin.hpp   # Main plugin interface
│   ├── LayoutCalculator.hpp # Layout mathematics
│   ├── AnimationSystem.hpp # Animation management  
│   ├── PhysicsMotion.hpp   # Physics simulation
│   └── BezierCurve.hpp     # Easing curves
├── src/                    # Implementation files (.cpp)
│   ├── Stack3DPlugin.cpp
│   ├── LayoutCalculator.cpp
│   ├── AnimationSystem.cpp
│   ├── PhysicsMotion.cpp
│   └── BezierCurve.cpp
├── main.cpp                # Plugin entry point
├── docs/                   # Documentation
├── flake.nix              # Nix development environment
├── justfile               # Development automation
├── Makefile               # Build system
└── CLAUDE.md              # AI assistant guidance
```

## 🏗️ Architecture Overview

### Component Relationships

```
┌─────────────────────────────────────────────────────────┐
│                Stack3DPlugin                            │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────────┐   │
│  │   State     │ │   Config    │ │   Event         │   │
│  │ Management  │ │ Management  │ │   Handling      │   │
│  └─────────────┘ └─────────────┘ └─────────────────┘   │
└─────────────┬───────────────┬───────────────┬─────────┘
              │               │               │
    ┌─────────▼─────────┐ ┌───▼─────────┐ ┌─▼─────────────┐
    │ LayoutCalculator  │ │ Animation   │ │ PhysicsMotion │
    │                   │ │ System      │ │               │
    │ • Grid Layout     │ │             │ │ • Spring      │
    │ • Circular        │ │ ┌─────────┐ │ │ • Damping     │
    │ • Spiral          │ │ │ Bezier  │ │ │ • Forces      │
    │ • Fibonacci       │ │ │ Curve   │ │ │               │
    │ • 3D Transform    │ │ └─────────┘ │ │               │
    └───────────────────┘ └─────────────┘ └───────────────┘
```

### Data Flow

1. **Event Trigger** → Stack3DPlugin receives input
2. **State Change** → Plugin determines new target state
3. **Layout Calculation** → LayoutCalculator computes positions
4. **Animation** → AnimationSystem interpolates between states
5. **Physics** → PhysicsMotion applies realistic motion
6. **Rendering** → Hyprland renders transformed windows

## 🔧 Build System

### Just Commands

The project uses [Just](https://github.com/casey/just) for development automation:

```bash
just build          # Standard build
just debug          # Debug build with symbols
just release        # Optimized release build
just clean          # Clean build artifacts
just install        # Install to user directory
just test           # Run tests
just format         # Format code
just lint           # Run linters
just check          # All quality checks
just dev            # Build + test workflow
just setup          # Enter development shell
just analyze        # Analyze binary
just package        # Create distribution package
```

### Make Targets

Traditional make is also supported:

```bash
make                # Build plugin
make clean          # Clean artifacts
make install        # Install to ~/.config/hypr/plugins/
make debug          # Debug build
make release        # Release build
```

### Nix Commands

For NixOS users:

```bash
nix develop         # Development environment
nix build           # Build with Nix
nix flake check     # Run all checks
nix flake update    # Update dependencies
```

## 🧪 Testing

### Running Tests

```bash
# Quick tests
just test

# Comprehensive testing
just test && just lint && just analyze
```

### Test Categories

**Unit Tests:**
- Component functionality
- Mathematical calculations
- Configuration handling

**Integration Tests:**
- Plugin initialization
- Event handling
- State transitions

**Performance Tests:**
- Animation smoothness
- Memory usage
- CPU performance

### Writing Tests

Test structure for new components:

```cpp
// tests/test_component.cpp
#include "catch2/catch.hpp"
#include "../include/Component.hpp"

TEST_CASE("Component basic functionality", "[component]") {
    Component comp;
    
    SECTION("initialization") {
        REQUIRE(comp.isValid());
    }
    
    SECTION("configuration") {
        comp.configure({});
        REQUIRE(comp.getConfig().isDefault());
    }
}
```

## 🎨 Code Style

### C++ Standards

- **Language Standard**: C++23
- **Compiler**: GCC 14+ or Clang 16+
- **Memory Management**: RAII, smart pointers
- **Error Handling**: Exceptions for logic errors, return codes for expected failures

### Naming Conventions

```cpp
// Classes: PascalCase
class Stack3DPlugin {};
class LayoutCalculator {};

// Methods and variables: camelCase
void calculateLayout();
float transitionDuration;

// Private members: m_ prefix
float m_duration;
std::unique_ptr<AnimationSystem> m_animationSystem;

// Constants: UPPER_SNAKE_CASE
const float MAX_TRANSITION_DURATION = 5.0f;
const int DEFAULT_STACK_DEPTH = 100;

// Enums: PascalCase with scoped values
enum class TransitionStyle {
    SMOOTH_SLIDE,
    BOUNCE_IN
};
```

### Code Formatting

```bash
# Format all code
just format

# Manual formatting with uncrustify
uncrustify -c .uncrustify.cfg --no-backup --replace **/*.{hpp,cpp}

# Format Nix code
nixpkgs-fmt flake.nix
```

### Include Order

```cpp
// 1. Corresponding header (for .cpp files)
#include "Stack3DPlugin.hpp"

// 2. C++ standard library
#include <memory>
#include <vector>
#include <chrono>

// 3. Third-party libraries
#include <hyprland/src/Compositor.hpp>
#include <pixman.h>

// 4. Project headers
#include "LayoutCalculator.hpp"
#include "AnimationSystem.hpp"
```

## 🔍 Debugging

### Debug Builds

```bash
# Build with debug symbols
just debug

# Or with make
CXXFLAGS="-g -O0 -DDEBUG" make
```

### Debugging Tools

**GDB:**
```bash
# Debug plugin loading
gdb --args Hyprland

# Set breakpoints
(gdb) break Stack3DPlugin::toggleState
(gdb) run
```

**Valgrind:**
```bash
# Memory leak detection
valgrind --leak-check=full --show-leak-kinds=all ./test_program
```

**Address Sanitizer:**
```bash
# Compile with ASAN
CXXFLAGS="-fsanitize=address -g" make debug
```

### Plugin-Specific Debugging

```cpp
// Debug logging (debug builds only)
#ifdef DEBUG
    std::cout << "State transition: " << oldState << " -> " << newState << std::endl;
#endif

// Hyprland logging
HyprlandAPI::addNotification(m_handle, 
    "Debug: " + debugMessage, 
    CColor{1.0f, 0.5f, 0.0f, 1.0f}, 2000);
```

## 📊 Performance Profiling

### Animation Performance

```cpp
// Timing animation frames
auto start = std::chrono::high_resolution_clock::now();
animationSystem.updateAnimation();
auto end = std::chrono::high_resolution_clock::now();

auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
std::cout << "Animation update: " << duration.count() << "μs" << std::endl;
```

### Memory Profiling

```bash
# Valgrind memory profiling
valgrind --tool=massif --detailed-freq=1 Hyprland

# Analyze results
ms_print massif.out.XXXXX
```

### CPU Profiling

```bash
# Perf profiling
perf record -g Hyprland
perf report
```

## 🔧 Advanced Development

### Custom Layout Types

Adding new layout algorithms:

```cpp
// In LayoutCalculator.hpp
enum class LayoutType {
    GRID,
    CIRCULAR,
    SPIRAL,  
    FIBONACCI,
    YOUR_NEW_LAYOUT  // Add here
};

// In LayoutCalculator.cpp
std::vector<WindowLayout> LayoutCalculator::calculateSpreadLayout(
    const std::vector<CWindow*>& windows, LayoutType layoutType) {
    
    switch (layoutType) {
        case LayoutType::YOUR_NEW_LAYOUT:
            return calculateYourNewLayout(windows);
        // ... existing cases
    }
}

// Implement your layout
std::vector<WindowLayout> LayoutCalculator::calculateYourNewLayout(
    const std::vector<CWindow*>& windows) {
    
    std::vector<WindowLayout> layouts;
    // Your layout logic here
    return layouts;
}
```

### Custom Animation Styles

Adding new animation easing:

```cpp
// In AnimationSystem.hpp
enum class TransitionStyle {
    // ... existing styles
    YOUR_NEW_STYLE
};

// In AnimationSystem.cpp
BezierCurve AnimationSystem::createEasingCurve(TransitionStyle style) {
    switch (style) {
        case TransitionStyle::YOUR_NEW_STYLE:
            return BezierCurve(0.1f, 0.7f, 0.9f, 0.3f);  // Your curve
        // ... existing cases
    }
}
```

### Plugin Extensions

Creating plugin extensions:

```cpp
// Extension interface
class Stack3DExtension {
public:
    virtual ~Stack3DExtension() = default;
    virtual void onStateChange(StackState from, StackState to) = 0;
    virtual void onWindowLayoutChanged(const std::vector<WindowLayout>& layouts) = 0;
};

// Register extensions
class Stack3DPlugin {
    std::vector<std::unique_ptr<Stack3DExtension>> m_extensions;
    
public:
    void registerExtension(std::unique_ptr<Stack3DExtension> extension) {
        m_extensions.push_back(std::move(extension));
    }
};
```

## 🤝 Contributing

### Development Process

1. **Fork & Clone**
   ```bash
   git clone https://github.com/yourusername/hyprland-stack3d.git
   cd hyprland-stack3d
   ```

2. **Setup Environment**
   ```bash
   nix develop  # or traditional setup
   ```

3. **Create Feature Branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

4. **Develop & Test**
   ```bash
   just dev     # iterative development
   just check   # quality checks
   ```

5. **Submit PR**
   - Clear description of changes
   - Include tests for new functionality
   - Update documentation as needed

### Code Review Checklist

- [ ] Code follows style guide
- [ ] All tests pass
- [ ] Documentation updated
- [ ] Performance implications considered
- [ ] Memory safety verified
- [ ] Thread safety maintained
- [ ] Backward compatibility preserved

### Commit Messages

Use conventional commit format:

```
feat: add spiral layout animation
fix: resolve memory leak in physics motion
docs: update API documentation
style: format code according to style guide
refactor: restructure animation system
test: add unit tests for bezier curves
```

## 📚 Useful Resources

### Hyprland Development
- [Hyprland Plugin API](https://hyprland.org/plugins/)
- [Hyprland Source Code](https://github.com/hyprwm/Hyprland)
- [Wayland Protocol Documentation](https://wayland.freedesktop.org/docs/html/)

### C++ Resources
- [C++23 Reference](https://cppreference.com/)
- [Modern C++ Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Real-Time C++](https://www.springer.com/gp/book/9783662629956)

### Graphics & Animation
- [3D Math Primer](http://gamemath.com/)
- [Animation Principles](https://en.wikipedia.org/wiki/12_basic_principles_of_animation)
- [Easing Functions](https://easings.net/)

### Tools Documentation
- [Just Manual](https://github.com/casey/just#readme)
- [Nix Manual](https://nixos.org/manual/nix/stable/)
- [GDB Tutorial](https://www.sourceware.org/gdb/documentation/)

## 🐛 Common Issues

### Build Issues

**Missing Dependencies:**
```bash
# Check dependencies
pkg-config --cflags --libs pixman-1 libdrm hyprland
```

**Compiler Version:**
```bash
# Verify C++23 support
gcc --version  # Should be 12+
g++ -std=c++23 -E -x c++ /dev/null >/dev/null
```

### Runtime Issues

**Plugin Not Loading:**
```bash
# Check plugin symbols
nm ~/.config/hypr/plugins/stack3d.so | grep PLUGIN
```

**Animation Issues:**
- Check frame rate with `hyprctl monitors`
- Verify GPU capabilities
- Monitor CPU usage during transitions

### Performance Issues

**High CPU Usage:**
- Reduce transition duration
- Disable motion blur
- Lower animation complexity

**Memory Leaks:**
- Run with Valgrind
- Check smart pointer usage
- Verify event hook cleanup

---

Ready to contribute? Check out [open issues](https://github.com/yourusername/hyprland-stack3d/issues) or [start a discussion](https://github.com/yourusername/hyprland-stack3d/discussions)!