# Building NW.js for ARM Devices

This document describes how to cross-compile NW.js for ARM (armhf/armel) and ARM64 (aarch64) targets on a Linux host.

## Prerequisites

### Host System Requirements

- Ubuntu 20.04+ or Debian 11+ (x86_64)
- At least 16GB RAM (32GB+ recommended)
- 100GB+ free disk space
- Fast internet connection (for fetching Chromium dependencies)

### Install Build Dependencies

```bash
# For 32-bit ARM (armhf) cross-compilation
sudo dpkg --add-architecture armhf
sudo apt-get update
sudo apt-get install -y \
  gcc-arm-linux-gnueabihf \
  g++-arm-linux-gnueabihf \
  libc6-dev-armhf-cross

# For 64-bit ARM (arm64/aarch64) cross-compilation
sudo dpkg --add-architecture arm64
sudo apt-get update
sudo apt-get install -y \
  gcc-aarch64-linux-gnu \
  g++-aarch64-linux-gnu \
  libc6-dev-arm64-cross

# Common build dependencies
sudo apt-get install -y \
  python3 git curl wget build-essential pkg-config \
  gperf bison flex libnotify-dev libgconf2-dev \
  libgnome-keyring-dev libgirepository1.0-dev \
  libdbus-1-dev libgtk-3-dev libboost-dev \
  libdrm-dev libxkbcommon-dev libatspi2.0-dev \
  libxss-dev libasound2-dev libcups2-dev \
  libpulse-dev libudev-dev libpci-dev libnss3-dev \
  libxrandr-dev libglib2.0-dev libxcomposite-dev \
  libxdamage-dev libxext-dev libxfixes-dev libxml2-dev
```

### Fetch depot_tools

```bash
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH="$PWD/depot_tools:$PATH"
```

## Fetching Source Code

```bash
# Create working directory
mkdir -p ~/nwjs-arm && cd ~/nwjs-arm

# Configure gclient
gclient config --name src/nw.js https://github.com/nwjs/nw.js.git

# Sync dependencies (this will take a while)
gclient sync --nohooks
```

## Building for ARM (armhf)

ARM hard-float (armhf) is the most common 32-bit ARM target, used on Raspberry Pi and similar devices.

### Generate Build Files

```bash
cd src

gn gen out/Release-arm --args='
  is_debug = false
  target_cpu = "arm"
  is_component_build = false
  is_official_build = true
  nwjs_sdk = false
  use_sysroot = true
  treat_warnings_as_errors = false
  enable_nacl = false
  is_desktop_linux = true
  use_qt = false
  use_ozone = true
  ozone_platform = "x11"
  use_vaapi = false
  use_v4l2_codec = false
  use_va = false
  use_cups = false
  use_dbus = false
  use_gio = false
  use_kerberos = false
  use_libpci = false
  use_pulseaudio = false
  use_udev = false
  symbol_level = 0
  blink_symbol_level = 0
  v8_symbol_level = 0
  chrome_pgo_phase = 0
'
```

### Build

```bash
autoninja -C out/Release-arm nwjs dist
```

### Output

The built binaries will be in `src/out/Release-arm/dist/` as a `.tar.gz` archive:
- `nwjs-vX.Y.Z-linux-armhf.tar.gz` (normal build)
- `nwjs-sdk-vX.Y.Z-linux-armhf.tar.gz` (SDK build with devtools)

## Building for ARM64 (aarch64)

ARM64 is the 64-bit ARM architecture, used on modern ARM devices like Raspberry Pi 4/5 (64-bit OS), Apple Silicon (via Linux VMs), and ARM servers.

### Generate Build Files

```bash
cd src

gn gen out/Release-arm64 --args='
  is_debug = false
  target_cpu = "arm64"
  is_component_build = false
  is_official_build = true
  nwjs_sdk = false
  use_sysroot = true
  treat_warnings_as_errors = false
  enable_nacl = false
  is_desktop_linux = true
  use_qt = false
  use_ozone = true
  ozone_platform = "x11"
  use_vaapi = false
  use_v4l2_codec = false
  use_va = false
  use_cups = false
  use_dbus = false
  use_gio = false
  use_kerberos = false
  use_libpci = false
  use_pulseaudio = false
  use_udev = false
  symbol_level = 0
  blink_symbol_level = 0
  v8_symbol_level = 0
  chrome_pgo_phase = 0
'
```

### Build

```bash
autoninja -C out/Release-arm64 nwjs dist
```

### Output

The built binaries will be in `src/out/Release-arm64/dist/` as a `.tar.gz` archive:
- `nwjs-vX.Y.Z-linux-arm64.tar.gz` (normal build)
- `nwjs-sdk-vX.Y.Z-linux-arm64.tar.gz` (SDK build with devtools)

## SDK vs Normal Build

- **Normal build**: Minimal runtime, no devtools or chromedriver
- **SDK build**: Includes Chrome DevTools and chromedriver for development/debugging

Set `nwjs_sdk = true` for SDK builds, `nwjs_sdk = false` for normal builds.

## Testing on ARM Devices

### Using QEMU (on x86_64 host)

```bash
# Install QEMU user-mode emulation
sudo apt-get install qemu-user-static binfmt-support

# Register binfmt handlers
sudo update-binfmts --enable qemu-arm
sudo update-binfmts --enable qemu-aarch64

# Test the binary (will run via QEMU)
./nw /path/to/your/app
```

### On Real Hardware

Copy the `.tar.gz` to your ARM device and extract:

```bash
tar xzf nwjs-v*.tar.gz
cd nwjs-v*/
./nw /path/to/your/app
```

## Troubleshooting

### "failed to locate toolchain" error
Ensure depot_tools is in your PATH and you've run `gclient sync`.

### "use_sysroot" errors
The sysroot is automatically downloaded by gclient. If it fails, try:
```bash
gclient sync --force --reset
```

### Build fails with missing headers
Ensure all build dependencies are installed. The Chromium build has extensive requirements.

### Slow build
ARM cross-compilation is CPU-intensive. Use `autoninja` (which auto-detects parallelism) or set `-jN` manually based on your CPU cores.

## Architecture Notes

- `target_cpu = "arm"` builds for 32-bit ARM hard-float (armhf)
- `target_cpu = "arm64"` builds for 64-bit ARM (aarch64)
- The build system uses Debian Bullseye sysroots for cross-compilation
- Vulkan/SwiftShader support may be limited on 32-bit ARM builds
