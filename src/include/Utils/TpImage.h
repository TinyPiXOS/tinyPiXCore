#ifndef __TP_IMAGE_H
#define __TP_IMAGE_H

#include "tpUtils.h"
#include "tpString.h"
#include "tpVector.h"
#include "tpGlobal.h"

TP_DEF_VOID_TYPE_VAR(ITpImageData);

/// @brief 图像处理类；用于加载图片资源，提供缩放、模糊等功能
class TpImage
{
public:
    enum ImageType
    {
        SVG_FMT,
        JPG_FMT,
        PNG_FMT,
        WEBP_FMT
    };
    friend class tpCanvas;

public:
    TpImage(const tpString &fileName = "");
    virtual ~TpImage();

public:
    /// @brief 加载图片资源文件，支持png, jpg, webp, SVG
    /// @param filename 资源文件路径
    /// @return 加载结果
    virtual bool load(const tpString &filename);

    /// @brief 指定size对图片进行缩放
    /// @param size 缩放后的尺寸
    /// @return 缩放后的资源对象
    TpImage scaled(const ItpSize &size);
    /// @brief 指定size对图片进行缩放
    /// @param width 缩放后的宽度
    /// @param height 缩放后的高度
    /// @return  缩放后的资源对象
    TpImage scaled(const int32_t &width, const int32_t &height);

    /// @brief 对图片资源进行高斯模糊处理;返回新对象(不会对原始数据进行更新)
    /// @param radius 模糊半径
    TpImage gaussianBlur(const int32_t &radius);

    /// @brief 获取surface的宽度
    /// @return 宽度值
    virtual int32_t width() const;
    /// @brief 获取surface的高度
    /// @return 高度值
    virtual int32_t height() const;

    /// @brief 图片资源是否为空；是否加载了图片
    /// @return 加载结果
    virtual bool isNull();

    /// @brief 指定矩形拷贝对象
    /// @param rect 拷贝矩形，将原始图片裁剪此矩形后返回新对象
    /// @return 新的图像管理对象
    virtual TpImage copy(const ItpRect &rect);
    /// @brief 指定矩形拷贝对象
    /// @param x 裁剪X坐标
    /// @param y 裁剪Y坐标
    /// @param w 裁剪宽度
    /// @param h 裁剪高度
    /// @return 新的图像管理对象
    virtual TpImage copy(int32_t x, int32_t y, int32_t w, int32_t h);

    /// @brief 指定文件绝对路径保存资源文件数据
    /// @param filename 文件绝对路径
    /// @param type 新存储文件类型
    /// @param jpguality 存储质量,取值范围[0, 100]
    /// @return 保存结果
    virtual bool save(const tpString &filename, ImageType type, int32_t jpguality = 100);

    // TODO 旋转接口
    
public:
    TpImage& operator=(const TpImage &others);

private:
    ITpImageData *data_;
};

#endif
