#!/bin/bash
# Development environment setup script for Hyprland Stack3D Plugin
# This script installs and configures pre-commit hooks and development tools

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Helper functions
log_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

log_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

log_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

log_error() {
    echo -e "${RED}❌ $1${NC}"
}

log_header() {
    echo -e "${CYAN}🚀 $1${NC}"
}

# Check if we're in the project root
check_project_root() {
    if [[ ! -f "flake.nix" ]] || [[ ! -f ".pre-commit-config.yaml" ]]; then
        log_error "Please run this script from the project root directory"
        exit 1
    fi
}

# Check system requirements
check_requirements() {
    log_header "Checking system requirements..."
    
    local missing_deps=()
    
    # Check for essential tools
    command -v git >/dev/null 2>&1 || missing_deps+=("git")
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        log_error "Missing required dependencies: ${missing_deps[*]}"
        log_info "Please install the missing dependencies and run this script again"
        exit 1
    fi
    
    log_success "System requirements satisfied"
}

# Install pre-commit
install_precommit() {
    log_header "Setting up pre-commit..."
    
    # Try to install pre-commit via different methods
    if command -v nix >/dev/null 2>&1; then
        log_info "Installing pre-commit via Nix..."
        nix-env -iA nixpkgs.pre-commit
    elif command -v pip3 >/dev/null 2>&1; then
        log_info "Installing pre-commit via pip..."
        pip3 install --user pre-commit
    elif command -v pipx >/dev/null 2>&1; then
        log_info "Installing pre-commit via pipx..."
        pipx install pre-commit
    else
        log_error "Could not find pip3, pipx, or nix to install pre-commit"
        log_info "Please install pre-commit manually: https://pre-commit.com/#install"
        exit 1
    fi
    
    log_success "Pre-commit installed"
}

# Install development tools
install_dev_tools() {
    log_header "Installing development tools..."
    
    local tools_needed=("clang-format" "clang-tidy" "cppcheck" "nixpkgs-fmt")
    local missing_tools=()
    
    for tool in "${tools_needed[@]}"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_tools+=("$tool")
        fi
    done
    
    if [[ ${#missing_tools[@]} -gt 0 ]]; then
        log_info "Installing missing tools: ${missing_tools[*]}"
        
        if command -v nix >/dev/null 2>&1; then
            # Install via Nix
            for tool in "${missing_tools[@]}"; do
                case $tool in
                    "clang-format"|"clang-tidy")
                        nix-env -iA nixpkgs.clang-tools
                        ;;
                    "cppcheck")
                        nix-env -iA nixpkgs.cppcheck
                        ;;
                    "nixpkgs-fmt")
                        nix-env -iA nixpkgs.nixpkgs-fmt
                        ;;
                esac
            done
        else
            # System package manager
            if command -v apt-get >/dev/null 2>&1; then
                sudo apt-get update
                sudo apt-get install -y clang-format clang-tidy cppcheck
            elif command -v dnf >/dev/null 2>&1; then
                sudo dnf install -y clang-tools-extra cppcheck
            elif command -v pacman >/dev/null 2>&1; then
                sudo pacman -S --needed clang cppcheck
            else
                log_warning "Could not detect package manager. Please install tools manually:"
                printf "%s\n" "${missing_tools[@]}"
            fi
        fi
    fi
    
    log_success "Development tools ready"
}

# Configure pre-commit hooks
setup_precommit_hooks() {
    log_header "Configuring pre-commit hooks..."
    
    # Install the git hook scripts
    pre-commit install
    pre-commit install --hook-type commit-msg
    
    # Install hooks for all supported hook types
    pre-commit install --hook-type pre-commit
    pre-commit install --hook-type pre-merge-commit
    pre-commit install --hook-type pre-push
    pre-commit install --hook-type prepare-commit-msg
    pre-commit install --hook-type commit-msg
    pre-commit install --hook-type post-commit
    pre-commit install --hook-type post-checkout
    pre-commit install --hook-type post-merge
    pre-commit install --hook-type post-rewrite
    
    log_success "Pre-commit hooks installed"
}

# Run initial hook setup
setup_initial_run() {
    log_header "Running initial pre-commit check..."
    
    # Update hook repositories
    pre-commit autoupdate
    
    # Run hooks on all files for the first time
    log_info "This may take a few minutes on the first run..."
    if pre-commit run --all-files; then
        log_success "All pre-commit checks passed!"
    else
        log_warning "Some pre-commit checks failed. Files may have been automatically fixed."
        log_info "Review the changes and commit them if they look good."
    fi
}

# Setup git configuration
setup_git_config() {
    log_header "Configuring git settings..."
    
    # Check if git user is configured
    if ! git config user.name >/dev/null 2>&1; then
        log_warning "Git user.name not set"
        read -p "Enter your name for git commits: " git_name
        git config user.name "$git_name"
    fi
    
    if ! git config user.email >/dev/null 2>&1; then
        log_warning "Git user.email not set"
        read -p "Enter your email for git commits: " git_email
        git config user.email "$git_email"
    fi
    
    # Configure git hooks path
    git config core.hooksPath .git/hooks
    
    # Configure merge strategy
    git config merge.ours.driver true
    
    log_success "Git configuration updated"
}

# Create initial secrets baseline
setup_secrets_baseline() {
    log_header "Setting up secrets detection..."
    
    if command -v detect-secrets >/dev/null 2>&1; then
        if [[ ! -f ".secrets.baseline" ]]; then
            log_info "Creating initial secrets baseline..."
            detect-secrets scan --baseline .secrets.baseline
        else
            log_info "Updating secrets baseline..."
            detect-secrets scan --baseline .secrets.baseline --update
        fi
        log_success "Secrets detection configured"
    else
        log_warning "detect-secrets not available, skipping secrets baseline setup"
    fi
}

# Display usage information
show_usage() {
    log_header "Development Environment Setup Complete!"
    echo
    echo -e "${GREEN}🎉 Your development environment is now ready!${NC}"
    echo
    echo -e "${BLUE}Next steps:${NC}"
    echo "1. Make some changes to the code"
    echo "2. Run: git add ."
    echo "3. Run: git commit"
    echo "   → Pre-commit hooks will automatically run"
    echo
    echo -e "${BLUE}Useful commands:${NC}"
    echo "• pre-commit run --all-files    # Run all hooks on all files"
    echo "• pre-commit run <hook-name>    # Run specific hook"
    echo "• pre-commit autoupdate         # Update hook repositories"
    echo "• cz commit                     # Use commitizen for standardized commits"
    echo
    echo -e "${BLUE}Build and test:${NC}"
    echo "• nix develop --command just build  # Build the plugin"
    echo "• nix develop --command just test   # Run tests"
    echo "• nix develop --command just lint   # Run linters"
    echo
    echo -e "${BLUE}Configuration files:${NC}"
    echo "• .pre-commit-config.yaml      # Pre-commit hooks configuration"
    echo "• .clang-format                # Code formatting rules"
    echo "• .clang-tidy                  # Static analysis rules"
    echo "• .commitizen.toml             # Commit message standards"
    echo
}

# Main execution
main() {
    log_header "🔧 Setting up Hyprland Stack3D development environment"
    echo
    
    check_project_root
    check_requirements
    install_precommit
    install_dev_tools
    setup_precommit_hooks
    setup_git_config
    setup_secrets_baseline
    setup_initial_run
    
    echo
    show_usage
}

# Handle script arguments
case "${1:-}" in
    --help|-h)
        echo "Usage: $0 [OPTIONS]"
        echo
        echo "Setup development environment for Hyprland Stack3D plugin"
        echo
        echo "Options:"
        echo "  --help, -h     Show this help message"
        echo "  --force        Force reinstall of all tools"
        echo
        exit 0
        ;;
    --force)
        log_warning "Force mode: Will reinstall all tools"
        export FORCE_INSTALL=1
        ;;
esac

# Run main function
main "$@"