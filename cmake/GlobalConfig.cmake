# 设置构建生成的根目录
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")

# 添加源码目录
set(SOURCE_PATH "${CMAKE_SOURCE_DIR}/src")

# 安装系统底层依赖
set(INSTALL_SYSTEM_DIR "/System")

# 安装目标目录
set(INSTALL_INCLUDE_DIR "/usr/include/TinyPiX")
# 安装资源文件
set(INSTALL_RESOURCE_DIR "/usr/res/TinyPiX")
# 安装数据文件
set(INSTALL_DATA_DIR "/usr/data/TinyPiX")
# 安装bin文件
set(INSTALL_BIN_DIR "/usr/bin/TinyPiX")
# 安装bin文件
set(INSTALL_LIB_DIR "/usr/lib/TinyPiX" CACHE PATH "Global library installation directory")
# 定义可执行程序的软链接目录
set(LINK_BIN_DIR "/usr/bin")
