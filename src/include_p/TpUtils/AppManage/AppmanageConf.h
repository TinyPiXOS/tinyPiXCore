/*///------------------------------------------------------------------------------------------------------------------------//
        通用的参数信息
说 明 :
日 期 : 2024.9.26

/*/
//------------------------------------------------------------------------------------------------------------------------//

#ifndef _APP_MANAGE_CONF_H_
#define _APP_MANAGE_CONF_H_

#include <stdint.h>
#include "openssl/md5.h"
#include "TpString.h"
#include "TpVector.h"
#include "JsonStructPackage/JsonStructPackageHeader.h"

#define MAX_PATH 2048

#define MAX_ITEMS 128     // 最大支持 128 项
#define MAX_ITEMS_LIB 512 // 最多支持安装512个库

#define MAX_LEN_APP_NAME 256  // APP名字最大长度
#define UUID_SIZE 37          // UUID大小
#define PATH_MAX_LENGTH 1024  // 路径最长的长度
#define CONFIG_MAX_LENGTH 512 // 配置项最大长度

#define PACKAGE_TYPE_CONFIG_LIB "#TinyPix SystemLib"
#define PACKAGE_TYPE_CONFIG_SAPP "#TinyPix SystemApp"
#define PACKAGE_TYPE_CONFIG_UAPP "#TinyPix UserApp"

#define PACKAGE_FILE_SUFFIX ".TpK"

// 硬件架构
enum TpEnumArchType
{
    TYPE_ARCH_NONE = 0,
    TYPE_ARCH_AMD64 = 1, // AMD64,X86-64,X64
    TYPE_ARCH_I386 = 2,  // I386,X86-32
    TYPE_ARCH_ARM64 = 3, // ARM64,AARCH-64
    TYPE_ARCH_ARM32 = 4, // ARM32,AARCH-32
    TYPE_ARCH_RV64GC = 5 // RISC-V
};

enum TypePackage
{
    TYPE_PACKAGE_DEFAULT = 0, // 默认
    TYPE_PACKAGE_LIB = 1,     // 系统库
    TYPE_PACKAGE_SAPP = 2,    // 系统应用
    TYPE_PACKAGE_APP = 3,     // 普通应用
    TYPE_PACKAGE_NONE = 255   // 未知类型
};

struct TpAppID
{
    TpString value;
};

struct TpVersion
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
};

struct PackageUserParam
{
    struct InstallSchedule *schedule; // 安装进度
};

struct AppPackageConfig
{
    uint8_t installFlag;             // 已经安装的标志
    TpString appID;                   // UUID，后续更改为struct
    TpString appName;                 // NAME
    TpString organization;            // 组织/公司
    TpVersion version;                // 版本
    TpString architecture;            // 硬件平台
    TpEnumArchType arch;              // 硬件平台，后续会改为此结构体
    TpString section;                 // 应用所属分类（常见值：utils、graphics、games），后续更改为enum
    TpString priority;                // 安装优先级（optional=非必需，standard=基础组件，required=系统关键组件），后续更改为enum
    TpString essential;               // 是否为系统核心组件（yes=不可卸载，no=可卸载）。后续更改为bool或enum
    TpString provides;                // 应用提供的功能标识
    TpString author;                  // 作者
    TpString contact;                 // 作者联系方式
    int diskspace;                    // 安装所需的最小磁盘空间
    TpString description;             // 软件描述
    TpString appexecName;             // 可执行文件的名字以及路径
    TpString signature;               // 数字签名
    TpString icon;                    // 图标路径
    TpVector<TpString> depend;        // 引用的开源库名字和版本
    TpVector<TpString> lib;           // 自己的库的路径(暂时不用，直接使用启动脚本的配置)
    TpVector<TpString> assertFiles;   // 作者自己的一些静态文件，会全部被复制到assert目录下
    TpVector<TpString> binFiles;      // 可执行文件
    TpVector<TpString> otherFiles;    // 作者自己的其他文件，会被复制到根目录下，安装时候复制到app目录下
    TpVector<TpString> fileExtension; // 支持打开的文件类型

    AppPackageConfig()
    {
    }
};
JSONTRANSLATE(AppPackageConfig, appID)

struct LibPackageConfig
{
    TpString architecture;          // 硬件平台
    TpEnumArchType arch;
    uint32_t diskspace;              // 占用内存空间
    struct TpVersion version[MAX_ITEMS_LIB];
    TpVector<TpString> systemLib;
    TpVector<TpString> file;
};

// 安装包信息
struct PackageConfigInfo
{
    TypePackage type;

    AppPackageConfig appConf;
    LibPackageConfig libConf;

    TpString path_pack;
    uint8_t md5[MD5_DIGEST_LENGTH];
    uint8_t md5_flag;
};

#endif