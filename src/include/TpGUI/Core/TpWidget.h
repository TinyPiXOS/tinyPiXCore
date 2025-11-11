#ifndef __TP_VWIDGET_H
#define __TP_VWIDGET_H

#include "TpObject.h"
#include "TpVariant.h"
#include "TpCssParser.h"

class TpRect;
class TpPoint;
class TpSurface;
class TpColors;
class TpLayout;
class TpBrush;
class TpImage;

class TpEvent;
class TpWheelEvent;
class TpKeyboardEvent;
class TpMouseEvent;
class TpFingerEvent;
class TpDollAREvent;
class TpMultiGestureEvent;
class TpMoveEvent;
class TpResizeEvent;
class TpFocusEvent;
class TpLeaveEvent;
class TpVisibleEvent;
class TpPaintEvent;
class TpActiveEvent;
class TpThemeChangeEvent;

class TpGraphicsBlurEffect;

class TpWidget
    : public TpObject
{
public:
    TpWidget(TpWidget *parent = nullptr);
    virtual ~TpWidget();

public:
    /// @brief 设置属性值
    /// @param _name 属性名称
    /// @param _value 属性值
    virtual void setProperty(const TpString &_name, const TpVariant &_value) override;

    /// @brief 删除自己，用于UI组件的删除，不会立即删除在下一个时间循环删除该指针
    virtual void deleteLater() override;

public:
    /// @brief 关闭本窗体,只隐藏并不会释放窗口
    /// @return 关闭结果
    virtual void close();

    /// @brief 显示窗体
    virtual void show();

    /// @brief 最大化显示窗体
    virtual void showMaximum();

    /// @brief 设置窗口显隐
    /// @param visible true显示，false隐藏
    virtual void setVisible(bool visible = true);
    /// @brief 获取当前窗口显隐状态
    /// @return true显示，false隐藏
    virtual bool visible();

    /// @brief 设置组件是否可用，禁用后不触发除绘制外所有事件
    /// @param enable true为可用，false为不可用
    void setEnabled(const bool &enable);

    /// @brief 获取当前组件可用状态
    /// @return 可用状态
    bool enabled();

public:
    /// @brief 获取虚拟键盘输入；需要获取时重写此虚函数
    /// @param text 输入文本
    virtual void virtualKeyboardInput(const Tp::VirtualKeyboardInputType &type, const TpString &text) {};

public:
    virtual void setRotateEnable(bool enabled = false) {};
    virtual bool rotateEnable() { return false; };
    virtual void setRotate(ItpRotateType type) {};
    virtual ItpRotateType rotate() { return TP_ROT_0; };

public:
    virtual int32_t offsetX(); // the distace X from top object left coordinate
    virtual int32_t offsetY(); // the distace Y from top object left coordinate

public:
    /// @brief 设置widget矩形区域
    /// @param rect X、Y、W、H
    virtual void setRect(const TpRect &rect);
    virtual void setRect(int32_t x, int32_t y, int32_t w, int32_t h);

public:
    /// @brief 获取widget相对于屏幕的矩形区域
    /// @return 矩形区域参数
    virtual TpRect toScreen();
    /// @brief 获取widget自身的矩形区域
    /// @return 矩形区域参数
    virtual TpRect rect();

    /// @brief 获取widget所在屏幕的屏幕尺寸
    /// @return 屏幕尺寸
    virtual TpSize screenSize();

public:
    /// @brief 设置窗口宽高
    /// @param width 宽度
    /// @param height 高度
    virtual void setSize(const int32_t &width, const int32_t &height);
    /// @brief 设置窗口宽高
    /// @param size 尺寸
    virtual void setSize(const TpSize &size);
    /// @brief 获取窗口尺寸
    /// @return
    TpSize size();

    /// @brief 设置窗口宽度
    /// @param width 宽度值，单位px
    virtual void setWidth(const int32_t &width);

    /// @brief 设置窗口高度
    /// @param height 高度值，单位px
    virtual void setHeight(const int32_t &height);

    /// @brief 获取窗口当前宽度
    /// @return 宽度值
    virtual int32_t width();

    /// @brief 获取窗口当前高度
    /// @return 高度值
    virtual int32_t height();

    /// @brief 设置窗口最小尺寸
    /// @param width 最小宽度
    /// @param height 最小高度
    virtual void setMinimumSize(const int32_t &width, const int32_t &height);
    /// @brief 设置窗口最小尺寸
    /// @param minimumSize 最小尺寸
    virtual void setMinimumSize(const TpSize &minimumSize);
    /// @brief 获取窗口的最小尺寸
    /// @return 最小尺寸
    virtual TpSize minimumSize();

    /// @brief 设置窗口最小宽度
    /// @param width 最小宽度
    virtual void setMinumumWidth(const int32_t &width);
    /// @brief 获取窗口最小宽度
    /// @return 最小宽度
    virtual int32_t minumumWidth();
    /// @brief 设置窗口最小高度
    /// @param height 最小高度
    virtual void setMinumumHeight(const int32_t &height);
    /// @brief 获取窗口最小高度
    /// @return 最小高度
    virtual int32_t minumumHeight();

    /// @brief 设置窗口最大尺寸
    /// @param width 最大宽度
    /// @param height 最大高度
    virtual void setMaximumSize(const int32_t &width, const int32_t &height);
    /// @brief 设置窗口最大尺寸
    /// @param maximumSize 最大尺寸
    virtual void setMaximumSize(const TpSize &maximumSize);
    /// @brief 获取窗口的最大尺寸
    /// @return 最大尺寸
    virtual TpSize maximumSize();

    /// @brief 设置窗口最大宽度
    /// @param width 最大宽度
    virtual void setMaxumumWidth(const int32_t &width);
    /// @brief 获取窗口最大宽度
    /// @return 窗口最大宽度
    virtual int32_t maxumumWidth();

    /// @brief设置窗口最大高度
    /// @param height最大高度
    virtual void setMaxumumHeight(const int32_t &height);
    /// @brief 获取窗口最大高度
    /// @return 最大高度
    virtual int32_t maxumumHeight();

    /// @brief 设置窗口固定大小，设置后窗口大小不会被改变
    /// @param width 固定宽度
    /// @param height 固定高度
    virtual void setFixedSize(const int32_t &width, const int32_t &height);
    /// @brief 设置窗口固定宽度，设置后窗口宽度不会被改变
    /// @param width固定宽度
    virtual void setFixedWidth(const int32_t &width);
    /// @brief 设置窗口固定高度，设置后窗口高度不会被改变
    /// @param height固定高度
    virtual void setFixedHeight(const int32_t &height);

    /// @brief 窗口是否是固定大小
    /// @return 是返回true，否则返回false
    bool isFixedSize();
    /// @brief 窗口是否是固定宽度
    /// @return 是返回true，否则返回false
    bool isFixedWidth();
    /// @brief 窗口是否是固定高度
    /// @return 是返回true，否则返回false
    bool isFixedHeight();

public:
    /// @brief 设置窗口坐标，以父窗口左上角顶点为（0，0）坐标
    /// @param x X坐标
    /// @param y Y坐标
    virtual void move(int32_t x, int32_t y);

    /// @brief 获取窗口当前坐标
    /// @return 返回窗口当前坐标
    virtual const TpPoint pos();

public:
    /// @brief 设置窗口的不透明度乘数；当前窗口及所有子窗口都会响应
    /// @param opacity [0,1]；0=0%不透明，1=100%不透明
    virtual void setWindowOpacity(float opacity);
    /// @brief 获取窗体当前不透明度乘数
    /// @return 当前不透明度乘数
    virtual float windowOpacity();

    /// @brief 将当前组件调整为父组件下的最顶级组件
    virtual void bringToTop();
    /// @brief 将当前组件调整为父组件下的最底层组件
    virtual void bringToBottom();

public:
    /// @brief 设置窗体布局，如果已经存在布局则设置失败，需要先释放旧布局指针
    /// @param layout 布局指针
    /// @return 设置结果。成功返回true
    virtual bool setLayout(TpLayout *layout);

    /// @brief 获取当前窗口的布局指针
    /// @return 布局指针，没有布局则返回空
    TpLayout *layout();

public:
    virtual void update(const TpRect &rect, bool onlyBlit = false);
    virtual void update(int32_t x, int32_t y, int32_t w, int32_t h, bool onlyBlit = false);
    virtual void update(bool onlyBlit = false);

public:
    /// @brief 设置是否时可选中模式
    /// @param _checkable 是否可选中，true为可选中，默认为false
    void setCheckable(const bool &_checkable);
    /// @brief 获取当前是否是可选中状态
    /// @return 是否可选中，true为可选中，默认为false
    bool checkable();

    /// @brief 设置选中状态
    /// @param _isChecked 选中状态，true为选中
    virtual void setChecked(const bool &_isChecked);
    /// @brief 获取当前选中状态
    /// @return true为已选中
    virtual bool checked();

    /// @brief 设置边框圆角 单位px
    /// @param round
    virtual void setRoundCorners(const uint32_t &round);

    /// @brief 获取边框圆角值，单位px
    /// @return 边框圆角值
    uint32_t roundCorners();

public:
    virtual TpImage backGroundCacheImage();

    /// @brief 设置背景图片显示
    /// @param image 背景图片对象
    /// @param keepAspectRatio 是否保持纵横比
    virtual void setBackGroundImage(TpImage image, bool keepAspectRatio = true);
    /// @brief 获取当前的背景图片资源
    /// @return 图片对象指针，没有则返回空
    virtual TpImage backGroundImage();
    /// @brief 获取是否启用背景图片绘制
    /// @return 启动返回true，否则返回false
    virtual bool enableBackGroundImage();
    /// @brief 设置是否启用背景图片绘制
    /// @param enable true绘制
    virtual void setEnableBackGroundImage(bool enable);

    /// @brief 设置背景颜色
    /// @param color 颜色值
    /// @param enable 启动/禁用背景颜色显示
    virtual void setBackGroundColor(const TpColors &color, bool enable = true);
    /// @brief 设置背景颜色
    /// @param color 颜色值
    /// @param enable 启动/禁用背景颜色显示
    virtual void setBackGroundColor(int32_t color, bool enable = true);

    /// @brief 设置背景填充效果
    /// @param bgBrush 填充效果
    /// @param enable 启动/禁用背景颜色显示
    virtual void setBackGroundColor(const TpBrush &bgBrush, bool enable = true);

    /// @brief 获取当前背景颜色RGBA
    /// @return RGBA值
    virtual uint32_t backGroundColor();
    /// @brief 获取是否启用背景颜色绘制
    /// @return 启用返回true，否则返回false
    virtual bool enableBackGroundColor();
    /// @brief 设置是否启用背景颜色绘制
    /// @param enable
    virtual void setEnableBackGroundColor(bool enable);

    /// @brief 设置边框颜色
    /// @param color 颜色值
    /// @param enable 启动/禁用边框颜色显示
    virtual void setBorderColor(const TpColors &color, bool enable = true);
    /// @brief 设置边框颜色
    /// @param color 颜色值
    /// @param enable 启动/禁用边框颜色显示
    virtual void setBorderColor(int32_t color, bool enable = true);
    /// @brief 设置边框填充效果;渐变效果等
    /// @param bgBrush 填充效果
    /// @param enable 启动/禁用边框颜色显示
    virtual void setBorderColor(const TpBrush &borderBrush, bool enable = true);
    /// @brief 获取当前边框颜色RGBA
    /// @return RGBA值
    virtual uint32_t borderColor();
    /// @brief 获取当前是否启用边框线绘制
    /// @return true启用，false禁用
    virtual bool enableBorderColor();
    /// @brief 设置启用/禁用边框线绘制
    /// @param enable true为启用，false禁用
    virtual void setEnabledBorderColor(bool enable);

    /// @brief 设置窗口模糊特效;设置后会自动启用背景模糊
    /// @param blurEffect 模糊特效对象
    virtual void setGraphicsEffect(const TpGraphicsBlurEffect &blurEffect);
    /// @brief 获取当前模糊特效属性；未设置则为默认模糊效果
    /// @return 模糊特效对象
    virtual TpGraphicsBlurEffect graphicsEffect();

    /// @brief 设置启用/禁用背景模糊
    /// @param enable 是否启用背景模糊
    virtual void setEnableGraphicsEffect(const bool &enable = false);

    /// @brief 获取当前是否启用背景模糊
    /// @return 是否启用背景模糊
    bool enableGraphicsEffect();

    /// @brief 重设当前窗体的父窗体
    /// @param parent 父窗体指针
    virtual void setParent(TpObject *parent) override;

public:
    /// @brief 设置鼠标事件监听函数
    /// @param func 事件监听函数
    // virtual void setMouseKeyEventListener(TpMouseKeyEventListenerFunc func);

public:
    /// @brief 键盘按下事件
    /// @param event
    virtual bool onKeyPressEvent(TpKeyboardEvent *event) { return true; }
    /// @brief 键盘释放事件
    /// @param event
    virtual bool onKeyReleaseEvent(TpKeyboardEvent *event) { return true; }

    virtual bool onMousePressEvent(TpMouseEvent *event);
    virtual bool onMouseRleaseEvent(TpMouseEvent *event);
    virtual bool onMouseDoubleClickEvent(TpMouseEvent *event) { return true; }
    virtual bool onMouseLongPressEvent(TpMouseEvent *event) { return true; }
    virtual bool onMouseMoveEvent(TpMouseEvent *event) { return true; }
    virtual bool onWheelEvent(TpWheelEvent *event) { return true; }

    virtual bool onFingerEvent(TpFingerEvent *event) { return true; }
    virtual bool onDollAREvent(TpDollAREvent *event) { return true; }
    virtual bool onMultiGestureEvent(TpMultiGestureEvent *event) { return true; }
    virtual bool onMoveEvent(TpMoveEvent *event) { return true; }
    virtual bool onResizeEvent(TpResizeEvent *event);
    virtual bool onFocusEvent(TpFocusEvent *event) { return true; }
    virtual bool onLeaveEvent(TpLeaveEvent *event);
    virtual bool onVisibleEvent(TpVisibleEvent *event) { return true; }
    /// @brief 绘制事件，禁止在该函数调用 paint和update函数
    /// @param event 绘制事件指针
    /// @return 返回true继续执行子控件绘制
    virtual bool onPaintEvent(TpPaintEvent *event);
    virtual bool onActiveEvent(TpActiveEvent *event) { return true; }

    virtual void onThemeChangeEvent(TpThemeChangeEvent *event);

public:
    /// @brief 获取对象类型，一般禁止重写
    /// @return 对象类型
    virtual Tp::TpObjectType objectType() /*final*/;

    virtual bool appChange(int32_t id, int32_t pid, int32_t visible, int32_t active, int32_t color, uint8_t alpha, int32_t require) { return true; }

    virtual TpWidget *find(const TpPoint &point);
    virtual TpWidget *find(int32_t x, int32_t y);

public:
    /// @brief 补充CSS样式，系统内置CSS除非同名否则不会被覆盖
    /// @param _styleSheetStr CSS样式字符串或者文件
    virtual void setStyleSheet(const TpString &_styleSheetStr);

    /// @brief 获取当前系统的CSS样式字符串
    /// @return CSS字符串
    TpString styleSheet();

    /// @brief 指定类名，状态，获取对象的CSS数据
    /// @param _className
    /// @param _status
    /// @return 读取的CSS数据指针
    tpShared<TpCssData> readCss(const TpString &_className, const TpCssParser::MouseStatus &_status);

    /// @brief 获取当前窗口截图
    /// @return 窗口图片
    TpImage grabWindow();

    virtual void broadSetTop();

public:
    /// @brief 组件类名，子类实现，返回子类类名字符串，用于匹配CSS中对应样式
    /// @return 类名字符串
    virtual TpString pluginType() { return TO_STRING(TpWidget); }

public:
    /// @brief 外部无需调用
    std::pair<void *, void *> canvasPtr();

protected:
    /// @brief 自动根据控件状态获取当前CSS（启用、悬停、选中、禁用等）
    /// @return CSS数据
    tpShared<TpCssData> currentStatusCss();

    tpShared<TpCssData> enabledCss();

    tpShared<TpCssData> disableCss();

    tpShared<TpCssData> hoveredCss();

    tpShared<TpCssData> pressedCss();

    tpShared<TpCssData> checkedCss();

    /// @brief 子类构造函数可调用该函数完成基础CSS的初始化，例如size、round等，前提需实现 pluginType 函数
    void refreshBaseCss();

public:
    /// @brief 对象
    /// @return
    virtual bool objectActive() { return false; }
};

#endif
