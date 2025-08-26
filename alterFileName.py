import os
import re
from pathlib import Path

# C++关键字和常见类型（防止误转换）
CPP_KEYWORDS = {
    'alignas', 'alignof', 'and', 'and_eq', 'asm', 'auto', 'bitand', 'bitor',
    'bool', 'break', 'case', 'catch', 'char', 'char8_t', 'char16_t', 'char32_t',
    'class', 'compl', 'concept', 'const', 'consteval', 'constexpr', 'const_cast',
    'continue', 'co_await', 'co_return', 'co_yield', 'decltype', 'default',
    'delete', 'do', 'double', 'dynamic_cast', 'else', 'enum', 'explicit',
    'export', 'extern', 'false', 'float', 'for', 'friend', 'goto', 'if', 'inline',
    'int', 'long', 'mutable', 'namespace', 'new', 'noexcept', 'not', 'not_eq',
    'nullptr', 'operator', 'or', 'or_eq', 'private', 'protected', 'public',
    'register', 'reinterpret_cast', 'requires', 'return', 'short', 'signed',
    'sizeof', 'static', 'static_assert', 'static_cast', 'struct', 'switch',
    'template', 'this', 'thread_local', 'throw', 'true', 'try', 'typedef',
    'typeid', 'typename', 'union', 'unsigned', 'using', 'virtual', 'void',
    'volatile', 'wchar_t', 'while', 'xor', 'xor_eq',
    # 常用标准库和类型
    'cout', 'cin', 'endl', 'string', 'vector', 'map', 'set', 'list', 'shared_ptr',
    'unique_ptr', 'make_shared', 'size_t', 'int32_t', 'uint32_t', 'int64_t', 'uint64_t',
    'FILE', 'memcpy', 'strlen', 'memset', 'std', 'main', 'endl'
}

class TpClassConverter:
    def __init__(self):
        # 类名映射: tpFile -> TpFile
        self.class_mapping = {}
        # 文件名映射: tpFile.h -> TpFile.h
        self.file_rename_map = {}
        # 需要处理的文件扩展名
        self.valid_extensions = ('.h', '.cpp', '.hpp')
    
    def is_tp_class(self, identifier):
        """检查是否是tp开头的类名"""
        if len(identifier) < 3:
            return False
        if identifier.lower() in CPP_KEYWORDS:
            return False
        if identifier.startswith('tp') and identifier[2].isalpha():
            return True
        return False
    
    def convert_class_name(self, class_name):
        """将tp类名转换为Tp开头的大驼峰格式"""
        # tpFile -> TpFile, tpMyClass -> TpMyClass
        return "Tp" + class_name[2].upper() + class_name[3:]
    
    def extract_tp_classes(self, content):
        """从代码内容中提取所有tp开头的类名"""
        # 匹配所有以tp开头的标识符
        pattern = r'\b(tp[a-zA-Z][a-zA-Z0-9_]*)\b'
        found_classes = set()
        
        # 查找所有可能的类名
        for match in re.finditer(pattern, content):
            class_name = match.group(1)
            if self.is_tp_class(class_name):
                found_classes.add(class_name)
        
        # 创建映射关系
        for class_name in found_classes:
            self.class_mapping[class_name] = self.convert_class_name(class_name)
    
    def rename_tp_files(self, root_path):
        """重命名以tp开头的文件"""
        for file_path in root_path.rglob('*'):
            if not file_path.is_file():
                continue
                
            # 检查文件扩展名
            suffix = file_path.suffix.lower()
            if suffix not in self.valid_extensions:
                continue
                
            # 获取文件名
            dir_name = str(file_path.parent)
            file_name = file_path.name
            
            # 只处理以tp开头的文件名
            if not file_name.lower().startswith('tp'):
                continue
                
            # 确保是有效的类文件名
            name, ext = os.path.splitext(file_name)
            if len(name) < 3:
                continue
                
            # 转换为Tp开头的文件名
            new_name = self.convert_class_name(name) + ext
            if new_name != file_name:
                new_path = os.path.join(dir_name, new_name)
                
                # 重命名文件
                os.rename(file_path, new_path)
                self.file_rename_map[file_name] = new_name
                print(f"Renamed file: {file_name} -> {new_name}")
    
    def convert_class_names_in_content(self, content):
        """在内容中转换所有tp类名（使用边界匹配）"""
        if not self.class_mapping:
            return content
        
        # 按长度降序排列类名，避免部分替换
        sorted_classes = sorted(self.class_mapping.items(), key=lambda x: -len(x[0]))
        
        for old_name, new_name in sorted_classes:
            # 使用非单词字符作为边界（避免替换部分单词）
            pattern = r'\b' + re.escape(old_name) + r'\b'
            content = re.sub(pattern, new_name, content)
        
        return content
    
    def update_file_includes(self, content):
        """更新#include中的文件名引用"""
        if not self.file_rename_map:
            return content
        
        # 处理所有#include引用
        for old_name, new_name in self.file_rename_map.items():
            # 处理引号形式
            quoted_pattern = r'#include\s+"([^"]*' + re.escape(old_name) + r')"'
            content = re.sub(
                quoted_pattern,
                lambda m: f'#include "{m.group(1).replace(old_name, new_name)}"',
                content
            )
            
            # 处理尖括号形式
            angled_pattern = r'#include\s+<([^>]*' + re.escape(old_name) + r')>'
            content = re.sub(
                angled_pattern,
                lambda m: f'#include <{m.group(1).replace(old_name, new_name)}>',
                content
            )
        
        return content
    
    def process_file(self, file_path):
        """处理单个文件内容"""
        # 读取文件内容
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # 更新#include引用
        content = self.update_file_includes(content)
        
        # 转换类名
        content = self.convert_class_names_in_content(content)
        
        # 写回文件
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
    
    def run(self, project_dir):
        """执行转换流程"""
        root_path = Path(project_dir)
        
        print("Step 1: Scanning for tp class declarations...")
        # 第一步：扫描所有文件查找类声明
        for file_path in root_path.rglob('*'):
            if file_path.is_file() and file_path.suffix.lower() in self.valid_extensions:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    self.extract_tp_classes(f.read())
        
        # 打印找到的类
        if self.class_mapping:
            print(f"\nFound {len(self.class_mapping)} tp classes to convert:")
            for old, new in self.class_mapping.items():
                print(f"  {old} -> {new}")
        else:
            print("\nNo tp classes found. Only file renaming will be performed.")
        
        print("\nStep 2: Renaming tp files...")
        # 第二步：重命名tp开头的文件
        self.rename_tp_files(root_path)
        
        # 打印重命名的文件
        if self.file_rename_map:
            print(f"\nRenamed {len(self.file_rename_map)} files:")
            for old, new in self.file_rename_map.items():
                print(f"  {old} -> {new}")
        else:
            print("\nNo tp files found to rename.")
        
        print("\nStep 3: Processing file contents...")
        # 第三步：处理所有文件内容（包括类名转换和#include更新）
        file_count = 0
        for file_path in root_path.rglob('*'):
            if file_path.is_file() and file_path.suffix.lower() in self.valid_extensions:
                try:
                    self.process_file(file_path)
                    file_count += 1
                    # 每10个文件打印一次进度
                    if file_count % 10 == 0:
                        print(f"  Processed {file_count} files...")
                except Exception as e:
                    print(f"Error processing {file_path}: {str(e)}")
        
        return len(self.class_mapping), len(self.file_rename_map), file_count

def main():
    import sys
    print("TpClass Converter - Final Version")
    print("=" * 50)
    
    # 获取项目目录
    if len(sys.argv) > 1:
        project_dir = sys.argv[1]
    else:
        project_dir = input("Enter project directory: ").strip()
    
    if not os.path.isdir(project_dir):
        print(f"Error: Invalid directory '{project_dir}'")
        return
    
    print(f"Processing directory: {project_dir}")
    
    # 执行转换
    converter = TpClassConverter()
    class_count, file_rename_count, file_process_count = converter.run(project_dir)
    
    print("\n" + "=" * 50)
    print("Conversion Summary:")
    print(f"- Found and converted {class_count} tp classes")
    print(f"- Renamed {file_rename_count} tp files")
    print(f"- Processed {file_process_count} files")
    print("\nIMPORTANT:")
    print("Please review the changes before committing")
    print("\nConversion completed successfully!")

if __name__ == "__main__":
    main()