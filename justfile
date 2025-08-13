# Hyprland Stack3D Plugin Build Automation
# Uses just command runner: https://github.com/casey/just

# Default recipe - show available commands
default:
    @just --list

# Build the plugin
build:
    @echo "Building Hyprland Stack3D Plugin..."
    make clean
    make

# Build with debug symbols and verbose output
debug:
    @echo "Building with debug symbols..."
    make clean
    CXXFLAGS="-shared -fPIC -g -O0 -std=c++23 -Wall -Wextra -DDEBUG" make

# Build optimized release version
release:
    @echo "Building optimized release..."
    make clean
    CXXFLAGS="-shared -fPIC -O3 -std=c++23 -Wall -Wextra -DNDEBUG" make

# Clean build artifacts
clean:
    @echo "Cleaning build artifacts..."
    make clean

# Install plugin to user directory
install: build
    @echo "Installing plugin to user directory..."
    make install

# Install to custom directory
install-to DIR: build
    @echo "Installing plugin to {{DIR}}..."
    mkdir -p "{{DIR}}"
    cp stack3d.so "{{DIR}}/"

# Run code formatting
format:
    @echo "Formatting C++ code..."
    find include src -name "*.hpp" -o -name "*.cpp" | xargs uncrustify -c .uncrustify.cfg --no-backup --replace || echo "uncrustify not configured, skipping..."
    @echo "Formatting Nix code..."
    nixpkgs-fmt flake.nix

# Run linting and static analysis
lint:
    @echo "Running cppcheck..."
    cppcheck --enable=all --std=c++23 --suppress=missingIncludeSystem include/ src/ main.cpp
    @echo "Running statix (Nix linter)..."
    statix check .
    @echo "Checking for dead Nix code..."
    deadnix --check .

# Check code quality
check: lint
    @echo "Running quality checks..."
    @echo "All checks passed!"

# Build and run comprehensive tests
test: build
    @echo "Building and running comprehensive test suite..."
    cd tests && make clean && make
    cd tests && ./test_stack3d

# Run specific test suites
test-bezier: 
    @echo "Testing BezierCurve component..."
    cd tests && make test-bezier

test-physics:
    @echo "Testing PhysicsMotion component..."
    cd tests && make test-physics

test-layout:
    @echo "Testing LayoutCalculator component..."
    cd tests && make test-layout

test-animation:
    @echo "Testing AnimationSystem component..."
    cd tests && make test-animation

test-plugin:
    @echo "Testing Stack3DPlugin component..."
    cd tests && make test-plugin

test-integration:
    @echo "Running integration tests..."
    cd tests && make test-integration

test-unit:
    @echo "Running all unit tests..."
    cd tests && make test-unit

# Test with memory checking
test-memory:
    @echo "Running tests with memory analysis..."
    cd tests && make test-memory

# Test with coverage analysis
test-coverage:
    @echo "Running tests with coverage analysis..."
    cd tests && make test-coverage

# Run quick smoke tests
test-smoke:
    @echo "Running smoke tests..."
    cd tests && make test-smoke

# Stress test the plugin
test-stress:
    @echo "Running stress tests..."
    cd tests && make test-stress

# Basic symbol and binary tests
test-basic: build
    @echo "Running basic binary tests..."
    @echo "Test: Check if plugin file exists..."
    test -f stack3d.so && echo "Plugin binary exists" || echo "Plugin binary missing"
    @echo "Test: Check if plugin has required symbols..."
    nm stack3d.so | grep -q "PLUGIN_API_VERSION" && echo "API version symbol found" || echo "API version symbol missing"
    nm stack3d.so | grep -q "PLUGIN_INIT" && echo "Plugin init symbol found" || echo "Plugin init symbol missing"
    nm stack3d.so | grep -q "PLUGIN_EXIT" && echo "Plugin exit symbol found" || echo "Plugin exit symbol missing"

# Run with Hyprland for testing (requires Hyprland to be installed)
run-hyprland: install
    @echo "Starting Hyprland with Stack3D plugin..."
    @echo "Make sure to add 'plugin = ~/.config/hypr/plugins/stack3d.so' to your Hyprland config"
    Hyprland

# Development workflow - build, test, and show status
dev: build test-basic test-unit
    @echo "Development build complete!"
    @ls -la stack3d.so

# Set up development environment
setup:
    @echo "Setting up development environment..."
    ./scripts/setup-dev.sh

# Enter Nix development shell
shell:
    @echo "Entering Nix development shell..."
    nix develop

# Run pre-commit hooks manually
precommit:
    @echo "Running pre-commit hooks..."
    pre-commit run --all-files

# Run pre-commit hooks on specific files
precommit-files *FILES:
    @echo "Running pre-commit hooks on {{FILES}}..."
    pre-commit run --files {{FILES}}

# Static analysis with clang-tidy (using config file)
analyze-static:
    @echo "Running static analysis with clang-tidy..."
    clang-tidy src/*.cpp main.cpp --config-file=.clang-tidy -- -Iinclude -std=c++23

# Security scanning with detect-secrets
security-scan:
    @echo "Running security scan..."
    detect-secrets scan --baseline .secrets.baseline --all-files

# Commit with commitizen
commit:
    @echo "Creating standardized commit..."
    cz commit

# Auto-format code with clang-format
format-code:
    @echo "Formatting C++ code with clang-format..."
    find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

# Update flake inputs
update:
    @echo "Updating flake inputs..."
    nix flake update

# Show project information
info:
    @echo "Hyprland Stack3D Plugin Project Info"
    @echo "========================================"
    @echo "Language: C++23"
    @echo "Plugin API: Hyprland Plugin API"
    @echo "Build System: GNU Make + Nix"
    @echo ""
    @echo "Project Structure:"
    @tree -I target

# Analyze dependencies
deps:
    @echo "Analyzing dependencies..."
    ldd stack3d.so 2>/dev/null || echo "Plugin not built yet"
    @echo ""
    @echo "Required packages:"
    @echo "- hyprland-dev"
    @echo "- pixman"
    @echo "- libdrm" 
    @echo "- gcc (C++23 support)"

# Check plugin size and symbols
analyze: build
    @echo "Plugin Analysis"
    @echo "=================="
    @echo "File size:"
    ls -lh stack3d.so
    @echo ""
    @echo "Dynamic symbol table:"
    nm -D stack3d.so | head -20
    @echo ""
    @echo "Section headers:"
    readelf -S stack3d.so | grep -E "(\.text|\.data|\.rodata)"

# Profile memory usage during build
profile-build:
    @echo "Profiling build process..."
    /usr/bin/time -v make 2>&1 | grep -E "(Maximum resident set size|Percent of CPU)"

# Package for all distributions (Debian, Fedora, Arch)
package:
    @echo "Building packages for all distributions..."
    ./scripts/build-packages.sh

# Package for distribution (legacy)
package-simple: release
    @echo "Creating simple distribution package..."
    mkdir -p dist/hyprland-stack3d-{{`date +%Y%m%d`}}
    cp stack3d.so dist/hyprland-stack3d-{{`date +%Y%m%d`}}/
    cp README.md dist/hyprland-stack3d-{{`date +%Y%m%d`}}/ 2>/dev/null || echo "README.md not found"
    cp hyprland_stack3d_plugin.md dist/hyprland-stack3d-{{`date +%Y%m%d`}}/
    cd dist && tar czf hyprland-stack3d-{{`date +%Y%m%d`}}.tar.gz hyprland-stack3d-{{`date +%Y%m%d`}}/
    @echo "Package created: dist/hyprland-stack3d-{{`date +%Y%m%d`}}.tar.gz"

# Clean everything including nix build artifacts
distclean: clean
    @echo "Deep clean..."
    rm -rf dist/
    rm -rf result result-*
    nix-collect-garbage

# Show git status and recent commits
status:
    @echo "Git Status"
    @echo "============="
    git status --porcelain
    @echo ""
    @echo "Recent commits:"
    git log --oneline -5

# Commit changes with conventional commit format
commit MESSAGE:
    @echo "Committing changes..."
    git add .
    git commit -m "{{MESSAGE}}"

# Run all checks before committing
pre-commit: format lint test-basic test-unit
    @echo "Pre-commit checks complete!"

# Show help for common development tasks
help:
    @echo "Hyprland Stack3D Plugin Development"
    @echo "======================================"
    @echo ""
    @echo "Build commands:"
    @echo "  just build     - Build the plugin"
    @echo "  just debug     - Build with debug symbols"  
    @echo "  just release   - Build optimized version"
    @echo ""
    @echo "Install commands:"
    @echo "  just install   - Install to ~/.config/hypr/plugins/"
    @echo "  just package   - Build all distribution packages (Debian, Fedora, Arch)"
    @echo ""
    @echo "Testing commands:"
    @echo "  just test           - Run comprehensive test suite"
    @echo "  just test-basic     - Run basic binary/symbol tests"
    @echo "  just test-unit      - Run all unit tests"
    @echo "  just test-bezier    - Test BezierCurve component"
    @echo "  just test-physics   - Test PhysicsMotion component"
    @echo "  just test-layout    - Test LayoutCalculator component"
    @echo "  just test-animation - Test AnimationSystem component"
    @echo "  just test-plugin    - Test Stack3DPlugin component"
    @echo "  just test-integration - Run integration tests"
    @echo "  just test-memory    - Test with memory analysis"
    @echo "  just test-coverage  - Test with coverage analysis"
    @echo "  just test-smoke     - Run quick smoke tests"
    @echo "  just test-stress    - Run stress tests"
    @echo "  just dev            - Full development workflow"
    @echo ""
    @echo "Quality commands:"
    @echo "  just format         - Format code (Nix + uncrustify)"
    @echo "  just format-code    - Format C++ code (clang-format)"
    @echo "  just lint           - Run linters"
    @echo "  just check          - All quality checks"
    @echo "  just precommit      - Run pre-commit hooks"
    @echo "  just analyze-static - Static analysis (clang-tidy)"
    @echo "  just security-scan  - Security scanning"
    @echo ""
    @echo "Environment:"
    @echo "  just setup     - Set up development environment"
    @echo "  just shell     - Enter Nix development shell"
    @echo "  just commit    - Create standardized commit (commitizen)"
    @echo "  just info      - Show project information"