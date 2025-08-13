# Installation Guide

This guide covers all methods to install the Hyprland Stack3D plugin on different systems.

## 📋 Prerequisites

Before installing, ensure you have:

- **Hyprland**: Latest version installed and working
- **C++23 Compiler**: GCC 12+ or Clang 15+
- **Development Headers**: Hyprland development packages
- **Build Tools**: Make, pkg-config

## 🚀 Quick Install (NixOS)

### Method 1: NixOS System Module

Add to your NixOS configuration:

```nix
# /etc/nixos/configuration.nix
{
  inputs.hyprland-stack3d.url = "github:yourusername/hyprland-stack3d";
  
  # In your system configuration
  programs.hyprland.plugins.stack3d = {
    enable = true;
    settings = {
      transition_duration = 0.8;
      default_layout = "grid";
      transition_style = "smooth_slide";
    };
    keybindings = {
      toggle = "SUPER, grave";
      peek = "SUPER, space";
      cycle_layout = "SUPER SHIFT, grave";
    };
  };
}
```

Then rebuild your system:
```bash
sudo nixos-rebuild switch
```

### Method 2: Home Manager Module

Add to your Home Manager configuration:

```nix
# home.nix
{
  inputs.hyprland-stack3d.url = "github:yourusername/hyprland-stack3d";
  
  # In your home configuration  
  wayland.windowManager.hyprland.plugins.stack3d = {
    enable = true;
    settings = {
      enable = true;
      transition_duration = 0.8;
      transition_style = "bounce_in";
    };
  };
}
```

Apply with:
```bash
home-manager switch
```

### Method 3: Direct Nix Install

```bash
# Install directly with Nix
nix profile install github:yourusername/hyprland-stack3d

# Or run temporarily
nix run github:yourusername/hyprland-stack3d
```

## 🔧 Manual Installation

### Step 1: Clone Repository

```bash
git clone https://github.com/yourusername/hyprland-stack3d.git
cd hyprland-stack3d
```

### Step 2: Install Dependencies

**Arch Linux:**
```bash
sudo pacman -S hyprland-dev gcc make pkg-config
```

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential pkg-config libpixman-1-dev libdrm-dev
# Install hyprland development packages manually
```

**Fedora:**
```bash
sudo dnf install gcc-c++ make pkg-config pixman-devel libdrm-devel
# Install hyprland development packages manually
```

**NixOS (development environment):**
```bash
nix develop  # Enters development shell with all dependencies
```

### Step 3: Build Plugin

Choose your preferred build method:

**Option A: Using Just (Recommended)**
```bash
# Install just if not available
nix profile install nixpkgs#just  # NixOS
cargo install just               # Other systems

# Build and install
just build
just install
```

**Option B: Using Make**
```bash
make clean
make
make install
```

**Option C: Using Nix**
```bash
nix build
cp result/lib/stack3d.so ~/.config/hypr/plugins/
```

### Step 4: Configure Hyprland

Add to your `~/.config/hypr/hyprland.conf`:

```bash
# Load the plugin
plugin = ~/.config/hypr/plugins/stack3d.so

# Plugin configuration
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

### Step 5: Restart Hyprland

```bash
# Restart Hyprland to load the plugin
hyprctl dispatch exit
# Then start Hyprland again
```

## 🎯 Development Installation

For plugin development or contribution:

### NixOS Development Environment

```bash
git clone https://github.com/yourusername/hyprland-stack3d.git
cd hyprland-stack3d

# Enter development shell
nix develop

# Available development commands
just --list
```

The development environment includes:
- GCC with C++23 support
- All required dependencies
- Development tools (gdb, valgrind, cppcheck)
- Code formatting and linting tools
- Nix development utilities

### Traditional Development Setup

```bash
# Install additional development tools
sudo pacman -S gdb valgrind cppcheck uncrustify

# Clone and setup
git clone https://github.com/yourusername/hyprland-stack3d.git
cd hyprland-stack3d

# Build debug version
just debug
# Or with make
CXXFLAGS="-g -O0 -DDEBUG" make
```

## 📦 Package Managers

### AUR (Arch Linux)

```bash
# Install via AUR helper (when available)
yay -S hyprland-stack3d-git
# or
paru -S hyprland-stack3d-git
```

### Homebrew (macOS)

*Note: macOS support is experimental*

```bash
brew tap yourusername/tap
brew install hyprland-stack3d
```

## 🔍 Verification

After installation, verify the plugin is working:

### Check Plugin Load
```bash
# Check if plugin is loaded
hyprctl plugins list | grep stack3d
```

### Test Functionality
1. Open some windows in Hyprland
2. Press `SUPER + grave` to toggle 3D stack mode
3. Press `SUPER + SHIFT + grave` to cycle layouts
4. Press `SUPER + space` for temporary peek (in stack mode)

### Check Configuration
```bash
# Verify plugin configuration
hyprctl plugin stack3d status
```

## 🛠️ Build Options

### Compiler Options

```bash
# Debug build
CXXFLAGS="-g -O0 -DDEBUG" make

# Optimized build  
CXXFLAGS="-O3 -DNDEBUG" make

# With additional warnings
CXXFLAGS="-Wall -Wextra -Wpedantic" make
```

### Install Locations

```bash
# User installation (default)
make install  # Installs to ~/.config/hypr/plugins/

# Custom location
make install PREFIX=/usr/local
```

## 📋 System Requirements

### Minimum Requirements
- **OS**: Linux with Wayland
- **Hyprland**: 0.30.0+
- **Compiler**: GCC 12+ or Clang 15+
- **RAM**: 512MB available
- **CPU**: Modern x86_64 or ARM64

### Recommended Requirements
- **OS**: NixOS (for best experience)
- **Hyprland**: Latest git version
- **Compiler**: GCC 14+ or Clang 16+
- **RAM**: 2GB+ available
- **CPU**: Multi-core processor for smooth animations

## 🚨 Troubleshooting

### Common Issues

**Plugin not loading:**
```bash
# Check Hyprland logs
hyprctl logs | grep stack3d

# Verify plugin path
ls -la ~/.config/hypr/plugins/stack3d.so
```

**Compilation errors:**
```bash
# Ensure dependencies are installed
pkg-config --cflags pixman-1 libdrm hyprland

# Check compiler version
gcc --version  # Should be 12+
```

**Missing symbols:**
```bash
# Check plugin symbols
nm ~/.config/hypr/plugins/stack3d.so | grep PLUGIN
```

For more troubleshooting, see [Troubleshooting Guide](./troubleshooting.md).

## 📚 Next Steps

After successful installation:

1. **Configure the plugin**: See [Configuration Guide](./configuration.md)
2. **Learn the features**: Read [User Manual](./guides/user-manual.md)
3. **Customize layouts**: Check [Layout Types](./guides/layouts.md)
4. **Explore animations**: See [Animation Styles](./guides/animations.md)

## 📝 Notes

- The plugin is designed for Wayland/Hyprland only
- X11 is not supported
- Some features may require latest Hyprland git version
- Performance may vary based on graphics hardware

---

Need help? Check our [Troubleshooting Guide](./troubleshooting.md) or open an issue on GitHub.