# Productivity Configuration Examples

Optimized configurations for work, development, and productivity-focused workflows.

## 🎯 Core Productivity Setup

### Fast & Efficient

Optimized for quick window management with minimal visual distraction:

```bash
# ~/.config/hypr/hyprland.conf
plugin {
    stack3d {
        enable = true
        
        # Fast transitions
        transition_duration = 0.4
        stagger_delay = 0.02
        
        # Clean, professional animation
        transition_style = "smooth_slide"
        motion_blur = false
        
        # Grid layout for maximum screen usage
        default_layout = "grid"
        spread_padding = 12
        
        # Snappy physics
        spring_strength = 1.0
        damping = 0.95
        
        # Productivity keybinds
        toggle_key = "SUPER, tab"
        peek_key = "SUPER, space"
        cycle_layout_key = "SUPER SHIFT, tab"
    }
}

# Additional productivity bindings
bind = SUPER, g, exec, hyprctl plugin stack3d set_layout grid
bind = SUPER CTRL, r, exec, hyprctl reload  # Quick config reload
```

### Office Professional

Balanced between speed and visual polish for office environments:

```bash
plugin {
    stack3d {
        enable = true
        
        # Professional timing
        transition_duration = 0.6
        stagger_delay = 0.03
        
        # Smooth, refined animation
        transition_style = "smooth_slide" 
        motion_blur = true
        
        # Grid with comfortable spacing
        default_layout = "grid"
        spread_padding = 20
        
        # Balanced physics
        spring_strength = 0.85
        damping = 0.92
        
        # 3D depth for visual hierarchy
        stack_depth_step = 80
        perspective = 700
        eye_distance = 900
    }
}
```

## 💻 Development Workflows

### Code Editor Focus

Optimized for working with multiple code windows:

```bash
plugin {
    stack3d {
        enable = true
        
        # Quick switching between files
        transition_duration = 0.3
        stagger_delay = 0.01
        
        # Minimal animation for focus
        transition_style = "smooth_slide"
        motion_blur = false
        
        # Grid layout for code windows
        default_layout = "grid"
        spread_padding = 15
        
        # Very responsive
        spring_strength = 1.2  
        damping = 0.98
        
        # IDE-friendly keybinds
        toggle_key = "SUPER, grave"
        peek_key = "SUPER, p"  # 'p' for preview
    }
}

# Development-specific bindings
bind = SUPER, e, exec, hyprctl plugin stack3d toggle  # 'e' for editor
bind = SUPER SHIFT, e, exec, hyprctl plugin stack3d peek 0.5

# Terminal integration
bind = SUPER, return, exec, alacritty
bind = SUPER SHIFT, return, exec, alacritty --class floating
```

### Multi-Monitor Development

Setup for developers with multiple monitors:

```bash
plugin {
    stack3d {
        enable = true
        per_monitor = true
        sync_monitors = false
        
        # Different behavior per monitor
        transition_duration = 0.5
        default_layout = "grid"
        spread_padding = 18
        
        # Optimized for coding
        motion_blur = false
        perspective = 600
    }
}

# Monitor-specific workflows
bind = SUPER, 1, focusmonitor, DP-1   # Primary: Editor
bind = SUPER, 2, focusmonitor, DP-2   # Secondary: Browser/Docs  
bind = SUPER, 3, focusmonitor, DP-3   # Tertiary: Terminal/Tools

# Context switching
bind = SUPER CTRL, 1, exec, hyprctl plugin stack3d set_layout grid      # Code windows
bind = SUPER CTRL, 2, exec, hyprctl plugin stack3d set_layout circular  # Research
bind = SUPER CTRL, 3, exec, hyprctl plugin stack3d set_layout fibonacci # Tools
```

### Terminal-Heavy Workflow

For users who work primarily in terminals:

```bash
plugin {
    stack3d {
        enable = true
        
        # Very fast for terminal switching
        transition_duration = 0.25
        stagger_delay = 0.01
        
        # Minimal visual effects
        transition_style = "smooth_slide"
        motion_blur = false
        
        # Grid for terminal windows
        default_layout = "grid" 
        spread_padding = 10
        
        # Instant response
        spring_strength = 1.5
        damping = 0.99
        
        # Vi-style keybinds
        toggle_key = "SUPER, j"
        peek_key = "SUPER, k"
        cycle_layout_key = "SUPER, l"
    }
}

# Terminal workflow bindings
bind = SUPER, h, exec, hyprctl plugin stack3d toggle
bind = SUPER SHIFT, h, focuswindow, ^(Alacritty)$

# Tmux-like session switching
bind = SUPER, 1, workspace, 1  # Main development
bind = SUPER, 2, workspace, 2  # Testing/debugging
bind = SUPER, 3, workspace, 3  # System monitoring
```

## 📊 Data Analysis Setup

### Research & Analysis

Configuration for data scientists and researchers:

```bash
plugin {
    stack3d {
        enable = true
        
        # Thoughtful transitions for data review
        transition_duration = 0.7
        stagger_delay = 0.04
        
        # Smooth for comparing data
        transition_style = "smooth_slide"
        motion_blur = true
        
        # Fibonacci for natural grouping
        default_layout = "fibonacci"
        spread_padding = 25
        
        # Gentle physics
        spring_strength = 0.7
        damping = 0.9
        
        # Research-friendly 3D
        perspective = 800
        stack_depth_step = 90
    }
}

# Research workflow bindings  
bind = SUPER, d, exec, hyprctl plugin stack3d toggle        # 'd' for data
bind = SUPER, f, exec, hyprctl plugin stack3d set_layout fibonacci
bind = SUPER, c, exec, hyprctl plugin stack3d set_layout circular

# Application launching
bind = SUPER, r, exec, rstudio
bind = SUPER SHIFT, r, exec, jupyter-lab
bind = SUPER, z, exec, zotero  # Reference management
```

### Financial/Trading Setup

High-density information display for traders:

```bash
plugin {
    stack3d {
        enable = true
        
        # Ultra-fast for market data
        transition_duration = 0.2
        stagger_delay = 0.005
        
        # Instant switching
        transition_style = "smooth_slide"
        motion_blur = false
        
        # Grid for maximum data density
        default_layout = "grid"
        spread_padding = 8
        
        # Immediate response
        spring_strength = 2.0
        damping = 0.99
        
        # Minimal 3D effects
        perspective = 400
        stack_depth_step = 60
        
        # Trading-optimized keys
        toggle_key = "SUPER, space"
        peek_key = "SUPER, shift"
    }
}

# Trading workflow
bind = SUPER, m, exec, hyprctl plugin stack3d toggle  # 'm' for market
bind = SUPER, 1, exec, firefox --new-window "https://tradingview.com"
bind = SUPER, 2, exec, terminal -e "python trading_bot.py"
```

## 📝 Content Creation

### Writing & Documentation

Setup for writers, technical writers, and documentation:

```bash
plugin {
    stack3d {
        enable = true
        
        # Calm, non-distracting transitions
        transition_duration = 0.8
        stagger_delay = 0.05
        
        # Gentle, flowing animation
        transition_style = "liquid_flow"
        motion_blur = true
        
        # Fibonacci for organic layout
        default_layout = "fibonacci"
        spread_padding = 30
        
        # Soft, natural physics
        spring_strength = 0.6
        damping = 0.85
        
        # Comfortable 3D perspective
        perspective = 850
        eye_distance = 1100
        
        # Writer-friendly keys
        toggle_key = "SUPER, w"      # 'w' for write
        peek_key = "SUPER, r"        # 'r' for reference
    }
}

# Writing workflow
bind = SUPER, t, exec, hyprctl plugin stack3d toggle  # 't' for text
bind = SUPER SHIFT, t, exec, hyprctl plugin stack3d set_layout fibonacci

# Application bindings
bind = SUPER, o, exec, obsidian     # Note-taking
bind = SUPER, n, exec, notion-app   # Documentation
bind = SUPER, g, exec, grammarly    # Grammar check
```

### Academic Research

Configuration for academic workflows:

```bash
plugin {
    stack3d {
        enable = true
        
        # Contemplative timing for deep work
        transition_duration = 1.0
        stagger_delay = 0.06
        
        # Elegant, scholarly animation
        transition_style = "elastic_out"
        motion_blur = true
        
        # Golden ratio layout for aesthetics
        default_layout = "fibonacci"
        spread_padding = 35
        
        # Thoughtful physics
        spring_strength = 0.5
        damping = 0.8
        
        # Academic perspective
        perspective = 900
        stack_depth_step = 100
    }
}

# Academic bindings
bind = SUPER, a, exec, hyprctl plugin stack3d toggle    # 'a' for academic
bind = SUPER, s, exec, hyprctl plugin stack3d set_layout spiral  # 's' for spiral

# Academic applications
bind = SUPER, z, exec, zotero                    # References
bind = SUPER, l, exec, libreoffice --writer      # Writing
bind = SUPER, m, exec, mathematica               # Math
bind = SUPER, p, exec, evince                    # PDF viewer
```

## ⚡ Performance Optimization

### High-Performance Workstation

Maximum performance for powerful hardware:

```bash
plugin {
    stack3d {
        enable = true
        
        # Utilize powerful hardware
        transition_duration = 0.8
        stagger_delay = 0.06
        
        # Rich visual effects
        transition_style = "liquid_flow"
        motion_blur = true
        
        # Any layout works smoothly
        default_layout = "spiral"
        spread_padding = 25
        
        # Complex physics calculations
        spring_strength = 0.6
        damping = 0.85
        
        # High-quality 3D rendering
        perspective = 1000
        eye_distance = 1200
        stack_depth_step = 120
    }
}
```

### Resource-Constrained Setup

Optimized for older or low-power hardware:

```bash
plugin {
    stack3d {
        enable = true
        
        # Minimal resource usage
        transition_duration = 0.3
        stagger_delay = 0.01
        
        # Efficient animation
        transition_style = "smooth_slide"
        motion_blur = false
        
        # Simple grid layout
        default_layout = "grid"
        spread_padding = 15
        
        # Lightweight physics
        spring_strength = 1.0
        damping = 0.95
        
        # Minimal 3D processing
        perspective = 500
        eye_distance = 700
        stack_depth_step = 60
    }
}
```

## 🔧 Integration Examples

### With Task Management

Integration with task management tools:

```bash
plugin {
    stack3d {
        enable = true
        transition_duration = 0.5
        default_layout = "grid"
        
        # Task-focused keybinds
        toggle_key = "SUPER, t"
        peek_key = "SUPER, p"
    }
}

# Task management integration
bind = SUPER, k, exec, hyprctl plugin stack3d toggle      # Kanban view
bind = SUPER SHIFT, k, exec, hyprctl plugin stack3d set_layout grid

# Application shortcuts
bind = SUPER, 1, exec, notion          # Project management
bind = SUPER, 2, exec, todoist         # Task tracking
bind = SUPER, 3, exec, calendar        # Time management
```

### With Note-Taking Systems

Zettelkasten and note-taking workflows:

```bash
plugin {
    stack3d {
        enable = true
        
        # Note-browsing optimized
        transition_duration = 0.6
        transition_style = "smooth_slide"
        default_layout = "fibonacci"  # Natural for note relationships
        
        # Knowledge work timing
        stagger_delay = 0.04
        spring_strength = 0.8
        damping = 0.9
    }
}

# Note-taking workflow
bind = SUPER, n, exec, hyprctl plugin stack3d toggle
bind = SUPER SHIFT, n, exec, hyprctl plugin stack3d set_layout fibonacci

# Zettelkasten applications
bind = SUPER, o, exec, obsidian
bind = SUPER, l, exec, logseq  
bind = SUPER, r, exec, roam-research
```

## 📋 Workspace Organization

### Project-Based Workspaces

Different configurations per project type:

```bash
# Base configuration
plugin {
    stack3d {
        enable = true
        transition_duration = 0.5
        default_layout = "grid"
    }
}

# Project-specific workspace setup
workspace = 1, name = "coding", layout = grid, animation = smooth_slide
workspace = 2, name = "design", layout = circular, animation = elastic_out  
workspace = 3, name = "research", layout = fibonacci, animation = liquid_flow
workspace = 4, name = "communication", layout = grid, animation = smooth_slide

# Workspace switching with layout changes
bind = SUPER, 1, workspace, 1
bind = SUPER, 1, exec, hyprctl plugin stack3d set_layout grid
bind = SUPER, 1, exec, hyprctl plugin stack3d set_style smooth_slide

bind = SUPER, 2, workspace, 2  
bind = SUPER, 2, exec, hyprctl plugin stack3d set_layout circular
bind = SUPER, 2, exec, hyprctl plugin stack3d set_style elastic_out
```

### Time-Based Configuration

Different behaviors for different parts of the day:

```bash
#!/bin/bash
# ~/.local/bin/stack3d-time-config

HOUR=$(date +%H)

if [ $HOUR -ge 9 ] && [ $HOUR -le 17 ]; then
    # Work hours: Fast and efficient
    hyprctl plugin stack3d set_duration 0.4
    hyprctl plugin stack3d set_style smooth_slide
    hyprctl plugin stack3d set_layout grid
elif [ $HOUR -ge 18 ] && [ $HOUR -le 22 ]; then
    # Evening: More relaxed
    hyprctl plugin stack3d set_duration 0.8
    hyprctl plugin stack3d set_style liquid_flow
    hyprctl plugin stack3d set_layout fibonacci
else
    # Night: Minimal distraction
    hyprctl plugin stack3d set_duration 0.3
    hyprctl plugin stack3d set_style smooth_slide
    hyprctl plugin stack3d enable false
fi
```

Run with cron:
```bash
# Crontab entry
0 * * * * ~/.local/bin/stack3d-time-config
```

## 🎚️ Advanced Productivity

### Focus Mode

Deep work configuration with minimal distractions:

```bash
#!/bin/bash
# ~/.local/bin/focus-mode

# Enable focus mode
hyprctl plugin stack3d set_duration 0.2        # Very fast
hyprctl plugin stack3d set_style smooth_slide  # No distractions
hyprctl plugin stack3d set_layout grid         # Maximum efficiency
hyprctl plugin stack3d set_motion_blur false   # Clean switching

# Disable non-essential features
hyprctl plugin stack3d set_stagger_delay 0.005
hyprctl plugin stack3d set_spring_strength 1.5
hyprctl plugin stack3d set_damping 0.98

echo "Focus mode enabled"
notify-send "Stack3D" "Focus mode enabled" -t 2000
```

### Presentation Mode

Configuration for screen sharing and presentations:

```bash
#!/bin/bash
# ~/.local/bin/presentation-mode

# Presentation-friendly settings
hyprctl plugin stack3d set_duration 1.0         # Visible transitions
hyprctl plugin stack3d set_style elastic_out    # Engaging animation
hyprctl plugin stack3d set_layout circular      # Visually interesting
hyprctl plugin stack3d set_motion_blur false    # Clean for recording
hyprctl plugin stack3d set_stagger_delay 0.08   # Dramatic effect

echo "Presentation mode enabled"
notify-send "Stack3D" "Presentation mode enabled" -t 2000
```

## 💡 Pro Tips

### Productivity Keybinding Sets

```bash
# Quick productivity shortcuts
bind = SUPER, f, fullscreen, 0               # Fullscreen current
bind = SUPER SHIFT, f, fullscreen, 1         # Maximize current
bind = SUPER, m, exec, hyprctl plugin stack3d toggle  # Toggle view
bind = SUPER, comma, exec, hyprctl reload    # Quick config reload

# Window management
bind = SUPER SHIFT, q, killactive
bind = SUPER, q, exec, hyprctl activewindow | grep -q floating && hyprctl dispatch togglefloating || hyprctl plugin stack3d toggle

# Workspace productivity
bind = SUPER, bracketleft, workspace, m-1    # Previous workspace
bind = SUPER, bracketright, workspace, m+1   # Next workspace
```

### Automation Scripts

```bash
#!/bin/bash  
# ~/.local/bin/work-session-start

# Start work session
echo "Starting work session..."

# Set productivity configuration
hyprctl plugin stack3d enable true
hyprctl plugin stack3d set_duration 0.4
hyprctl plugin stack3d set_layout grid
hyprctl plugin stack3d set_style smooth_slide

# Launch productivity applications
hyprctl dispatch workspace 1
code ~/projects/current &
sleep 2

hyprctl dispatch workspace 2  
firefox &
sleep 2

hyprctl dispatch workspace 3
alacritty &

# Return to main workspace
hyprctl dispatch workspace 1
hyprctl plugin stack3d toggle

echo "Work session ready!"
```

---

These productivity configurations prioritize efficiency, speed, and minimal distraction while maintaining the visual benefits of the Stack3D plugin. Adjust timing and visual effects based on your hardware capabilities and personal preferences.