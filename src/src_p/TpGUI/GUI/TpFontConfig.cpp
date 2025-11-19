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
        resFontName += "Regular";
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

TpFontConfig::TpFontConfig()
{
    TpVector<TpString> defaultFontNameList;
    TpDir fontDir("/usr/data/TinyPiX/fonts/" + TpString(DEFAULT_FONT_FAMILY));
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
    fontMap[TpString(DEFAULT_FONT_FAMILY)] = defaultFontNameList;
}

TpFontConfig::~TpFontConfig()
{
}
