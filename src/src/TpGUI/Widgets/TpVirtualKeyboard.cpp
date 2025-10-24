#include "TpVirtualKeyboard.h"
#include "TpInputMethod.h"
#include "TpFont.h"
#include "TpEvent.h"
#include "TpVBoxLayout.h"
#include "TpHBoxLayout.h"
#include "TpApp.h"
#include "TpPainter.h"
#include "TpButton.h"

#ifndef TOP_MARGIN
#define TOP_MARGIN 12
#endif

#ifndef DOWN_MARGIN
#define DOWN_MARGIN 20
#endif

#ifndef LEFT_MARGIN
#define LEFT_MARGIN 9
#endif

#ifndef RIGHT_MARGIN
#define RIGHT_MARGIN 9
#endif

// 每一行间距
#ifndef V_INTERVAL_MARGIN
#define V_INTERVAL_MARGIN 10
#endif

// 每一个按钮水平间距
#ifndef H_INTERVAL_MARGIN
#define H_INTERVAL_MARGIN 6
#endif

const TpString globalInputChineseIconPath = "/usr/res/TinyPiX/virtualKeyboard/中文输入.png";
const TpString globalInputEnglishIconPath = "/usr/res/TinyPiX/virtualKeyboard/英文输入.png";

struct VirtualKeyButton
{
    TpButton *keyQ;
    TpButton *keyW;
    TpButton *keyE;
    TpButton *keyR;
    TpButton *keyT;
    TpButton *keyY;
    TpButton *keyU;
    TpButton *keyI;
    TpButton *keyO;
    TpButton *keyP;

    TpButton *keyA;
    TpButton *keyS;
    TpButton *keyD;
    TpButton *keyF;
    TpButton *keyG;
    TpButton *keyH;
    TpButton *keyJ;
    TpButton *keyK;
    TpButton *keyL;

    // 大小写切换
    TpButton *toggleCaseKey;

    TpButton *keyZ;
    TpButton *keyX;
    TpButton *keyC;
    TpButton *keyV;
    TpButton *keyB;
    TpButton *keyN;
    TpButton *keyM;

    // 删除按钮
    TpButton *deleteKey;

    // 切换数字符号按钮
    TpButton *symbolKey;

    // 空格
    TpButton *sapceKey;

    // 中英文切换
    TpButton *switchChineseKey;

    // 确定按钮
    TpButton *applyKey;

    VirtualKeyButton(TpWidget *_this)
    {
        auto InitButtonFunc = [=](TpButton *&button)
        {
            button = new TpButton();
            button->installEventFilter(_this);
            button->setProperty("type", "VirtualKeyboardButton");
        };

        InitButtonFunc(keyQ);
        InitButtonFunc(keyW);
        InitButtonFunc(keyE);
        InitButtonFunc(keyR);
        InitButtonFunc(keyT);
        InitButtonFunc(keyY);
        InitButtonFunc(keyU);
        InitButtonFunc(keyI);
        InitButtonFunc(keyO);
        InitButtonFunc(keyP);
        InitButtonFunc(keyA);
        InitButtonFunc(keyS);
        InitButtonFunc(keyD);
        InitButtonFunc(keyF);
        InitButtonFunc(keyG);
        InitButtonFunc(keyH);
        InitButtonFunc(keyJ);
        InitButtonFunc(keyK);
        InitButtonFunc(keyL);
        InitButtonFunc(toggleCaseKey);
        InitButtonFunc(keyZ);
        InitButtonFunc(keyX);
        InitButtonFunc(keyC);
        InitButtonFunc(keyV);
        InitButtonFunc(keyB);
        InitButtonFunc(keyN);
        InitButtonFunc(keyM);
        InitButtonFunc(deleteKey);
        InitButtonFunc(symbolKey);
        InitButtonFunc(sapceKey);
        InitButtonFunc(switchChineseKey);

        // 确定按钮
        InitButtonFunc(applyKey);
        applyKey->setProperty("type", "VirtualKeyboardApplyButton");
    }

    ~VirtualKeyButton()
    {
        keyQ->deleteLater();
        keyW->deleteLater();
        keyE->deleteLater();
        keyR->deleteLater();
        keyT->deleteLater();
        keyY->deleteLater();
        keyU->deleteLater();
        keyI->deleteLater();
        keyO->deleteLater();
        keyP->deleteLater();
        keyA->deleteLater();
        keyS->deleteLater();
        keyD->deleteLater();
        keyF->deleteLater();
        keyG->deleteLater();
        keyH->deleteLater();
        keyJ->deleteLater();
        keyK->deleteLater();
        keyL->deleteLater();
        toggleCaseKey->deleteLater();
        keyZ->deleteLater();
        keyX->deleteLater();
        keyC->deleteLater();
        keyV->deleteLater();
        keyB->deleteLater();
        keyN->deleteLater();
        keyM->deleteLater();
        deleteKey->deleteLater();
        symbolKey->deleteLater();
        sapceKey->deleteLater();
        switchChineseKey->deleteLater();
        applyKey->deleteLater();
    }
};
typedef tpShared<VirtualKeyButton> VirtualKeyButtonSPtr;

/// @brief 候选词窗口
class TpCandidateWidget : public TpWidget
{
public:
    TpCandidateWidget(TpWidget *panret = nullptr);
    ~TpCandidateWidget();

    // 设置拼音字符
    void setPinyinStr(const TpString &str);

    // 设置候选词列表
    void setCandidateWordList(const TpVector<TpString> &wordList);

    // 第一个候选词
    TpString firstCandidateWord();

    // 设置选词回调函数
    typedef std::function<void(const TpString &)> SelectCandidateWordFunc;
    void setSlectCandidateFunc(SelectCandidateWordFunc func);

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onMouseMoveEvent(TpMouseEvent *event) override;

private:
    TpString cachePinyin_;
    TpVector<TpString> wordList_;
    TpFont *textFont_;

    SelectCandidateWordFunc selectCandidateDealFunc_;

    // 候选词偏移量
    int32_t curWordOffset_;

    TpPoint mousePressPos_;

    const uint32_t bgLeftRightMargin_ = 12;
    const uint32_t bgTopBottomMargin_ = 5;

    // 所有候选词区域，用于判定点击
    TpVector<TpRect> allWordRect_;

    // 是否鼠标按到候选词以及候选词背景rect
    bool isPressWordRect_ = false;
    TpRect pressWordRect_;
};

struct TpVirtualKeyboardData
{
    VirtualKeyButtonSPtr virtualKeyButtonPtr;
    TpCandidateWidget *candidateWindow;

    TpInputMethod *inputMethod;

    // 是否是中文输入
    bool isChinese = true;

    // 当前是否是字符页面
    bool isSymbol = false;

    ~TpVirtualKeyboardData()
    {
        candidateWindow->deleteLater();
        candidateWindow = nullptr;
    }
};

// 默认状态字符显示 isUpper：是否大写
void refreshNormalCharacter(VirtualKeyButtonSPtr buttonGroup, bool isUpper)
{
    buttonGroup->keyQ->setText(isUpper ? "Q" : "q");
    buttonGroup->keyW->setText(isUpper ? "W" : "w");
    buttonGroup->keyE->setText(isUpper ? "E" : "e");
    buttonGroup->keyR->setText(isUpper ? "R" : "r");
    buttonGroup->keyT->setText(isUpper ? "T" : "t");
    buttonGroup->keyY->setText(isUpper ? "Y" : "y");
    buttonGroup->keyU->setText(isUpper ? "U" : "u");
    buttonGroup->keyI->setText(isUpper ? "I" : "i");
    buttonGroup->keyO->setText(isUpper ? "O" : "o");
    buttonGroup->keyP->setText(isUpper ? "P" : "p");
    buttonGroup->keyA->setText(isUpper ? "A" : "a");
    buttonGroup->keyS->setText(isUpper ? "S" : "s");
    buttonGroup->keyD->setText(isUpper ? "D" : "d");
    buttonGroup->keyF->setText(isUpper ? "F" : "f");
    buttonGroup->keyG->setText(isUpper ? "G" : "g");
    buttonGroup->keyH->setText(isUpper ? "H" : "h");
    buttonGroup->keyJ->setText(isUpper ? "J" : "j");
    buttonGroup->keyK->setText(isUpper ? "K" : "k");
    buttonGroup->keyL->setText(isUpper ? "L" : "l");
    buttonGroup->keyZ->setText(isUpper ? "Z" : "z");
    buttonGroup->keyX->setText(isUpper ? "X" : "x");
    buttonGroup->keyC->setText(isUpper ? "C" : "c");
    buttonGroup->keyV->setText(isUpper ? "V" : "v");
    buttonGroup->keyB->setText(isUpper ? "B" : "b");
    buttonGroup->keyN->setText(isUpper ? "N" : "n");
    buttonGroup->keyM->setText(isUpper ? "M" : "m");

    // 配置大小写按钮属性
    if (!buttonGroup->toggleCaseKey->checkable())
    {
        buttonGroup->toggleCaseKey->setCheckable(true);
        buttonGroup->toggleCaseKey->setChecked(false);
        buttonGroup->toggleCaseKey->setButtonStyle(TpButton::IconOnly);
        buttonGroup->toggleCaseKey->update();
    }
}

// isAll true为第一级字符页面 false为第二级
void refreshSymbolCharacter(VirtualKeyButtonSPtr buttonGroup, bool isAll, bool isChinese)
{
    buttonGroup->keyQ->setText(isAll ? "1" : (isChinese ? "【" : "["));
    buttonGroup->keyW->setText(isAll ? "2" : (isChinese ? "】" : "]"));
    buttonGroup->keyE->setText(isAll ? "3" : "{");
    buttonGroup->keyR->setText(isAll ? "4" : "}");
    buttonGroup->keyT->setText(isAll ? "5" : "#");
    buttonGroup->keyY->setText(isAll ? "6" : "%");
    buttonGroup->keyU->setText(isAll ? "7" : "^");
    buttonGroup->keyI->setText(isAll ? "8" : "*");
    buttonGroup->keyO->setText(isAll ? "9" : "+");
    buttonGroup->keyP->setText(isAll ? "0" : "=");
    buttonGroup->keyA->setText(isAll ? "-" : (isChinese ? "——" : "_"));
    buttonGroup->keyS->setText(isAll ? "/" : "\\");
    buttonGroup->keyD->setText(isAll ? (isChinese ? "：" : ":") : "|");
    buttonGroup->keyF->setText(isAll ? (isChinese ? "；" : ";") : "~");
    buttonGroup->keyG->setText(isAll ? (isChinese ? "（" : "(") : (isChinese ? "《" : "<"));
    buttonGroup->keyH->setText(isAll ? (isChinese ? "）" : ")") : (isChinese ? "》" : ">"));
    buttonGroup->keyJ->setText(isAll ? (isChinese ? "￥" : "$") : "&");
    buttonGroup->keyK->setText(isAll ? (isChinese ? "“" : "\"") : (isChinese ? "‘" : "\'"));
    buttonGroup->keyL->setText(isAll ? (isChinese ? "”" : "\"") : (isChinese ? "’" : "\'"));

    buttonGroup->keyZ->setText(isAll ? "@" : (isChinese ? "·" : "`"));
    buttonGroup->keyX->setText(isChinese ? "。" : ".");
    buttonGroup->keyC->setText(isChinese ? "，" : "\"");
    buttonGroup->keyV->setText(isChinese ? "、" : "\"");
    buttonGroup->keyB->setText(isChinese ? "？" : "\"");
    buttonGroup->keyN->setText(isChinese ? "！" : "\"");
    buttonGroup->keyM->setText("...");

    // 配置大小写按钮属性
    if (buttonGroup->toggleCaseKey->checkable())
    {
        buttonGroup->toggleCaseKey->setCheckable(false);
        buttonGroup->toggleCaseKey->setChecked(false);
        buttonGroup->toggleCaseKey->setButtonStyle(TpButton::TextOnly);
        buttonGroup->toggleCaseKey->setText(isAll ? "#+=" : "123");
        buttonGroup->toggleCaseKey->update();
    }
}

TpVirtualKeyboard::TpVirtualKeyboard()
    : TpDialog(), data_(nullptr)
{
    setBackGroundColor(_RGB(228, 230, 232));

    TpVirtualKeyboardData *vkData = new TpVirtualKeyboardData();
    data_ = vkData;

    vkData->inputMethod = new TpInputMethod();
    vkData->virtualKeyButtonPtr = tpMakeShared<VirtualKeyButton>(this);
    vkData->candidateWindow = new TpCandidateWidget();

    // 注册选词回调
    auto selectCandidateWordFunc = [=](const TpString &selectWord)
    {
        vkData->inputMethod->clear();
        finishChinese.emit(selectWord);
    };
    vkData->candidateWindow->setSlectCandidateFunc(selectCandidateWordFunc);

    configWindowLayout();
    configSignal();
}

TpVirtualKeyboard::~TpVirtualKeyboard()
{
    TpVirtualKeyboardData *vkData = static_cast<TpVirtualKeyboardData *>(data_);
    if (vkData)
    {
        delete vkData;
        vkData = nullptr;
        data_ = nullptr;
    }
}

bool TpVirtualKeyboard::isChinese()
{
    TpVirtualKeyboardData *vkData = static_cast<TpVirtualKeyboardData *>(data_);
    return vkData->isChinese;
}

bool TpVirtualKeyboard::switchChinese(const bool &isChinese)
{
    TpVirtualKeyboardData *vkData = static_cast<TpVirtualKeyboardData *>(data_);
    vkData->isChinese = isChinese;

    vkData->virtualKeyButtonPtr->switchChineseKey->setIcon(isChinese ? globalInputChineseIconPath : globalInputEnglishIconPath);
    // std::cout << "vkData->isChinese " << vkData->isChinese << std::endl;

    return true;
}

bool TpVirtualKeyboard::isUpper()
{
    TpVirtualKeyboardData *vkData = static_cast<TpVirtualKeyboardData *>(data_);
    return vkData->virtualKeyButtonPtr->toggleCaseKey->checked();
}

void TpVirtualKeyboard::show()
{
    TpVirtualKeyboardData *vkData = static_cast<TpVirtualKeyboardData *>(data_);

    TpWidget *mainScreen = TpApp::Inst()->vScreen();

    setSize(mainScreen->width(), 300);
    move(0, mainScreen->height() - height());

    refreshNormalCharacter(vkData->virtualKeyButtonPtr, false);

    TpDialog::show();
}

bool TpVirtualKeyboard::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_KEYBOARD_PRESS_TYPE)
    {
        TpKeyboardEvent *keyboardEvent = dynamic_cast<TpKeyboardEvent *>(event);
        onKeyPressEvent(keyboardEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_KEYBOARD_RELEASE_TYPE)
    {
        TpKeyboardEvent *keyboardEvent = dynamic_cast<TpKeyboardEvent *>(event);
        onKeyReleaseEvent(keyboardEvent);
    }
    else
    {
    }

    return false;
}

bool TpVirtualKeyboard::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    return true;
}

bool TpVirtualKeyboard::onKeyPressEvent(TpKeyboardEvent *event)
{
    return true;
}

bool TpVirtualKeyboard::onKeyReleaseEvent(TpKeyboardEvent *event)
{
    return true;
}

bool TpVirtualKeyboard::onResizeEvent(TpResizeEvent *event)
{
    TpWidget::onResizeEvent(event);

    TpVirtualKeyboardData *vkData = static_cast<TpVirtualKeyboardData *>(data_);
    if (!vkData)
        return true;

    // 窗口大小切换重新绘制
    int32_t buttonWidth = 0;
    int32_t buttonHeight = 0;

    // 第一行的按钮是充满的；所以根据第一行的按钮数量和窗口宽度，计算一个按钮的宽度
    buttonWidth = (width() - LEFT_MARGIN - RIGHT_MARGIN - H_INTERVAL_MARGIN * 9) / 10.0;
    // 根据窗口高度，和 四行键位 + 1行候选词 来计算按钮的高度
    buttonHeight = (height() - TOP_MARGIN - DOWN_MARGIN - V_INTERVAL_MARGIN * 4) / 6.0;

    // 字母键固定最小尺寸
    TpList<TpButton *> characterBtnList =
        {
            vkData->virtualKeyButtonPtr->keyQ,
            vkData->virtualKeyButtonPtr->keyW,
            vkData->virtualKeyButtonPtr->keyE,
            vkData->virtualKeyButtonPtr->keyR,
            vkData->virtualKeyButtonPtr->keyT,
            vkData->virtualKeyButtonPtr->keyY,
            vkData->virtualKeyButtonPtr->keyU,
            vkData->virtualKeyButtonPtr->keyI,
            vkData->virtualKeyButtonPtr->keyO,
            vkData->virtualKeyButtonPtr->keyP,
            vkData->virtualKeyButtonPtr->keyA,
            vkData->virtualKeyButtonPtr->keyS,
            vkData->virtualKeyButtonPtr->keyD,
            vkData->virtualKeyButtonPtr->keyF,
            vkData->virtualKeyButtonPtr->keyG,
            vkData->virtualKeyButtonPtr->keyH,
            vkData->virtualKeyButtonPtr->keyJ,
            vkData->virtualKeyButtonPtr->keyK,
            vkData->virtualKeyButtonPtr->keyL,
            vkData->virtualKeyButtonPtr->keyZ,
            vkData->virtualKeyButtonPtr->keyX,
            vkData->virtualKeyButtonPtr->keyC,
            vkData->virtualKeyButtonPtr->keyV,
            vkData->virtualKeyButtonPtr->keyB,
            vkData->virtualKeyButtonPtr->keyN,
            vkData->virtualKeyButtonPtr->keyM};

    for (const auto &characterButton : characterBtnList)
    {
        characterButton->setMinimumSize(buttonWidth, buttonHeight);
    }

    // 大小写。删除、符号键，设置固定尺寸，
    TpList<TpButton *> operateBtnList =
        {
            vkData->virtualKeyButtonPtr->toggleCaseKey,
            vkData->virtualKeyButtonPtr->deleteKey,
            vkData->virtualKeyButtonPtr->symbolKey,
            vkData->virtualKeyButtonPtr->switchChineseKey};
    for (const auto &operateBtn : operateBtnList)
    {
        operateBtn->setFixedWidth(buttonWidth * 1.2);
        operateBtn->setMinumumHeight(buttonHeight);
    }

    vkData->virtualKeyButtonPtr->sapceKey->setMinumumHeight(buttonHeight);

    // 确定键设置固定尺寸
    vkData->virtualKeyButtonPtr->applyKey->setFixedWidth(buttonWidth * 2.05);
    vkData->virtualKeyButtonPtr->applyKey->setMinumumHeight(buttonHeight);

    return true;
}

void TpVirtualKeyboard::configWindowLayout()
{
    TpVirtualKeyboardData *vkData = static_cast<TpVirtualKeyboardData *>(data_);

    TpVBoxLayout *mainLayout = new TpVBoxLayout();
    mainLayout->setContentsMargins(LEFT_MARGIN, TOP_MARGIN, RIGHT_MARGIN, DOWN_MARGIN);
    mainLayout->setSpacing(V_INTERVAL_MARGIN);

    TpHBoxLayout *secondRowLayout = new TpHBoxLayout();
    secondRowLayout->setContentsMargins(0, 0, 0, 0);
    TpList<TpButton *> secondRowBtnList =
        {
            vkData->virtualKeyButtonPtr->keyQ,
            vkData->virtualKeyButtonPtr->keyW,
            vkData->virtualKeyButtonPtr->keyE,
            vkData->virtualKeyButtonPtr->keyR,
            vkData->virtualKeyButtonPtr->keyT,
            vkData->virtualKeyButtonPtr->keyY,
            vkData->virtualKeyButtonPtr->keyU,
            vkData->virtualKeyButtonPtr->keyI,
            vkData->virtualKeyButtonPtr->keyO,
            vkData->virtualKeyButtonPtr->keyP,
        };
    for (const auto &button : secondRowBtnList)
    {
        secondRowLayout->addWidget(button);
    }

    TpHBoxLayout *thirdRowLayout = new TpHBoxLayout();
    thirdRowLayout->setContentsMargins(0, 0, 0, 0);
    TpList<TpButton *> thirdRowBtnList =
        {
            vkData->virtualKeyButtonPtr->keyA,
            vkData->virtualKeyButtonPtr->keyS,
            vkData->virtualKeyButtonPtr->keyD,
            vkData->virtualKeyButtonPtr->keyF,
            vkData->virtualKeyButtonPtr->keyG,
            vkData->virtualKeyButtonPtr->keyH,
            vkData->virtualKeyButtonPtr->keyJ,
            vkData->virtualKeyButtonPtr->keyK,
            vkData->virtualKeyButtonPtr->keyL};
    thirdRowLayout->addSpacer(new TpSpacerItem(10, 10, TpSpacerItem::Expanding));
    for (const auto &button : thirdRowBtnList)
    {
        thirdRowLayout->addWidget(button);
    }
    thirdRowLayout->addSpacer(new TpSpacerItem(10, 10, TpSpacerItem::Expanding));

    TpHBoxLayout *forceRowLayout = new TpHBoxLayout();
    forceRowLayout->setContentsMargins(0, 0, 0, 0);
    TpList<TpButton *> forceRowBtnList =
        {
            vkData->virtualKeyButtonPtr->keyZ,
            vkData->virtualKeyButtonPtr->keyX,
            vkData->virtualKeyButtonPtr->keyC,
            vkData->virtualKeyButtonPtr->keyV,
            vkData->virtualKeyButtonPtr->keyB,
            vkData->virtualKeyButtonPtr->keyN,
            vkData->virtualKeyButtonPtr->keyM};
    forceRowLayout->addWidget(vkData->virtualKeyButtonPtr->toggleCaseKey);
    forceRowLayout->addSpacer(new TpSpacerItem(10, 10, TpSpacerItem::Expanding));
    for (const auto &button : forceRowBtnList)
    {
        forceRowLayout->addWidget(button);
    }
    forceRowLayout->addSpacer(new TpSpacerItem(10, 10, TpSpacerItem::Expanding));
    forceRowLayout->addWidget(vkData->virtualKeyButtonPtr->deleteKey);

    TpHBoxLayout *fifthRowLayout = new TpHBoxLayout();
    fifthRowLayout->setContentsMargins(0, 0, 0, 0);
    TpList<TpButton *> fifthRowBtnList =
        {
            vkData->virtualKeyButtonPtr->symbolKey,
            vkData->virtualKeyButtonPtr->switchChineseKey,
            vkData->virtualKeyButtonPtr->sapceKey,
            vkData->virtualKeyButtonPtr->applyKey};
    for (const auto &button : fifthRowBtnList)
    {
        fifthRowLayout->addWidget(button);
    }

    mainLayout->addWidget(vkData->candidateWindow, 3);
    mainLayout->addLayout(secondRowLayout, 2);
    mainLayout->addLayout(thirdRowLayout, 2);
    mainLayout->addLayout(forceRowLayout, 2);
    mainLayout->addLayout(fifthRowLayout, 2);

    setLayout(mainLayout);

    // 配置一些按钮属性
    vkData->virtualKeyButtonPtr->toggleCaseKey->setCheckable(true);
    vkData->virtualKeyButtonPtr->toggleCaseKey->setButtonStyle(TpButton::IconOnly);
    vkData->virtualKeyButtonPtr->toggleCaseKey->setIconSize(22, 22);
    vkData->virtualKeyButtonPtr->toggleCaseKey->setIcon("/usr/res/TinyPiX/virtualKeyboard/大小写切换.png");

    vkData->virtualKeyButtonPtr->deleteKey->setButtonStyle(TpButton::IconOnly);
    vkData->virtualKeyButtonPtr->deleteKey->setIconSize(22, 17);
    vkData->virtualKeyButtonPtr->deleteKey->setIcon("/usr/res/TinyPiX/virtualKeyboard/删除.png");

    vkData->virtualKeyButtonPtr->switchChineseKey->setButtonStyle(TpButton::IconOnly);
    vkData->virtualKeyButtonPtr->switchChineseKey->setIconSize(26, 29);
    vkData->virtualKeyButtonPtr->switchChineseKey->setIcon(globalInputChineseIconPath);

    vkData->virtualKeyButtonPtr->symbolKey->setText(".?123");
    vkData->virtualKeyButtonPtr->sapceKey->setText("");
    vkData->virtualKeyButtonPtr->applyKey->setText("确定");
}

void TpVirtualKeyboard::configSignal()
{
    TpVirtualKeyboardData *vkData = static_cast<TpVirtualKeyboardData *>(data_);

    TpList<TpButton *> letterBtnList =
        {
            vkData->virtualKeyButtonPtr->keyQ,
            vkData->virtualKeyButtonPtr->keyW,
            vkData->virtualKeyButtonPtr->keyE,
            vkData->virtualKeyButtonPtr->keyR,
            vkData->virtualKeyButtonPtr->keyT,
            vkData->virtualKeyButtonPtr->keyY,
            vkData->virtualKeyButtonPtr->keyU,
            vkData->virtualKeyButtonPtr->keyI,
            vkData->virtualKeyButtonPtr->keyO,
            vkData->virtualKeyButtonPtr->keyP,
            vkData->virtualKeyButtonPtr->keyA,
            vkData->virtualKeyButtonPtr->keyS,
            vkData->virtualKeyButtonPtr->keyD,
            vkData->virtualKeyButtonPtr->keyF,
            vkData->virtualKeyButtonPtr->keyG,
            vkData->virtualKeyButtonPtr->keyH,
            vkData->virtualKeyButtonPtr->keyJ,
            vkData->virtualKeyButtonPtr->keyK,
            vkData->virtualKeyButtonPtr->keyL,
            vkData->virtualKeyButtonPtr->keyZ,
            vkData->virtualKeyButtonPtr->keyX,
            vkData->virtualKeyButtonPtr->keyC,
            vkData->virtualKeyButtonPtr->keyV,
            vkData->virtualKeyButtonPtr->keyB,
            vkData->virtualKeyButtonPtr->keyN,
            vkData->virtualKeyButtonPtr->keyM};

    for (const auto &letterBtn : letterBtnList)
    {
        connect(letterBtn, onClicked, [=](bool)
                { pressLetterButton(letterBtn); });
    }

    connect(vkData->virtualKeyButtonPtr->symbolKey, onClicked, [=](bool)
            { 
                // 如果已经是字符状态；切换为拼音输入
                if (vkData->isSymbol)
                {
                    refreshNormalCharacter(vkData->virtualKeyButtonPtr, false);
                    vkData->isSymbol = false;
                    
                }
                else
                {
                    // 切换为字符输入
                    refreshSymbolCharacter(vkData->virtualKeyButtonPtr, true, !vkData->virtualKeyButtonPtr->switchChineseKey->checked());
                    vkData->isSymbol = true;
                } });

    connect(vkData->virtualKeyButtonPtr->toggleCaseKey, onClicked, [=](bool)
            { 
                // 字符状态；点击大小写切换全字符或数字字符
                if (vkData->isSymbol)
                {
                    TpString keyQText = vkData->virtualKeyButtonPtr->keyQ->text();
                    if (keyQText.compare("1") == 0)
                        refreshSymbolCharacter(vkData->virtualKeyButtonPtr, false, !vkData->virtualKeyButtonPtr->switchChineseKey->checked());
                    else
                        refreshSymbolCharacter(vkData->virtualKeyButtonPtr, true, !vkData->virtualKeyButtonPtr->switchChineseKey->checked());

                }
                else
                {
                    refreshNormalCharacter(vkData->virtualKeyButtonPtr, vkData->virtualKeyButtonPtr->toggleCaseKey->checked());
                } });

    connect(vkData->virtualKeyButtonPtr->switchChineseKey, onClicked, [=](bool)
            { 
                switchChinese(!isChinese());               

                // 字符状态；点击大小写切换全字符或数字字符
                if (vkData->isSymbol)
                {
                    TpString keyQText = vkData->virtualKeyButtonPtr->keyQ->text();
                    if (keyQText.compare("1") == 0)
                        refreshSymbolCharacter(vkData->virtualKeyButtonPtr, true, vkData->isChinese);
                    else
                        refreshSymbolCharacter(vkData->virtualKeyButtonPtr, false, vkData->isChinese);

                } });

    connect(vkData->virtualKeyButtonPtr->deleteKey, onClicked, [=](bool)
            {
                TpString curKeysequence = vkData->inputMethod->keySequence();
                if (curKeysequence.empty())
                {
                    deleteSymbol.emit();
                    return;
                }

                curKeysequence = curKeysequence.mid(0, curKeysequence.length() - 1);
                vkData->inputMethod->clear();
                vkData->inputMethod->inputKeySequence(curKeysequence);
                
                vkData->candidateWindow->setPinyinStr(curKeysequence);
                if (curKeysequence.empty())
                    vkData->candidateWindow->setCandidateWordList(TpVector<TpString>());
                else
                    vkData->candidateWindow->setCandidateWordList(vkData->inputMethod->allCandidates()); });

    connect(vkData->virtualKeyButtonPtr->sapceKey, onClicked, [=](bool)
            {
                TpString firstCandidateWord = vkData->candidateWindow->firstCandidateWord();
                if (firstCandidateWord.empty())
                    return;
                // 清空候选词和拼音
                vkData->inputMethod->clear();
                vkData->candidateWindow->setPinyinStr("");
                vkData->candidateWindow->setCandidateWordList(TpVector<TpString>());
                finishChinese.emit(firstCandidateWord); });

    connect(vkData->virtualKeyButtonPtr->applyKey, onClicked, [=](bool)
            {
                TpString firstCandidateWord = vkData->candidateWindow->firstCandidateWord();
                if (firstCandidateWord.empty())
                    return;
                // 清空候选词和拼音
                vkData->inputMethod->clear();
                vkData->candidateWindow->setPinyinStr("");
                vkData->candidateWindow->setCandidateWordList(TpVector<TpString>());
                finishChinese.emit(firstCandidateWord); });
}

void TpVirtualKeyboard::pressLetterButton(TpButton *pressBtn)
{
    TpVirtualKeyboardData *vkData = static_cast<TpVirtualKeyboardData *>(data_);

    // 先判断是不是拼音字符，如果是拼音字符则给入输入法处理；不是则直接对外输出字符
    TpString btnText = pressBtn->text();
    if (btnText.empty())
        return;

    char btnCharacter = btnText.front();

    if (!((btnCharacter >= 'a' && btnCharacter <= 'z') || (btnCharacter >= 'A' && btnCharacter <= 'Z')))
    {
        inputCharacter.emit(btnCharacter);
        return;
    }

    // 英文字母处理
    // 判断是否开启大写键；开启则直接返回字母即可
    if (vkData->virtualKeyButtonPtr->toggleCaseKey->checked())
    {
        inputCharacter.emit(btnCharacter);
        return;
    }

    // 判断是否是中文输入模式，不是中文输入则直接返回字符
    if (!vkData->isChinese)
    {
        inputCharacter.emit(btnCharacter);
        return;
    }

    // 中文拼音处理
    vkData->inputMethod->inputKeySequence(btnCharacter);
    // std::cout << "vkData->cachePinyin  " << vkData->inputMethod->keySequence() << std::endl;

    inputPinyin.emit(vkData->inputMethod->keySequence());

    vkData->candidateWindow->setPinyinStr(vkData->inputMethod->keySequence());
    vkData->candidateWindow->setCandidateWordList(vkData->inputMethod->allCandidates());

    // TpVector<TpString> candidatesList = vkData->inputMethod->allCandidates();
    // for (int i = 0; i < candidatesList.size(); ++i)
    // {
    //     std::cout << i << ": " << candidatesList.at(i) << std::endl;
    // }
}

TpCandidateWidget::TpCandidateWidget(TpWidget *parent)
    : TpWidget(parent), cachePinyin_(""), textFont_(new TpFont()), curWordOffset_(0)
{
    textFont_->setFontSize(15);
    textFont_->setFontColor(_RGB(38, 38, 38), _RGB(38, 38, 38));

    // setBackGroundColor(_RGB(255, 0, 0));
}

TpCandidateWidget::~TpCandidateWidget()
{
    delete textFont_;
    textFont_ = nullptr;
}

void TpCandidateWidget::setPinyinStr(const TpString &str)
{
    cachePinyin_ = str;
    update();
}

void TpCandidateWidget::setCandidateWordList(const TpVector<TpString> &wordList)
{
    wordList_ = wordList;
    update();
}

TpString TpCandidateWidget::firstCandidateWord()
{
    if (wordList_.size() == 0)
        return "";
    return wordList_.front();
}

void TpCandidateWidget::setSlectCandidateFunc(SelectCandidateWordFunc func)
{
    selectCandidateDealFunc_ = func;
}

bool TpCandidateWidget::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    TpPainter *painter = event->painter();

    int32_t pinyinHeight = 0;
    if (!cachePinyin_.empty())
    {
        // 绘制拼音
        textFont_->setText(cachePinyin_);
        painter->drawText(*textFont_, 0, 0);
        pinyinHeight = textFont_->pixelHeight();
    }

    allWordRect_.clear();
    if (wordList_.size() > 0)
    {
        int32_t curX = bgLeftRightMargin_ + curWordOffset_;
        for (int i = 0; i < wordList_.size(); ++i)
        {
            textFont_->setText(wordList_.at(i));

            // 第一个词绘制背景
            if (i == 0)
            {
                int32_t bgBoxWidth = textFont_->pixelWidth() + bgLeftRightMargin_ * 2;
                int32_t bgBoxHeight = textFont_->pixelHeight() /*+ bgTopBottomMargin_ * 2*/;

                painter->setPen(_RGB(251, 251, 252));
                painter->setBrush(TpBrush(_RGB(251, 251, 252)));
                painter->drawRect(curWordOffset_, pinyinHeight + 5, bgBoxWidth, bgBoxHeight, 6);
            }
            painter->drawText(*textFont_, curX, pinyinHeight + 5);

            TpRect curCandidateWordRect(curX - bgLeftRightMargin_, pinyinHeight + 5,
                                        textFont_->pixelWidth() + bgLeftRightMargin_ * 2, textFont_->pixelHeight());

            allWordRect_.emplace_back(curCandidateWordRect);

            curX += 30 + textFont_->pixelWidth();
        }
    }

    return true;
}

bool TpCandidateWidget::onMousePressEvent(TpMouseEvent *event)
{
    mousePressPos_ = event->pos();

    if (isPressWordRect_)
        update();

    return true;
}

bool TpCandidateWidget::onMouseRleaseEvent(TpMouseEvent *event)
{
    // 获取点击区域rect和索引
    // 二分查找
    int32_t findIndex = 0;
    isPressWordRect_ = false;
    if (allWordRect_.size() > 0)
    {
        TpPoint curPos = event->pos();
        curPos.setX(curPos.x() + curWordOffset_);

        int findRight = allWordRect_.size() - 1;
        int findLeft = 0;

        for (int i = findRight / 2; i >= findLeft, i <= findRight;)
        {
            TpRect curRect = allWordRect_.at(i);
            if (curRect.contains(curPos))
            {
                findIndex = i;
                isPressWordRect_ = true;
                pressWordRect_ = curRect;

                std::cout << "Find True : " << wordList_.at(findIndex) << std::endl;
                break;
            }

            // 比当前矩形靠后，继续二分
            if (curPos.x() > (curRect.x() + curRect.width()))
            {
                findLeft = i;

                int32_t addIndex = (findRight - i) / 2;
                if (addIndex == 0)
                    addIndex = 1;

                i = i + addIndex;
            }
            else
            {
                // 向前二分
                findRight = i;
                i = i / 2;
            }

            // 查询结束
            if (i == findRight || i == findLeft)
            {
                curRect = allWordRect_.at(i);
                if (curRect.contains(curPos))
                {
                    findIndex = i;
                    isPressWordRect_ = true;
                    pressWordRect_ = curRect;

                    std::cout << "Find True : " << wordList_.at(findIndex) << std::endl;
                    break;
                }
                break;
            }
        }
    }

    if (curWordOffset_ > 0)
    {
        curWordOffset_ = 0;
        update();
    }

    TpPoint curPos = event->pos();
    if (isPressWordRect_ && std::abs(curPos.x() - mousePressPos_.x()) < 5 && std::abs(curPos.y() - mousePressPos_.y()) < 5)
    {
        // 计算点击坐标对应的候选词
        TpString candidateWord = wordList_.at(findIndex);

        // 通知回调选择了候选词
        if (selectCandidateDealFunc_)
        {
            selectCandidateDealFunc_(candidateWord);
        }

        // 清空候选词和拼音
        setPinyinStr("");
        setCandidateWordList(TpVector<TpString>());
    }

    return true;
}

bool TpCandidateWidget::onMouseMoveEvent(TpMouseEvent *event)
{
    TpWidget::onMouseMoveEvent(event);

    if (event->state())
    {
        TpPoint curPos = event->pos();
        curWordOffset_ = curPos.x() - mousePressPos_.x();
        isPressWordRect_ = false;
        update();
    }

    return true;
}