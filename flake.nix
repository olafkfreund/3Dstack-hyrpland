{
  description = "Hyprland 3D Stack Sliding Animation Plugin";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    
    hyprland = {
      url = "github:hyprwm/Hyprland";
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
            gcc
            gnumake
            pkg-config
          ];

          buildInputs = with pkgs; [
            pixman
            libdrm
            hyprlandDev
          ] ++ hyprlandDev.buildInputs;

          # Set C++ standard and compilation flags
          makeFlags = [
            "CXX=${pkgs.gcc}/bin/g++"
            "CXXFLAGS=-shared -fPIC -g -std=c++23 -Wall -Wextra"
            "INCLUDES=-Iinclude ${pkgs.pkg-config}/bin/pkg-config --cflags pixman-1 libdrm hyprland"
            "LIBS=${pkgs.pkg-config}/bin/pkg-config --libs pixman-1 libdrm hyprland"
          ];

          buildPhase = ''
            runHook preBuild
            make
            runHook postBuild
          '';

          # Skip tests for nix build (tests require special setup)
          doCheck = false;

          installPhase = ''
            runHook preInstall
            mkdir -p $out/lib
            cp stack3d.so $out/lib/libhyprland-stack3d.so
            runHook postInstall
          '';

          meta = with pkgs.lib; {
            description = "3D stack sliding animations plugin for Hyprland";
            homepage = "https://github.com/yourusername/hyprland-stack3d";
            license = licenses.mit;
            platforms = platforms.linux;
            maintainers = [];
          };
        };

        # Development shell with all necessary dependencies
        devShell = pkgs.mkShell {
          name = "hyprland-stack3d-dev";
          
          nativeBuildInputs = with pkgs; [
            # Build tools
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
          ] ++ hyprlandDev.buildInputs;

          # Environment variables
          PKG_CONFIG_PATH = "${pkgs.lib.makeSearchPathOutput "lib" "lib/pkgconfig" [
            pkgs.pixman
            pkgs.libdrm
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

      in {
        packages = {
          default = stack3d;
          hyprland-stack3d = stack3d;
        };

        devShells = {
          default = devShell;
        };

        # NixOS module for easy integration
        nixosModules.default = { config, lib, pkgs, ... }:
          with lib;
          let
            cfg = config.programs.hyprland.plugins.stack3d;
          in {
            options.programs.hyprland.plugins.stack3d = {
              enable = mkEnableOption "Hyprland Stack3D plugin";
              
              package = mkOption {
                type = types.package;
                default = self.packages.${pkgs.system}.default;
                description = "The Stack3D plugin package to use";
              };

              settings = mkOption {
                type = types.attrs;
                default = {
                  enable = true;
                  transition_duration = 0.8;
                  stagger_delay = 0.05;
                  transition_style = "smooth_slide";
                  stack_depth_step = 100;
                  spread_padding = 20;
                  default_layout = "grid";
                  spring_strength = 0.8;
                  damping = 0.92;
                  motion_blur = true;
                  perspective = 800;
                  eye_distance = 1000;
                };
                description = "Configuration settings for Stack3D plugin";
              };

              keybindings = mkOption {
                type = types.attrs;
                default = {
                  toggle = "SUPER, grave";
                  peek = "SUPER, space";
                  cycle_layout = "SUPER SHIFT, grave";
                };
                description = "Keybinding configuration";
              };
            };

            config = mkIf cfg.enable {
              programs.hyprland = {
                enable = true;
                plugins = [ cfg.package ];
              };

              # Add plugin configuration to Hyprland config
              programs.hyprland.extraConfig = ''
                # Stack3D Plugin Configuration
                plugin = ${cfg.package}/lib/stack3d.so

                plugin {
                  stack3d {
                    ${lib.concatStringsSep "\n    " (lib.mapAttrsToList 
                      (name: value: "${name} = ${toString value}") cfg.settings)}
                    
                    toggle_key = "${cfg.keybindings.toggle}"
                    peek_key = "${cfg.keybindings.peek}"
                    cycle_layout_key = "${cfg.keybindings.cycle_layout}"
                  }
                }
              '';
            };
          };

        # Home Manager module
        homeManagerModules.default = { config, lib, pkgs, ... }:
          with lib;
          let
            cfg = config.wayland.windowManager.hyprland.plugins.stack3d;
          in {
            options.wayland.windowManager.hyprland.plugins.stack3d = {
              enable = mkEnableOption "Hyprland Stack3D plugin";
              
              package = mkOption {
                type = types.package;
                default = self.packages.${pkgs.system}.default;
                description = "The Stack3D plugin package to use";
              };

              settings = mkOption {
                type = types.attrs;
                default = {};
                description = "Plugin settings";
              };
            };

            config = mkIf cfg.enable {
              wayland.windowManager.hyprland = {
                plugins = [ cfg.package ];
                
                settings = {
                  plugin = {
                    stack3d = cfg.settings;
                  };
                };
              };
            };
          };

        # Formatter for `nix fmt`
        formatter = pkgs.nixpkgs-fmt;

        # Checks for `nix flake check`
        checks = {
          build = stack3d;
          
          # Code quality checks
          statix = pkgs.runCommand "statix-check" { 
            nativeBuildInputs = [ pkgs.statix ]; 
          } ''
            statix check ${./.}
            touch $out
          '';
          
          deadnix = pkgs.runCommand "deadnix-check" { 
            nativeBuildInputs = [ pkgs.deadnix ]; 
          } ''
            deadnix ${./.}
            touch $out
          '';
        };
      }
    );
}