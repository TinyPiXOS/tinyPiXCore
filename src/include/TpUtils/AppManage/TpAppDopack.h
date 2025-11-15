#ifndef __TP_APP_DOPACK_H
#define __TP_APP_DOPACK_H

#include <TpCore.h>
#include "TpString.h"
#include "TpUuid.h"

TP_DEF_VOID_TYPE_VAR(ItpAppDopackData);

class TpAppDopack
{
public:
    enum TpPackageType
    {
        TP_PACKAGE_TYPE_DEFAULT, // 默认
        TP_PACKAGE_TYPE_APP,     // 用户APP
        TP_PACKAGE_TYPE_SAPP     // 系统APP
    };
    enum TpArchType
    {
        TP_ARCH_TYPE_AMD64,
        TP_ARCH_TYPE_I386,
        TP_ARCH_TYPE_ARM64,
        TP_ARCH_TYPE_ARM32,
        TP_ARCH_TYPE_RISCV
    };

public:
    TpAppDopack();
    ~TpAppDopack();

public:
    /// @brief 设置安装包类型
    /// @param type
    /// @return
    int setPackageType(TpPackageType type);
    /// @brief 设置应用的UUID
    /// @param id 字符串格式的UUID
    void setAppID(const TpString &id);
    /// @brief 设置应用的UUID
    /// @param id tpUUID类型的UUID
    void setAppID(const TpUuid id);
    /// @brief 设置应用的名字
    /// @param name
    void setAppName(const TpString &name);
    /// @brief 设置应用的版本号
    /// @param x
    /// @param y
    /// @param z
    void setVersion(tpUInt8 x, tpUInt8 y, tpUInt8 z);
    /// @brief 设置硬件架构
    /// @param architecture
    void setArchitecture(const TpString &architecture);
    /// @brief 设置
    /// @param section
    void setSection(const TpString &section);
    /// @brief
    /// @param priority
    void setPriority(const TpString &priority);
    /// @brief
    /// @param essential
    void setEssential(const TpString &essential);
    /// @brief 设置作者名字
    /// @param author
    void setAuthor(const TpString &author);
    /// @brief 设置作者联系方式
    /// @param contact
    void setContact(const TpString &contact);
    /// @brief
    /// @param provides
    void setProvides(const TpString &provides);
    /// @brief 设置组织或公司
    /// @param organization
    void setOrganization(const TpString &organization);
    /// @brief 设置应用占用磁盘大小
    /// @param size
    void setDiskSpace(int size);
    /// @brief 设置软件描述
    /// @param description
    /// @return
    void setDescription(const TpString &description);
    /// @brief 设置数字签名
    /// @param sig
    /// @return
    void setSignature(const TpString &sig);
    /// @brief 添加公共依赖库
    /// @param depend
    /// @param x
    /// @param y
    /// @param z
    /// @return
    void addDepend(const TpString &depend, tpUInt8 x, tpUInt8 y, tpUInt8 z);
    /// @brief 添加私有依赖库路径
    /// @param lib
    /// @return
    void addLib(const TpString &lib);
    /// @brief 设置应用图标
    /// @param icon 图标路径和文件名
    /// @return
    void setIcon(const TpString &icon);
    /// @brief 添加静态文件，这些文件在小省级中会被保留
    /// @param assert
    /// @return
    void addAssert(const TpString &assert);
    /// @brief 添加可执行文件
    /// @param bin
    /// @return
    void addBin(const TpString &bin);
    /// @brief 添加其他文件
    /// @param file
    /// @return
    void addFile(const TpString &file);
    /// @brief 添加软件支持的文件格式(后缀)
    /// @param type
    /// @return
    void addExtension(const TpString &type);
    /// @brief 设置开始文件脚本(暂时不支持，会根据配置信息自动生成)
    /// @param start
    /// @return
    void setStart(const TpString &start);
    /// @brief 设置卸载文件脚本(暂时不支持，会全部卸载)
    /// @param remove
    /// @return
    void setRemove(const TpString &remove);
    /// @brief 设置app可执行文件路径
    /// @param app
    /// @return
    void setAppPath(const TpString &app);
    /// @brief 从json文件中获取全部安装包打包参数，此操作需要提前准备json文件
    /// @param json json文件路径和名字
    /// @return
    void getAllConfig(const TpString &json);
    /// @brief 设置生成的安装包的名字(会自动添加后缀)
    /// @param name
    /// @return
    void setPackageName(const TpString &name);
    /// @brief 创建打包文件
    /// @param path 安装包的生成路径(不用包含名字)
    void creatPackage(const TpString &path);

    /// @brief 设置启动脚本中环境变量
    /// @param key
    /// @param value
    /// @return
    void addEnvironmentVar(const TpString &key, const TpString &value);
    /// @brief 在启动脚本中添加依赖库(在使用addDepend的时候会自动调用此接口)
    /// @param lib
    /// @return
    void addStartDepend(const TpString &lib);
    /// @brief 添加启动脚本的参数
    /// @param arg
    /// @return
    void addStartArg(const TpString &arg);
    /// @brief 暂不支持
    /// @param log_file
    /// @return
    void setLogFile(const TpString &log_file);
    /// @brief 暂不支持
    /// @param config_file
    /// @return
    void setConfigFile(const TpString &config_file);
    /// @brief 设置可执行文件路径
    /// @param name
    /// @return
    void setExecPath(const TpString &name);

private:
    ItpAppDopackData *data_;
};

#endif