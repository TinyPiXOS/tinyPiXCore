# 设置构建生成的根目录
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")

# 设置不同类型的目标文件的输出目录
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/bin")

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
# 定义动态库软链接的目标目录
set(LINK_LIB_DIR "/usr/lib")
