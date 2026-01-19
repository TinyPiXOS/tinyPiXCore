#include "TpFontConfig.h"
#include <TpDir.h>
#include "thorVG/thorvg.h"

tpShared<TpFontConfig> TpFontConfig::Inst()
{
    static tpShared<TpFontConfig> globalFontConfig = nullptr;
    if (globalFontConfig == nullptr)
        globalFontConfig = std::shared_ptr<TpFontConfig>(new TpFontConfig());
    return globalFontConfig;
}

TpString TpFontConfig::fontName(const TpString &fontName, FontWeight fontWeight)
{
    TpString resFontName = fontName + "-";

    if (!containsFont(fontName))
        resFontName = TpString(DEFAULT_FONT_FAMILY) + "-";

    switch (fontWeight)
    {
    case FONT_WEIGHT_NORMAL:
        resFontName += "Normal";
        break;
    case FONT_WEIGHT_BOLD:
        resFontName += "Bold";
        break;
    case FONT_WEIGHT_LIGHT:
        resFontName += "Light";
        break;
    case FONT_WEIGHT_EXTRALIGHT:
        resFontName += "ExtraLight";
        break;
    case FONT_WEIGHT_HEAVY:
        resFontName += "Heavy";
        break;
    case FONT_WEIGHT_MEDIUM:
        resFontName += "Medium";
        break;
    case FONT_WEIGHT_SEMIBOLD:
        resFontName += "SemiBold";
        break;
    default:
        resFontName += "Regular";
        break;
    }
    
    TpVector<TpString> fontNameList = fontMap.value(fontName);
    if (!fontNameList.contains(resFontName))
    {
        resFontName = fontNameList.front();
    }

    return resFontName;
}

bool TpFontConfig::containsFont(const TpString &fontName)
{
    if (fontName.empty())
        return false;
    return fontMap.contains(fontName);
}

TpList<TpString> TpFontConfig::families()
{
    return fontMap.keys();
}

bool TpFontConfig::loadFont(const TpString &fontPath)
{
    TpFileInfo ttfFile(fontPath);
    if (!ttfFile.exists())
        return false;

    if (ttfFile.suffix().compare("ttf") != 0)
        return false;

    // 加载字体
    tvg::Text::load(fontPath.c_str());
    fontMap[ttfFile.baseName()] = TpVector<TpString>{ttfFile.baseName()};

    return false;
}

TpFontConfig::TpFontConfig()
{
    TpVector<TpString> defaultFontNameList;
    TpString defaultFontDirPath = "/usr/data/TinyPiX/fonts/" + TpString(DEFAULT_FONT_FAMILY);

#if 0 // 全量加载字体库
    TpDir fontDir(defaultFontDirPath);
    for (const auto &fileInfo : fontDir.entryInfoList(TpDir::Files))
    {
        if (!fileInfo.isFile())
            continue;

        if (fileInfo.suffix().compare("ttf") != 0)
            continue;

        TpString filePath = fileInfo.filePath();
        // std::cout << "filePath " << filePath << std::endl;
        tvg::Text::load(filePath.c_str());

        defaultFontNameList.emplace_back(fileInfo.baseName());
    }
#else
    // 仅加载默认字体
    tvg::Text::load(TpString(defaultFontDirPath + "/" + TpString(DEFAULT_FONT_FAMILY) + "-Normal.ttf").c_str());
    //tvg::Text::load("/usr/share/fonts/wqy-zenhei.ttf");
    defaultFontNameList.emplace_back(TpString(DEFAULT_FONT_FAMILY) + "-Normal");
    //defaultFontNameList.emplace_back(TpString("wqy-zenhei"));
#endif

    fontMap[TpString(DEFAULT_FONT_FAMILY)] = defaultFontNameList;
}

TpFontConfig::~TpFontConfig()
{
}
