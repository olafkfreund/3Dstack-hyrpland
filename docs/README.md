# Hyprland Stack3D Plugin Documentation

Welcome to the comprehensive documentation for the Hyprland 3D Stack Sliding Animation Plugin. This plugin brings smooth, physics-based 3D animations to window management in Hyprland.

## 📚 Documentation Overview

### Getting Started
- [Installation Guide](./installation.md) - Complete installation instructions for various systems
- [Quick Start](./quickstart.md) - Get up and running in 5 minutes
- [Configuration](./configuration.md) - Full configuration reference

### User Guides
- [User Manual](./guides/user-manual.md) - Complete guide to using the plugin
- [Layout Types](./guides/layouts.md) - Understanding Grid, Circular, Spiral, and Fibonacci layouts
- [Animation Styles](./guides/animations.md) - Overview of all 7 animation styles
- [Keybindings](./guides/keybindings.md) - Default and custom keybinding setup

### Developer Documentation
- [Architecture Overview](./architecture.md) - System design and component interactions
- [Development Guide](./guides/development.md) - Setting up development environment
- [API Reference](./api/) - Complete API documentation
- [Contributing](./guides/contributing.md) - How to contribute to the project

### Advanced Topics
- [NixOS Integration](./guides/nixos.md) - Detailed NixOS setup and module configuration
- [Performance Tuning](./guides/performance.md) - Optimization tips and settings
- [Troubleshooting](./troubleshooting.md) - Common issues and solutions
- [Migration Guide](./guides/migration.md) - Upgrading from older versions

### Examples & Tutorials
- [Configuration Examples](./examples/) - Real-world configuration examples
- [Custom Animations](./examples/custom-animations.md) - Creating custom animation styles
- [Integration Examples](./examples/integrations.md) - Integration with other tools

## 🚀 Quick Navigation

| I want to... | Go to |
|--------------|-------|
| Install the plugin | [Installation Guide](./installation.md) |
| Configure the plugin | [Configuration Reference](./configuration.md) |
| Understand how it works | [Architecture Overview](./architecture.md) |
| Set up development | [Development Guide](./guides/development.md) |
| Use with NixOS | [NixOS Integration](./guides/nixos.md) |
| Fix problems | [Troubleshooting](./troubleshooting.md) |
| See examples | [Examples Directory](./examples/) |

## 🎯 Plugin Features

### ✨ Core Features
- **3D Stack Animations**: Smooth transitions between stacked and spread window layouts
- **Multiple Layout Types**: Grid, Circular, Spiral, and Fibonacci arrangements  
- **Physics-Based Motion**: Spring-damping physics for natural window movement
- **Bezier Easing**: 7 different animation styles with customizable timing
- **Real-time Configuration**: Hot-reload configuration without restarting Hyprland

### 🎨 Animation Styles
1. **Smooth Slide** - Clean, linear transitions
2. **Bounce In** - Playful bounce effect on arrival
3. **Elastic Out** - Elastic overshoot and settle
4. **Cascade Wave** - Staggered wave animations
5. **Spiral Motion** - Rotating spiral transitions
6. **Magnetic Attract** - Magnetic pull effect
7. **Liquid Flow** - Fluid, organic movement

### 📐 Layout Types
- **Grid Layout** - Optimal grid arrangement based on window count
- **Circular Layout** - Windows arranged in a circle
- **Spiral Layout** - Expanding spiral pattern with rotation
- **Fibonacci Layout** - Golden ratio-based recursive splitting

## 🛠️ Build Systems

The plugin supports multiple build systems for different workflows:

- **GNU Make** - Traditional build system (`make`, `make install`)
- **Nix Flakes** - NixOS integration (`nix build`, `nix develop`)
- **Just** - Development automation (`just build`, `just dev`)

## 🌟 Quick Demo

```bash
# Install via NixOS (recommended)
nix develop
just build
just install

# Add to Hyprland config
echo 'plugin = ~/.config/hypr/plugins/stack3d.so' >> ~/.config/hypr/hyprland.conf

# Default keybindings
# SUPER + ` (grave) - Toggle between 3D stack and spread layout
# SUPER + SPACE - Temporary peek when in stack mode
# SUPER + SHIFT + ` - Cycle through layout types
```

## 📖 Version Information

- **Current Version**: 1.0.0
- **Hyprland API**: Compatible with latest Hyprland
- **Language**: C++23
- **License**: MIT (see LICENSE file)

## 🤝 Community & Support

- **Issues**: Report bugs and request features on GitHub
- **Discussions**: Join community discussions for help and ideas
- **Contributing**: See [Contributing Guide](./guides/contributing.md)

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](../LICENSE) file for details.

---

*Generated with ❤️ for the Hyprland community*