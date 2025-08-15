{
  description = "Hyprland 3D Stack Sliding Animation Plugin";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    hyprland = {
      url = "github:hyprwm/Hyprland/4e242d086e20b32951fdc0ebcbfb4d41b5be8dcc";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils, hyprland }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        hyprlandPkgs = hyprland.packages.${system};
        hyprlandDev = hyprland.packages.${system}.hyprland-debug;

        stack3d = pkgs.stdenv.mkDerivation rec {
          pname = "hyprland-stack3d";
          version = "1.0.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            gcc
            pkg-config
          ];

          buildInputs = with pkgs; [
            pixman
            libdrm
            hyprlandDev
            wayland
          ] ++ hyprlandDev.buildInputs;

          # Use CMake for building
          configurePhase = ''
            runHook preConfigure
            cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
            runHook postConfigure
          '';

          buildPhase = ''
            runHook preBuild
            cmake --build build
            runHook postBuild
          '';

          # Skip tests for nix build (tests require special setup)
          doCheck = false;

          installPhase = ''
            runHook preInstall
            mkdir -p $out/lib
            cp build/stack3d.so $out/lib/libhyprland-stack3d.so
            runHook postInstall
          '';

          meta = with pkgs.lib; {
            description = "3D stack sliding animations plugin for Hyprland";
            homepage = "https://github.com/yourusername/hyprland-stack3d";
            license = licenses.mit;
            platforms = platforms.linux;
            maintainers = [ ];
          };
        };

        # Development shell with all necessary dependencies
        devShell = pkgs.mkShell {
          name = "hyprland-stack3d-dev";

          nativeBuildInputs = with pkgs; [
            # Build tools
            cmake
            gcc
            gnumake
            pkg-config
            just

            # Development and testing tools
            gdb
            valgrind
            clang-tools
            lcov

            # Code quality tools
            cppcheck
            uncrustify

            # Nix tools
            nixd
            nil
            statix
            deadnix
            nix-tree
            nix-diff

            # General development
            git
            gitlint
            pre-commit
          ];

          buildInputs = with pkgs; [
            # Hyprland development
            hyprlandDev
            pixman
            libdrm
            wayland
          ] ++ hyprlandDev.buildInputs;

          # Environment variables
          PKG_CONFIG_PATH = "${pkgs.lib.makeSearchPathOutput "lib" "lib/pkgconfig" [
            pkgs.pixman
            pkgs.libdrm
            pkgs.wayland
            hyprlandDev
          ]}";

          HYPRLAND_HEADERS = "${hyprlandDev}/include";

          shellHook = ''
            echo "🚀 Hyprland Stack3D Plugin Development Environment"
            echo "📦 Available tools:"
            echo "  • just build    - Build the plugin"
            echo "  • just clean    - Clean build artifacts"  
            echo "  • just install  - Install to user directory"
            echo "  • just test     - Run tests"
            echo "  • just format   - Format code"
            echo "  • just lint     - Run linters"
            echo ""
            echo "🔧 Build tools: gcc, make, pkg-config"
            echo "🐛 Debug tools: gdb, valgrind"  
            echo "✨ Quality tools: cppcheck, clang-tools"
            echo "📝 Nix tools: nixd, statix, deadnix"
            echo ""
            echo "📍 Hyprland headers: $HYPRLAND_HEADERS"
            echo "📦 PKG_CONFIG_PATH configured for dependencies"
          '';
        };

      in
      {
        packages = {
          default = stack3d;
          hyprland-stack3d = stack3d;
        };

        devShells = {
          default = devShell;
        };

        # NixOS module for system-wide installation
        nixosModules.default = { config, lib, pkgs, ... }:
          with lib;
          let
            cfg = config.programs.hyprland.stack3d;
          in
          {
            options.programs.hyprland.stack3d = {
              enable = mkEnableOption "Hyprland Stack3D plugin";

              package = mkOption {
                type = types.package;
                default = self.packages.${pkgs.system}.default;
                description = "The Stack3D plugin package to use";
              };

              settings = mkOption {
                type = with types; attrsOf (oneOf [ bool int float str ]);
                default = {
                  enabled = true;
                  transition_duration = 0.8;
                  stagger_delay = 0.05;
                  transition_style = 0; # 0 = SMOOTH_SLIDE
                  stack_depth_step = 100.0;
                  spread_padding = 20.0;
                  default_layout = 0; # 0 = GRID
                  spring_strength = 0.8;
                  damping = 0.92;
                  motion_blur = 1; # 1 = true
                  perspective = 800.0;
                  eye_distance = 1000.0;
                };
                description = ''
                  Configuration settings for Stack3D plugin.
                  See the plugin documentation for available options.
                '';
              };

              keybindings = mkOption {
                type = with types; attrsOf str;
                default = {
                  toggle = "SUPER, grave";
                  cycle = "SUPER, TAB";
                  peek = "SUPER, space";
                };
                description = "Keybinding configuration for Stack3D commands";
              };
            };

            config = mkIf cfg.enable {
              # Ensure Hyprland is enabled with our plugin
              programs.hyprland = {
                enable = mkDefault true;
                plugins = [ cfg.package ];
              };

              # Add Hyprland configuration file with plugin settings
              environment.etc."hypr/stack3d.conf".text = ''
                # Stack3D Plugin Configuration
                # Load plugin
                plugin = ${cfg.package}/lib/libhyprland-stack3d.so

                # Plugin settings
                plugin {
                    stack3d {
                        ${lib.concatStringsSep "\n        " (lib.mapAttrsToList 
                          (name: value: "${name} = ${toString value}") cfg.settings)}
                    }
                }

                # Keybindings for Stack3D
                bind = ${cfg.keybindings.toggle}, exec, hyprctl dispatch stack3d toggle
                bind = ${cfg.keybindings.cycle}, exec, hyprctl dispatch stack3d cycle
                bind = ${cfg.keybindings.peek}, exec, hyprctl dispatch stack3d peek
              '';

              # Inform users about the configuration file
              system.userActivationScripts.stack3d-config = ''
                echo "Stack3D plugin configuration available at /etc/hypr/stack3d.conf"
                echo "Include it in your Hyprland config with: source = /etc/hypr/stack3d.conf"
              '';
            };
          };

        # Home Manager module for user-level installation
        homeManagerModules.default = { config, lib, pkgs, ... }:
          with lib;
          let
            cfg = config.wayland.windowManager.hyprland.stack3d;
          in
          {
            options.wayland.windowManager.hyprland.stack3d = {
              enable = mkEnableOption "Hyprland Stack3D plugin";

              package = mkOption {
                type = types.package;
                default = self.packages.${pkgs.system}.default;
                description = "The Stack3D plugin package to use";
              };

              settings = mkOption {
                type = with types; attrsOf (oneOf [ bool int float str ]);
                default = {
                  enabled = true;
                  transition_duration = 0.8;
                  default_layout = "grid";
                };
                description = "Plugin configuration settings";
              };

              keybindings = mkOption {
                type = with types; attrsOf str;
                default = {
                  toggle = "SUPER, grave";
                  cycle = "SUPER, TAB";
                  peek = "SUPER, space";
                };
                description = "Keybinding configuration";
              };
            };

            config = mkIf cfg.enable {
              wayland.windowManager.hyprland = {
                plugins = [ cfg.package ];

                # Add plugin configuration to Hyprland settings
                extraConfig = ''
                  # Stack3D Plugin Configuration
                  plugin {
                      stack3d {
                          ${lib.concatStringsSep "\n          " (lib.mapAttrsToList 
                            (name: value: "${name} = ${toString value}") cfg.settings)}
                      }
                  }

                  # Stack3D Keybindings
                  bind = ${cfg.keybindings.toggle}, exec, hyprctl dispatch stack3d toggle
                  bind = ${cfg.keybindings.cycle}, exec, hyprctl dispatch stack3d cycle
                  bind = ${cfg.keybindings.peek}, exec, hyprctl dispatch stack3d peek
                '';
              };
            };
          };

        # Formatter for `nix fmt`
        formatter = pkgs.nixpkgs-fmt;

        # Checks for `nix flake check`
        checks = {
          build = stack3d;

          # Code quality checks
          statix = pkgs.runCommand "statix-check"
            {
              nativeBuildInputs = [ pkgs.statix ];
            } ''
            statix check ${./.}
            touch $out
          '';

          deadnix = pkgs.runCommand "deadnix-check"
            {
              nativeBuildInputs = [ pkgs.deadnix ];
            } ''
            deadnix ${./.}
            touch $out
          '';
        };
      }
    );
}
