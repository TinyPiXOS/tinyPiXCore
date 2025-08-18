#ifndef __TP_SMART_GUI_MUSIC_PLAYER_VIEW_H
#define __TP_SMART_GUI_MUSIC_PLAYER_VIEW_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpString.h"
#include "tpSurface.h"
#include "smartDeviceGUI/FrameworkGlobal.h"

SMART_DEVICE_GUI_NAMESPACE_BEGIN

TP_DEF_VOID_TYPE_VAR(ITpMusicPlayerViewData);
/// @brief 音乐播放视图
class TpMusicPlayerView : public tpChildWidget
{
public:
	TpMusicPlayerView(tpChildWidget *parent = nullptr);

	virtual ~TpMusicPlayerView();

    /// @brief 设置歌曲名字
    /// @param name 歌曲名字
    void setName(const tpString& name);
    /// @brief 获取歌曲名字
    /// @return 歌曲名字
    tpString name();

    /// @brief 设置歌曲歌手
    /// @param author 歌手
    void setAuthor(const tpString& author);
    /// @brief 获取歌手
    /// @return 歌手
    tpString author();

    /// @brief 设置当前显示歌词
    /// @param curLyric 歌词
    void setLyric(const tpString& curLyric);
    /// @brief 获取当前显示歌词
    /// @return 歌词
    tpString lyric();

    /// @brief 设置音乐图片资源绝对路径
    /// @param imagePath 资源路径
    void setImage(const tpString& imagePath);
    /// @brief 设置音乐图片资源对象
    /// @param image 资源对象
    void setImage(tpShared<tpSurface> image);

public:
	virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

	virtual tpString pluginType() override { return TO_STRING(TpMusicPlayerView); }

private:
	ITpMusicPlayerViewData *data_;
};

SMART_DEVICE_GUI_NAMESPACE_END

#endif
