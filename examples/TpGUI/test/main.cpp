#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include <TpGUI.h>
#include "TpImage.h"
#include "TpBattery.h"
#include "TpLabel.h"
#include "TpTimer.h"
#include "TpDialog.h"
#include "TpFont.h"
#include "TpLinearGradient.h"
#include "TpRadialGradient.h"
#include "TpSlider.h"
#include "TpGraphicsBlurEffect.h"
#include "TpButton.h"
#include "tinyPiXSys.h"
#include "tinyPiXUtils.h"
#include "TpSurface.h"
#include "png.h"
#include "TpMainWindow.h"
#include "Service/TpSystemApi.h"

class ThorVgPaintWidget : public TpWidget
// class ThorVgPaintWidget : public TpDialog
{
public:
    ThorVgPaintWidget(TpWidget *parent)
        : TpWidget(parent)
    {
        setBackGroundColor(_RGBA(100, 100, 100, 200));
        // setBackGroundImage(TpImage(applicationDirPath() + "/test.svg"));
        // setBackGroundImage(TpImage(applicationDirPath() + "/icon.png"));
    }
    ~ThorVgPaintWidget()
    {
    }

    virtual bool onMousePressEvent(TpMouseEvent *event) override
    {

        std::cout << "pos: " << pos().x() << " , " << pos().y() << std::endl;

        move(pos().x() + 10, pos().y());
        if (pos().x() + width() > 1080)
        {
            move(150, pos().y());
        }

        TpImage grabImage = grabWindow();

        return true;
    }

    virtual bool onPaintEvent(TpPaintEvent *event) override
    {
        // static uint64_t paintCount = 0;
        // std::cout << "ThorVgPaintWidget::onPaintEvent " << paintCount++ << std::endl;

        TpWidget::onPaintEvent(event);
        // TpDialog::onPaintEvent(event);

        TpPainter *painter = event->painter();
        // painter->paintTest();

        TpFont testFont;
        testFont.setText("这是一个测试字符串");
        testFont.setFontSize(15);
        testFont.setFontColor(_RGB(220, 0, 0));

        painter->drawText(testFont, 20, 20);

        return true;
    }

private:
};

IPiSysApiAgent *globalAgent = tinyPiX_sys_create();

TpImage getImage()
{
    IPiWFSurface *surfacePtr = tinyPiX_sys_get_process_surface(globalAgent, getpid());
    if (!surfacePtr)
        return TpImage();

    tpShared<TpSurface> appDisplayImage = tpMakeShared<TpSurface>(surfacePtr);

    TpImage resImage;
    resImage.load(appDisplayImage->matrix(), TpSize(appDisplayImage->width(), appDisplayImage->height()),
                  TpRect(0, 36, appDisplayImage->width(), appDisplayImage->height() - 36));

    TpImage copyImage = resImage;

    tinyPiX_surface_free(surfacePtr);

    return copyImage;
}


// 自定义绘图组件  
class TpPenDemoWidget : public TpWidget  
{  
public:  
    TpPenDemoWidget(TpWidget *parent = nullptr) : TpWidget(parent)   
    {  
        setBackGroundColor(_RGBA(240, 240, 240, 255));  
          
        // 初始化渐变对象(成员变量)  
        gradientForDemo.setStart(TpPointF(0, 0));  
        gradientForDemo.setFinalStop(TpPointF(300, 0));  
        gradientForDemo.setColorAt(0.0, TpColors(255, 0, 0));  
        gradientForDemo.setColorAt(0.5, TpColors(0, 255, 0));  
        gradientForDemo.setColorAt(1.0, TpColors(0, 0, 255));  
          
        // 创建所有标签  
        int y = 5;  
        int spacing = 60;  
          
        label1 = new TpLabel(this);  
        label1->setText("1. 不同宽度的线条");  
        label1->setRect(10, y, 300, 20);  
        label1->setBackGroundColor(_RGBA(0, 0, 0, 0));  
        y += spacing;  
          
        label2 = new TpLabel(this);  
        label2->setText("2. 不同颜色的线条");  
        label2->setRect(10, y, 300, 20);  
        label2->setBackGroundColor(_RGBA(0, 0, 0, 0));  
        y += spacing;  
          
        label3 = new TpLabel(this);  
        label3->setText("3. 线帽样式 (ButtCap, RoundCap, SquareCap)");  
        label3->setRect(10, y, 450, 20);  
        label3->setBackGroundColor(_RGBA(0, 0, 0, 0));  
        y += spacing;  
          
        label4 = new TpLabel(this);  
        label4->setText("4. 连接样式 (MiterJoin, RoundJoin, BevelJoin)");  
        label4->setRect(10, y, 450, 20);  
        label4->setBackGroundColor(_RGBA(0, 0, 0, 0));  
        y += spacing + 20;  
          
        label5 = new TpLabel(this);  
        label5->setText("5. 渐变线条");  
        label5->setRect(10, y, 300, 20);  
        label5->setBackGroundColor(_RGBA(0, 0, 0, 0));  
        y += spacing;  
          
        label6 = new TpLabel(this);  
        label6->setText("6. 形状轮廓绘制");  
        label6->setRect(10, y, 300, 20);  
        label6->setBackGroundColor(_RGBA(0, 0, 0, 0));  
    }  
      
protected:  
    virtual bool onPaintEvent(TpPaintEvent *event) override  
    {  
        TpWidget::onPaintEvent(event);  
        TpPainter *painter = event->painter();  
          
        int startY = 20;  
        int lineSpacing = 60;  
          
        // ========== 示例 1: 不同宽度 ==========  
        for (int i = 1; i <= 5; i++) {  
            TpPen pen(_RGB(0, 0, 0), i);  
            painter->setPen(pen);  
            painter->drawLine(50, startY + (i-1) * 10, 350, startY + (i-1) * 10);  
        }  
        startY += lineSpacing;  
          
        // ========== 示例 2: 不同颜色 ==========  
        TpColors colors[] = {  
            _RGB(255, 0, 0), _RGB(0, 255, 0), _RGB(0, 0, 255),  
            _RGB(255, 255, 0), _RGB(255, 0, 255)  
        };  
        for (int i = 0; i < 5; i++) {  
            TpPen pen(colors[i], 3);  
            painter->setPen(pen);  
            painter->drawLine(50, startY + i * 8, 350, startY + i * 8);  
        }  
        startY += lineSpacing;  
          
        // ========== 示例 3: 线帽样式 ==========  
        TpPen buttPen(_RGB(255, 0, 0), 10);  
        buttPen.setCapStyle(Tp::ButtCap);  
        painter->setPen(buttPen);  
        painter->drawLine(50, startY, 150, startY);  
          
        TpPen roundPen(_RGB(0, 255, 0), 10);  
        roundPen.setCapStyle(Tp::RoundCap);  
        painter->setPen(roundPen);  
        painter->drawLine(180, startY, 280, startY);  
          
        TpPen squarePen(_RGB(0, 0, 255), 10);  
        squarePen.setCapStyle(Tp::SquareCap);  
        painter->setPen(squarePen);  
        painter->drawLine(310, startY, 410, startY);  
        startY += lineSpacing;  
          
        // ========== 示例 4: 连接样式 ==========  
        TpPen miterPen(_RGB(255, 0, 0), 8);  
        miterPen.setJoinStyle(Tp::MiterJoin);  
        painter->setPen(miterPen);  
        painter->setBrush(TpBrush(Tp::NoBrush));  
        TpVector<TpPoint> miterPoints = {  
            TpPoint(50, startY + 30), TpPoint(80, startY), TpPoint(110, startY + 30)  
        };  
        painter->drawPolygon(miterPoints);  
          
        TpPen roundJoinPen(_RGB(0, 255, 0), 8);  
        roundJoinPen.setJoinStyle(Tp::RoundJoin);  
        painter->setPen(roundJoinPen);  
        TpVector<TpPoint> roundPoints = {  
            TpPoint(180, startY + 30), TpPoint(210, startY), TpPoint(240, startY + 30)  
        };  
        painter->drawPolygon(roundPoints);  
          
        TpPen bevelPen(_RGB(0, 0, 255), 8);  
        bevelPen.setJoinStyle(Tp::BevelJoin);  
        painter->setPen(bevelPen);  
        TpVector<TpPoint> bevelPoints = {  
            TpPoint(310, startY + 30), TpPoint(340, startY), TpPoint(370, startY + 30)  
        };  
        painter->drawPolygon(bevelPoints);  
        startY += lineSpacing + 20;  
          
        // ========== 示例 5: 渐变线条 ==========  
        TpBrush gradientBrush(&gradientForDemo);  
        TpPen gradientPen;  
        gradientPen.setWidth(10);  
        gradientPen.setBrush(gradientBrush);  
        painter->setPen(gradientPen);  
        painter->drawLine(50, startY, 350, startY);  
        startY += lineSpacing;  
          
        // ========== 示例 6: 形状轮廓 ==========  
        TpPen rectPen(_RGB(255, 0, 0), 3);  
        painter->setPen(rectPen);  
        painter->setBrush(TpBrush(Tp::NoBrush));  
        painter->drawRect(50, startY, 80, 40, 5);  
          
        TpPen circlePen(_RGB(0, 255, 0), 3);  
        painter->setPen(circlePen);  
        painter->drawEllipse(190, startY + 20, 20, 20);  
          
        TpPen polygonPen(_RGB(0, 0, 255), 3);  
        painter->setPen(polygonPen);  
        TpVector<TpPoint> polygonPoints = {  
            TpPoint(280, startY + 40), TpPoint(310, startY),  
            TpPoint(340, startY + 40), TpPoint(325, startY + 50),  
            TpPoint(295, startY + 50)  
        };  
        painter->drawPolygon(polygonPoints);  
          
        return true;  
    }  
      
private:  
    TpLinearGradient gradientForDemo;  // 成员变量,确保生命周期  
    TpLabel *label1, *label2, *label3, *label4, *label5, *label6;  
};  

int32_t main(int32_t argc, char *argv[])
{
    TpApp app(argc, argv);
    app.setStyle(Tp::SmartDeviceGUIStyle);

    TpMainWindow *vScreen = new TpMainWindow();
    vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
    // vScreen->setBackGroundImage(TpImage(applicationDirPath() + "/icon.png"));

    // TpLabel *bgLabel = new TpLabel(vScreen);
    // bgLabel->setBorderColor(_RGB(255, 0, 0));
    // bgLabel->setRect(250, 50, 450, 450);

    // TpButton *testBtn = new TpButton(vScreen);
    // testBtn->setText("获取当前进程截图");
    // testBtn->setRect(50, 50, 150, 50);
    // connect(testBtn, onClicked, [=](bool)
    //         { bgLabel->setBackGroundImage(getImage()); });

    // ThorVgPaintWidget *thorVGPaint = new ThorVgPaintWidget(vScreen);
    // thorVGPaint->setRect(0, 0, 500, 500);

    // TpLabel *textTestLabel = new TpLabel("自动获取", vScreen);
    // textTestLabel->setBackGroundColor(_RGB(255, 0, 0));
    // textTestLabel->setAlign(Tp::AlignLeft);
    // textTestLabel->font()->setFontSize(19);
    // textTestLabel->setRect(520, 20, textTestLabel->font()->pixelWidth(), textTestLabel->font()->pixelHeight());

    // TpLabel *textTestLabel2 = new TpLabel("以太网", vScreen);
    // textTestLabel2->setBackGroundColor(_RGB(255, 0, 0));
    // textTestLabel2->font()->setFontSize(19);
    // textTestLabel2->setAlign(Tp::AlignBottom);
    // textTestLabel2->setRect(520, 200, textTestLabel2->font()->pixelWidth(), textTestLabel2->font()->pixelHeight());

    TpPenDemoWidget* gradiantWidget = new TpPenDemoWidget(vScreen);
    gradiantWidget->setRect(10, 10, 600, 600);

    vScreen->update();

    return app.run();
}
