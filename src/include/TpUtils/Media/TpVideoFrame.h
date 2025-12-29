#ifndef __TP_VIDEO_FRAME_H__
#define __TP_VIDEO_FRAME_H__

#include <TpCore.h>
#include <TpSize.h>

class TpVideoFrame
{
public:
    TpVideoFrame() {};
    TpVideoFrame(uint8_t **datas, int *lines, TpSize size, void *user_data) : datas_(datas), lines_(lines), size_(size), userdata_(user_data) {};
    ~TpVideoFrame() {};

    /// @brief 拷贝构造函数
    TpVideoFrame &operator=(const TpVideoFrame &other)
    {
        if (this != &other)
        {
            datas_ = other.datas_;
            lines_ = other.lines_;
            size_ = other.size_;
            format_ = other.format_;
            userdata_ = other.userdata_;
        }
        return *this;
    }

    /// @brief 移动构造函数
    TpVideoFrame(TpVideoFrame &&other) noexcept
        : datas_(other.datas_), lines_(other.lines_), size_(other.size_), format_(other.format_), userdata_(other.userdata_)
    {
        other.datas_ = nullptr;
        other.lines_ = nullptr;
        other.size_ = TpSize();
        other.format_ = 0;
        other.userdata_ = nullptr;
    }

public:
    /// @brief 获取视频帧尺寸
    /// @return 视频帧尺寸
    TpSize size() const { return size_; };
    /// @brief 获取视频帧数据，只有一行取datas_[0]，多行则取对应行数
    /// @return 视频帧数据
    uint8_t **data() const { return datas_; };
    /// @brief 获取视频帧行数,对于RGB格式一般只有一行，对于YUV格式可能有多行
    /// @return 视频帧行数
    int *lines() const { return lines_; };
    /// @brief 获取视频帧格式
    /// @return 视频帧格式
    uint32_t format() const { return format_; };

    void *userdata() const { return userdata_; };
    
    /// @brief 设置视频帧数据，一般无需调用此函数
    void setData(uint8_t **datas, int *lines, TpSize size, void *user_data)
    {
        datas_ = datas;
        lines_ = lines;
        size_ = size;
        userdata_ = user_data;
    }

private:
    uint8_t **datas_;
    int *lines_;
    uint32_t format_;
    TpSize size_;
    void *userdata_;
};

#endif