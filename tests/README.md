# Stack3D Plugin Test Suite

Comprehensive test suite for the Hyprland Stack3D plugin with unit tests, integration tests, and performance analysis.

## 🧪 Test Structure

```
tests/
├── test_framework.hpp        # Custom testing framework
├── test_runner.cpp           # Main test runner
├── Makefile                  # Test build system
├── unit/                     # Unit tests for individual components
│   ├── test_bezier_curve.cpp
│   ├── test_physics_motion.cpp
│   ├── test_layout_calculator.cpp
│   ├── test_animation_system.cpp
│   └── test_stack3d_plugin.cpp
├── integration/              # Integration tests
│   └── test_full_plugin.cpp
└── mocks/                    # Mock implementations
    └── hyprland_mocks.hpp
```

## 🚀 Running Tests

### Quick Start

```bash
# Run all tests
just test

# Run specific test suites
just test-unit
just test-integration
just test-bezier
just test-physics
just test-layout
just test-animation
just test-plugin
```

### Advanced Testing

```bash
# Memory analysis
just test-memory

# Coverage analysis
just test-coverage

# Quick smoke tests
just test-smoke

# Stress testing
just test-stress

# Basic binary validation
just test-basic
```

### Manual Test Execution

```bash
# Build and run tests manually
cd tests
make clean
make
./test_stack3d

# Run specific test suite
./test_stack3d bezier
./test_stack3d physics
./test_stack3d layout
./test_stack3d animation
./test_stack3d plugin
./test_stack3d integration
```

## 🧩 Test Components

### Unit Tests

**BezierCurve Tests** (`test_bezier_curve.cpp`)
- Basic Bezier evaluation
- Easing curve presets (ease-in, ease-out, ease-in-out, elastic, bounce)
- Monotonicity validation
- Performance benchmarks
- Edge case handling

**PhysicsMotion Tests** (`test_physics_motion.cpp`)
- Spring-damper system dynamics
- Critical, overdamped, and underdamped behaviors
- Force calculations (spring, damping, gravity, repulsion)
- Energy conservation validation
- Stability testing with various parameters

**LayoutCalculator Tests** (`test_layout_calculator.cpp`)
- All layout algorithms (Grid, Circular, Spiral, Fibonacci)
- 3D perspective transformations
- Bounds checking and padding effects
- Multi-window scaling
- Layout consistency validation

**AnimationSystem Tests** (`test_animation_system.cpp`)
- Animation timeline management
- Property interpolation (position, size, rotation, opacity)
- Staggered animations
- Animation interruption and completion
- Performance with many windows

**Stack3DPlugin Tests** (`test_stack3d_plugin.cpp`)
- Plugin initialization and lifecycle
- State transitions (Stacked ↔ Spread)
- Configuration updates and validation
- Window management (add/remove)
- Multi-workspace and multi-monitor scenarios

### Integration Tests

**Full Plugin Integration** (`test_full_plugin.cpp`)
- Complete workflow testing
- Dynamic window management
- Multi-workspace scenarios
- Monitor switching
- Configuration hot-reloading
- Stress testing with many windows
- Error recovery validation

### Test Framework

**Custom Framework** (`test_framework.hpp`)
- Lightweight test runner
- Rich assertion library (`ASSERT_EQ`, `ASSERT_NEAR`, `ASSERT_TRUE`, etc.)
- Performance timing
- Exception handling
- Test isolation

**Mock System** (`hyprland_mocks.hpp`)
- Complete Hyprland API mocking
- Window, workspace, and monitor simulation
- Event system simulation
- Configuration management
- Memory-safe cleanup

## 📊 Test Categories

### Functional Tests
- ✅ Algorithm correctness
- ✅ State machine validation
- ✅ Configuration handling
- ✅ Event processing

### Performance Tests
- ⚡ Animation frame rates
- ⚡ Memory usage patterns
- ⚡ CPU utilization
- ⚡ Scalability with window count

### Reliability Tests
- 🛡️ Error handling
- 🛡️ Memory leak detection
- 🛡️ Thread safety
- 🛡️ Resource cleanup

### Integration Tests
- 🔗 Component interaction
- 🔗 Full workflow validation
- 🔗 Cross-platform compatibility
- 🔗 Real-world scenarios

## 🔧 Build System Integration

### Makefile Targets

```bash
make                    # Build test suite
make test              # Run all tests
make test-bezier       # Run specific component tests
make test-memory       # Run with Valgrind
make test-coverage     # Generate coverage report
make test-smoke        # Quick validation
make test-stress       # Stress testing
make clean             # Clean artifacts
```

### Nix Integration

Tests are integrated into the Nix build system:

```bash
nix build              # Builds and runs tests
nix develop            # Development shell with test tools
```

### CI/CD Integration

GitHub Actions workflow (`.github/workflows/tests.yml`) runs:
- Unit tests on every push
- Integration tests on pull requests
- Memory leak detection
- Coverage analysis
- Cross-platform validation

## 📈 Coverage Goals

| Component | Target Coverage | Current Status |
|-----------|----------------|----------------|
| BezierCurve | 95% | ✅ Complete |
| PhysicsMotion | 90% | ✅ Complete |
| LayoutCalculator | 85% | ✅ Complete |
| AnimationSystem | 90% | ✅ Complete |
| Stack3DPlugin | 80% | ✅ Complete |
| Integration | 70% | ✅ Complete |

## 🐛 Debugging Tests

### Debug Build
```bash
make test-debug        # Build with debug symbols
gdb ./test_stack3d     # Debug with GDB
```

### Memory Analysis
```bash
make test-memory       # Run with Valgrind
valgrind --tool=massif ./test_stack3d  # Memory profiling
```

### Performance Profiling
```bash
make test-perf         # Profile with perf
perf record -g ./test_stack3d
perf report
```

## 📝 Writing New Tests

### Adding Unit Tests

1. Create test file in `unit/` directory
2. Include test framework: `#include "../test_framework.hpp"`
3. Include mocks: `#include "../mocks/hyprland_mocks.hpp"`
4. Create test namespace and functions
5. Add to test runner

Example:
```cpp
#include "../test_framework.hpp"
#include "../mocks/hyprland_mocks.hpp"
#include "../../include/MyComponent.hpp"

namespace MyComponentTests {

void testBasicFunctionality() {
    MyComponent component;
    ASSERT_EQ(component.getValue(), 42, "Initial value should be 42");
}

void runAllTests() {
    TestSuite suite("MyComponent");
    suite.addTest("Basic Functionality", testBasicFunctionality);
    suite.run();
}

} // namespace MyComponentTests
```

### Best Practices

- **Isolate tests**: Each test should be independent
- **Use descriptive names**: Clear test and assertion messages
- **Test edge cases**: Boundary conditions, null inputs, invalid states
- **Mock external dependencies**: Use Hyprland mocks for consistent testing
- **Performance awareness**: Include timing-sensitive tests where relevant
- **Cleanup resources**: Prevent memory leaks in test code

## 🎯 Test Scenarios

### Real-World Scenarios
- Opening/closing many windows rapidly
- Switching between workspaces during animations
- Changing monitor configurations
- Network interruptions (for distributed testing)
- System resource constraints

### Edge Cases
- Zero windows in workspace
- Single window scenarios
- Very large number of windows (50+)
- Extremely small/large window sizes
- Rapid state changes
- Invalid configuration values

### Error Conditions
- Memory allocation failures
- Null pointer dereference
- Configuration file corruption
- Plugin initialization failures
- Animation system overload

---

This test suite ensures the Stack3D plugin is robust, performant, and reliable across all supported use cases and environments.