#!/bin/bash
# Complete local packaging script for Hyprland Stack3D Plugin
# Builds plugin with nix and creates packages for Debian, Fedora, and Arch

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Helper functions
log_info() {
    echo -e "${BLUE}INFO: $1${NC}"
}

log_success() {
    echo -e "${GREEN}SUCCESS: $1${NC}"
}

log_warning() {
    echo -e "${YELLOW}WARNING: $1${NC}"
}

log_error() {
    echo -e "${RED}ERROR: $1${NC}"
}

log_header() {
    echo -e "${CYAN}$1${NC}"
}

# Configuration
PLUGIN_NAME="hyprland-stack3d"
PLUGIN_VERSION="1.0.0"
PLUGIN_DESCRIPTION="3D stack layout plugin for Hyprland"
MAINTAINER_NAME="Stack3D Team"
MAINTAINER_EMAIL="team@example.com"
GITHUB_URL="https://github.com/olafkfreund/3Dstack-hyrpland"

# Ensure we're in the project root
if [[ ! -f "flake.nix" ]] || [[ ! -f "main.cpp" ]]; then
    log_error "Please run this script from the project root directory"
    exit 1
fi

log_header "Building Hyprland Stack3D Plugin Packages"

# Step 1: Build the plugin with Nix
log_info "Building plugin with Nix..."
if nix build; then
    log_success "Plugin built successfully with Nix"
else
    log_error "Nix build failed"
    exit 1
fi

# Verify the plugin binary exists
if [[ ! -f "result/lib/stack3d.so" ]]; then
    log_error "Plugin binary not found in Nix build result"
    exit 1
fi

log_info "Plugin binary: $(file result/lib/stack3d.so)"
log_info "Plugin size: $(du -h result/lib/stack3d.so | cut -f1)"

# Step 2: Create package directories
log_info "Creating package directory structure..."
rm -rf packages/
mkdir -p packages/{debian,fedora,arch}

# Step 3: Create Debian package
log_header "Creating Debian package..."
DEBIAN_PKG_DIR="packages/debian/${PLUGIN_NAME}-${PLUGIN_VERSION}"
mkdir -p "${DEBIAN_PKG_DIR}/DEBIAN"
mkdir -p "${DEBIAN_PKG_DIR}/usr/lib/hyprland"
mkdir -p "${DEBIAN_PKG_DIR}/usr/share/doc/${PLUGIN_NAME}"

# Copy plugin binary
cp result/lib/stack3d.so "${DEBIAN_PKG_DIR}/usr/lib/hyprland/"

# Copy documentation
cp hyprland_stack3d_plugin.md "${DEBIAN_PKG_DIR}/usr/share/doc/${PLUGIN_NAME}/README.md"
cp examples/hyprland.conf "${DEBIAN_PKG_DIR}/usr/share/doc/${PLUGIN_NAME}/example-hyprland.conf"

# Create Debian control file
cat > "${DEBIAN_PKG_DIR}/DEBIAN/control" << EOF
Package: ${PLUGIN_NAME}
Version: ${PLUGIN_VERSION}
Section: x11
Priority: optional
Architecture: amd64
Depends: hyprland
Maintainer: ${MAINTAINER_NAME} <${MAINTAINER_EMAIL}>
Description: ${PLUGIN_DESCRIPTION}
 Adds 3D perspective stack window management to Hyprland compositor
 with physics-based animations and smooth transitions.
 .
 This package provides advanced window stacking capabilities including:
 * Physics-based animations with spring dynamics
 * Multiple layout modes (grid, cascade, spiral)  
 * Customizable transition effects
 * Bezier curve easing functions
 * Motion blur effects
 * Configurable keybindings
 .
 The plugin integrates seamlessly with Hyprland's existing window
 management system and can be controlled via hyprctl commands.
EOF

# Create installation instructions
cat > "${DEBIAN_PKG_DIR}/usr/share/doc/${PLUGIN_NAME}/INSTALL.md" << EOF
# Installation Instructions for ${PLUGIN_NAME}

## Automatic Installation (Recommended)

The package automatically installs the plugin to the correct location.
Simply add the plugin to your Hyprland configuration:

\`\`\`
plugin = /usr/lib/hyprland/stack3d.so
\`\`\`

## Configuration

Add plugin settings to your Hyprland config:

\`\`\`
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
\`\`\`

## Testing

Test the plugin with:
\`\`\`bash
hyprctl dispatch stack3d toggle
hyprctl dispatch stack3d cycle
\`\`\`

## Troubleshooting

If the plugin fails to load:
1. Ensure Hyprland version compatibility
2. Check plugin permissions: \`ls -la /usr/lib/hyprland/stack3d.so\`
3. View logs: \`journalctl -f\` while starting Hyprland
EOF

# Build .deb package
log_info "Building Debian package..."
cd packages/debian/
if command -v dpkg-deb >/dev/null 2>&1; then
    dpkg-deb --build "${PLUGIN_NAME}-${PLUGIN_VERSION}"
    mv "${PLUGIN_NAME}-${PLUGIN_VERSION}.deb" "../${PLUGIN_NAME}-${PLUGIN_VERSION}-amd64.deb"
    log_success "Debian package created: ${PLUGIN_NAME}-${PLUGIN_VERSION}-amd64.deb"
else
    # Create tarball if dpkg-deb not available
    tar czf "../${PLUGIN_NAME}-${PLUGIN_VERSION}-debian.tar.gz" "${PLUGIN_NAME}-${PLUGIN_VERSION}/"
    log_warning "dpkg-deb not available, created tarball instead: ${PLUGIN_NAME}-${PLUGIN_VERSION}-debian.tar.gz"
fi
cd ../../

# Step 4: Create Fedora/RPM package structure
log_header "Creating Fedora package..."
FEDORA_PKG_DIR="packages/fedora/${PLUGIN_NAME}-${PLUGIN_VERSION}"
mkdir -p "${FEDORA_PKG_DIR}/usr/lib64/hyprland"
mkdir -p "${FEDORA_PKG_DIR}/usr/share/doc/${PLUGIN_NAME}"

# Copy plugin binary
cp result/lib/stack3d.so "${FEDORA_PKG_DIR}/usr/lib64/hyprland/"

# Copy documentation  
cp hyprland_stack3d_plugin.md "${FEDORA_PKG_DIR}/usr/share/doc/${PLUGIN_NAME}/README.md"
cp examples/hyprland.conf "${FEDORA_PKG_DIR}/usr/share/doc/${PLUGIN_NAME}/example-hyprland.conf"

# Create RPM spec file
cat > "packages/fedora/${PLUGIN_NAME}.spec" << EOF
Name:           ${PLUGIN_NAME}
Version:        ${PLUGIN_VERSION}
Release:        1%{?dist}
Summary:        ${PLUGIN_DESCRIPTION}

License:        MIT
URL:            ${GITHUB_URL}
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc-c++ >= 11
BuildRequires:  make
BuildRequires:  pkg-config
BuildRequires:  hyprland-devel
BuildRequires:  pixman-devel
BuildRequires:  libdrm-devel

Requires:       hyprland
Requires:       pixman
Requires:       libdrm

%description
${PLUGIN_DESCRIPTION} with physics-based animations and smooth transitions.

Features:
* Physics-based animations with spring dynamics
* Multiple layout modes (grid, cascade, spiral)
* Customizable transition effects  
* Bezier curve easing functions
* Motion blur effects
* Configurable keybindings

%prep
%setup -q

%build
# Plugin is pre-built in this package

%install
mkdir -p %{buildroot}/usr/lib64/hyprland
mkdir -p %{buildroot}/usr/share/doc/%{name}
cp usr/lib64/hyprland/stack3d.so %{buildroot}/usr/lib64/hyprland/
cp usr/share/doc/%{name}/* %{buildroot}/usr/share/doc/%{name}/

%files
/usr/lib64/hyprland/stack3d.so
/usr/share/doc/%{name}/

%changelog
* $(date '+%a %b %d %Y') ${MAINTAINER_NAME} <${MAINTAINER_EMAIL}> - ${PLUGIN_VERSION}-1
- Initial release
- 3D window stacking with physics animations
- Multiple layout modes and customization options
EOF

# Create Fedora tarball
cd packages/fedora/
tar czf "../${PLUGIN_NAME}-${PLUGIN_VERSION}-fedora.tar.gz" "${PLUGIN_NAME}-${PLUGIN_VERSION}/" "${PLUGIN_NAME}.spec"
log_success "Fedora package created: ${PLUGIN_NAME}-${PLUGIN_VERSION}-fedora.tar.gz"
cd ../../

# Step 5: Create Arch package  
log_header "Creating Arch package..."
ARCH_PKG_DIR="packages/arch/${PLUGIN_NAME}"
mkdir -p "${ARCH_PKG_DIR}"

# Copy plugin binary for packaging
cp result/lib/stack3d.so "${ARCH_PKG_DIR}/"
cp hyprland_stack3d_plugin.md "${ARCH_PKG_DIR}/README.md" 
cp examples/hyprland.conf "${ARCH_PKG_DIR}/example-hyprland.conf"

# Create PKGBUILD
cat > "${ARCH_PKG_DIR}/PKGBUILD" << EOF
# Maintainer: ${MAINTAINER_NAME} <${MAINTAINER_EMAIL}>

pkgname=${PLUGIN_NAME}
pkgver=${PLUGIN_VERSION}
pkgrel=1
pkgdesc="${PLUGIN_DESCRIPTION}"
arch=('x86_64')
url="${GITHUB_URL}"
license=('MIT')
depends=('hyprland' 'pixman' 'libdrm')
makedepends=('gcc' 'make' 'pkg-config')
source=("stack3d.so" "README.md" "example-hyprland.conf")
sha256sums=('SKIP' 'SKIP' 'SKIP')

package() {
    # Install plugin binary
    install -Dm755 "\$srcdir/stack3d.so" "\$pkgdir/usr/lib/hyprland/stack3d.so"
    
    # Install documentation
    install -Dm644 "\$srcdir/README.md" "\$pkgdir/usr/share/doc/\$pkgname/README.md"
    install -Dm644 "\$srcdir/example-hyprland.conf" "\$pkgdir/usr/share/doc/\$pkgname/example-hyprland.conf"
    
    # Create installation instructions
    cat > "\$pkgdir/usr/share/doc/\$pkgname/INSTALL.md" << 'INSTEOF'
# ${PLUGIN_NAME} Installation

## Configuration

Add to your Hyprland config:

\`\`\`
plugin = /usr/lib/hyprland/stack3d.so

plugin {
    stack3d {
        enable = true
        transition_duration = 0.8
        default_layout = grid
    }
}

# Keybindings
bind = SUPER, grave, stack3d, toggle
bind = SUPER SHIFT, grave, stack3d, cycle_layout
\`\`\`

## Usage

- \`SUPER + grave\`: Toggle stack view
- \`SUPER + SHIFT + grave\`: Cycle layout modes
- \`hyprctl dispatch stack3d toggle\`: Manual toggle

INSTEOF
}
EOF

# Create .SRCINFO
cat > "${ARCH_PKG_DIR}/.SRCINFO" << EOF
pkgbase = ${PLUGIN_NAME}
	pkgdesc = ${PLUGIN_DESCRIPTION}
	pkgver = ${PLUGIN_VERSION}
	pkgrel = 1
	url = ${GITHUB_URL}
	arch = x86_64
	license = MIT
	makedepends = gcc
	makedepends = make
	makedepends = pkg-config
	depends = hyprland
	depends = pixman
	depends = libdrm
	source = stack3d.so
	source = README.md
	source = example-hyprland.conf
	sha256sums = SKIP
	sha256sums = SKIP
	sha256sums = SKIP

pkgname = ${PLUGIN_NAME}
EOF

# Create Arch tarball
cd packages/arch/
tar czf "../${PLUGIN_NAME}-${PLUGIN_VERSION}-arch.tar.gz" "${PLUGIN_NAME}/"
log_success "Arch package created: ${PLUGIN_NAME}-${PLUGIN_VERSION}-arch.tar.gz"
cd ../../

# Step 6: Create package summary
log_header "Package Build Summary"
echo
log_success "All packages built successfully!"
echo
echo "📦 Distribution packages created:"
echo "  • Debian: $(ls packages/*.deb 2>/dev/null || echo 'packages/*-debian.tar.gz')"
echo "  • Fedora: packages/${PLUGIN_NAME}-${PLUGIN_VERSION}-fedora.tar.gz"  
echo "  • Arch:   packages/${PLUGIN_NAME}-${PLUGIN_VERSION}-arch.tar.gz"
echo
echo "📋 Package contents:"
echo "  • Plugin binary: stack3d.so ($(du -h result/lib/stack3d.so | cut -f1))"
echo "  • Documentation and examples"
echo "  • Distribution-specific metadata" 
echo "  • Installation instructions"
echo
log_info "Plugin built from source with Nix: $(nix path-info --closure-size result | tail -1)"
echo

# Create checksums
log_info "Creating checksums..."
cd packages/
sha256sum *.{deb,tar.gz} 2>/dev/null > checksums.sha256 || sha256sum *.tar.gz > checksums.sha256
log_success "Checksums created: packages/checksums.sha256"
cd ../

echo
log_header "🎉 Build Complete!"
log_info "All packages are ready for distribution"
log_info "Plugin source: Nix flake build"  
log_info "Binary size: $(du -h result/lib/stack3d.so | cut -f1)"
log_info "Total packages: $(ls packages/*.{deb,tar.gz} 2>/dev/null | wc -l)"

echo
echo "📋 Next steps:"
echo "  • Test packages on target distributions"
echo "  • Upload to package repositories"
echo "  • Create GitHub release with artifacts"
echo "  • Update installation documentation"