# OpenRGB with AULA GS8 support

This repository is a fork of [OpenRGB](https://gitlab.com/CalcProgrammer1/OpenRGB) that adds a native
controller for the **AULA GS8** USB gaming soundbar.

> **Not affiliated with the OpenRGB project.** This is an independent fork maintained for the purpose
> of adding AULA GS8 support. The upstream OpenRGB project and its authors are not responsible for
> this build. For the official OpenRGB releases, see <https://openrgb.org>.

## Download

A pre-built Windows x64 installer is available under
[Releases](https://github.com/selisun/OpenRGB-AULA-GS8/releases):

- OpenRGB 1.1 built with Qt 6.12 and MinGW, including the AULA GS8 controller
- OpenRGB Effects Plugin 1.1 compiled with the same toolchain (58 effects)
- Simplified Chinese UI and Chinese installer
- Registers a logon scheduled task that starts OpenRGB minimized with the SDK server enabled

PawnIO must be installed separately for motherboard onboard RGB control: <https://pawnio.eu/>

## What this fork adds

A new `AulaGS8Controller` under `Controllers/AulaGS8Controller/` that talks to the AULA GS8's
lighting MCU over a 16-byte HID output report (VID `0x3654`, PID `0x0517`, interface 3, collection
`COL02`). The controller exposes 22 addressable LEDs in **Direct** mode only; the speaker's built-in
dynamic effects are driven by its own firmware and cannot be selected from the host.

The protocol was reverse engineered from USB captures of the AULA "Shenguang" sync application.

- `AulaGS8Controller.h` / `.cpp` — low-level HID driver
- `AulaGS8ControllerDetect.cpp` — detector, registered through `REGISTER_HID_DETECTOR_I`
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

### Windows (MSYS2 MinGW64, Qt 6)

```sh
pacman -S --needed mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-svg \
    mingw-w64-x86_64-qt6-tools mingw-w64-x86_64-qt6-serialport \
    mingw-w64-x86_64-qt6-5compat mingw-w64-x86_64-hidapi \
    mingw-w64-x86_64-libusb mingw-w64-x86_64-mbedtls \
    mingw-w64-x86_64-toolchain

qmake6 OpenRGB.pro
mingw32-make -j$(nproc)
```

The resulting binary is `release/OpenRGB.exe`; deploy the Qt runtime with
`windeployqt6 release/OpenRGB.exe`.

Qt 5 also builds, but the official Effects Plugin releases are built with MSVC and Qt 6, so a Qt 5
build cannot load them. Use Qt 6 if you want the effects plugin.

### Linux and macOS

```sh
sudo apt install qtbase5-dev libqt5svg5-dev libhidapi-dev libusb-1.0-0-dev \
    libmbedtls-dev build-essential pkg-config
qmake OpenRGB.pro && make -j$(nproc)
```

The upstream build instructions in [`README-upstream.md`](README-upstream.md) still apply.

### Effects plugin

The plugin published on openrgb.org is built with MSVC, so a MinGW build of OpenRGB cannot load it
(`Cannot load library ... 找不到指定的程序`). Build the plugin from source with the same toolkit:

```sh
git clone https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin.git
git submodule update --init --recursive   # QCodeEditor, SimplexNoise
```

Two settings are required in `OpenRGBEffectsPlugin.pro` when building with MinGW:

```
mingw:QMAKE_CXXFLAGS += -B$$shell_quote($$[QT_INSTALL_BINS])
mingw:QMAKE_LFLAGS   += -B$$shell_quote($$[QT_INSTALL_BINS])
mingw:LIBS += -lole32 -loleaut32 -luuid -lgdi32 -luser32 -lwinmm -ladvapi32 -lshell32
```

Without `-B` the compiler silently fails to run the assembler; without the system libraries the link
step fails on `CoInitializeEx` and the GDI screen-capture calls. The plugin DLL is then placed in a
`plugins/` directory next to `OpenRGB.exe`.

## Usage

1. Plug in the AULA GS8.
2. Start `OpenRGB`. The speaker should appear in the device list as **AULA GS8**.
3. Use the GUI, the command line, or the OpenRGB SDK to set colors.

### Headless SDK server

```sh
OpenRGB.exe --server --server-port 6742 --startminimized --noautoconnect
```

Running OpenRGB as administrator (or through a scheduled task with highest privileges) is required
for the PawnIO driver, which handles motherboard SMBus lighting.

## Third-party components and licenses

This fork redistributes the OpenRGB source tree under the terms of the
[GNU General Public License v2.0 or later](LICENSE). In addition, the build depends on several
third-party libraries whose licenses are reproduced in their respective directories:

| Component | License |
|-----------|---------|
| Qt 6 / Qt 5 | LGPL-3.0 |
| libusb | LGPL-2.1 |
| hidapi (with hotplug patch) | GPL-3.0 / HIDAPI license (see `dependencies/hidapi`) |
| mbedTLS | Apache-2.0 |
| hueplusplus | LGPL-3.0 |
| cpp-httplib | MIT |
| nlohmann/json | MIT |
| PawnIO | See `dependencies/pawnio` |

No additional third-party code was introduced by this fork beyond what upstream already includes.

## Modifications to upstream files

The controller registers itself through `REGISTER_HID_DETECTOR_I`, so no detector registration table
had to be edited. The upstream files that were changed are:

- `OpenRGB.pro` — controller sources are picked up by the existing glob, but the project was adapted
  for MinGW and Qt 6: architecture detection from `QT_ARCH`, MSYS2 mbedTLS archives, `-B` for the
  Qt binutils, `WINVER`/`_WIN32_WINNT`, explicit Windows system libraries, exclusion of the
  duplicated `GenesisXenon200Controller.cpp`, and disabled include-based dependency scanning.
  `lrelease`/`embed_translations` are enabled so the UI translations are embedded.
- `dependencies/hueplusplus-1.2.0/src/TimePattern.cpp` — maps `setenv`/`unsetenv` to `_putenv_s`,
  which MinGW does not provide.

Each modified file keeps its original copyright header.

## Reverting to upstream

```sh
git checkout c6fc801
```

or remove `Controllers/AulaGS8Controller/` and revert the changes above.

## License

This fork is distributed under the same license as upstream OpenRGB:
**GNU General Public License v2.0 or later**. See [LICENSE](LICENSE) for the full text.

---

# 支持 AULA GS8 的 OpenRGB

本仓库是 [OpenRGB](https://gitlab.com/CalcProgrammer1/OpenRGB) 的衍生分支，为 **AULA GS8** USB 游戏音箱
新增了原生控制器。

> **本项目与 OpenRGB 官方无关。** 这是一个独立维护的衍生分支，目的只是增加 AULA GS8 支持。
> OpenRGB 官方项目及其作者不对本构建负责。官方版本请访问 <https://openrgb.org>。

## 下载

Windows x64 安装包见 [Releases](https://github.com/selisun/OpenRGB-AULA-GS8/releases)：

- 使用 Qt 6.12 + MinGW 编译的 OpenRGB 1.1，含 AULA GS8 控制器
- 同一工具链编译的 OpenRGB 灯效插件 1.1（58 个效果）
- 简体中文界面与中文安装向导
- 自动注册登录启动的计划任务，最小化运行并开启 SDK 服务器

主板板载灯控制需要单独安装 PawnIO：<https://pawnio.eu/>

## 本分支新增的内容

在 `Controllers/AulaGS8Controller/` 下新增 `AulaGS8Controller`，通过 16 字节 HID 输出报告与
AULA GS8 的灯控 MCU 通信（VID `0x3654`、PID `0x0517`、接口 3、集合 `COL02`）。控制器只提供
**Direct（直接）模式**下的 22 颗可寻址灯珠；音箱内置的动态效果由固件自行驱动，无法从主机选择。

协议是通过抓取 AULA「神光同步」软件的 USB 通信逆向得到的。

- `AulaGS8Controller.h` / `.cpp` —— 底层 HID 驱动
- `AulaGS8ControllerDetect.cpp` —— 检测器，通过 `REGISTER_HID_DETECTOR_I` 自动注册
- `RGBController_AulaGS8.h` / `.cpp` —— OpenRGB 设备绑定

## 上游基线

本分支基于 OpenRGB 提交

```
c6fc801
```

来源 `https://gitlab.com/CalcProgrammer1/OpenRGB`。上游原始 README 保留为
[`README-upstream.md`](README-upstream.md)。

## 支持的平台

控制器使用跨平台的 `hidapi`，除检测器中一处大小写不敏感的路径比较（Windows 下 hidapi 返回的
路径大小写混杂）外没有平台相关代码。理论上 Windows、Linux、macOS 均可构建运行，但目前只在
Windows 上实测过。

## 构建

### Windows（MSYS2 MinGW64 + Qt 6）

```sh
pacman -S --needed mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-svg \
    mingw-w64-x86_64-qt6-tools mingw-w64-x86_64-qt6-serialport \
    mingw-w64-x86_64-qt6-5compat mingw-w64-x86_64-hidapi \
    mingw-w64-x86_64-libusb mingw-w64-x86_64-mbedtls \
    mingw-w64-x86_64-toolchain

qmake6 OpenRGB.pro
mingw32-make -j$(nproc)
```

产物为 `release/OpenRGB.exe`，运行库用 `windeployqt6 release/OpenRGB.exe` 部署。

Qt 5 也能编译，但官方灯效插件是用 MSVC + Qt 6 构建的，Qt 5 版本无法加载。需要插件请使用 Qt 6。

### Linux 与 macOS

```sh
sudo apt install qtbase5-dev libqt5svg5-dev libhidapi-dev libusb-1.0-0-dev \
    libmbedtls-dev build-essential pkg-config
qmake OpenRGB.pro && make -j$(nproc)
```

上游的构建说明见 [`README-upstream.md`](README-upstream.md)。

### 灯效插件

openrgb.org 上发布的插件是 MSVC 编译的，MinGW 构建的 OpenRGB 无法加载
（报错 `Cannot load library ... 找不到指定的程序`）。需要用同一套工具链自行编译：

```sh
git clone https://gitlab.com/OpenRGBDevelopers/OpenRGBEffectsPlugin.git
git submodule update --init --recursive   # QCodeEditor、SimplexNoise
```

MinGW 下需要在 `OpenRGBEffectsPlugin.pro` 中补两处配置：

```
mingw:QMAKE_CXXFLAGS += -B$$shell_quote($$[QT_INSTALL_BINS])
mingw:QMAKE_LFLAGS   += -B$$shell_quote($$[QT_INSTALL_BINS])
mingw:LIBS += -lole32 -loleaut32 -luuid -lgdi32 -luser32 -lwinmm -ladvapi32 -lshell32
```

缺少 `-B` 时编译器无法调用汇编器，且不打印任何诊断信息就退出；缺少系统库时链接会在
`CoInitializeEx` 和 GDI 屏幕捕获函数上报未定义引用。编译完成后把插件 DLL 放到
`OpenRGB.exe` 同级的 `plugins/` 目录。

## 使用

1. 插上 AULA GS8。
2. 启动 `OpenRGB`，设备列表中应出现 **AULA GS8**。
3. 用图形界面、命令行或 OpenRGB SDK 设置颜色。

### 无界面 SDK 服务器

```sh
OpenRGB.exe --server --server-port 6742 --startminimized --noautoconnect
```

主板 SMBus 灯控依赖 PawnIO 驱动，需要以管理员权限运行（或通过最高权限的计划任务启动）。

## 第三方组件与许可

本分支按 [GNU 通用公共许可证 v2.0 或更高版本](LICENSE) 的条款分发 OpenRGB 源码树。构建过程还依赖
以下第三方库，其许可文件位于各自目录中：

| 组件 | 许可 |
|------|------|
| Qt 6 / Qt 5 | LGPL-3.0 |
| libusb | LGPL-2.1 |
| hidapi（含热插拔补丁） | GPL-3.0 / HIDAPI 许可（见 `dependencies/hidapi`） |
| mbedTLS | Apache-2.0 |
| hueplusplus | LGPL-3.0 |
| cpp-httplib | MIT |
| nlohmann/json | MIT |
| PawnIO | 见 `dependencies/pawnio` |

除上游已包含的内容外，本分支没有引入额外的第三方代码。

## 对上游文件的修改

控制器通过 `REGISTER_HID_DETECTOR_I` 宏自动注册，因此没有改动检测器注册表。实际修改的上游文件是：

- `OpenRGB.pro` —— 控制器源文件由原有的通配规则自动收集，但工程做了 MinGW 与 Qt 6 适配：
  用 `QT_ARCH` 推导架构、改用 MSYS2 的 mbedTLS 静态库、为 Qt 的 binutils 加 `-B`、
  补充 `WINVER`/`_WIN32_WINNT`、显式链接 Windows 系统库、排除被重复包含的
  `GenesisXenon200Controller.cpp`、关闭基于 include 的依赖扫描；同时启用
  `lrelease`/`embed_translations`，使界面翻译被嵌入程序。
- `dependencies/hueplusplus-1.2.0/src/TimePattern.cpp` —— 把 `setenv`/`unsetenv` 映射到
  `_putenv_s`，MinGW 没有提供这两个函数。

所有被修改的文件均保留其原始版权声明。

## 回退到上游

```sh
git checkout c6fc801
```

或删除 `Controllers/AulaGS8Controller/` 目录并撤销上述改动。

## 许可

本分支与上游 OpenRGB 采用相同许可：**GNU 通用公共许可证 v2.0 或更高版本**。完整文本见
[LICENSE](LICENSE)。
