# Installation and Configuration Guide

This guide provides detailed instructions for installing and configuring the Hyprland 3D Stack plugin.

## Prerequisites

- **Hyprland v0.50.1+** - Plugin requires modern Hyprland API
- **Linux system** - Currently supports Linux only
- **Wayland** - Plugin is designed for Wayland compositors

### Checking Hyprland Version

```bash
hyprctl version
```

Ensure you're running Hyprland v0.50.1 or later.

## Installation Methods

### Method 1: NixOS System-wide (Recommended for NixOS users)

This method installs the plugin system-wide and provides automatic configuration.

#### Step 1: Add Flake Input

Add to your `/etc/nixos/flake.nix`:

```nix
{
  inputs = {
    # ... your existing inputs
    hyprland-stack3d.url = "github:yourusername/3Dstack-hyrpland";
  };

  outputs = { self, nixpkgs, hyprland-stack3d, ... }: {
    nixosConfigurations.yourhostname = nixpkgs.lib.nixosSystem {
      modules = [
        # ... your existing modules
        hyprland-stack3d.nixosModules.default
        ./configuration.nix
      ];
    };
  };
}
```

#### Step 2: Configure in NixOS Configuration

Add to your `/etc/nixos/configuration.nix`:

```nix
{
  # Enable Hyprland with Stack3D plugin
  programs.hyprland.stack3d = {
    enable = true;
    
    # Optional: Custom settings
    settings = {
      enabled = true;
      transition_duration = 0.8;
      default_layout = "grid";
      motion_blur = true;
    };
    
    # Optional: Custom keybindings
    keybindings = {
      toggle = "SUPER, grave";
      cycle = "SUPER, TAB";
      peek = "SUPER, space";
    };
  };
}
```

#### Step 3: Rebuild and Configure

```bash
# Rebuild your system
sudo nixos-rebuild switch

# The plugin configuration is automatically placed at /etc/hypr/stack3d.conf
# Include it in your Hyprland config:
echo "source = /etc/hypr/stack3d.conf" >> ~/.config/hypr/hyprland.conf
```

### Method 2: Home Manager (Recommended for user-level)

This method installs the plugin per-user using Home Manager.

#### Step 1: Add to Home Manager Flake

Add to your `flake.nix`:

```nix
{
  inputs = {
    # ... your existing inputs
    hyprland-stack3d.url = "github:yourusername/3Dstack-hyrpland";
  };

  outputs = { self, nixpkgs, home-manager, hyprland-stack3d, ... }: {
    homeConfigurations.yourusername = home-manager.lib.homeManagerConfiguration {
      modules = [
        # ... your existing modules
        hyprland-stack3d.homeManagerModules.default
        ./home.nix
      ];
    };
  };
}
```

#### Step 2: Configure in Home Manager

Add to your `home.nix`:

```nix
{
  # Enable Hyprland with Stack3D plugin
  wayland.windowManager.hyprland = {
    enable = true;
    
    stack3d = {
      enable = true;
      
      # Plugin settings
      settings = {
        enabled = true;
        transition_duration = 0.8;
        default_layout = "grid";
      };
      
      # Keybindings
      keybindings = {
        toggle = "SUPER, grave";
        cycle = "SUPER, TAB";
        peek = "SUPER, space";
      };
    };
  };
}
```

#### Step 3: Apply Configuration

```bash
# Apply Home Manager configuration
home-manager switch
```

### Method 3: Manual Nix Build

For users who want to build manually without system integration.

#### Step 1: Clone and Build

```bash
# Clone the repository
git clone https://github.com/yourusername/3Dstack-hyrpland.git
cd 3Dstack-hyrpland

# Build with Nix
nix build

# The plugin will be available at ./result/lib/libhyprland-stack3d.so
```

#### Step 2: Load Plugin

```bash
# Check your Hyprland instance
hyprctl instances

# Set the correct instance signature
export HYPRLAND_INSTANCE_SIGNATURE=your_instance_signature

# Load the plugin
hyprctl plugin load ./result/lib/libhyprland-stack3d.so
```

#### Step 3: Configure Manually

Add to your `~/.config/hypr/hyprland.conf`:

```bash
# Plugin configuration
plugin {
    stack3d {
        enabled = true
        transition_duration = 0.8
        default_layout = "grid"
    }
}

# Keybindings
bind = SUPER, grave, exec, hyprctl dispatch stack3d toggle
bind = SUPER, TAB, exec, hyprctl dispatch stack3d cycle
bind = SUPER, space, exec, hyprctl dispatch stack3d peek
```

### Method 4: Traditional Build (Non-Nix systems)

For users not using Nix/NixOS.

#### Step 1: Install Dependencies

**Arch Linux:**
```bash
sudo pacman -S hyprland cmake gcc pkg-config git
```

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install hyprland-dev cmake gcc pkg-config git libwayland-dev libpixman-1-dev libdrm-dev
```

**Fedora:**
```bash
sudo dnf install hyprland-devel cmake gcc pkg-config git wayland-devel pixman-devel libdrm-devel
```

#### Step 2: Build Plugin

```bash
# Clone repository
git clone https://github.com/yourusername/3Dstack-hyrpland.git
cd 3Dstack-hyrpland

# Build with CMake
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# The plugin will be at ./libhyprland-stack3d.so
```

#### Step 3: Install and Load

```bash
# Copy to plugins directory
mkdir -p ~/.config/hypr/plugins
cp libhyprland-stack3d.so ~/.config/hypr/plugins/

# Load in Hyprland config
echo "plugin = ~/.config/hypr/plugins/libhyprland-stack3d.so" >> ~/.config/hypr/hyprland.conf
```

## Configuration Reference

### Complete Configuration Example

```bash
plugin {
    stack3d {
        # Core functionality
        enabled = true                    # Enable/disable plugin
        transition_duration = 0.8         # Animation duration (seconds)
        stagger_delay = 0.05             # Delay between animations
        
        # Visual effects
        transition_style = 0             # 0=smooth_slide, 1=bounce_in, 2=elastic_out
        stack_depth_step = 100.0         # Z-depth between layers
        spread_padding = 20.0            # Padding around windows
        motion_blur = 1                  # Enable motion blur (1=true, 0=false)
        
        # 3D perspective
        perspective = 800.0              # Perspective distance
        eye_distance = 1000.0            # Camera distance
        
        # Layout options
        default_layout = 0               # 0=grid, 1=circular, 2=spiral, 3=fibonacci
        
        # Physics (future features)
        spring_strength = 0.8            # Spring animation strength
        damping = 0.92                   # Motion damping factor
    }
}

# Recommended keybindings
bind = SUPER, grave, exec, hyprctl dispatch stack3d toggle
bind = SUPER, TAB, exec, hyprctl dispatch stack3d cycle
bind = SUPER, space, exec, hyprctl dispatch stack3d peek

# Alternative bindings
bind = ALT, TAB, exec, hyprctl dispatch stack3d cycle
bind = SUPER_SHIFT, grave, exec, hyprctl dispatch stack3d toggle
```

### Configuration Presets

#### Productivity (Fast & Clean)
```bash
plugin {
    stack3d {
        enabled = true
        transition_duration = 0.4        # Fast transitions
        transition_style = 0             # Smooth slide
        motion_blur = 0                  # Disabled for performance
        default_layout = 0               # Grid layout
    }
}
```

#### Aesthetic (Beautiful & Smooth)
```bash
plugin {
    stack3d {
        enabled = true
        transition_duration = 1.2        # Slower, more dramatic
        transition_style = 2             # Elastic out
        motion_blur = 1                  # Enabled for beauty
        perspective = 1000               # Enhanced 3D effect
        default_layout = 2               # Spiral layout
    }
}
```

#### Gaming (Ultra Fast)
```bash
plugin {
    stack3d {
        enabled = true
        transition_duration = 0.3        # Minimal delay
        transition_style = 0             # Simple transitions
        motion_blur = 0                  # Performance focus
        spring_strength = 1.5            # Snappy animations
    }
}
```

## Troubleshooting

### Plugin Won't Load

**Issue:** `Plugin could not be loaded`

**Solutions:**
1. Check Hyprland version compatibility:
   ```bash
   hyprctl version
   ```

2. Verify plugin file exists and has correct permissions:
   ```bash
   ls -la /path/to/libhyprland-stack3d.so
   ```

3. Check Hyprland instance signature:
   ```bash
   hyprctl instances
   export HYPRLAND_INSTANCE_SIGNATURE=your_signature
   ```

### Dispatcher Not Found

**Issue:** `Invalid dispatcher`

**Solutions:**
1. Verify plugin is loaded:
   ```bash
   hyprctl plugin list
   ```

2. Check for "Hyprland 3D Stack" in the output.

3. If not listed, the plugin failed to initialize. Check Hyprland logs.

### Performance Issues

**Issue:** Stuttering or lag during animations

**Solutions:**
1. Reduce animation duration:
   ```bash
   transition_duration = 0.4
   ```

2. Disable motion blur:
   ```bash
   motion_blur = 0
   ```

3. Use faster transition style:
   ```bash
   transition_style = 0  # smooth_slide
   ```

### Window Positioning Issues

**Issue:** Windows don't return to original positions

**Solutions:**
1. Toggle stack mode off and on:
   ```bash
   hyprctl dispatch stack3d toggle
   hyprctl dispatch stack3d toggle
   ```

2. Check for conflicting window management plugins.

3. Restart Hyprland if issues persist.

## Advanced Configuration

### Custom Keybindings

You can create more complex keybinding schemes:

```bash
# Quick toggle with visual feedback
bind = SUPER, grave, exec, hyprctl dispatch stack3d toggle && notify-send "Stack3D" "Toggled"

# Cycle with counter
bind = SUPER, TAB, exec, hyprctl dispatch stack3d cycle && hyprctl notify 1 1000 "rgb(ff0000)" "Cycled"

# Mode-aware binding
bind = SUPER, space, exec, hyprctl dispatch stack3d peek || hyprctl dispatch stack3d toggle
```

### Integration with Other Tools

#### With Waybar
```json
{
    "custom/stack3d": {
        "format": "{}",
        "exec": "hyprctl plugin list | grep -q 'Stack3D' && echo '📚' || echo '📖'",
        "interval": 5,
        "on-click": "hyprctl dispatch stack3d toggle"
    }
}
```

#### With Rofi/Wofi
Create a script for mode selection:
```bash
#!/bin/bash
mode=$(echo -e "Toggle\nCycle\nPeek" | rofi -dmenu -p "Stack3D:")
case $mode in
    "Toggle") hyprctl dispatch stack3d toggle ;;
    "Cycle") hyprctl dispatch stack3d cycle ;;
    "Peek") hyprctl dispatch stack3d peek ;;
esac
```

## Updating

### NixOS/Home Manager
```bash
# Update flake inputs
nix flake update

# Rebuild
sudo nixos-rebuild switch  # or home-manager switch
```

### Manual Installation
```bash
cd 3Dstack-hyrpland
git pull
nix build  # or make clean && make
hyprctl plugin load ./result/lib/libhyprland-stack3d.so
```

## Uninstallation

### NixOS/Home Manager
Remove the plugin configuration from your Nix files and rebuild.

### Manual Installation
```bash
# Unload plugin
hyprctl plugin unload libhyprland-stack3d.so

# Remove files
rm ~/.config/hypr/plugins/libhyprland-stack3d.so

# Remove configuration from hyprland.conf
```

## Support

If you encounter issues:

1. **Check the troubleshooting section** above
2. **Review Hyprland logs** for error messages
3. **Open an issue** on GitHub with:
   - Your Hyprland version (`hyprctl version`)
   - Your system information (NixOS version, etc.)
   - Plugin configuration
   - Error messages or unexpected behavior
4. **Join discussions** on GitHub Discussions for community help

## Contributing

See the main README.md for development setup and contribution guidelines.