# Examples & Tutorials

Real-world configuration examples and tutorials for the Hyprland Stack3D plugin.

## 📚 Example Categories

### 🎯 Use Case Examples
- [**Productivity Setup**](./productivity.md) - Optimized for work and efficiency
- [**Gaming Configuration**](./gaming.md) - Low-latency, high-performance setup
- [**Creative Workflow**](./creative.md) - Beautiful animations for content creation
- [**Developer Environment**](./developer.md) - Code-focused multi-window layouts

### 🎨 Visual Configurations  
- [**Aesthetic Showcase**](./aesthetic.md) - Eye-catching visual effects
- [**Minimal Clean**](./minimal.md) - Simple, distraction-free setup
- [**Ultra Smooth**](./smooth.md) - Maximum animation quality
- [**Retro Style**](./retro.md) - Nostalgic visual effects

### 🔧 Technical Examples
- [**Multi-Monitor Setup**](./multi-monitor.md) - Complex monitor configurations
- [**Custom Keybindings**](./keybindings.md) - Advanced input configuration
- [**Integration Examples**](./integrations.md) - Working with other tools
- [**Performance Tuning**](./performance.md) - Optimization for different hardware

## 🚀 Quick Start Examples

### Basic Setup

```bash
# ~/.config/hypr/hyprland.conf
plugin = ~/.config/hypr/plugins/stack3d.so

plugin {
    stack3d {
        enable = true
        transition_duration = 0.8
        default_layout = "grid"
        transition_style = "smooth_slide"
    }
}
```

### Productivity Focused

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.4
        stagger_delay = 0.02
        transition_style = "smooth_slide"
        default_layout = "grid"
        spread_padding = 15
        motion_blur = false
    }
}
```

### Visual Appeal

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
    }
}
```

## 🎬 Animation Showcase

### Smooth Professional

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.6
        stagger_delay = 0.04
        transition_style = "smooth_slide"
        spring_strength = 0.9
        damping = 0.95
        motion_blur = true
    }
}
```

### Bouncy & Fun

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.8
        stagger_delay = 0.06
        transition_style = "bounce_in"
        spring_strength = 0.7
        damping = 0.88
        default_layout = "circular"
    }
}
```

### Liquid Smooth

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 1.0
        stagger_delay = 0.05
        transition_style = "liquid_flow"
        spring_strength = 0.5
        damping = 0.85
        motion_blur = true
        perspective = 900
    }
}
```

## 📐 Layout Examples

### Grid Master

```bash
plugin {
    stack3d {
        enable = true
        default_layout = "grid"
        spread_padding = 20
        transition_style = "smooth_slide"
        
        # Grid-optimized settings
        transition_duration = 0.5
        stagger_delay = 0.03
    }
}

# Custom keybinds for grid variations
bind = SUPER, 1, exec, hyprctl plugin stack3d set_layout grid
bind = SUPER, g, exec, hyprctl plugin stack3d toggle
```

### Circular Elegance

```bash
plugin {
    stack3d {
        enable = true
        default_layout = "circular"
        spread_padding = 35
        transition_style = "spiral_motion"
        
        # Circular-optimized settings
        transition_duration = 1.0
        perspective = 800
    }
}
```

### Golden Ratio (Fibonacci)

```bash
plugin {
    stack3d {
        enable = true
        default_layout = "fibonacci"
        spread_padding = 25
        transition_style = "elastic_out"
        
        # Fibonacci-optimized settings
        transition_duration = 0.8
        spring_strength = 0.7
        damping = 0.9
    }
}
```

## ⌨️ Keybinding Examples

### Standard Setup

```bash
plugin {
    stack3d {
        toggle_key = "SUPER, grave"
        peek_key = "SUPER, space"
        cycle_layout_key = "SUPER SHIFT, grave"
    }
}
```

### Vim-Inspired

```bash
plugin {
    stack3d {
        toggle_key = "SUPER, j"
        peek_key = "SUPER, k"
        cycle_layout_key = "SUPER, l"
    }
}

# Additional vim-style bindings
bind = SUPER, h, exec, hyprctl plugin stack3d set_layout grid
bind = SUPER SHIFT, h, exec, hyprctl plugin stack3d set_layout circular
```

### Gaming Optimized

```bash
plugin {
    stack3d {
        toggle_key = "SUPER, tab"
        peek_key = "SUPER, capslock"
        cycle_layout_key = "SUPER CTRL, tab"
    }
}

# Quick disable for gaming
bind = SUPER, F12, exec, hyprctl plugin stack3d enable false
```

## 🖥️ Multi-Monitor Examples

### Dual Monitor Setup

```bash
plugin {
    stack3d {
        enable = true
        per_monitor = true
        sync_monitors = false
        
        # Optimized for dual monitor
        transition_duration = 0.6
        spread_padding = 18
    }
}

# Monitor-specific bindings
bind = SUPER, 1, focusmonitor, DP-1
bind = SUPER, 2, focusmonitor, DP-2
bind = SUPER CTRL, grave, exec, hyprctl plugin stack3d toggle
```

### Triple Monitor Workflow

```bash
plugin {
    stack3d {
        enable = true
        per_monitor = true
        
        # Different layouts per monitor
        transition_duration = 0.5
        default_layout = "grid"
    }
}

# Monitor workflow bindings
bind = SUPER, q, focusmonitor, DP-1  # Left: Code
bind = SUPER, w, focusmonitor, DP-2  # Center: Main
bind = SUPER, e, focusmonitor, DP-3  # Right: Tools
```

## 🎮 Hardware-Specific Examples

### High-End Gaming PC

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 1.2
        stagger_delay = 0.08
        motion_blur = true
        perspective = 1200
        eye_distance = 1500
        
        # High-quality settings
        spring_strength = 0.6
        damping = 0.85
        transition_style = "liquid_flow"
    }
}
```

### Low-End/Laptop Setup

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.3
        stagger_delay = 0.02
        motion_blur = false
        perspective = 500
        
        # Performance settings
        spring_strength = 1.0
        damping = 0.95
        transition_style = "smooth_slide"
    }
}
```

### Balanced Performance

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.6
        stagger_delay = 0.04
        motion_blur = true
        perspective = 700
        
        # Balanced settings
        spring_strength = 0.8
        damping = 0.92
        transition_style = "smooth_slide"
    }
}
```

## 🔧 Integration Examples

### With Waybar Status

```bash
# ~/.config/waybar/config
{
    "custom/stack3d": {
        "exec": "hyprctl plugin stack3d status | grep -o 'STACKED\\|SPREAD'",
        "interval": 1,
        "format": "{}",
        "on-click": "hyprctl plugin stack3d toggle"
    }
}
```

### With Rofi Launcher

```bash
#!/bin/bash
# ~/.local/bin/stack3d-menu

options="Toggle Stack/Spread\nGrid Layout\nCircular Layout\nSpiral Layout\nFibonacci Layout\nSmooth Animation\nBounce Animation\nElastic Animation"

selected=$(echo -e "$options" | rofi -dmenu -p "Stack3D")

case "$selected" in
    "Toggle Stack/Spread") hyprctl plugin stack3d toggle ;;
    "Grid Layout") hyprctl plugin stack3d set_layout grid ;;
    "Circular Layout") hyprctl plugin stack3d set_layout circular ;;
    "Spiral Layout") hyprctl plugin stack3d set_layout spiral ;;
    "Fibonacci Layout") hyprctl plugin stack3d set_layout fibonacci ;;
    "Smooth Animation") hyprctl plugin stack3d set_style smooth_slide ;;
    "Bounce Animation") hyprctl plugin stack3d set_style bounce_in ;;
    "Elastic Animation") hyprctl plugin stack3d set_style elastic_out ;;
esac
```

### With Eww Widgets

```scheme
; ~/.config/eww/eww.yuck
(defwidget stack3d []
  (box :class "stack3d"
    (button :onclick "hyprctl plugin stack3d toggle"
      (label :text {stack3d_status}))
    (button :onclick "hyprctl plugin stack3d cycle_layout"
      (label :text "⚏"))))

(defpoll stack3d_status :interval "1s"
  "hyprctl plugin stack3d status | grep -o 'STACKED\\|SPREAD' | head -1")
```

## 📱 Workspace Examples

### Per-Workspace Configuration

```bash
# Different behavior per workspace
workspace = 1, animation = slide, layout = grid
workspace = 2, animation = bounce_in, layout = circular
workspace = 3, animation = elastic_out, layout = spiral

# Workspace-specific keybinds
bind = SUPER, 1, workspace, 1
bind = SUPER, 1, exec, hyprctl plugin stack3d set_layout grid

bind = SUPER, 2, workspace, 2  
bind = SUPER, 2, exec, hyprctl plugin stack3d set_layout circular
```

### Work vs Play Workspaces

```bash
# Work workspaces (1-5): Productivity focused
plugin {
    stack3d {
        enable = true
        transition_duration = 0.4
        transition_style = "smooth_slide"
        default_layout = "grid"
    }
}

# Entertainment workspaces (6-10): Visual focused
bind = SUPER, 6, exec, hyprctl plugin stack3d set_style elastic_out
bind = SUPER, 6, exec, hyprctl plugin stack3d set_layout spiral
bind = SUPER, 6, workspace, 6
```

## 🎨 Theme Integration Examples

### Nord Theme

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.7
        transition_style = "smooth_slide"
        default_layout = "grid"
        
        # Nord-inspired smooth animations
        spring_strength = 0.8
        damping = 0.92
        motion_blur = true
        perspective = 750
    }
}

# Nord color notifications
exec-once = hyprctl plugin stack3d set_notification_color "88C0D0"
```

### Dracula Theme

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.9
        transition_style = "bounce_in"
        default_layout = "spiral"
        
        # Dracula-inspired dramatic animations
        spring_strength = 0.7
        damping = 0.88
        motion_blur = true
        perspective = 900
    }
}
```

## 🔍 Debugging Examples

### Debug Configuration

```bash
plugin {
    stack3d {
        enable = true
        debug_mode = true
        debug_animations = true
        debug_physics = true
        
        # Conservative settings for debugging
        transition_duration = 1.0
        stagger_delay = 0.1
        transition_style = "smooth_slide"
    }
}
```

### Performance Monitoring

```bash
#!/bin/bash
# ~/.local/bin/stack3d-monitor

echo "Stack3D Performance Monitor"
echo "=========================="

while true; do
    echo "$(date): $(hyprctl plugin stack3d status)"
    echo "CPU: $(top -bn1 | grep "Cpu(s)" | awk '{print $2}' | cut -d'%' -f1)"
    echo "Memory: $(free -m | awk 'NR==2{printf "%.1f%%", $3*100/$2}')"
    echo "---"
    sleep 2
done
```

## 📊 Comparison Examples

### Before & After Configurations

**Before (Basic):**
```bash
plugin {
    stack3d {
        enable = true
    }
}
```

**After (Optimized):**
```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.6
        stagger_delay = 0.04
        transition_style = "smooth_slide"
        default_layout = "grid"
        spread_padding = 18
        spring_strength = 0.85
        damping = 0.93
        motion_blur = true
        perspective = 750
    }
}
```

## 🎭 Creative Examples

### Presentation Mode

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 1.5
        stagger_delay = 0.1
        transition_style = "elastic_out"
        default_layout = "circular"
        
        # Dramatic presentation effects
        motion_blur = true
        perspective = 1000
        spring_strength = 0.5
        damping = 0.8
    }
}

# Presentation keybinds
bind = SUPER, p, exec, hyprctl plugin stack3d toggle
bind = SUPER SHIFT, p, exec, hyprctl plugin stack3d peek 3.0
```

### Screen Recording Setup

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.8
        stagger_delay = 0.06
        transition_style = "liquid_flow"
        default_layout = "fibonacci"
        
        # Optimized for recording
        motion_blur = false  # Cleaner for video
        spring_strength = 0.8
        damping = 0.9
    }
}
```

## 📚 Tutorial Series

### Beginner Tutorial
1. [Basic Installation](./tutorials/01-installation.md)
2. [First Configuration](./tutorials/02-basic-config.md)
3. [Understanding Layouts](./tutorials/03-layouts.md)
4. [Animation Styles](./tutorials/04-animations.md)

### Intermediate Tutorial
1. [Performance Tuning](./tutorials/05-performance.md)
2. [Custom Keybindings](./tutorials/06-keybindings.md)
3. [Multi-Monitor Setup](./tutorials/07-multimonitor.md)
4. [Integration with Tools](./tutorials/08-integrations.md)

### Advanced Tutorial
1. [Plugin Development](./tutorials/09-development.md)
2. [Custom Layouts](./tutorials/10-custom-layouts.md)
3. [Advanced Scripting](./tutorials/11-scripting.md)
4. [Contributing](./tutorials/12-contributing.md)

---

**Want to contribute your configuration?** 
Submit a PR with your setup in the [examples directory](https://github.com/yourusername/hyprland-stack3d/tree/main/docs/examples)!