#!/bin/bash
# 使用./dopack.sh <架构>
# 架构可以设置x86_64或arm_64,如果不设置会按照当前环境默认打包
# ====================== 配置区域 ======================
BASE_NAME="TinyPiXOS" 	# 生成的安装包的名字,会自动拼接架构和后缀
TMP_ROOT_DIR="package_build"	# 生成的临时文件的名字
KEEP_TMP_DIR=false		# 是否保留中间生成的打包源文件
SCRIPTS_DIR="config"	# 禁止修改，如果需要修改，需要同步修“改智能安装器“部分的SCRIPTS_DIR

if [ $# -ge 1 ]; then
    ARCH="$1"
else
    ARCH="auto"  # 默认自动检测
fi

# 源目录 → 目标路径映射 (完整保留多路径映射)
declare -A PATH_MAPPINGS=(
    # 相对路径会自动转换为绝对路径
    # 格式: [源目录]="模式:目标路径"
	# 模式支持: overwrite(覆盖) | merge(合并) | update(更新) | preserve(跳过以有同名文件)
    ["./{ARCH}/lib"]="overwrite:/usr/lib/TinyPiX"
	["./{ARCH}/bin"]="overwrite:/usr/bin/TinyPiX"

	["../src/depend_lib/dynamic/{ARCH}"]="update:/usr/lib/TinyPiX"
#	["../src/depend_lib/static/{ARCH}"]="update:/usr/lib/TinyPiX"
    
    # 示例 2: 数据目录重定位
    ["./{ARCH}/data"]="update:/usr/data/TinyPiX"  # 源目录安装到新位置
    
    # 示例 3: 头文件
    ["./{ARCH}/include"]="overwrite:/usr/include/TinyPiX"
    
    # 示例 4: 资源文件到自定义位置
	["./{ARCH}/res"]="update:/usr/res/TinyPiX"

    #System
    ["./system"]="overwrite:/System"

	# 系统基本构建环境（必须使用preserve模式）
	["./build/{ARCH}/lib"]="preserve:/usr/lib/TinyPiX/build"
	["./build/{ARCH}/bin"]="preserve:/usr/bin/TinyPiX/build"
	["./build/{ARCH}/libexec"]="preserve:/usr/libexec/TinyPiX/build"
	["./build/{ARCH}/etc"]="preserve:/usr/etc/TinyPiX"

	["./build/{ARCH}/systemd*.service"]="preserve:/usr/lib/TinyPiX/systemd"	#这个目录仅用于映射保存，实际安装位置为/usr/lib/systemd/system
)
# =====================================================

# ---------------------- 路径处理函数 ----------------------
# 安全目录创建
safe_mkdir() {
    mkdir -p "$1" || {
        echo "❌ 无法创建目录: $1" >&2
        exit 1
    }
}

# 解析架构
resolve_architecture() {
    if [ "$ARCH" = "auto" ]; then
        MACHINE_ARCH=$(uname -m)
        if [ "$MACHINE_ARCH" = "x86_64" ]; then
            echo "x86_64"
        elif [ "$MACHINE_ARCH" = "aarch64" ]; then
            echo "arm_64"
        else
            echo "❌ 无法自动检测系统架构: $MACHINE_ARCH" >&2
            exit 1
        fi
    else
        echo "$ARCH"
    fi
}

# 替换路径中的架构占位符
resolve_arch_path() {
    local path="$1"
    local arch="$2"
    echo "${path//\{ARCH\}/$arch}"
}

# 相对路径转绝对路径
resolve_path() {
    local path="$1"
    if [[ "$path" == ./* ]] || [[ "$path" == ../* ]]; then
        realpath -m "$path"
    else
        echo "$path"
    fi
}

# 智能拷贝函数
intelligent_copy() {
    local src="$1"
    local mode="$2"  # 新增模式参数
    local dst="$3"
    
	# 处理空模式情况
    if [ -z "$mode" ]; then
        echo "⚠️  拷贝模式未指定，使用默认覆盖模式" >&2
        mode="overwrite"
    fi

    echo "  → $src => $dst (模式: $mode)"
    
    case "$mode" in
        overwrite)
            # 覆盖模式：完全替换目标目录
            if [ -d "$dst" ] && [ -n "$(ls -A "$dst")" ]; then
                local backup_dir="${dst}.bak-$(date +%s)"
                echo "  🔄 目标非空，创建备份: $backup_dir"
                mv "$dst" "$backup_dir"
            fi
            ;;
        merge)
            # 合并模式：保留目标目录已有文件
            if [ ! -d "$dst" ]; then
                mkdir -p "$dst"
            fi
            ;;
        update)
            # 更新模式：只覆盖旧文件
			if [ ! -d "$full_dest" ]; then
				echo "  📁 创建目标目录 (update 模式): $full_dest"
				mkdir -p "$full_dest"
			else
				echo "  🔄 保留目标目录内容 (模式: update)"
			fi
			;;
        *)
            echo "❌ 未知拷贝模式: $mode" >&2
            exit 1
            ;;
    esac
    
    # 递归拷贝
    safe_mkdir "$(dirname "$dst")"
    
    case "$mode" in
        overwrite|merge)
            cp -a "$src" "$dst"
            ;;
        update)
            rsync -a -u "$src/" "$dst/"
            ;;
    esac || {
        echo "❌ 复制失败: $src => $dst" >&2
        exit 1
    }
}

# ---------------------- 主流程 ----------------------
echo "===== 开始灵活路径打包 ====="
#解析架构
ACTUAL_ARCH=$(resolve_architecture)
#拼接输出文件名
OUTPUT_NAME="${BASE_NAME}_${ACTUAL_ARCH}.run"

# 1. 创建临时根目录
echo "▸ 创建临时工作区: $TMP_ROOT_DIR"
rm -rf "$TMP_ROOT_DIR"
safe_mkdir "$TMP_ROOT_DIR"


#需要打包的脚本文件
echo "▸ 添加安装脚本目录: $SCRIPTS_DIR"
if [ -d "$SCRIPTS_DIR" ]; then
    # 复制整个脚本目录
    cp -r "$SCRIPTS_DIR" "$TMP_ROOT_DIR/"
    echo "    ✓ 已添加脚本目录"
    
    # 列出所有脚本
    echo "    ▸ 包含的脚本:"
    find "$SCRIPTS_DIR" -type f -name "*.sh" | while read -r script; do
        echo "      - $(basename "$script")"
    done
else
    echo "  ⚠️  警告: 找不到安装脚本目录: $SCRIPTS_DIR"
fi

# 2. 创建路径映射表
MAPPING_FILE="$TMP_ROOT_DIR/path_mappings"
echo "# TinyPiXOS 路径映射表" > "$MAPPING_FILE"
echo "# 格式: 源路径<|>目标路径" >> "$MAPPING_FILE"

# 3. 按目标路径分组源目录
declare -A target_groups
declare -A mode_map  # 存储目标路径到模式的映射
echo "▸ 处理路径映射 (ARCH=$ACTUAL_ARCH)"
for src_key in "${!PATH_MAPPINGS[@]}"; do
    # 获取原始映射值
    mapping_value="${PATH_MAPPINGS[$src_key]}"
    echo "  - 源键: $src_key => 映射值: $mapping_value"
    
    # 分割模式和目标路径
    IFS=':' read -r mode target_path <<< "$mapping_value"
    if [[ -z "$mode" || -z "$target_path" ]]; then
        echo "⚠️  无效映射值: $mapping_value (源键: $src_key), 跳过" >&2
        continue
    fi
    
    # 解析源路径
    resolved_src=$(resolve_arch_path "$src_key" "$ACTUAL_ARCH")
    resolved_src=$(resolve_path "$resolved_src")
    
    echo "  - 解析后源路径: $resolved_src"
    
    # 验证源目录
    if [ ! -e "$resolved_src" ]; then
        echo "⚠️  源路径不存在: $resolved_src (源键: $src_key), 跳过" >&2
        continue
    fi
    
    # 将源目录按目标路径分组
    if [ -z "${target_groups[$target_path]}" ]; then
        target_groups["$target_path"]="$resolved_src"
    else
        target_groups["$target_path"]+=$'\n'"$resolved_src"
    fi
    
    # 存储目标路径到模式的映射
    mode_map["$target_path"]="$mode"
    echo "  - 目标路径: $target_path, 模式: $mode"
done

# 4. 处理每个目标路径组
echo "▸ 开始处理目标路径组"
for target_path in "${!target_groups[@]}"; do
    # 获取模式
    mode="${mode_map[$target_path]}"
    
    echo "▷ 目标路径: $target_path (模式: $mode)"
    
    # 获取所有源目录
    mapfile -t src_paths <<< "${target_groups[$target_path]}"
    
    # 生成唯一标识符
    map_id="MAP_$(echo "$target_path" | md5sum | cut -c1-8)"
    target_dir="$TMP_ROOT_DIR/sources/$map_id/$(basename "$target_path")"
    
    # 创建目标目录
    safe_mkdir "$target_dir"
    
    # 复制所有源目录内容
    for src_path in "${src_paths[@]}"; do
        [[ -z "$src_path" ]] && continue
        
        echo "  → 复制: $src_path => $target_dir"
        
        # 使用简单的复制命令（先忽略模式）
        rsync -a "$src_path/" "$target_dir/"
    done

    # 记录映射关系
    echo "记录映射: sources/$map_id/$(basename "$target_path")<|>${mode}<|>${target_path}"
    echo "sources/$map_id/$(basename "$target_path")<|>${mode}<|>${target_path}" >> "$MAPPING_FILE"
done

# 5. 创建智能安装器 (添加软链接功能)
cat > "$TMP_ROOT_DIR/installer.sh" <<'EOF'
#!/bin/bash
# TinyPiXOS 智能安装器 (完整覆盖版)
SCRIPTS_DIR="config"
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

show_help() {
    echo "灵活路径安装系统 (支持多种拷贝模式)"
    echo "用法: $0 [选项]"
    echo "  -t, --target DIR     指定安装目标目录 (默认: /)"
    echo "  -d, --dry-run        模拟运行不实际修改"
    echo "  -h, --help           显示帮助信息"
}

# ====================== 通用脚本执行框架 ======================
run_install_scripts() {
    local phase="$1"  # "pre" 或 "post"
    local scripts_dir="${SCRIPT_DIR}/$SCRIPTS_DIR"
    
    echo "▸ 执行 $phase 阶段脚本"
    
    if [ ! -d "$scripts_dir" ]; then
        echo "  ℹ️  未找到脚本目录"
        return 0
    fi
    
    # 获取所有脚本并按文件名排序
    local script_files=()
    while IFS= read -r -d $'\0' file; do
        script_files+=("$file")
    done < <(find "$scripts_dir" -type f -name "*.sh" -print0 | sort -z)
    
    if [ ${#script_files[@]} -eq 0 ]; then
        echo "  ℹ️  未找到脚本"
        return 0
    fi
    
    # 执行所有脚本
    local script_count=0
    for script_path in "${script_files[@]}"; do
        script_name=$(basename "$script_path")
        script_count=$((script_count + 1))
        
        echo "  → [$script_count] 执行: $script_name"
        echo "     路径: $script_path"
        echo "     参数: $TARGET_DIR"
        
        # 设置执行权限
        chmod +x "$script_path"
        
        if $DRY_RUN; then
            echo "    [模拟] 跳过执行"
            continue
        fi
        
        # 执行脚本
        if /bin/bash "$script_path" "$TARGET_DIR"; then
            echo "    ✅ 脚本执行成功"
        else
            local exit_code=$?
            echo "    ❌ 脚本执行失败 (退出码: $exit_code)" >&2
            return $exit_code
        fi
    done
    
    echo "    ✓ 所有脚本执行完成 ($script_count 个)"
    return 0   
}

# ====================== 安装阶段定义 ======================
run_pre_install_scripts() {
    run_install_scripts "pre" "$TARGET_DIR"
}

run_post_install_scripts() {
    run_install_scripts "post" "$TARGET_DIR"
}

# 参数解析
TARGET_DIR="/"
DRY_RUN=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        -t|--target)
            TARGET_DIR="${2%/}"
            shift 2
            ;;
        -d|--dry-run)
            DRY_RUN=true
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo "❌ 未知选项: $1" >&2
            show_help
            exit 1
            ;;
    esac
done

# 直接使用脚本所在目录的映射文件
SCRIPT_DIR="$(dirname "$(realpath "$0")")"
MAPPING_FILE="$SCRIPT_DIR/path_mappings"

# 确保映射文件存在
if [ ! -f "$MAPPING_FILE" ]; then
    echo "❌ 错误: 找不到路径映射表" >&2
    echo "   当前目录: $(pwd)" >&2
    echo "   脚本目录: $SCRIPT_DIR" >&2
    echo "   映射文件路径: $MAPPING_FILE" >&2
    echo "   目录内容: $(ls -l "$SCRIPT_DIR")" >&2
    exit 1
fi

# 检查目标目录
if [ ! -d "$TARGET_DIR" ]; then
    echo "▸ 创建目标目录: $TARGET_DIR"
    $DRY_RUN || mkdir -p "$TARGET_DIR"
fi

echo "===== 开始安装 ====="
echo "目标目录: $TARGET_DIR"
echo "映射文件: $MAPPING_FILE"

# 检查系统依赖包 ----------------------------------------
echo "▸ 正在检查系统依赖包 (架构: $ARCH)"
packages=(
    libsdl2-dev libdrm-dev libudev-dev
	libcairo2-dev libpango1.0-dev libglib2.0-dev \
	libpangocairo-1.0-0 libfontconfig-dev libfreetype-dev \
	libgbm-dev libgles2 libegl-dev \
	libasound2-dev libjson-c-dev libssl-dev libavcodec-dev libavformat-dev \
	libavutil-dev libswscale-dev libswresample-dev \
	libavfilter-dev libavdevice-dev librsvg2-dev bluez libbluetooth-dev \
	libdbus-1-dev bluez-alsa-utils libasound2-plugin-bluez bluez-obexd  libusb-1.0-0-dev \
	libleveldb-dev libmarisa-dev libopencc-dev libyaml-cpp-dev libgoogle-glog-dev
)
# 可能已经通过手动安装的库
declare -A LIB_DETECT_FUNCTIONS=(
    ["libsdl2-dev"]="check_sdl2_installed"
    # 添加新库示例：["libopencv-dev"]="check_opencv_installed"
)

# 检查sdl2安装
check_sdl2_installed() {
    # 检查关键文件：头文件、库文件、pkg-config
    [ -f /usr/local/include/SDL2/SDL.h ] || \
    [ -f /usr/include/SDL2/SDL.h ] || \
    (pkg-config --exists sdl2 2>/dev/null && [ -f $(pkg-config --variable=libdir sdl2)/libSDL2.so ])
}

#网络检查函数
check_network() {
    local servers=("baidu.com" "8.8.8.8" "1.1.1.1")
    local connected=0
    
    echo "▸ 检查网络连接..."
    
    for server in "${servers[@]}"; do
        echo "  测试连接到: $server"
        if ping -c 1 -W 1 "$server" &> /dev/null; then
            echo "  ✓ 可以连接到 $server"
            connected=1
            break
        fi
    done
    
    if [ $connected -eq 0 ]; then
        echo " 错误: 无法连接到任何网络服务器" >&2
        echo "  • 请检查您的网络连接" >&2
        echo "  • 或者使用离线安装包" >&2
        return 1  # 改为返回错误码而不是退出
    fi
    
    echo "✓ 网络连接正常"
    return 0
}

check_and_install_packages() {
	local has_network=$1  # 接收网络状态参数
    shift  # 移除第一个参数，剩余参数为包列表
	local packages=("$@") #包列表

    # 检测系统可用的包管理工具
    local pkg_manager=""
    if command -v apt > /dev/null 2>&1; then
        pkg_manager="apt"
    elif command -v apt-get > /dev/null 2>&1; then
        pkg_manager="apt-get"
    else
        echo "❌ 当前系统未检测到 apt 或 apt-get 包管理器。" >&2
        echo "📋 需要手动安装的软件包列表: ${packages[*]}" >&2
        echo "💡 请根据您的嵌入式平台文档，通过源码或特定包管理器安装上述软件。" >&2
        return 1
    fi

	if [ $has_network -eq 0 ]; then
        echo "▸ 检测到包管理器: $pkg_manager, 开始自动安装流程..."
        echo "▸ 更新软件包列表..."
        if ! sudo $pkg_manager update > /dev/null 2>&1; then
            echo "❌ 更新包列表失败" >&2
            return 1
        fi

        echo "▸ 正在安装软件包: ${packages[*]} ..."
        if sudo $pkg_manager install -y "${packages[@]}" > /dev/null 2>&1; then
            echo "✓ 所有软件包安装成功。"
            return 0
        else
            echo "❌ 软件包安装失败，请检查:" >&2
            echo "  • 网络连接" >&2
            echo "  • 软件源配置" >&2
            echo "  • 软件包名称拼写" >&2
            return 1
        fi

    # 4. 有包管理器但无网络：提示手动安装
    else
        echo "❌ 检测到包管理器但无网络连接，无法自动安装。" >&2
        echo "📋 需要手动安装的软件包列表: ${packages[*]}" >&2
        echo "💡 请在有网络的环境下执行自动安装，或手动下载上述软件的 .deb 包及其依赖后进行离线安装。" >&2
        return 1
    fi

	#循环安装所有包
	: '		#按块注释调这部分，改为直接安装
    for pkg in "$@"; do
        # 方法1：使用 dpkg-query 精确检查（推荐）
         # 使用 dpkg-query 精确检查
        if dpkg-query -W -f='${Status}' "$pkg" 2>/dev/null | grep -q "install ok installed"; then
            echo "  ✓ $pkg 已安装"
			continue;
		fi
		# 检查可能通过手动安装的库
		if [[ -n "${LIB_DETECT_FUNCTIONS[$pkg]}" ]]; then
            if ${LIB_DETECT_FUNCTIONS[$pkg]}; then
                echo "  ✓ [$pkg] 检测到手动编译安装"
                continue  # 跳过安装流程
            fi
        fi
		# 如果没有网络连接，无法安装新包
		if [ $has_network -ne 0 ]; then
			echo "❌ 错误: $pkg 未安装且无网络连接" >&2
			echo "  • 请手动安装此包或检查网络连接" >&2
			return 1
		fi
        echo "  ▸ 正在安装 $pkg ..."
		if ! $pkg_manager install -y "$pkg" >/dev/null; then
			echo "[错误] 安装 $pkg 失败，请检查网络连接或软件源配置" >&2
			return 1
		fi
        echo "  ✓ $pkg 安装成功"
    done
	'
}

if check_network; then
    has_network=0  # 有网络
else
    has_network=1  # 无网络
fi

# 安装依赖包，传递网络状态和包列表
# 保存依赖包安装状态
DEPENDENCY_STATUS=0
PACKAGES_TO_INSTALL_MANUALLY=()

# 安装依赖包
check_and_install_packages $has_network "${packages[@]}"
DEPENDENCY_STATUS=$?

# 根据状态码记录需要手动安装的包
if [ $DEPENDENCY_STATUS -ne 0 ]; then
    PACKAGES_TO_INSTALL_MANUALLY=("${packages[@]}")
fi

# ====================== 文件复制逻辑 ======================
# 处理映射表
declare -A processed_targets  # 记录已处理的目标路径

while IFS= read -r mapping; do
    # 跳过空行和注释
    [[ "$mapping" == \#* ]] || [[ -z "$mapping" ]] && continue
    
    # 使用两次分割提取三个部分
    part1="${mapping%%<|>*}"
    rest="${mapping#*<|>}"
    part2="${rest%%<|>*}"
    part3="${rest#*<|>}"
    
    # 验证分割结果
    if [[ -z "$part1" || -z "$part2" || -z "$part3" ]]; then
        echo "⚠️  映射行格式错误: $mapping" >&2
        continue
    fi
    
    src_part="$part1"
    mode_part="$part2"
    dest_part="$part3"
    
    # 调试输出
    echo "  - 映射行解析:"
    echo "    src_part: $src_part"
    echo "    mode_part: $mode_part"
    echo "    dest_part: $dest_part"
    
    # 计算完整源路径
    src_path="${SCRIPT_DIR}/${src_part}"
    full_dest="${TARGET_DIR}${dest_part}"
    
    # 计算完整源路径
    src_path="${SCRIPT_DIR}/${src_part}"
    full_dest="${TARGET_DIR}${dest_part}"
    
    if [ ! -d "$src_path" ]; then
        echo "❌ 错误: 源目录不存在 - $src_path" >&2
        echo "    映射行: $mapping" >&2
        echo "    脚本目录: $SCRIPT_DIR" >&2
        echo "    源路径: $src_path" >&2
        continue
    fi

    if $DRY_RUN; then
        echo "🔍 [模拟] $src_path => $full_dest (模式: $mode_part)"
        continue
    fi
    
    echo "▸ 处理映射: $src_path => $full_dest (模式: $mode_part)"
    
    # 第一次遇到目标路径时，根据模式处理
    if [ -z "${processed_targets[$full_dest]}" ]; then
        case "$mode_part" in
            overwrite)
                # 覆盖模式：清空目标目录
                echo "  🗑️  清空目标目录: $full_dest"
                rm -rf "$full_dest" 2>/dev/null
                ;;
            merge|update)
                # 合并/更新模式：保留目标目录
                if [ ! -d "$full_dest" ]; then
                    echo "  📁 创建目标目录: $full_dest"
                    mkdir -p "$full_dest"
                else
                    echo "  🔄 保留目标目录内容 (模式: $mode_part)"
                fi
                ;;
            *)
                echo "⚠️  未知拷贝模式: $mode_part, 使用默认覆盖模式" >&2
                rm -rf "$full_dest" 2>/dev/null
                ;;
        esac
        processed_targets["$full_dest"]=1
    fi
    
    # 确保目标目录存在
    mkdir -p "$full_dest"
    
    # 根据拷贝模式执行不同操作
    case "$mode_part" in
        overwrite)
            # 完全覆盖
            echo "  → 完全覆盖: $src_path/ => $full_dest/"
            rsync -a --delete "$src_path/" "$full_dest/"
            ;;
        merge)
            # 合并目录（不删除目标目录已有文件）
            echo "  → 合并内容: $src_path/ => $full_dest/"
            rsync -a "$src_path/" "$full_dest/"
            ;;
        update)
            # 只更新较新的文件
            echo "  → 更新内容: $src_path/ => $full_dest/ (仅更新)"
            rsync -a -u "$src_path/" "$full_dest/"
            ;;
        *)
            # 默认使用覆盖模式
            echo "⚠️  未知拷贝模式: $mode_part, 使用覆盖模式" >&2
            rsync -a --delete "$src_path/" "$full_dest/"
            ;;
    esac
    
    # 确保所有文件可访问
    chmod -R a+rX "$full_dest"
    
done < "$MAPPING_FILE"

# ====================== 安全软链接替换函数 ======================
safe_create_link() {
    local link_path="$1"
    local target_path="$2"
    
    # 1. 删除任何已存在的链接或文件
    if [ -e "$link_path" ] || [ -L "$link_path" ]; then
        if ! rm -f "$link_path"; then
            echo "❌ 错误: 无法删除旧链接 - $link_path" >&2
            return 1
        fi
        echo "    🗑️  已移除旧链接: $link_path"
    fi
    
    # 2. 创建新链接
    if ! ln -s "$target_path" "$link_path"; then
        echo "❌ 错误: 无法创建链接 - $link_path" >&2
        return 2
    fi
    
    return 0
}

# ====================== 合并软链接处理 （对bin，lib，字体创建软链接）======================
create_symlinks() {
    echo "▸ 创建绝对路径符号链接 (安全替换)"
    
    # 1. 库文件链接
	LIB_DIR="${TARGET_DIR}/usr/lib/TinyPiX"
	if [ -d "$LIB_DIR" ]; then
		echo "  → 处理库文件目录: $LIB_DIR"
		find "$LIB_DIR" -maxdepth 1 -type f \( -name "*.so" -o -name "*.so.*" \) | while read -r lib; do
			lib_name=$(basename "$lib")
			target_path="$LIB_DIR/${lib_name}"
			
			# 处理版本化共享库
			if [[ "$lib_name" =~ \.so\.[0-9] ]]; then
				# 提取基础库名和版本信息
				base_name="${lib_name%%.so.*}.so"
				version="${lib_name#*.so.}"
				major_version="${version%%.*}"
				
				# 创建两个链接
				echo "    ▸ 版本化库: $lib_name"
				
				# 1. 创建基础链接 (libname.so)
				link1_path="${TARGET_DIR}/usr/lib/${base_name}"
				if safe_create_link "$link1_path" "$target_path"; then
					echo "      ✓ $link1_path → $target_path"
				fi
				
				# 2. 创建主版本链接 (libname.so.major)
				link2_path="${TARGET_DIR}/usr/lib/${base_name}.${major_version}"
				if safe_create_link "$link2_path" "$target_path"; then
					echo "      ✓ $link2_path → $target_path"
				fi
			else
				# 非版本化库，只创建一个链接
				link_path="${TARGET_DIR}/usr/lib/${lib_name}"
				if safe_create_link "$link_path" "$target_path"; then
					echo "    ✓ $link_path → $target_path"
				fi
			fi
		done
	else
		echo "  ⚠️  库目录不存在: $LIB_DIR"
	fi
    
    # 2. 二进制文件链接
    BIN_DIR="${TARGET_DIR}/usr/bin/TinyPiX"
    if [ -d "$BIN_DIR" ]; then
        echo "  → 处理二进制目录: $BIN_DIR"
        find "$BIN_DIR" -maxdepth 1 -type f -executable | while read -r bin; do
            bin_name=$(basename "$bin")
            link_path="${TARGET_DIR}/usr/bin/${bin_name}"
            target_path="$BIN_DIR/${bin_name}"
            
            # 安全创建链接
            if safe_create_link "$link_path" "$target_path"; then
                echo "    ✓ $link_path → $target_path"
            fi
        done
    else
        echo "  ⚠️  二进制目录不存在: $BIN_DIR"
    fi

	#3. 字体库文件链接
	FONTS_DIR="${TARGET_DIR}/usr/data/TinyPiX/fonts"
	if [ -d "$FONTS_DIR" ]; then
		echo "  → 处理字体源目录: $FONTS_DIR"
		# 目标字体目录 (此处直接放用户目录避免嵌套)
		FONT_TARGET_DIR="${TARGET_DIR}/usr/share/fonts/opentype/TinyPiX"
		mkdir -p "$FONT_TARGET_DIR"
		
		# 遍历字体目录中的文件
		find "$FONTS_DIR" -maxdepth 1 -type d ! -path "$FONTS_DIR" | while read -r font_dir; do
        dir_name=$(basename "$font_dir")
        link_path="${FONT_TARGET_DIR}/${dir_name}"
        
        # 安全创建整个目录的链接
        if safe_create_link "$link_path" "$font_dir"; then
            echo "    ✓ $link_path → $font_dir"
        fi
		
		#更新字体库缓存
		sudo fc-cache -fv "$FONT_TARGET_DIR"
    	[ $? -eq 0 ] && echo "    ✓ 字体缓存更新成功"
    done
	else
		echo "  ⚠️  字体源目录不存在: $FONTS_DIR"
	fi
}

create_corrected_symlinks() {
    echo "▸ 开始处理第三方软链接 "
    
    local SYSTEM_BIN_DIR="${TARGET_DIR}/usr/bin"
    local SYSTEM_LIB_DIR="${TARGET_DIR}/usr/lib"
    local TINYPIX_BIN_DIR="${TARGET_DIR}/usr/bin/TinyPix"
    local TINYPIX_LIB_DIR="${TARGET_DIR}/usr/lib/TinyPix"
    
    # 1. 处理第三方二进制文件 (位于 build 子目录，智能链接)
    if [ -d "${TINYPIX_BIN_DIR}/build" ]; then
        echo "  → 处理第三方二进制文件 (/build/ 目录，智能链接)"
        find "${TINYPIX_BIN_DIR}/build" -maxdepth 1 -type f -executable | while read -r bin; do
            local bin_name=$(basename "$bin")
            local system_bin_path="${SYSTEM_BIN_DIR}/${bin_name}"
            
            # 核心逻辑：只有系统路径不存在时才创建链接
            if [ ! -e "$system_bin_path" ] && [ ! -L "$system_bin_path" ]; then
                echo "    ✅ 创建第三方命令链接: $bin_name -> TinyPix/build/$bin_name"
                if ! $DRY_RUN; then
                    ln -sf "$bin" "$system_bin_path"
                fi
            else
                echo "    ⏭️  系统已存在命令 '$bin_name'，跳过链接"
            fi
        done
    fi
    
    # 2. 处理第三方库文件 (位于 build 子目录，智能链接)
    if [ -d "${TINYPIX_LIB_DIR}/build" ]; then
        echo "  → 处理第三方库文件 (/build/ 目录，智能链接)"
        find "${TINYPIX_LIB_DIR}/build" -type f \( -name "*.so" -o -name "*.so.*" \) | while read -r lib; do
            local lib_name=$(basename "$lib")
            
            # 版本化库处理逻辑
            if [[ "$lib_name" =~ \.so\. ]]; then
                local base_name="${lib_name%%.so.*}.so"
                local major_version="${lib_name#*.so.}"; major_version="${major_version%%.*}"
                
                local major_link="${SYSTEM_LIB_DIR}/${base_name}.${major_version}"
                local base_link="${SYSTEM_LIB_DIR}/${base_name}"
                
                # 只创建系统缺失的链接
                if [ ! -e "$major_link" ] && [ ! -L "$major_link" ]; then
                    echo "    ✅ 创建版本库链接: ${base_name}.${major_version} -> TinyPix/build/$lib_name"
                    if ! $DRY_RUN; then
                        ln -sf "$lib" "$major_link"
                    fi
                else
                    echo "    ⏭️  系统已存在库链接 '${base_name}.${major_version}'，跳过"
                fi
                
                if [ ! -e "$base_link" ] && [ ! -L "$base_link" ]; then
                    echo "    ✅ 创建基础库链接: $base_name -> TinyPix/build/$lib_name"
                    if ! $DRY_RUN; then
                        ln -sf "$lib" "$base_link"
                    fi
                else
                    echo "    ⏭️  系统已存在库链接 '$base_name'，跳过"
                fi
            else
                # 非版本化库
                local link_path="${SYSTEM_LIB_DIR}/${lib_name}"
                if [ ! -e "$link_path" ] && [ ! -L "$link_path" ]; then
                    echo "    ✅ 创建库链接: $lib_name -> TinyPix/build/$lib_name"
                    if ! $DRY_RUN; then
                        ln -sf "$lib" "$link_path"
                    fi
                else
                    echo "    ⏭️  系统已存在库 '$lib_name'，跳过"
                fi
            fi
        done
    fi
	echo "✓ 第三方软链接处理完成"
}

# ====================== service文件安装 ======================
install_systemd_services() {
  	local SERVICE_SOURCE_DIR="${TARGET_DIR}/usr/lib/TinyPiX/systemd"
    local SYSTEMD_TARGET_DIR="/etc/systemd/system"

    echo "▸ 开始部署 systemd 服务文件 (模式: preserve)..."

    # 1. 检查源目录是否存在
    if [ ! -d "$SERVICE_SOURCE_DIR" ]; then
        echo "    ℹ️  未找到服务文件源目录: $SERVICE_SOURCE_DIR"
        return 0
    fi

    # 2. 确保目标目录存在
    safe_mkdir "$SYSTEMD_TARGET_DIR"

    # 3. 遍历并处理所有 .service 文件
    find "$SERVICE_SOURCE_DIR" -name "*.service" | while read -r service_file; do
        local service_name=$(basename "$service_file")
        local systemd_dest="${SYSTEMD_TARGET_DIR}/${service_name}"

        echo "    → 处理服务: $service_name"

        # Preserve 模式核心逻辑：检查目标是否已存在
        if [ -e "$systemd_dest" ] || [ -L "$systemd_dest" ]; then
            echo "      ⏭️  系统已存在服务 '$service_name'，为保活现有配置，跳过部署。"
        else
            echo "      ✅ 目标不存在，执行初始安装。"
            if ! $DRY_RUN; then
                # 复制文件并设置正确权限
                cp "$service_file" "$systemd_dest"
                chmod 644 "$systemd_dest"
                echo "      ✓ 服务文件安装完成。"
            fi
        fi
    done

    # 4. 重新加载 systemd 配置（关键步骤！）
    if ! $DRY_RUN; then
        echo "▸ 重新加载 systemd 配置..."
        if systemctl daemon-reload; then
            echo "    ✅ systemd 配置重载成功。"
        else
            echo "    ⚠️  systemd 配置重载完成（请注意环境）。"
        fi
    fi

    echo "✓ 服务文件部署完成。"
}

deploy_etc_configs() {
    local ETC_SOURCE_DIR="${TARGET_DIR}/usr/etc/TinyPiX"
    local ETC_TARGET_DIR="/etc"

    echo "▸ 开始部署 etc 配置文件 (模式: preserve)..."

    # 1. 检查源目录是否存在
    if [ ! -d "$ETC_SOURCE_DIR" ]; then
        echo "    ℹ️  未找到 etc 配置源目录: $ETC_SOURCE_DIR"
        return 0
    fi

    # 2. 递归遍历源目录中的所有文件
    find "$ETC_SOURCE_DIR" -type f | while read -r source_file; do
        # 计算相对于源目录的相对路径
        local relative_path="${source_file#$ETC_SOURCE_DIR/}"
        local dest_file="${ETC_TARGET_DIR}/${relative_path}"

        echo "    → 处理配置: $relative_path"

        # Preserve 模式核心逻辑：检查目标是否已存在
        if [ -f "$dest_file" ]; then
            echo "      ⏭️  配置文件已存在，为保活用户修改，跳过: $relative_path"
        else
            echo "      ✅ 配置文件不存在，执行初始安装。"
            if ! $DRY_RUN; then
                # 确保目标文件的目录存在
                safe_mkdir "$(dirname "$dest_file")"
                # 复制文件
                cp "$source_file" "$dest_file"
                # 建议设置严谨的权限，例如对于敏感配置可设为 600
                chmod 600 "$dest_file"
                echo "      ✓ 配置文件安装完成。"
            fi
        fi
    done

    echo "✓ etc 配置文件部署完成。"
}



#调用脚本执行
run_post_install_scripts

# 在文件复制后调用软链接处理
create_symlinks
create_corrected_symlinks

if [ $DEPENDENCY_STATUS -ne 0 ]; then
    echo ""
    echo "⚠️  以下依赖包需要手动安装:"
    for pkg in "${packages[@]}"; do
        echo "   $pkg"
    done
    echo ""
fi

echo -e "\n✅ 安装成功完成"
exit 0
EOF
chmod +x "$TMP_ROOT_DIR/installer.sh"

# 6. 创建自解压包 - 增加调试信息
cat > "$OUTPUT_NAME" <<'EOF'
#!/bin/bash
# TinyPiXOS 自解压安装器 (增加调试信息)

INSTALL_DIR="${1:-}"
EXTRACT_DIR=$(mktemp -d -t pix_install.XXXXXX)

# 增加调试输出
echo "▸ 临时目录: $EXTRACT_DIR"
echo "▸ 解压安装数据..."

# 定位数据起始位置
ARCHIVE_START=$(awk '/^__ARCHIVE_BELOW__/ {print NR + 1; exit 0; }' "$0")

# 解包数据
tail -n +$ARCHIVE_START "$0" | base64 -d | tar -xzf - -C "$EXTRACT_DIR"

# 增加调试：检查解压内容
echo "▸ 解压目录内容:"
#ls -lR "$EXTRACT_DIR"

# 执行安装器
if [ -n "$INSTALL_DIR" ]; then
    echo "▸ 安装到目录: $INSTALL_DIR"
    "$EXTRACT_DIR/installer.sh" --target "$INSTALL_DIR"
else
    echo "▸ 安装到默认位置"
    "$EXTRACT_DIR/installer.sh"
fi

# 清理
rm -rf "$EXTRACT_DIR"
echo "安装流程完成!"
exit 0

__ARCHIVE_BELOW__
EOF

# 打包并附加数据
(cd "$TMP_ROOT_DIR" && tar cz .) | base64 >> "$OUTPUT_NAME"
chmod +x "$OUTPUT_NAME"

# 7. 按需清理临时目录
if [ "$KEEP_TMP_DIR" = true ]; then
    echo -e "\n🔍 临时目录已保留: $TMP_ROOT_DIR"
    echo "  目录结构:"
    tree -L 3 "$TMP_ROOT_DIR"
else
    rm -rf "$TMP_ROOT_DIR"
    echo "▸ 临时目录已清理"
fi

# 输出结果
echo -e "\n✅ 安装包生成成功: $OUTPUT_NAME"
echo "▸ 目标架构: $ACTUAL_ARCH"
echo "▸ 包含的路径映射:"
for src_dir in "${!PATH_MAPPINGS[@]}"; do
    # 显示原始映射（包含占位符）
    echo "  - $src_dir => ${PATH_MAPPINGS[$src_dir]}"
    
    # 显示实际解析后的路径
    resolved_src=$(resolve_arch_path "$src_dir" "$ACTUAL_ARCH")
    resolved_src=$(resolve_path "$resolved_src")
    echo "    实际源路径: $resolved_src"
done
echo -e "\n💡 安装命令:"
echo "  默认安装: ./$OUTPUT_NAME"
echo "  指定位置: ./$OUTPUT_NAME /custom/install/path"
echo "  自定义映射: ./$OUTPUT_NAME -m /path/to/custom_mappings.txt"