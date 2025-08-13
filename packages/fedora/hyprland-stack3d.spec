Name:           hyprland-stack3d
Version:        1.0.0
Release:        1%{?dist}
Summary:        3D stack layout plugin for Hyprland

License:        MIT
URL:            https://github.com/olafkfreund/3Dstack-hyrpland
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
3D stack layout plugin for Hyprland with physics-based animations and smooth transitions.

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
* Wed Aug 13 2025 Stack3D Team <team@example.com> - 1.0.0-1
- Initial release
- 3D window stacking with physics animations
- Multiple layout modes and customization options
