#!/bin/bash
# 使用./dopack.sh <架构>
# 架构可以设置x86_64或arm_64,如果不设置会按照当前环境默认打包
# ====================== 配置区域 ======================
BASE_NAME="tinyPiXCore" 	# 生成的安装包的名字,会自动拼接架构和后缀
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
	# 模式支持: overwrite(覆盖) | merge(合并) | update(更新)
    ["./{ARCH}/lib"]="overwrite:/usr/lib/tinyPiX"
	["./{ARCH}/bin"]="overwrite:/usr/bin/tinyPiX"

	["../src/depend_lib/dynamic/{ARCH}"]="update:/usr/lib/tinyPiX"
#	["../src/depend_lib/static/{ARCH}"]="update:/usr/lib/tinyPiX"
    
    # 示例 2: 数据目录重定位
    ["./{ARCH}/data"]="update:/usr/data/tinyPiX"  # 源目录安装到新位置
    
    # 示例 3: 头文件
    ["./{ARCH}/include"]="overwrite:/usr/include/tinyPiX"
    
    # 示例 4: 资源文件到自定义位置
	["./{ARCH}/res"]="update:/usr/res/tinyPiX"
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

# 修复点1: 直接使用脚本所在目录的映射文件
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
	libsdl2-image-dev libsdl2-gfx-dev
    libcairo2-dev libpango1.0-dev libglib2.0-dev
    libpangocairo-1.0-0 libfontconfig-dev libfreetype-dev
    libgbm-dev libgles2 libegl-dev 
	librsvg2-dev libssl-dev libavcodec-dev libavformat-dev libavutil-dev libavfilter-dev libavdevice-dev
	bluez-obexd bluez-alsa-utils libasound2-plugin-bluez
	libboost-all-dev libleveldb-dev libmarisa-dev libopencc-dev libyaml-cpp-dev libgoogle-glog-dev
)

check_and_install_packages() {
    if ! command -v apt-get >/dev/null; then
        echo "[错误] 只支持基于Debian的系统（如Ubuntu）自动安装依赖包" >&2
        echo "请手动安装以下包：${packages[*]}" >&2
        exit 1
    fi

    # 更新包索引
    echo "  更新包列表..."
    apt-get update >/dev/null

    for pkg in "$@"; do
        # 方法1：使用 dpkg-query 精确检查（推荐）
        if dpkg-query -W -f='${Status}' "$pkg" 2>/dev/null | grep -q "install ok installed"; then
            echo "  ✓ $pkg 已安装"
        else
            echo "  ▸ 正在安装 $pkg ..."
            if ! apt-get install -y "$pkg" >/dev/null; then
                echo "[错误] 安装 $pkg 失败，请检查网络连接或软件源配置" >&2
                exit 1
            fi
        fi
    done
}

check_and_install_packages "${packages[@]}"

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

# ====================== 合并软链接处理 ======================
create_symlinks() {
    echo "▸ 创建绝对路径符号链接 (安全替换)"
    
    # 1. 库文件链接
	LIB_DIR="${TARGET_DIR}/usr/lib/tinyPiX"
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
    BIN_DIR="${TARGET_DIR}/usr/bin/tinyPiX"
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
}

#调用脚本执行
run_post_install_scripts

# 在文件复制后调用
create_symlinks

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
ls -lR "$EXTRACT_DIR"

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
echo "✅ 安装流程完成!"
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