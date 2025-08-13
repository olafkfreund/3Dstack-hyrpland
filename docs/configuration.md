# Configuration Reference

Complete configuration reference for the Hyprland Stack3D plugin.

## 📝 Basic Configuration

Add to your `~/.config/hypr/hyprland.conf`:

```bash
plugin {
    stack3d {
        # Core settings
        enable = true
        
        # Animation settings  
        transition_duration = 0.8
        stagger_delay = 0.05
        transition_style = "smooth_slide"
        
        # Layout settings
        stack_depth_step = 100
        spread_padding = 20
        default_layout = "grid"
        
        # Physics settings
        spring_strength = 0.8
        damping = 0.92
        
        # Visual effects
        motion_blur = true
        perspective = 800
        eye_distance = 1000
    }
}
```

## ⚙️ Configuration Options

### Core Settings

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `enable` | boolean | `true` | Enable/disable the plugin |
| `toggle_key` | string | `"SUPER, grave"` | Keybind to toggle stack/spread modes |
| `peek_key` | string | `"SUPER, space"` | Keybind for temporary peek in stack mode |
| `cycle_layout_key` | string | `"SUPER SHIFT, grave"` | Keybind to cycle through layouts |

### Animation Settings

| Option | Type | Default | Range | Description |
|--------|------|---------|-------|-------------|
| `transition_duration` | float | `0.8` | `0.1-5.0` | Animation duration in seconds |
| `stagger_delay` | float | `0.05` | `0.0-1.0` | Delay between window animations |
| `transition_style` | string | `"smooth_slide"` | See below | Animation easing style |

#### Transition Styles

| Style | Description | Visual Effect |
|-------|-------------|---------------|
| `"smooth_slide"` | Linear, clean transitions | Smooth movement |
| `"bounce_in"` | Bounce effect on arrival | Playful bounce |
| `"elastic_out"` | Elastic overshoot and settle | Spring-like motion |
| `"cascade_wave"` | Staggered wave animation | Ripple effect |
| `"spiral_motion"` | Rotating spiral transitions | Spinning movement |
| `"magnetic_attract"` | Magnetic pull effect | Snap-to-place |
| `"liquid_flow"` | Fluid, organic movement | Smooth curves |

### Layout Settings

| Option | Type | Default | Range | Description |
|--------|------|---------|-------|-------------|
| `stack_depth_step` | float | `100.0` | `50.0-500.0` | Distance between stack layers |
| `spread_padding` | float | `20.0` | `0.0-100.0` | Padding between windows in spread mode |
| `default_layout` | string | `"grid"` | See below | Default layout for spread mode |

#### Layout Types

| Layout | Description | Best For |
|--------|-------------|----------|
| `"grid"` | Optimal grid arrangement | General use, many windows |
| `"circular"` | Windows arranged in circle | Few windows, aesthetic |
| `"spiral"` | Expanding spiral pattern | Medium windows, artistic |
| `"fibonacci"` | Golden ratio-based splits | 2-8 windows, elegant |

### Physics Settings

| Option | Type | Default | Range | Description |
|--------|------|---------|-------|-------------|
| `spring_strength` | float | `0.8` | `0.1-2.0` | Physics spring force strength |
| `damping` | float | `0.92` | `0.1-1.0` | Physics damping factor |

### Visual Effects

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `motion_blur` | boolean | `true` | Enable motion blur during animations |
| `perspective` | float | `800.0` | 3D perspective strength |
| `eye_distance` | float | `1000.0` | Camera distance for 3D effect |

## 🎨 Configuration Examples

### Minimal Configuration
```bash
plugin {
    stack3d {
        enable = true
    }
}
```

### Performance Optimized
```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.4
        stagger_delay = 0.02
        motion_blur = false
        transition_style = "smooth_slide"
    }
}
```

### Aesthetic/Showy Configuration
```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 1.2
        stagger_delay = 0.08
        transition_style = "elastic_out"
        default_layout = "spiral"
        motion_blur = true
        perspective = 1000
        spring_strength = 0.6
        damping = 0.85
    }
}
```

### Productivity Focused
```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.6
        stagger_delay = 0.03
        transition_style = "smooth_slide"
        default_layout = "grid"
        stack_depth_step = 80
        spread_padding = 15
    }
}
```

### Gaming/Low Latency
```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.3
        stagger_delay = 0.01
        transition_style = "smooth_slide"
        motion_blur = false
        spring_strength = 1.0
        damping = 0.95
    }
}
```

## 🎹 Keybinding Configuration

### Default Keybindings

```bash
# These are set automatically by the plugin
bind = SUPER, grave, exec, hyprctl plugin stack3d toggle
bind = SUPER, space, exec, hyprctl plugin stack3d peek  
bind = SUPER SHIFT, grave, exec, hyprctl plugin stack3d cycle_layout
```

### Custom Keybindings

You can override the default keys:

```bash
plugin {
    stack3d {
        toggle_key = "SUPER, tab"
        peek_key = "SUPER, p"
        cycle_layout_key = "SUPER CTRL, tab"
    }
}
```

### Advanced Keybindings

```bash
# Manual control via hyprctl
bind = SUPER, 1, exec, hyprctl plugin stack3d set_layout grid
bind = SUPER, 2, exec, hyprctl plugin stack3d set_layout circular  
bind = SUPER, 3, exec, hyprctl plugin stack3d set_layout spiral
bind = SUPER, 4, exec, hyprctl plugin stack3d set_layout fibonacci

# Animation style switching
bind = SUPER ALT, 1, exec, hyprctl plugin stack3d set_style smooth_slide
bind = SUPER ALT, 2, exec, hyprctl plugin stack3d set_style bounce_in
bind = SUPER ALT, 3, exec, hyprctl plugin stack3d set_style elastic_out
```

## 🔧 Dynamic Configuration

The plugin supports runtime configuration changes:

### Via Hyprctl Commands

```bash
# Toggle plugin on/off
hyprctl plugin stack3d enable true
hyprctl plugin stack3d enable false

# Change transition duration
hyprctl plugin stack3d set transition_duration 1.0

# Change layout
hyprctl plugin stack3d set_layout circular

# Change animation style  
hyprctl plugin stack3d set_style elastic_out
```

### Via Configuration Reload

```bash
# Reload Hyprland configuration
hyprctl reload

# Plugin automatically picks up new settings
```

## 📊 Performance Tuning

### Low-End Hardware
```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.4
        stagger_delay = 0.02
        motion_blur = false
        perspective = 600
        spring_strength = 1.0
        damping = 0.95
    }
}
```

### High-End Hardware
```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 1.0
        stagger_delay = 0.06
        motion_blur = true
        perspective = 1200
        eye_distance = 1500
        spring_strength = 0.7
        damping = 0.88
    }
}
```

## 🎯 Layout-Specific Settings

### Grid Layout Optimization
```bash
plugin {
    stack3d {
        default_layout = "grid"
        spread_padding = 15        # Tight grid
        transition_style = "smooth_slide"
    }
}
```

### Circular Layout Optimization  
```bash
plugin {
    stack3d {
        default_layout = "circular"
        spread_padding = 30        # More space for circles
        transition_style = "spiral_motion"
    }
}
```

### Spiral Layout Optimization
```bash
plugin {
    stack3d {
        default_layout = "spiral"
        transition_duration = 1.0  # Slower for visual appeal
        transition_style = "elastic_out"
    }
}
```

## 🔍 Debugging Configuration

Enable debug mode for troubleshooting:

```bash
plugin {
    stack3d {
        enable = true
        debug_mode = true          # Enable debug logging
        debug_animations = true    # Show animation debug info
        debug_physics = true       # Show physics calculations
    }
}
```

Check plugin status:
```bash
hyprctl plugin stack3d status
hyprctl plugin stack3d debug_info
```

## 🌐 Multi-Monitor Configuration

The plugin works per-workspace but you can configure behavior:

```bash
plugin {
    stack3d {
        enable = true
        per_monitor = true         # Separate state per monitor
        sync_monitors = false      # Don't sync animations across monitors
    }
}
```

## ⚡ Quick Configuration Tips

### For Productivity Users
- Use `"grid"` layout with fast transitions
- Keep `motion_blur = false` for clarity
- Set `transition_duration = 0.4-0.6`

### For Content Creators  
- Use `"spiral"` or `"circular"` layouts
- Enable `motion_blur = true`
- Use `"elastic_out"` or `"bounce_in"` styles

### For Gamers
- Minimize animation duration (`0.2-0.4`)
- Disable motion blur
- Use `"grid"` layout
- Set high spring strength for snappy movement

### For Artists/Designers
- Use longer durations (`0.8-1.2`)
- Try `"fibonacci"` layout
- Use `"liquid_flow"` or `"elastic_out"` styles
- Enable all visual effects

## 📋 Configuration Validation

The plugin validates all configuration values:

```bash
# Check for configuration errors
hyprctl plugin stack3d validate_config

# View current configuration
hyprctl plugin stack3d get_config
```

Invalid values will fall back to defaults and log warnings.

## 🔄 Configuration Migration

When upgrading, the plugin handles configuration migration automatically. Check the [Migration Guide](./guides/migration.md) for version-specific changes.

---

For more advanced configuration scenarios, see [Examples](./examples/) directory.