# TinyPiXOS 🚀

<div align="center">

![License](https://file.tinypixos.com/tinypixos/License-Apache2.0.svg)  ![Language](https://file.tinypixos.com/tinypixos/language-c++-red.svg)  ![Platform](https://file.tinypixos.com/tinypixos/platform-linux-lightgrey.svg)

[📖 TinyPiXOS Official Site](https://www.tinypixos.com/)

</div>

TinyPiXOS includes four main parts: a window manager (TpWM), core library (TpCore), GUI library (TpGUI), and utility library (TpUtils).

---

# 1. Project Overview 📚

**TinyPiXOS** is built on open-source Linux. Through innovative kernel-level lightweight refactoring and streamlined design, it abandons traditional graphics stacks such as X11/Wayland. Using pure C/C++, it rebuilds an ultra‑lightweight end-to-end graphics stack from the ground up—including efficient window management, a minimal GUI framework, and deeply optimized kernel mechanisms.

TinyPiXOS signifies a self-owned, complete foundational stack built from scratch. Its core positioning is to provide an embedded desktop OS development platform that is **independent, lightweight, and highly customizable**.  

**Core goals:** With minimal hardware resources, ensure smooth multi-application concurrency and deliver complete desktop-grade core capabilities (resource management, peripheral drivers, graphics rendering, etc.).

## TpWM

TpWM is the window management engine of TinyPiXOS and a core pillar of the system. The engine focuses on minimal, streamlined interfaces, discarding redundant complexity. Inspired by RISC-style minimal instruction sets, it offers window designers flexible building blocks for free composition, enabling highly customizable products with diverse behaviors.

## TpService

TpService is TinyPiXOS's background service module, built on nanomsg for efficient messaging. It currently provides inter-process communication (IPC) and LAN device communication, and will gradually expand into more powerful system service nodes.

## TpCore

The TpCore library is the core library for TinyPiXOS applications. It provides signals/slots, fundamental data structures, file operations, JSON processing, and generic types.

---

- Unified data abstraction: TpString, TpList, TpHash, TpVector serve as common container base types across the framework, building a unified data structure system to ensure type safety and memory efficiency.
- Efficient serialization: Unified conversion between binary and JSON formats, supporting fast transforms between structs and memory buffers for high-performance storage and transport.
- Event-driven scheduling: Built-in signals/slots based on the observer pattern enable loosely coupled component communication with async event propagation and callbacks.

---

## TpGUI

The TpGUI library provides the core GUI framework for TinyPiXOS applications. It implements a comprehensive widget-based UI system with CSS styling, event handling, and animation. TpGUI is the foundation for building graphical applications on TinyPiXOS.

---

- Unified object management: `TpObject` acts as the common base class across modules, establishing a unified object lifecycle management system for rigorous resource handling.  
- Efficient event dispatch: Unified processing and propagation of user input and system events.  
- Smart memory management: Automatic or semi-automatic widget memory handling reduces risk of leaks.  
- Rendering scheduler: Optimized drawing pipeline coordinating component update order to ensure smooth UI.  
- Declarative styling: Powerful stylesheet system enables fast, declarative customization of widget look and overall UI themes, avoiding hard-coding.  

---

## TpUtils

The TpUtils library provides comprehensive system-level integration capabilities for TinyPiXOS applications. It bridges high-level application logic and low-level system resources, offering standardized interfaces for media processing, network communication, Bluetooth connectivity, application lifecycle management, hardware monitoring, and device management.

---

- Unified media processing: `TpAudioInterface` and `TpVideoInterface` provide a unified abstraction layer for A/V, integrating FFmpeg codecs and supporting efficient playback and recording of multi-format media.
- Device management: `TpDiskManage` and `TpUsbManager` auto-detect and mount hot-plug devices, with signal-driven notifications for state changes to reduce peripheral management complexity.
- High-performance networking: Built-in TCP/UDP socket framework with standardized interfaces (`TpTcpSocket`, `TpUdpSocket`, `TpTcpServer`), supporting async event-driven connection management and data transfer.
- Bluetooth stack: Based on BlueZ, supports full device pairing, audio transport, and file exchange; `TpBluetoothLocal` and `TpBluetoothAudioManager` provide declarative device management APIs.
- Application lifecycle: `TpAppInstall` and `TpAppDopack` implement sandboxed app installation, supporting permission control, dependency injection, and cryptographic signature verification for security.
- System resource monitoring: `TpCpuManage`, `TpMemory`, and `TpDiskManage` collect CPU/memory/disk I/O metrics in real time to support performance optimization.
- Display system integration: `TpDisplay` provides DPI scaling and resolution adaptation with the `TpWM` window manager, supporting multi-monitor setups and runtime resolution switching.

---


# 2. Key Advantages 🌟

- Independent and controllable: In-house TpWM window manager and GUI framework; no dependency on X11/Wayland.  
- Lightweight and flexible: Modular architecture with on-demand trimming, optimized memory and process scheduling, extreme resource efficiency; deployable from tiny embedded devices to high-performance mobile terminals.  
- Heterogeneous compatibility: Unified C/C++ APIs with seamless migration across ARM/x86/RISC-V platforms.  
- Ready to use: Built-in UI component library accelerates application development.  
- Long-term maintenance: Dedicated technical team ensures ongoing support and updates.  
- Chinese support: Friendly Chinese text rendering throughout the system.  
- Free for commercial use: Free to use commercially (retain copyright notice); enterprises can build customized systems at low cost to accelerate product delivery.  

---

# 3. Architecture 💻

TinyPiXOS Architecture Diagram

<div align="center">
<img src="https://file.tinypixos.com/tinypixos/TinyPiXOS整体架构图.jpg" width="600" height="500" >
</div>
---

# 4. License 📜

**TinyPiXOS** is released under the **Apache License 2.0**. You may freely use, modify, and distribute the code as long as you comply with the license terms. Apache 2.0 is a permissive license that allows use in commercial projects while retaining the original author's copyright notice.

---

# 5. Open-Source Goals 🎯

As a desktop operating system targeting lightweight hardware platforms, **TinyPiXOS** features a complex architecture and numerous functional modules. While the current version provides foundational capabilities, it still needs community contributions to mature. We open source the core framework to aggregate developer wisdom and co-build a domestic embedded OS ecosystem.

- Co-build a domestic foundation: Use TpGUI and other core modules as the base to jointly create an independent, controllable embedded development ecosystem.  
- Inclusive co-creation: The current version may have shortcomings; please provide constructive feedback from a developer perspective so we can improve together.  
- Create unlimited possibilities: Encourage innovative applications built on the open-source framework; outstanding projects will receive official technical support.  
- Ongoing commitment: The core team will maintain the desktop and application ecosystem long term, releasing key updates regularly.  

---

# 6. How to Contribute 🤝

## Project Build

### System Environment

- Operating system: Ubuntu 22.04.4 LTS  
- Compilers: gcc/g++ 11.4.0  
- Build tools: >= CMake 3.5.0  
- Language standard: C++11  

In principle there is no OS restriction; validation has currently been done on Ubuntu 22.04.4 LTS only. Compiler and build tool versions must meet or exceed the specified versions.

### Source Code Download

Fetch the code using the Gitee mirror or the synchronized GitHub repository:

```bash
# git clone https://github.com/TinyPiXOS/TinyPiXOS.git
git clone https://gitee.com/tinypixos/TinyPiXOS.git
```

### Dependency List

| Dependency | Library filename | Recommended version (included in project) | Supported versions |
|:------:|:------:|:------:|:------:|
| libasound2-dev    | libasound.so  | 1.2.6.1   | 1.2.6.1 |
| libavcodec-dev    | libavcodec.so    | 7:4.4.2  | 7:4.4.2, 7:3.4.11 |
| libavformat-dev   | libavformat.so   | 7:4.4.2  | 7:4.4.2, 7:3.4.11 |
| libavutil-dev     | libavutil.so     | 7:4.4.2  | 7:4.4.2, 7:3.4.11 |
| libswscale-dev    | libswscale.so    | 7:4.4.2  | 7:4.4.2, 7:3.4.11 |
| libswresample-dev | libswresample.so | 7:4.4.2  | 7:4.4.2, 7:3.4.11 |
| libavfilter-dev   | libavfilter.so   | 7:4.4.2  | 7:4.4.2, 7:3.4.11 |
| libavdevice-dev   | libavdevice.so   | 7:4.4.2  | 7:4.4.2, 7:3.4.11 |
| libssl-dev        | libssl.so        | 3.0.2    | 3.0.2 |
| librsvg2-dev      | librsvg-2.so     | 2.52.5   | 2.52.5  |
| libbluetooth      | libbluetooth.so  | 5.64     | 5.64    |
| libdbus           | libdbus-1.so     | 1.12.20  | 1.12.20 |
| bluez-alsa-utils  |                  |          | 3.0.0-2 |
| libasound2-plugin-bluez|             |          | 3.0.0-2 |
| bluez-obexd       |                  |          | 5.64-0  |
| libusb-1.0        |                  |          | 1.0.25  |

### Build and Install

- **Install dependencies**

```bash
sudo apt install \
  libsdl2-dev libcairo2-dev libpango1.0-dev libglib2.0-dev \
  libpangocairo-1.0-0 libfontconfig-dev libfreetype-dev \
  libgbm-dev libgles2 libegl-dev \
  libasound2-dev libssl-dev libavcodec-dev libavformat-dev \
  libavutil-dev libswscale-dev libswresample-dev \
  libavfilter-dev libavdevice-dev librsvg2-dev bluez libbluetooth-dev \
  libdbus-1-dev bluez-alsa-utils libasound2-plugin-bluez bluez-obexd  libusb-1.0-0-dev \
  libleveldb-dev libmarisa-dev libopencc-dev libyaml-cpp-dev libgoogle-glog-dev
```

- **Build TinyPiXOS dependency submodules (optional)**

```bash
# 更新子模块
git submodule update --init --recursive
# 构建并安装所有子模块依赖库
make -f deps.mk
```

- **Build TinyPiXOS Debug version**

```bash
cmake .
make
make install
```

Or specify the build preset explicitly

```bash
cmake --preset=debug
make
make install
```

- **Build TinyPiXOS Release version**

```bash
cmake --preset=release
make
make install
```

- **Cross-compile TinyPiXOS Arm64-Debug (replace 64 with 32 for 32-bit)**

```bash
cmake --preset=arm64-debug \
    -DCMAKE_C_COMPILER=/your/custom/path/arm-linux-gnueabihf-gcc \
    -DCMAKE_CXX_COMPILER=/your/custom/path/arm-linux-gnueabihf-g++ \
    -DARM_SDK_PATH=/your/sdk/path
make
make install
```

- **Cross-compile TinyPiXOS Arm64-Release (replace 64 with 32 for 32-bit)**

```bash
cmake --preset=arm64-release \
    -DCMAKE_C_COMPILER=/your/custom/path/arm-linux-gnueabihf-gcc \
    -DCMAKE_CXX_COMPILER=/your/custom/path/arm-linux-gnueabihf-g++ \
    -DARM_SDK_PATH=/your/sdk/path
make
make install
```

### Uninstall

```bash
make uninstall
```

### Usage

#### Directory Layout

- **Executable install path**  
  `/usr/bin/TinyPiX`
- **Header install path**  
  `/usr/include/TinyPiX`
- **Shared library install path**  
  `/usr/lib/TinyPiX`
- **Resource install path**  
  `/usr/res/TinyPiX`
- **Data install path**  
  `/usr/data/TinyPiX`
- **System support files path**  
  `/System`

#### Linking in Applications

```bash
export LD_LIBRARY_PATH="/usr/lib/TinyPiX:$LD_LIBRARY_PATH"
```

```cmake
# TpCore
include_directories(/usr/include/TinyPiX/TpCore)
link_directories("/usr/lib/TinyPiX")
link_directories("/usr/lib/TpWM")
target_link_libraries(your_app_name TpCore)

# TpGUI
include_directories(/usr/include/TinyPiX/TpGUI)
link_directories("/usr/lib/TinyPiX")
target_link_libraries(your_app_name TpGUI)

# TpUtils
include_directories(/usr/include/TinyPiX/TpUtils)
link_directories("/usr/lib/TinyPiX")
target_link_libraries(your_app_name TpUtils)
```

#### Configuration Parsing

### 2. Configuration Parsing

Configuration file path: `/System/conf/tinyPiX.conf`

Sample configuration; do not copy comments into `tinyPiX.conf`:

```ini
[display-setting]
width     = 1080   # Screen width (recommended default)
height    = 720    # Screen height (recommended default)
format    = 32     # Color depth (not recommended to change)

[attribute-setting]
daemon      = 0    # 0=foreground, 1=background
acclerate   = 1    # 0=disable hardware acceleration, 1=enable
brightness  = 255  # Screen brightness (0-255)
sharemem    = 0    # 0=disk; 1=memory
shareone    = 1    # 0=multiple apps without shared memory; 1=use shared memory

[system-setting]
simulator   = 1
quitwait    = 15

[mode-setting]
startdir = /opt/project-main/tinyPiXApp/Application/deskTop/bin/
startapp = TpDesktop
```

## Contributing

- Submit issues: Use the repository Issues page to report problems or propose improvements.
- Contribute code: Follow the contribution guide to submit Pull Requests and help improve the project.
- Join discussions: Participate in the Knowledge Planet community to exchange experience with other developers.

## Feedback and Suggestions

### 📝 Pre-submission Checklist

> [!TIP]
✅ Searched for similar issues in [TinyPiXOS Issues](https://github.com/TinyPiXOS/TinyPiXOS/issues).  
✅ Verified with the latest version and the issue still exists.  
✅ Searched in [Knowledge Planet](https://t.zsxq.com/JzbkN) but found no similar issues.  

### Feedback Channels

#### Repository Issues

Submit issues or improvement suggestions on the repository Issues page.

#### Knowledge Planet Feedback

> [!TIP]
If you are interested in the project and have not yet joined Knowledge Planet, we recommend joining [Knowledge Planet](https://t.zsxq.com/JzbkN) to learn TinyPiXOS in depth and exchange with the community.  

🎁 Join ["TinyPiXOS Developer Alliance" Knowledge Planet](https://t.zsxq.com/JzbkN)

- Use the platform to browse historical answers and submit new feedback

![Issue Feedback - Knowledge Planet](http://file.tinypixos.com/tinypixos/20250707175310357_repeat_1751881992501__128722.png)

#### Questionnaire Feedback

<div align="center">

<img src="http://file.tinypixos.com/tinypixos/问题反馈问卷表单二维码_repeat_1751882497523__731873.png" width="250" height="250" alt="Feedback Questionnaire QR Code">

</div>

[Submit Feedback Questionnaire](https://wj.qq.com/s2/22794485/2341/)

#### Email Feedback

TinyPiXOS developer support email
📧 <dev@tinypixos.com>

## Follow Us

<div align="center">
<img src="https://file.tinypixos.com/tinypixos/关注我们_repeat_1752211821267__540343.jpg" >
</div>

## How to Learn TinyPiXOS Systematically

- Around the TinyPiXOS tech stack, we built the “Build a Desktop OS from Scratch” curriculum, including “TinyPiXOS System Learning”, “TinyPiXApp Application Development Practice”, and “TpWM Advanced Usage Practice”, plus several sub-tracks—aiming to progressively upskill developers to master full-stack embedded OS development.
- To deepen mastery of the TinyPiXOS stack while breaking domain boundaries, we invite cross-domain experts in embedded software, hardware, AI, and domesticization to provide guidance, helping you build a multi-dimensional skill matrix and achieve depth plus breadth.

<div align="center">
<img src="https://file.tinypixos.com/tinypixos/其他公开网站.png" alt="Knowledge Planet">
</div>

## Core Team

<div align="center">
<img src="https://file.tinypixos.com/tinypixos/团队.png" width="550" height="300" alt="Core Team">
</div>

## Support the Authors

<div align="center">
<img src="https://file.tinypixos.com/tinypixos/微信赞赏二维码.png" width="300" height="300" alt="WeChat Appreciation QR Code">
</div>
