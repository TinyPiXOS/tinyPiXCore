#ifndef __TP_MUSIC_PLAYER_VIEW_H
#define __TP_MUSIC_PLAYER_VIEW_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"
#include "TpImage.h"

TP_DEF_VOID_TYPE_VAR(ITpMusicPlayerViewData);
/// @brief 音乐播放视图
class TpMusicPlayerView : public TpChildWidget
{
public:
    TpMusicPlayerView(TpChildWidget *parent = nullptr);

    virtual ~TpMusicPlayerView();

    /// @brief 设置歌曲名字
    /// @param name 歌曲名字
    void setName(const TpString &name);
    /// @brief 获取歌曲名字
    /// @return 歌曲名字
    TpString name();

    /// @brief 设置歌曲歌手
    /// @param author 歌手
    void setAuthor(const TpString &author);
    /// @brief 获取歌手
    /// @return 歌手
    TpString author();

    /// @brief 设置当前显示歌词
    /// @param curLyric 歌词
    void setLyric(const TpString &curLyric);
    /// @brief 获取当前显示歌词
    /// @return 歌词
    TpString lyric();

    /// @brief 设置音乐图片资源绝对路径
    /// @param imagePath 资源路径
    void setImage(const TpString &imagePath);
    /// @brief 设置音乐图片资源对象
    /// @param image 资源对象
    void setImage(TpImage image);

public
signals:
    /// @brief 点击上一首
    declare_signal(onPreviousMusic);
    /// @brief 点击下一首
    declare_signal(onNextMusic);
    /// @brief 点击继续播放
    declare_signal(onPlayingMusic);
    /// @brief 点击暂停播放
    declare_signal(onPauseMusic);

public:
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;

    virtual TpString pluginType() override { return TO_STRING(TpMusicPlayerView); }

private:
    ITpMusicPlayerViewData *data_;
};

#endif
