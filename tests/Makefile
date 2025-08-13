# Test Makefile for Stack3D Plugin Tests

CXX := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -g -O0 -DDEBUG
INCLUDES := -I../include -I../src -I.
LDFLAGS := -pthread

# Test directories
TEST_DIR := .
UNIT_DIR := $(TEST_DIR)/unit
INTEGRATION_DIR := $(TEST_DIR)/integration
MOCKS_DIR := $(TEST_DIR)/mocks

# Source files
TEST_RUNNER := test_runner.cpp
TEST_FRAMEWORK := test_framework.hpp
MOCK_HEADERS := $(MOCKS_DIR)/hyprland_mocks.hpp

# Test executables
TEST_BINARY := test_stack3d
UNIT_TESTS := test_bezier test_physics test_layout test_animation test_plugin
INTEGRATION_TESTS := test_integration

# Default target
all: $(TEST_BINARY)

# Main test runner
$(TEST_BINARY): $(TEST_RUNNER) $(TEST_FRAMEWORK) $(MOCK_HEADERS)
	@echo "Building comprehensive test suite..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(TEST_RUNNER) $(LDFLAGS)
	@echo "Test suite built successfully!"

# Individual unit test binaries (for isolated testing)
test_bezier: $(UNIT_DIR)/test_bezier_curve.cpp $(TEST_FRAMEWORK) $(MOCK_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DUNIT_TEST_BEZIER -o $@ $< $(LDFLAGS)

test_physics: $(UNIT_DIR)/test_physics_motion.cpp $(TEST_FRAMEWORK) $(MOCK_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DUNIT_TEST_PHYSICS -o $@ $< $(LDFLAGS)

test_layout: $(UNIT_DIR)/test_layout_calculator.cpp $(TEST_FRAMEWORK) $(MOCK_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DUNIT_TEST_LAYOUT -o $@ $< $(LDFLAGS)

test_animation: $(UNIT_DIR)/test_animation_system.cpp $(TEST_FRAMEWORK) $(MOCK_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DUNIT_TEST_ANIMATION -o $@ $< $(LDFLAGS)

test_plugin: $(UNIT_DIR)/test_stack3d_plugin.cpp $(TEST_FRAMEWORK) $(MOCK_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DUNIT_TEST_PLUGIN -o $@ $< $(LDFLAGS)

test_integration: $(INTEGRATION_DIR)/test_full_plugin.cpp $(TEST_FRAMEWORK) $(MOCK_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DINTEGRATION_TEST -o $@ $< $(LDFLAGS)

# Test execution targets
.PHONY: test test-all test-unit test-integration test-bezier test-physics test-layout test-animation test-plugin

# Run all tests
test: $(TEST_BINARY)
	@echo "=== Running All Tests ==="
	./$(TEST_BINARY)

test-all: test

# Run specific test suites
test-bezier: $(TEST_BINARY)
	@echo "=== Running BezierCurve Tests ==="
	./$(TEST_BINARY) bezier

test-physics: $(TEST_BINARY)
	@echo "=== Running PhysicsMotion Tests ==="
	./$(TEST_BINARY) physics

test-layout: $(TEST_BINARY)
	@echo "=== Running LayoutCalculator Tests ==="
	./$(TEST_BINARY) layout

test-animation: $(TEST_BINARY)
	@echo "=== Running AnimationSystem Tests ==="
	./$(TEST_BINARY) animation

test-plugin: $(TEST_BINARY)
	@echo "=== Running Stack3DPlugin Tests ==="
	./$(TEST_BINARY) plugin

test-integration: $(TEST_BINARY)
	@echo "=== Running Integration Tests ==="
	./$(TEST_BINARY) integration

# Run only unit tests
test-unit: $(TEST_BINARY)
	@echo "=== Running Unit Tests ==="
	./$(TEST_BINARY) bezier
	./$(TEST_BINARY) physics
	./$(TEST_BINARY) layout
	./$(TEST_BINARY) animation
	./$(TEST_BINARY) plugin

# Test with debugging info
test-debug: CXXFLAGS += -DDEBUG_TESTS -g3
test-debug: clean $(TEST_BINARY)
	@echo "=== Running Tests (Debug Mode) ==="
	gdb --batch --ex run --ex bt --ex quit --args ./$(TEST_BINARY)

# Test with valgrind memory checking
test-memory: $(TEST_BINARY)
	@echo "=== Running Tests (Memory Check) ==="
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_BINARY)

# Test with performance profiling
test-perf: $(TEST_BINARY)
	@echo "=== Running Tests (Performance Analysis) ==="
	perf record -g ./$(TEST_BINARY)
	perf report

# Coverage testing (requires gcov)
test-coverage: CXXFLAGS += --coverage
test-coverage: LDFLAGS += --coverage
test-coverage: clean $(TEST_BINARY)
	@echo "=== Running Tests (Coverage Analysis) ==="
	./$(TEST_BINARY)
	gcov $(TEST_RUNNER)
	@echo "Coverage files generated: *.gcov"

# Quick smoke test (basic functionality)
test-smoke: $(TEST_BINARY)
	@echo "=== Running Smoke Tests ==="
	timeout 30s ./$(TEST_BINARY) bezier || echo "Bezier tests completed/timed out"
	timeout 30s ./$(TEST_BINARY) physics || echo "Physics tests completed/timed out"

# Stress test with multiple runs
test-stress: $(TEST_BINARY)
	@echo "=== Running Stress Tests ==="
	for i in {1..10}; do \
		echo "Stress test run $$i/10"; \
		./$(TEST_BINARY) || exit 1; \
	done
	@echo "All stress test runs completed successfully!"

# Clean up
clean:
	rm -f $(TEST_BINARY) $(UNIT_TESTS) $(INTEGRATION_TESTS)
	rm -f *.gcov *.gcda *.gcno
	rm -f perf.data*
	rm -f core core.*
	@echo "Cleaned test artifacts"

# Clean and rebuild
rebuild: clean all

# Help target
help:
	@echo "Available test targets:"
	@echo "  all                 - Build test suite"
	@echo "  test               - Run all tests"
	@echo "  test-bezier        - Run BezierCurve tests"
	@echo "  test-physics       - Run PhysicsMotion tests"
	@echo "  test-layout        - Run LayoutCalculator tests"
	@echo "  test-animation     - Run AnimationSystem tests"
	@echo "  test-plugin        - Run Stack3DPlugin tests"
	@echo "  test-integration   - Run integration tests"
	@echo "  test-unit          - Run all unit tests"
	@echo "  test-debug         - Run tests with debugger"
	@echo "  test-memory        - Run tests with memory checking"
	@echo "  test-coverage      - Run tests with coverage analysis"
	@echo "  test-smoke         - Run quick smoke tests"
	@echo "  test-stress        - Run stress tests (10 iterations)"
	@echo "  clean              - Clean test artifacts"
	@echo "  rebuild            - Clean and rebuild"
	@echo "  help               - Show this help"

# Dependencies
$(TEST_FRAMEWORK): $(TEST_DIR)/test_framework.hpp

$(MOCK_HEADERS): $(MOCKS_DIR)/hyprland_mocks.hpp

# Ensure directories exist
$(shell mkdir -p $(TEST_DIR) $(UNIT_DIR) $(INTEGRATION_DIR) $(MOCKS_DIR))