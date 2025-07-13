# tinyPiXCore 🚀

<div align="center">

![License](https://file.tinypixos.com/tinypixos/License-Apache2.0.svg)  ![Language](https://file.tinypixos.com/tinypixos/language-c++-red.svg)  ![Platform](https://file.tinypixos.com/tinypixos/platform-linux-lightgrey.svg)

[📖 TinyPiXOS官网](https://www.tinypixos.com/)

</div>

tinyPiXCore 提供tinyPiXOS的底层支撑库，包括GUI组件（PiXSingleGUI）、工具库（PiXUtil）、扩展库（PiXExternUtils）三大部分。

---

# 一、项目简介 📚

**TinyPiXOS** 以开源Linux为基础，通过创新的内核级轻量化改造与精简设计，摒弃了X11/Wayland等传统图形方案，运用纯C/C++从底层重构出超轻量级的整体图形技术栈——包含高效窗口管理、精简GUI框架及深度优化的内核机制。

**核心目标：** 在最小化硬件资源配置的前提下，确保流畅的多应用并行处理能力，提供完备的桌面级核心功能体验（资源管理、外设驱动、图形渲染等）。

**TinyPiXOS**，以硬核技术实力追求极致性能与效率！

## PiXSingleGUI

PiXSingleGUI库是​​tiniPiXCore框架的核心GUI组件库​​，采用​​分层架构设计​​，致力于为应用提供一套​​完整的、高性能的图形用户界面解决方案​​。

统一基类与核心机制​​
----------------

- ​统一对象管理：​​ 提供tpObject作为框架中所有模块组件的​​公共基类​​，建立​​统一的对象生命周期管理体系​​，确保资源管理的严谨性。
- ​控件抽象核心：​​ 以tpChildWidget作为​​所有GUI控件​​（Widget）的​​抽象基类​​。此基类内部集成以下关键机制：
  - ​高效事件分发：​​ 统一处理和传播用户输入、系统事件。
  - ​智能内存管理：​​ 自动或半自动处理控件内存，降低资源泄漏风险。
  - ​渲染调度系统：​​ 优化绘制流程，协调组件间的渲染顺序与更新，保障界面流畅性。

标准化控件与性能保障​​
----------------

- ​开箱即用的UI组件：​​ 提供一系列​​经过精心设计和性能优化​​的​​标准化UI控件​​。
- ​​丰富控件示例：​​ 典型控件包括：
  - ​tpComBox：​​ 支持​​动态数据绑定​​的下拉选择框。
  - ​tpLineEdit：​​ 集成了​​输入法支持​​的文本输入框。
- ​​卓越性能：​​ 所有控件设计均以高效运行为核心目标，确保即使在​​嵌入式或资源受限的环境​​下也能展现​​流畅的用户体验​​。

​扩展性与灵活性​​
----------------

- ​声明式样式定制：​​ 内建强大的​​样式表系统​​，允许开发者通过​​声明式配置​​快速、便捷地定制控件外观和整体界面风格，​​避免硬编码​​。
- ​​开放式组件扩展：​​ 提供​​标准的插件化扩展接口​​。开发者可以通过这些​​规范接口​​，轻松实现并集成​​自定义GUI组件​​，有效扩展库的功能边界。

核心价值
----------------

PiXSingleGUI库通过其​***​分层的架构***、***坚实的基类支撑***、***丰富的高性能标准控件***、***灵活强大的样式系统和扩展接口***​​，在***​​GUI开发效率***​​与​***​界面运行的灵活性及高效性​***​之间取得了理想的平衡点，是构建tiniPiXCore应用程序用户界面的可靠基石。

---

## PiXUtils

PiXUtils​​ 是 ​​tiniPiXCore​​ 框架的核心工具库，提供高效通用功能模块，涵盖数据序列化、文件操作、通信机制及常用数据结构等关键功能，大幅提升开发效率与代码复用性。

​核心功能模块​​
----------------

- ​**​二进制序列化**​​：支持结构体与内存缓冲区的快速互转（Buffer 序列化/反序列化），适用于高性能数据存储与传输。  
​- **​JSON 处理**​​：提供结构体与 JSON 的双向转换，并集成高效的文件读写接口，简化配置文件管理。  
- **​​CSS 解析器​**​：支持样式表解析与动态加载，便于 GUI 控件的运行时样式调整。  
- ​**​日期与定时器**​​：提供高精度时间计算、定时任务调度及时间格式化功能。  
- ​**​文件系统操作​**​：封装跨平台文件/目录管理接口，支持异步 IO 操作。  
- ​**​节点间通信网关​**​：实现局域网内设备间的轻量级通信协议。  
- ​**​信号槽系统​​**：基于观察者模式的事件驱动机制，支持松耦合组件交互。  
- ​**​泛型封装**​​：提供类型安全的模板容器，如动态数组（tpVector）、双向链表（tpList）、哈希表（tpHash）及高性能字符串（tpString），优化内存与计算效率。  

**PiXUtils 通过高度封装的工具链，显著降低底层开发复杂度，​模块化架构​​：各功能解耦设计，支持按需链接，降低最终程序体积。**

## PiXExternUtils

PiXExternalUtils 提供系统级基础能力。

核心功能模块
----------------

- **标准化打包**  
  支持交互式配置与JSON描述文件生成安装包
- **沙箱化安装**  
  - 自动构建隔离目录结构  
  - 权限控制粒度配置  
  - 智能依赖注入管理  

- **外设管理核心**  
  - 蓝牙设备配对认证  
  - 动态磁盘挂载检测  
  - 多声道输出切换  
- **系统资源看板**  
  - 实时采集CPU/内存/磁盘I/O等系统指标
- **热插拔响应引擎**  
  - 自动感知设备连接状态变化并触发回调

- **文件服务**  
  - ZIP流式解压  
  - Gzip内存压缩  
  - TAR增量打包  
- **网络通信**  
  - UDP广播发现  
  - TCP长连接池管理  

# 二、系统优势 🌟

相较于目前成熟的国产桌面操作系统及其所依赖的现有GUI框架，我们在核心模块的稳定性和应用生态的完备性方面，​仍存在一定的追赶距离。

**TinyPiXOS** 的诞生标志着我们从零起步，完成了基础技术栈的完整构建与自主掌控。其核心定位在于**提供一个独立可控、架构轻量且高度定制化的嵌入式桌面操作系统开发平台**。

**自主可控：** 自主研发窗口管理器PiXWM与GUI框架，不依赖X11/Wayland。

**轻量灵活：** 模块化架构按需裁剪，内存与进程调度优化，资源极致利用，从微型嵌入式设备到高性能移动终端均可灵活部署。

**异构兼容：** 统一C/C++开发接口，ARM/x86/RISC-V多平台无缝迁移。

**开箱即用：** 预置UI组件库，方便用户快速上手开发应用。

**长期维护：** 技术团队长期维护，不用担心遇到问题导致项目无法推动。

**中文支持：** 目前技术文档仅有中文文档，系统内文字中文支持友好。

**免费商用：** 免费商用（需保留版权标识），企业可低成本构建定制化系统，支撑产品快速落地。

---

# 三、技术架构 💻

TinyPiXOS整体架构图

![TinyPiXOS](https://file.tinypixos.com/tinypixos/TinyPiXOS整体架构图.jpg)

---

# 四、开源协议 📜

**TinyPiXOS** 采用 **Apache License 2.0** 开源协议。您可以自由使用、修改和分发代码，但需遵守协议条款。Apache License 是一种宽松的开源协议，允许您在商业项目中使用本项目代码，同时保留原作者的版权声明。

---

# 五、开源目标 🎯

**TinyPiXOS** 作为一款面向轻量化硬件平台的桌面操作系统，其架构设计复杂、功能模块众多。目前现有版本虽已实现基础能力，仍需要开发者社区的力量共同完善。我们选择将核心框架开源，旨在汇聚开发者智慧，共同打造国产嵌入式操作系统生态基座。

🛠️共建国产基座：以PiXSingleGUI等三大模块为基座，联合打造自主可控的嵌入式开发生态

🤝包容共创：现有版本难免存在不足，请以开发者视角包容反馈，共同优化

🚀创造无限可能：鼓励基于开源框架构建创新应用，优秀项目将获官方技术支持

🔄持续承诺：核心团队将长期维护系统桌面与应用生态，定期发布关键更新

---

# 六、如何参与 🤝

## 项目构建

### 系统环境

- ​**操作系统**: Ubuntu 22.04.4 LTS  
- ​**编译器**: gcc g++ 11.4.0  
- ​**构建工具**: >=CMake 3.5.0  
- ​**语言标准**: C++11  

理论上对操作系统无限制；目前只在 Ubuntu 22.04.4 LTS 进行了验证。编译器版本和构建工具必须相同或高于要求版本。

### 源码下载

拉取代码，可以使用Github地址直接下拉，或者使用Gitee同步仓库拉取

```bash
git clone https://github.com/TinyPiXOS/tinyPiXCore.git
# git clone https://gitee.com/tinypixos/tinyPiXCore.git
```

### 构建安装

- ​**安装 PiXWM 基础库**

```bash
# 需要先安装完成 PiXWM 相关依赖；参考官网 PiXWM 安装教程
```

- ​**安装 依赖环境**

```bash
sudo apt install libsdl2-image-dev libsdl2-gfx-dev \
  libcairo2-dev libpango1.0-dev libglib2.0-dev \
  libpangocairo-1.0-0 libfontconfig-dev libfreetype-dev \
  libgbm-dev libgles2 libegl-dev \
  libasound2-dev libjson-c-dev libssl-dev libavcodec-dev libavformat-dev \
  libavutil-dev libswscale-dev libswresample-dev \
  libavfilter-dev libavdevice-dev libssl-dev librsvg2-dev bluez libbluetooth-dev \
  libdbus-1-dev bluez-alsa-utils libasound2-plugin-bluez bluez-obexd  libusb-1.0-0-dev \
  libboost-all-dev libleveldb-dev libmarisa-dev libopencc-dev libyaml-cpp-dev libgoogle-glog-dev
```

- ​**构建 tinyPiXCore**

```bash
cmake .
make
make install
```

### 使用说明

#### 目录结构

- ​**头文件安装路径**​  
  `/usr/include/tinyPiX`
- ​**动态库安装路径**​  
  `/usr/lib/tinyPiX`
- ​**资源文件安装路径**​  
  `/usr/res/tinyPiX`
- ​**数据文件安装路径**​  
  `/usr/data/tinyPiX`

#### 应用程序库引入

```cmake
# SingleGUI引入：
include_directories(/usr/include/tinyPiX/SingleGUI)
link_directories("/usr/lib")
target_link_libraries(你的应用程序名称 PiXSingleGUI)

# Utils引入：
include_directories(/usr/include/tinyPiX/Utils)
link_directories("/usr/lib")
target_link_libraries(你的应用程序名称 PiXUtils)

# ExternUtils引入
include_directories(/usr/include/tinyPiX/ExternUtils)
link_directories("/usr/lib")
target_link_libraries(你的应用程序名称 PiXExternUtils)

# desktopGUI引入
include_directories(/usr/include/tinyPiX/SingleGUI)
include_directories(/usr/include/tinyPiX/Utils)
include_directories(/usr/include/tinyPiX/ExternUtils)
include_directories(/usr/include/tinyPiX/GUIFramework/desktopGUI)
link_directories("/usr/lib")
target_link_libraries(你的应用程序名称 PiXDesktopGUI PiXSingleGUI PiXUtils PiXExternUtils)
```

## 贡献代码

- **提交问题**：在开源仓库的 Issues 页面提交问题或改进建议。
- **贡献代码**：按照贡献指南提交 Pull Request，帮助完善项目。
- **参与讨论**：加入知识星球社区，与其他开发者交流经验。

## 反馈与建议

### 📝 提交前自查

> [!TIP]
✅ 已在 [tinyPiXCore Issues](https://github.com/TinyPiXOS/tinyPiXCore/issues)和[tinyPiXApp Issues](https://github.com/TinyPiXOS/tinyPiXApp/issues)搜索过同类问题。  
✅ 使用最新版本进行验证后，问题仍然存在。  
✅ 在[知识星球](https://t.zsxq.com/JzbkN)当中检索，但是未找到同类问题。  

### 问题反馈渠道

#### 开源仓库Issues

在开源仓库的 Issues 页面提交问题或改进建议。

#### 知识星球反馈

> [!TIP]
如果您对项目很感兴趣，还未加入知识星球，我们建议您加入[知识星球](https://t.zsxq.com/JzbkN)深度了解和学习TinyPiXOS开源项目，与星球众多伙伴共同交流进步。  

🎁加入[“TinyPiXOS开发者联盟”知识星球](https://t.zsxq.com/JzbkN)，==即赠送20个专业版授权额度==。  

`活动随时取消，请有需要的个人/企业抓住机会！`

- 通过星球查询历史问题回复和进行新问题反馈

![问题反馈-星球](http://file.tinypixos.com/tinypixos/20250707175310357_repeat_1751881992501__128722.png)

#### 问卷表单反馈

<div align="center">

<img src="http://file.tinypixos.com/tinypixos/问题反馈问卷表单二维码_repeat_1751882497523__731873.png" width="300" height="300" alt="问卷表单二维码">

</div>

[提交问题反馈问卷](https://wj.qq.com/s2/22794485/2341/)

#### 邮箱反馈

TinyPiXOS开发者服务邮箱
📧 <dev@tinypixos.com>

## 关注我们

![关注我们](https://file.tinypixos.com/tinypixos/关注我们_repeat_1752211821267__540343.jpg)

## 如何系统学习TinyPiXOS

- 我们围绕TinyPiXOS项目技术栈，搭建了“从零构建桌面操作系统”课程，包括“TinyPiXCore系统学习”. “TinyPiXApp应用开发实战”和“PiXWM高级用法实战”三大板块和若干个子专栏，**通过阶梯式能力进阶，赋能开发者​体系化掌握嵌入式OS全栈开发能力**。
- 为使开发者深度掌握TinyPiXOS技术栈的同时突破领域局限，我们特邀**嵌入式软件开发、硬件开发、人工智能、国产化**等跨领域专家驻场指导，助你构建多维度能力矩阵，实现**领域纵深+技能广度的双重提升**。

![星球社区](https://file.tinypixos.com/tinypixos/其他公开网站.png)
