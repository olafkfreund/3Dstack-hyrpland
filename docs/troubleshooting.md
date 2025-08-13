# Troubleshooting Guide

Common issues and solutions for the Hyprland Stack3D plugin.

## 🚨 Plugin Loading Issues

### Plugin Not Loading

**Symptoms:**
- No 3D stack functionality
- Keybindings don't work
- No plugin notifications

**Check 1: Plugin File Exists**
```bash
ls -la ~/.config/hypr/plugins/stack3d.so
```

**Solution:**
If file missing, reinstall:
```bash
just install
# or
make install
```

**Check 2: Plugin Configuration**
```bash
grep -n "plugin.*stack3d" ~/.config/hypr/hyprland.conf
```

Should show:
```
plugin = ~/.config/hypr/plugins/stack3d.so
```

**Check 3: Hyprland Logs**
```bash
hyprctl logs | grep -i stack3d
```

Look for loading messages or errors.

**Check 4: Plugin Symbols**
```bash
nm ~/.config/hypr/plugins/stack3d.so | grep PLUGIN
```

Should show:
```
T PLUGIN_API_VERSION
T PLUGIN_INIT  
T PLUGIN_EXIT
```

### Permission Errors

**Symptoms:**
- "Permission denied" when loading plugin

**Solution:**
```bash
# Fix file permissions
chmod +x ~/.config/hypr/plugins/stack3d.so

# Check plugin directory permissions
ls -la ~/.config/hypr/plugins/
```

### Architecture Mismatch

**Symptoms:**
- "Invalid ELF header" or similar errors

**Check Architecture:**
```bash
file ~/.config/hypr/plugins/stack3d.so
uname -m
```

**Solution:**
Rebuild for correct architecture:
```bash
just clean
just build
just install
```

## 🔧 Compilation Issues

### Missing Dependencies

**Symptoms:**
- "hyprland/src/Window.hpp: No such file or directory"
- "pixman.h: No such file or directory"

**Arch Linux Solution:**
```bash
sudo pacman -S hyprland-dev pixman libdrm gcc make pkg-config
```

**NixOS Solution:**
```bash
nix develop  # Automatically provides all dependencies
```

**Ubuntu/Debian Solution:**
```bash
sudo apt install build-essential pkg-config libpixman-1-dev libdrm-dev
# Install hyprland-dev manually or from source
```

### C++23 Compiler Issues

**Symptoms:**
- "error: 'std::expected' has not been declared"
- Various C++23 feature errors

**Check Compiler Version:**
```bash
gcc --version  # Should be 12.0+
g++ -std=c++23 -E -x c++ /dev/null >/dev/null
```

**Solution:**
Update compiler or use older C++ standard:
```bash
# Temporary fix - build with C++20
CXXFLAGS="-std=c++20" make
```

### PKG-Config Issues

**Symptoms:**
- "Package 'hyprland' was not found in the pkg-config search path"

**Debug PKG-Config:**
```bash
pkg-config --list-all | grep hyprland
pkg-config --cflags hyprland
echo $PKG_CONFIG_PATH
```

**Solution:**
```bash
# Find hyprland.pc file
find /usr -name "hyprland.pc" 2>/dev/null

# Add to PKG_CONFIG_PATH if needed
export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"
```

## 🎬 Animation Issues

### Animations Not Working

**Symptoms:**
- Windows snap instantly to position
- No smooth transitions

**Check 1: Plugin State**
```bash
hyprctl plugin stack3d status
```

**Check 2: Configuration**
```bash
grep -A 20 "plugin.*stack3d" ~/.config/hypr/hyprland.conf
```

Verify `enable = true` is set.

**Check 3: Hyprland Animations**
```bash
hyprctl animations
```

Ensure Hyprland animations are enabled.

**Solution:**
```bash
# Force enable in config
echo "
plugin {
    stack3d {
        enable = true
        transition_duration = 0.8
    }
}
" >> ~/.config/hypr/hyprland.conf

# Reload config
hyprctl reload
```

### Laggy/Choppy Animations

**Symptoms:**
- Stuttering during transitions
- Low frame rate animations

**Performance Optimization:**
```bash
# Add to config
plugin {
    stack3d {
        transition_duration = 0.4    # Shorter duration
        motion_blur = false          # Disable blur
        stagger_delay = 0.02         # Reduce stagger
    }
}
```

**System Check:**
```bash
# Check system performance
htop
nvidia-smi  # For NVIDIA GPUs
```

### Animation Stuck/Frozen

**Symptoms:**
- Windows frozen mid-transition
- Plugin becomes unresponsive

**Immediate Fix:**
```bash
# Reset plugin state
hyprctl plugin stack3d reset
# or restart Hyprland
hyprctl dispatch exit
```

**Configuration Fix:**
```bash
# Reduce complexity
plugin {
    stack3d {
        spring_strength = 1.0
        damping = 0.95
        transition_style = "smooth_slide"
    }
}
```

## 🎯 Layout Issues

### Windows Overlapping

**Symptoms:**
- Windows positioned on top of each other
- Incorrect spacing in layouts

**Check Window Count:**
```bash
hyprctl clients | grep -c "class:"
```

**Solution for Many Windows:**
```bash
plugin {
    stack3d {
        spread_padding = 25          # Increase padding
        default_layout = "grid"      # Use grid for many windows
    }
}
```

### Windows Off-Screen

**Symptoms:**
- Windows positioned outside monitor bounds
- Some windows not visible

**Check Monitor Setup:**
```bash
hyprctl monitors
```

**Solution:**
```bash
# Reset window positions
hyprctl plugin stack3d reset_positions

# Adjust layout for monitor size
plugin {
    stack3d {
        spread_padding = 15          # Reduce padding
        perspective = 600           # Reduce perspective
    }
}
```

### Layout Not Cycling

**Symptoms:**
- `SUPER + SHIFT + grave` doesn't change layout
- Stuck on one layout type

**Check Keybinding:**
```bash
hyprctl binds | grep stack3d
```

**Solution:**
```bash
# Manually set layout
hyprctl plugin stack3d set_layout grid
hyprctl plugin stack3d set_layout circular
hyprctl plugin stack3d set_layout spiral
hyprctl plugin stack3d set_layout fibonacci
```

## ⌨️ Keybinding Issues

### Keybindings Not Working

**Symptoms:**
- `SUPER + grave` doesn't toggle
- No response to plugin keybindings

**Check 1: Keybinding Registration**
```bash
hyprctl binds | grep -E "(grave|space)" | head -5
```

**Check 2: Key Conflicts**
Look for conflicting bindings in config.

**Solution:**
```bash
# Use different keys
plugin {
    stack3d {
        toggle_key = "SUPER, tab"
        peek_key = "SUPER, p"
        cycle_layout_key = "SUPER CTRL, tab"
    }
}
```

### Custom Keybindings Not Loading

**Symptoms:**
- Custom keybindings in config ignored
- Default keybindings still active

**Solution:**
```bash
# Reload config after changes
hyprctl reload

# Or restart Hyprland
hyprctl dispatch exit
```

## 🔧 Configuration Issues

### Config Changes Not Applied

**Symptoms:**
- Changes to plugin config don't take effect
- Plugin uses default values

**Check Config Syntax:**
```bash
# Validate config syntax
hyprctl configerrors
```

**Force Reload:**
```bash
# Reload Hyprland config
hyprctl reload

# Check if plugin loaded new config
hyprctl plugin stack3d get_config
```

### Invalid Configuration Values

**Symptoms:**
- Plugin falls back to defaults
- Warnings in logs

**Check Valid Ranges:**
```bash
# See configuration.md for valid ranges
transition_duration = 0.1-5.0
stagger_delay = 0.0-1.0
spring_strength = 0.1-2.0
damping = 0.1-1.0
```

## 🖥️ Multi-Monitor Issues

### Plugin Only Works on One Monitor

**Symptoms:**
- Stack3D only affects primary monitor
- Secondary monitors don't respond

**Solution:**
```bash
# Configure per-monitor behavior
plugin {
    stack3d {
        per_monitor = true
        sync_monitors = false
    }
}
```

### Incorrect Positioning on Multiple Monitors

**Symptoms:**
- Windows positioned on wrong monitor
- Layout calculations incorrect

**Debug:**
```bash
hyprctl monitors
hyprctl workspaces
```

**Solution:**
Focus the correct monitor before toggling:
```bash
# Focus monitor first, then toggle
hyprctl dispatch focusmonitor DP-2
# Then use SUPER + grave
```

## 💻 System-Specific Issues

### NixOS Specific Issues

**Missing Hyprland Development Files:**
```bash
# Use the flake development environment
nix develop

# Or install hyprland with dev outputs
nix profile install 'nixpkgs#hyprland.dev'
```

**Flake Issues:**
```bash
# Update flake
nix flake update

# Check flake
nix flake check

# Clean nix store
nix-collect-garbage -d
```

### Arch Linux Specific Issues

**AUR Package Issues:**
```bash
# Clean build
rm -rf ~/.cache/yay/hyprland-stack3d-git/
yay -S hyprland-stack3d-git
```

### Performance Issues on Low-End Hardware

**Symptoms:**
- High CPU usage
- System becomes sluggish during animations

**Optimization:**
```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.3
        stagger_delay = 0.01
        motion_blur = false
        perspective = 500
        spring_strength = 1.0
        damping = 0.98
        transition_style = "smooth_slide"
    }
}
```

## 🐛 Debug Mode

### Enable Debug Logging

**Add to config:**
```bash
plugin {
    stack3d {
        debug_mode = true
        debug_animations = true
        debug_physics = true
    }
}
```

**View Debug Info:**
```bash
hyprctl plugin stack3d debug_info
tail -f ~/.local/share/hypr/hyprland.log | grep stack3d
```

### Memory Debug

**Check for Memory Leaks:**
```bash
# Build with debug symbols
just debug

# Run with Valgrind
valgrind --leak-check=full --show-leak-kinds=all Hyprland
```

### Performance Profiling

**Profile Plugin Performance:**
```bash
# Time critical functions
hyprctl plugin stack3d benchmark

# System resource monitoring
htop
iotop
```

## 🔍 Advanced Debugging

### GDB Debugging

```bash
# Attach to running Hyprland
gdb -p $(pidof Hyprland)

# Set breakpoints
(gdb) break Stack3DPlugin::toggleState
(gdb) continue
```

### Core Dumps

**Enable Core Dumps:**
```bash
ulimit -c unlimited
echo "core" > /proc/sys/kernel/core_pattern
```

**Analyze Core Dump:**
```bash
gdb Hyprland core.12345
(gdb) bt  # Backtrace
(gdb) info registers
```

## 📝 Reporting Issues

When reporting issues, include:

### System Information
```bash
# System info
uname -a
hyprctl version
gcc --version

# Plugin info
ls -la ~/.config/hypr/plugins/stack3d.so
hyprctl plugin stack3d status
```

### Configuration
```bash
# Plugin config section
grep -A 20 "plugin.*stack3d" ~/.config/hypr/hyprland.conf
```

### Logs
```bash
# Recent Hyprland logs
tail -100 ~/.local/share/hypr/hyprland.log

# Plugin-specific logs
hyprctl logs | grep -i stack3d
```

### Reproduction Steps
1. Exact steps to reproduce the issue
2. Expected behavior
3. Actual behavior
4. Screenshots/videos if visual issue

## 🆘 Getting Help

### Community Support
- **GitHub Issues**: Technical bugs and feature requests
- **GitHub Discussions**: Questions and community help
- **Hyprland Discord**: General Hyprland support
- **Reddit**: r/hyprland community

### Before Asking for Help
1. Check this troubleshooting guide
2. Search existing GitHub issues
3. Try the latest version
4. Gather system information
5. Create minimal reproduction case

### Issue Template
```
**Bug Description:**
Brief description of the issue

**System Information:**
- OS: [Arch Linux/NixOS/Ubuntu/etc]
- Hyprland Version: [output of hyprctl version]
- Plugin Version: [1.0.0]
- GPU: [NVIDIA RTX 4080/AMD RX 7800 XT/Intel Arc/etc]

**Configuration:**
```bash
plugin {
    stack3d {
        // Your config here
    }
}
```

**Steps to Reproduce:**
1. Step one
2. Step two
3. Step three

**Expected Behavior:**
What should happen

**Actual Behavior:**
What actually happens

**Logs:**
```
// Relevant log output
```

**Additional Context:**
Any other relevant information
```

---

Still having issues? [Open a GitHub issue](https://github.com/yourusername/hyprland-stack3d/issues) with detailed information.