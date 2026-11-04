# OpenRGB with AULA GS8 support

This repository is a fork of [OpenRGB](https://gitlab.com/CalcProgrammer1/OpenRGB) that adds a native
controller for the **AULA GS8** USB gaming soundbar.

> **Not affiliated with the OpenRGB project.** This is an independent fork maintained for the purpose
> of adding AULA GS8 support. The upstream OpenRGB project and its authors are not responsible for
> this build. For the official OpenRGB releases, see <https://openrgb.org>.

## What this fork adds

A new `AulaGS8Controller` under `Controllers/AulaGS8Controller/` that talks to the AULA GS8's
lighting MCU over a 16-byte HID output report (VID `0x3654`, PID `0x0517`, interface 3, collection
`COL02`). The controller exposes 22 addressable LEDs in **Direct** mode only; the speaker's built-in
dynamic effects are driven by its own firmware and cannot be selected from the host.

The protocol was reverse engineered from USB captures of the AULA "Shenguang" sync application.

- `AulaGS8Controller.h` / `.cpp` — low-level HID driver
- `AulaGS8ControllerDetect.cpp` — detector
- `RGBController_AulaGS8.h` / `.cpp` — OpenRGB device binding

## Upstream baseline

This fork is based on OpenRGB commit

```
c6fc801
```

from `https://gitlab.com/CalcProgrammer1/OpenRGB`. The original upstream README is preserved as
[`README-upstream.md`](README-upstream.md).

## Supported platforms

The controller uses the cross-platform `hidapi` and has no OS-specific calls other than a
case-insensitive path comparison in the detector (needed because hidapi returns mixed-case paths on
Windows). It should build and run on Windows, Linux, and macOS, though it has only been tested on
Windows so far.

## Building

This fork ships the same build system as upstream. On Windows the maintainer builds with the
MSYS2 MinGW64 toolchain and Qt 5.15.

### Windows (MSYS2 MinGW64)

```sh
pacman -S --needed mingw-w64-x86_64-qt5 mingw-w64-x86_64-hidapi \
    mingw-w64-x86_64-libusb mingw-w64-x86_64-mbedtls \
    mingw-w64-x86_64-toolchain make

qmake OpenRGB.pro
make -j$(nproc)
```

The resulting binary is `release/OpenRGB.exe`.

### Linux

```sh
sudo apt install qtbase5-dev libqt5svg5-dev libhidapi-dev libusb-1.0-0-dev \
    libmbedtls-dev build-essential pkg-config

qmake OpenRGB.pro
make -j$(nproc)
```

### macOS

```sh
brew install qt@5 hidapi libusb mbedtls
$(brew --prefix qt@5)/bin/qmake OpenRGB.pro
make -j$(sysctl -n hw.ncpu)
```

## Usage

1. Plug in the AULA GS8.
2. Start `OpenRGB`. The speaker should appear in the device list as **AULA GS8**.
3. Use the GUI, the command line, or the OpenRGB SDK to set colors.

### Headless SDK server

```sh
OpenRGB.exe --server --server-port 6742 --startminimized --noautoconnect
```

## Third-party components and licenses

This fork redistributes the OpenRGB source tree under the terms of the
[GNU General Public License v2.0 or later](LICENSE). In addition, the build depends on several
third-party libraries whose licenses are reproduced in their respective directories:

| Component | License |
|-----------|---------|
| Qt 5 | LGPL-3.0 |
| libusb | LGPL-2.1 |
| hidapi (with hotplug patch) | GPL-3.0 / HIDAPI license (see `dependencies/hidapi`) |
| mbedTLS | Apache-2.0 |
| hueplusplus | LGPL-3.0 |
| cpp-httplib | MIT |
| nlohmann/json | MIT |
| PawnIO | See `dependencies/pawnio` |

No additional third-party code was introduced by this fork beyond what upstream already includes.

## Modifications to upstream files

In addition to the new files under `Controllers/AulaGS8Controller/`, the following upstream files
were modified to register the controller:

- `OpenRGB.pro` — added the new controller sources and headers
- `Controllers/ControllerDetector.cpp` / `ControllerDetector.h` — registered the detector

Each modified file keeps its original copyright header; the changes are marked in the usual way.

## Reverting to upstream

To remove this fork's changes and go back to a stock OpenRGB build, check out the baseline commit:

```sh
git checkout c6fc801
```

or simply remove the `Controllers/AulaGS8Controller/` directory and revert the detector
registration in `ControllerDetector`.

## License

This fork is distributed under the same license as upstream OpenRGB:
**GNU General Public License v2.0 or later**. See [LICENSE](LICENSE) for the full text.
