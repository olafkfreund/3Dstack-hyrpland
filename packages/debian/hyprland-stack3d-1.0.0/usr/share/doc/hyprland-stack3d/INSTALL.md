# Installation Instructions for hyprland-stack3d

## Automatic Installation (Recommended)

The package automatically installs the plugin to the correct location.
Simply add the plugin to your Hyprland configuration:

```
plugin = /usr/lib/hyprland/stack3d.so
```

## Configuration

Add plugin settings to your Hyprland config:

```
plugin {
    stack3d {
        enable = true
        transition_duration = 0.8
        stagger_delay = 0.05
        transition_style = smooth_slide
        stack_depth_step = 100
        spread_padding = 20
        default_layout = grid
        spring_strength = 0.8
        damping = 0.92
        motion_blur = true
        perspective = 800
        eye_distance = 1000
    }
}

# Keybindings
bind = SUPER, grave, stack3d, toggle
bind = SUPER, space, stack3d, peek
bind = SUPER SHIFT, grave, stack3d, cycle_layout
```

## Testing

Test the plugin with:
```bash
hyprctl dispatch stack3d toggle
hyprctl dispatch stack3d cycle
```

## Troubleshooting

If the plugin fails to load:
1. Ensure Hyprland version compatibility
2. Check plugin permissions: `ls -la /usr/lib/hyprland/stack3d.so`
3. View logs: `journalctl -f` while starting Hyprland
