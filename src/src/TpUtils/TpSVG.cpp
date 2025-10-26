#include "TpSVG.h"
#include "TpVector.h"

struct TpSVGData
{
    TpString svgPath = "";
    TpString svgValueStr = "";
    TpSize curSize;
};

TpSVG::TpSVG() noexcept
{
    TpSVGData *svgData = new TpSVGData();
    data_ = svgData;
}

TpSVG::TpSVG(const TpString &filePath)
{
    TpSVGData *svgData = new TpSVGData();
    data_ = svgData;
    load(filePath);
}

TpSVG::~TpSVG()
{
    TpSVGData *svgData = static_cast<TpSVGData *>(data_);
    if (svgData)
    {
        delete svgData;
        svgData = nullptr;
        data_ = nullptr;
    }
}

int32_t TpSVG::width() const
{
    TpSVGData *svgData = static_cast<TpSVGData *>(data_);
    return svgData->curSize.width();
}

int32_t TpSVG::height() const
{
    TpSVGData *svgData = static_cast<TpSVGData *>(data_);
    return svgData->curSize.height();
}

TpSize TpSVG::size() const
{
    TpSVGData *svgData = static_cast<TpSVGData *>(data_);
    return svgData->curSize;
}

TpString TpSVG::extractAttribute(const TpString &svgContent, const TpString &attributeName) const
{
    // 查找属性名
    int32_t startPos = svgContent.indexOf(attributeName);
    if (startPos == -1)
        return "";

    // 查找等号
    int32_t equalPos = svgContent.indexOf("=", startPos);
    if (equalPos == -1)
        return "";

    // 查找引号开始位置
    int32_t quoteStart = equalPos + 1;
    while (quoteStart < svgContent.length() &&
           (svgContent[quoteStart] == ' ' || svgContent[quoteStart] == '\t'))
    {
        quoteStart++;
    }

    if (quoteStart >= svgContent.length())
        return "";

    char quoteChar = svgContent[quoteStart];
    if (quoteChar != '\'' && quoteChar != '"')
        return "";

    // 查找引号结束位置
    int32_t quoteEnd = svgContent.indexOf(quoteChar, quoteStart + 1);
    if (quoteEnd == -1)
        return "";

    // 提取属性值
    return svgContent.mid(quoteStart + 1, quoteEnd - quoteStart - 1);
}

void TpSVG::replaceAttributeValue(TpString &content, const TpString &attribute,
                                  const TpString &oldValue, const TpString &newValue)
{
    // 构建要查找的字符串
    TpString searchStr = attribute + "=\"" + oldValue + "\"";
    TpString replaceStr = attribute + "=\"" + newValue + "\"";

    // 执行替换
    content = content.replace(searchStr, replaceStr);

    // 尝试单引号版本
    searchStr = attribute + "='" + oldValue + "'";
    replaceStr = attribute + "='" + newValue + "'";
    content = content.replace(searchStr, replaceStr);
}

void TpSVG::replaceAttributeValue(TpString &content, const TpString &attribute, const TpString &newColor)
{
    // 查找属性名
    int32_t pos = 0;
    while ((pos = content.indexOf(attribute, pos)) != -1)
    {
        // 检查是否是完整的属性
        bool isAttribute = (pos == 0) ||
                           (content[pos - 1] == ' ' || content[pos - 1] == '\t' || content[pos - 1] == '\n');

        if (!isAttribute)
        {
            pos += attribute.length();
            continue;
        }

        // 查找等号
        int32_t equalPos = content.indexOf("=", pos);
        if (equalPos == -1)
        {
            pos += attribute.length();
            continue;
        }

        // 查找引号开始位置
        int32_t quoteStart = equalPos + 1;
        while (quoteStart < content.length() &&
               (content[quoteStart] == ' ' || content[quoteStart] == '\t'))
        {
            quoteStart++;
        }

        if (quoteStart >= content.length())
        {
            pos += attribute.length();
            continue;
        }

        char quoteChar = content[quoteStart];
        if (quoteChar != '\'' && quoteChar != '"')
        {
            pos += attribute.length();
            continue;
        }

        // 查找引号结束位置
        int32_t quoteEnd = content.indexOf(quoteChar, quoteStart + 1);
        if (quoteEnd == -1)
        {
            pos += attribute.length();
            continue;
        }

        // 替换属性值
        int32_t valueStart = quoteStart + 1;
        int32_t valueEnd = quoteEnd;

        // 替换颜色值
        content = content.replace(valueStart, valueEnd - valueStart, newColor);

        // 更新位置
        pos = valueStart + newColor.length();
    }
}

void TpSVG::replaceStyleColor(TpString &styleContent, const TpString &oldColor,
                              const TpString &newColor)
{
    // 查找所有颜色出现的位置
    int32_t pos = 0;
    while ((pos = styleContent.indexOf(oldColor, pos)) != -1)
    {
        // 检查是否是独立的颜色值（前后不是字母数字）
        bool isStartValid = (pos == 0) || !isalnum(styleContent[pos - 1]);
        bool isEndValid = (pos + oldColor.length() >= styleContent.length()) ||
                          !isalnum(styleContent[pos + oldColor.length()]);

        if (isStartValid && isEndValid)
        {
            // 替换颜色值
            styleContent = styleContent.replace(pos, oldColor.length(), newColor);
            pos += newColor.length();
        }
        else
        {
            pos += oldColor.length();
        }
    }
}

void TpSVG::replaceStyleColor(TpString &styleContent, const TpString &newColor)
{
    // 需要查找的颜色属性列表
    TpVector<TpString> colorProperties = {"fill", "stroke", "stop-color", "flood-color", "lighting-color"};

    // 遍历所有颜色属性
    for (const auto &prop : colorProperties)
    {
        int32_t pos = 0;
        while ((pos = styleContent.indexOf(prop, pos)) != -1)
        {
            // 检查是否是完整的属性名
            bool isProperty = (pos == 0) ||
                              (styleContent[pos - 1] == ' ' || styleContent[pos - 1] == '\t' || styleContent[pos - 1] == ';');

            if (!isProperty)
            {
                pos += prop.length();
                continue;
            }

            // 查找冒号
            int32_t colonPos = styleContent.indexOf(":", pos);
            if (colonPos == -1)
            {
                pos += prop.length();
                continue;
            }

            // 查找值开始位置
            int32_t valueStart = colonPos + 1;
            while (valueStart < styleContent.length() &&
                   (styleContent[valueStart] == ' ' || styleContent[valueStart] == '\t'))
            {
                valueStart++;
            }

            if (valueStart >= styleContent.length())
            {
                pos += prop.length();
                continue;
            }

            // 查找值结束位置（分号或行尾）
            int32_t valueEnd = styleContent.indexOf(";", valueStart);
            if (valueEnd == -1)
            {
                valueEnd = styleContent.length();
            }

            // 提取当前值
            TpString currentValue = styleContent.mid(valueStart, valueEnd - valueStart).trimmed();

            // 跳过特殊值
            if (currentValue.compare("none") == 0 || currentValue.compare("currentColor") == 0 || currentValue.compare("transparent") == 0)
            {
                pos = valueEnd;
                continue;
            }

            // 替换颜色值
            styleContent = styleContent.replace(valueStart, valueEnd - valueStart, newColor);

            // 更新位置
            pos = valueStart + newColor.length();
        }
    }
}

bool TpSVG::load(const TpString &filePath)
{
    TpSVGData *svgData = static_cast<TpSVGData *>(data_);
    if (!svgData)
        return false;

    // 读取文件内容
    TpFile svgFile(filePath);
    if (!svgFile.open(TpFile::ReadOnly))
        return false;

    svgData->svgValueStr = svgFile.readAll();
    svgFile.close();
    if (svgData->svgValueStr.empty())
        return false;

    svgData->svgPath = filePath;

    // 解析SVG尺寸
    TpSize size(0, 0);

    // 提取宽度和高度
    TpString widthStr = extractAttribute(svgData->svgValueStr, "width");
    TpString heightStr = extractAttribute(svgData->svgValueStr, "height");

    if (!widthStr.empty() && !heightStr.empty())
    {
        // 去除单位（如px）
        widthStr = widthStr.replace("px", "").trimmed();
        heightStr = heightStr.replace("px", "").trimmed();

        size.setWidth(widthStr.toInt());
        size.setHeight(heightStr.toInt());
    }
    svgData->curSize = size;

    return true;
}

void TpSVG::replaceColor(const TpString &oldColor, const TpString &newColor, int options)
{
    TpSVGData *svgData = static_cast<TpSVGData *>(data_);
    if (!svgData || svgData->svgValueStr.empty())
        return;

    // 确保颜色格式正确
    TpString normalizedOld = oldColor.trimmed();
    TpString normalizedNew = newColor.trimmed();

    // 需要替换的属性列表
    TpVector<TpString> attributes = {"fill", "stroke"};
    if (options == 1)
    {
        // 扩展替换属性
        attributes.append({"stop-color", "flood-color", "lighting-color"});
    }

    // 遍历所有需要替换的属性
    for (const auto &attr : attributes)
    {
        replaceAttributeValue(svgData->svgValueStr, attr, normalizedOld, normalizedNew);
    }

    // 处理style属性中的颜色值
    int32_t pos = 0;
    while ((pos = svgData->svgValueStr.indexOf("style=\"", pos)) != -1)
    {
        // "style=\"".length()
        int32_t styleStart = pos + 7;
        int32_t styleEnd = svgData->svgValueStr.indexOf("\"", styleStart);
        if (styleEnd == -1)
            break;

        // 提取style内容
        TpString styleContent = svgData->svgValueStr.mid(styleStart, styleEnd - styleStart);

        // 替换颜色
        replaceStyleColor(styleContent, normalizedOld, normalizedNew);

        // 更新SVG内容
        svgData->svgValueStr = svgData->svgValueStr.replace(styleStart, styleEnd - styleStart, styleContent);

        // 移动到下一个位置
        pos = styleStart + styleContent.length() + 1;
    }
}

void TpSVG::replaceColor(const TpString &newColor, int options)
{
    TpSVGData *svgData = static_cast<TpSVGData *>(data_);
    if (!svgData || svgData->svgValueStr.empty())
        return;

    // 确保颜色格式正确
    TpString normalizedNew = newColor.trimmed();

    // 需要替换的属性列表
    TpVector<TpString> attributes = {"fill", "stroke"};
    if (options == 1)
    {
        // 扩展替换属性
        attributes.append({"stop-color", "flood-color", "lighting-color"});
    }

    // 遍历所有需要替换的属性
    for (const auto &attr : attributes)
    {
        replaceAttributeValue(svgData->svgValueStr, attr, normalizedNew);
    }

    // 处理style属性中的颜色值
    int32_t pos = 0;
    while ((pos = svgData->svgValueStr.indexOf("style=\"", pos)) != -1)
    {
        int32_t styleStart = pos + 7; // "style=\"".length()
        int32_t styleEnd = svgData->svgValueStr.indexOf("\"", styleStart);
        if (styleEnd == -1)
            break;

        // 提取style内容
        TpString styleContent = svgData->svgValueStr.mid(styleStart, styleEnd - styleStart);

        // 替换所有颜色
        replaceStyleColor(styleContent, normalizedNew);

        // 更新SVG内容
        svgData->svgValueStr = svgData->svgValueStr.replace(styleStart, styleEnd - styleStart, styleContent);

        // 移动到下一个位置
        pos = styleStart + styleContent.length() + 1;
    }
}

TpString TpSVG::svgValue() const
{
    TpSVGData *svgData = static_cast<TpSVGData *>(data_);
    return svgData ? svgData->svgValueStr : "";
}

bool TpSVG::save(const TpString &filePath) const
{
    TpSVGData *svgData = static_cast<TpSVGData *>(data_);
    if (!svgData || svgData->svgValueStr.empty())
        return false;

    TpString savePath = filePath.empty() ? svgData->svgPath : filePath;
    if (savePath.empty())
        return false;

    TpFile saveFile(filePath);
    if (!saveFile.open(TpFile::WriteOnly))
        return false;

    saveFile.write(svgData->svgValueStr);
    saveFile.close();
    return true;
}