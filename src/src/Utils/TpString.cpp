#include "TpString.h"
#include <sstream>
#include <regex>
#include <iomanip>
#include <iostream>
#include <cerrno> // 用于错误检测
#include <cstdlib>
#include <climits>

int32_t safeStoi(const std::string &str, int32_t base)
{
    try
    {
        return std::stoi(str, nullptr, base);
    }
    catch (const std::invalid_argument &)
    {
        // 字符串格式错误（如非数字）
        return 0.0f;
    }
    catch (const std::out_of_range &)
    {
        // 转换结果超出 float 范围
        return 0.0f;
    }

    return 0.0;
}

TpString TpString::number(const int32_t &num)
{
    return std::to_string(num);
}

TpString TpString::number(const uint32_t &num)
{
    return std::to_string(num);
}

TpString TpString::number(const uint64_t &num)
{
    return std::to_string(num);
}

TpString TpString::number(const double &num, const uint32_t &precision)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << num;
    std::string result = out.str();
    return result;
}

TpList<TpString> TpString::split(const char &_ch) const
{
    TpList<TpString> resList;
    if (this->empty())
        return resList;

    std::string token;
    std::istringstream tokenStream(*this);

    while (std::getline(tokenStream, token, _ch))
    {
        resList.emplace_back(token);
    }

    return resList;
}

TpString TpString::simplified() const
{
    TpString result;
    bool inWhitespace = false;

    for (char ch : *this)
    {
        if (std::isspace(static_cast<uint8_t>(ch)))
        {
            if (!inWhitespace && !result.empty())
            {
                result += ' ';
                inWhitespace = true;
            }
        }
        else
        {
            result += ch;
            inWhitespace = false;
        }
    }

    // 移除末尾可能添加的空格
    if (!result.empty() && result.back() == ' ')
    {
        result.pop_back();
    }

    return result;
}

TpString TpString::replace(const TpString &from, const TpString &to) const
{
    if (from.length() == 0)
        return *this;

    if (from.compare(to) == 0)
        return *this;

    if (this->compare(to) == 0)
        return *this;

    TpString result = *this;
    size_type start_pos = result.find(from, 0);

    // 循环遍历字符串，查找所有出现的子字符串
    while (start_pos != std::string::npos)
    {
        // 将子字符串之前的部分添加到结果中
        result = result.std::string::replace(start_pos, from.length(), to);

        // 查找下一个出现的子字符串
        start_pos = result.find(from, 0);
    }

    return result;
}

TpString TpString::replace(const TpRegex &regex, const TpString &to) const
{
    TpString result = *this;
    TpString regexStr = regex.regexStr();

    try
    {
        std::regex reg(regexStr); // 尝试构造正则表达式对象

        result = std::regex_replace(result, std::regex(regexStr), to);
    }
    catch (const std::regex_error &e)
    {
        // 捕获并处理正则表达式错误
        std::cerr << "Regex error: " << e.what() << std::endl;
        // 可以根据需要处理错误，例如返回错误代码、退出程序等
        return "";
    }

    return result;
}

TpString TpString::replace(uint64_t pos, uint64_t len, const TpString &str)
{
    if (pos >= this->length())
        return "";

    if ((pos + len) > this->length())
        return "";

    // 调用基类 std::string 的 replace 方法
    std::string::replace(pos, len, str);
    return *this;
}

bool TpString::contains(const TpString &find) const
{
    size_type start_pos = std::string::find(find, 0);

    return start_pos != std::string::npos;
}

TpString TpString::mid(const uint32_t &_pos, const int32_t &_count) const
{
    size_t startByte = logicalPosToBytePos(_pos);
    if (startByte > size())
        return TpString();

    size_t remaining = (_count == -1) ? (logicalLength() - _pos) : static_cast<size_t>(_count);
    size_t endByte = startByte;
    for (size_t i = 0; i < remaining && endByte < size(); ++i)
    {
        endByte = nextCharBytePos(endByte);
    }
    return substr(startByte, endByte - startByte);
}

void TpString::remove(const uint32_t &_pos, const uint32_t &_count)
{
    // 获取字符串的逻辑长度（字符数）
    uint32_t len = logicalLength();

    // 边界检查：如果起始位置超出字符串长度，直接返回
    if (_pos >= len)
    {
        return;
    }

    // 计算实际要移除的字符数
    uint32_t count = std::min(_count, len - _pos); // 确保不越界

    // 转换为字节位置
    uint32_t startByte = logicalPosToBytePos(_pos);
    uint32_t endByte = logicalPosToBytePos(_pos + count);

    // 调用基类的 erase 方法移除字节区间
    std::string::erase(startByte, endByte - startByte);
}

int16_t TpString::toShort(int32_t base) const
{
    return safeStoi(*this, base);
}

int32_t TpString::toInt(int32_t base) const
{
    return safeStoi(*this, base);
}

double TpString::toDouble() const
{
    try
    {
        return std::stof(*this);
    }
    catch (const std::invalid_argument &)
    {
        // 字符串格式错误（如非数字）
        return 0.0f;
    }
    catch (const std::out_of_range &)
    {
        // 转换结果超出 float 范围
        return 0.0f;
    }

    return 0.0;
}

// 转换为无符号短整型
uint16_t TpString::toUShort(bool *ok, int base) const
{
    if (this->empty())
    {
        if (ok)
            *ok = false;
        return 0;
    }

    // 使用 strtoul 进行转换，因为它可以检测溢出
    char *endPtr = nullptr;
    errno = 0; // 清除错误状态
    unsigned long value = std::strtoul(this->c_str(), &endPtr, base);

    // 检查转换是否成功
    bool success = (endPtr != this->c_str()) && // 至少有一个字符被转换
                   (*endPtr == '\0') &&         // 整个字符串都被转换
                   (errno == 0) &&              // 没有发生错误
                   (value <= USHRT_MAX);        // 值在 uint16_t 范围内

    if (ok)
        *ok = success;

    return success ? static_cast<uint16_t>(value) : 0;
}

// 转换为无符号整型
uint32_t TpString::toUInt(bool *ok, int base) const
{
    if (this->empty())
    {
        if (ok)
            *ok = false;
        return 0;
    }

    // 使用 strtoul 进行转换
    char *endPtr = nullptr;
    errno = 0; // 清除错误状态
    unsigned long value = std::strtoul(this->c_str(), &endPtr, base);

    // 检查转换是否成功
    bool success = (endPtr != this->c_str()) && // 至少有一个字符被转换
                   (*endPtr == '\0') &&         // 整个字符串都被转换
                   (errno == 0) &&              // 没有发生错误
                   (value <= UINT_MAX);         // 值在 uint32_t 范围内

    if (ok)
        *ok = success;

    return success ? static_cast<uint32_t>(value) : 0;
}

// 转换为大写
TpString TpString::toUpper() const
{
    TpString result = *this;
    for (size_t i = 0; i < result.size(); i++)
    {
        char c = result[i];
        if (c >= 'a' && c <= 'z')
        {
            result[i] = c - 32;
        }
    }
    return result;
}

// 转换为小写
TpString TpString::toLower() const
{
    TpString result = *this;
    for (size_t i = 0; i < result.size(); i++)
    {
        char c = result[i];
        if (c >= 'A' && c <= 'Z')
        {
            result[i] = c + 32;
        }
    }
    return result;
}

// 在字符串左侧填充字符 (安全使用 append)
TpString TpString::leftJustified(uint32_t width, char fill) const
{
    if (this->length() >= width)
    {
        return *this;
    }

    TpString result;
    uint32_t padding = width - this->length();

    // 安全添加填充字符
    for (uint32_t i = 0; i < padding; i++)
    {
        result += fill; // 使用 += 追加单个字符
    }

    result.append(*this); // 使用您的 append 实现
    return result;
}

// 在字符串右侧填充字符 (安全使用 append)
TpString TpString::rightJustified(uint32_t width, char fill) const
{
    if (this->length() >= width)
    {
        return *this;
    }

    TpString result = *this;
    uint32_t padding = width - this->length();

    // 安全添加填充字符
    for (uint32_t i = 0; i < padding; i++)
    {
        result += fill; // 使用 += 追加单个字符
    }

    return result;
}

// 重复字符串n次 (安全使用 append)
TpString TpString::repeated(uint32_t times) const
{
    if (times == 0)
        return "";
    if (times == 1)
        return *this;

    TpString result;
    for (uint32_t i = 0; i < times; i++)
    {
        result.append(*this); // 使用您的 append 实现
    }
    return result;
}

// 获取字符的十六进制表示
TpString TpString::toHex(char separator) const
{
    if (this->empty())
        return "";

    static const char hexDigits[] = "0123456789ABCDEF";
    TpString result;

    for (size_t i = 0; i < this->length(); i++)
    {
        uint8_t byte = static_cast<uint8_t>((*this)[i]);

        if (i > 0 && separator != '\0')
        {
            result += separator;
        }

        result += hexDigits[byte >> 4];   // 高4位
        result += hexDigits[byte & 0x0F]; // 低4位
    }

    return result;
}

// 检查字符串是否包含任何指定字符集中的字符
bool TpString::containsAnyOf(const TpString &charSet) const
{
    if (this->empty() || charSet.empty())
        return false;

    for (char c : *this)
    {
        if (charSet.find(c) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

// 检查字符串是否仅包含指定字符集中的字符
bool TpString::containsOnly(const TpString &charSet) const
{
    if (this->empty())
        return true; // 空字符串视为包含在任意字符集中
    if (charSet.empty())
        return false; // 非空字符串不能包含在空字符集中

    for (char c : *this)
    {
        if (charSet.find(c) == std::string::npos)
        {
            return false;
        }
    }
    return true;
}
// 转换为布尔值
bool TpString::toBool() const
{
    TpString lower = this->toLower();

    // 使用显式转换避免歧义
    if (lower == TpString("true") ||
        lower == TpString("1") ||
        lower == TpString("yes") ||
        lower == TpString("on"))
    {
        return true;
    }

    // 使用显式转换避免歧义
    if (lower == TpString("false") ||
        lower == TpString("0") ||
        lower == TpString("no") ||
        lower == TpString("off"))
    {
        return false;
    }

    try
    {
        int num = std::stoi(lower);
        return num != 0;
    }
    catch (...)
    {
        // 转换失败，默认为 false
        return false;
    }
}

// 删除所有指定字符
TpString TpString::removeChar(char ch) const
{
    if (this->find(ch) == std::string::npos)
    {
        return *this;
    }

    TpString result;
    for (char c : *this)
    {
        if (c != ch)
        {
            result += c; // 追加单个字符
        }
    }

    return result;
}

// 字符串反转 (修复构造函数问题)
TpString TpString::reversed() const
{
    if (this->empty())
        return "";

    // 创建字符向量
    std::vector<char> chars(this->begin(), this->end());

    // 反转向量
    std::reverse(chars.begin(), chars.end());

    // 从向量创建字符串
    return TpString(chars.data());
}

void TpString::insert(const uint32_t &pos, const TpString &str)
{
    // 获取字节位置
    size_t bytePos = logicalPosToBytePos(pos > logicalLength() ? logicalLength() : pos);
    // 调用基类的insert方法插入内容
    std::string::insert(bytePos, str);
}

void TpString::append(const TpString &str)
{
    // 调用基类的append方法追加内容
    std::string::append(str);
}

TpString TpString::left(const uint32_t &_length)
{
    if (_length > length())
        return mid(0, length());
    return mid(0, _length);
}

TpString TpString::right(const uint32_t &_length)
{
    if (_length > length())
        return mid(0, length());
    return mid(length() - _length);
}

bool TpString::startsWith(const TpString &prefix) const
{
    // 如果前缀长度大于当前字符串长度，直接返回false
    if (prefix.size() > this->size())
        return false;

    // 比较字符串的前缀部分
    return std::equal(prefix.begin(), prefix.end(), this->begin());
}

bool TpString::endsWith(const TpString &suffix) const
{
    const size_t suffix_len = strlen(suffix.c_str());
    const size_t this_len = this->size();

    // 如果后缀比原字符串长，直接返回 false
    if (suffix_len > this_len)
        return false;

    // 从原字符串末尾开始比较
    return (this->compare(this_len - suffix_len, suffix_len, suffix.c_str()) == 0);
}

uint32_t TpString::logicalLength() const
{
    uint32_t len = 0;
    uint32_t bytePos = 0;
    while (bytePos < size())
    {
        bytePos += getCharByteLength(bytePos);
        len++;
    }
    return len;
}

uint32_t TpString::logicalPosToBytePos(uint32_t logicalPos) const
{
    size_t bytePos = 0;
    size_t currentLogical = 0;
    while (bytePos < size() && currentLogical < logicalPos)
    {
        bytePos += getCharByteLength(bytePos);
        currentLogical++;
    }
    return bytePos;
}

uint32_t TpString::nextCharBytePos(uint32_t currentBytePos) const
{
    if (currentBytePos >= size())
        return size();
    return currentBytePos + getCharByteLength(currentBytePos);
}

uint32_t TpString::prevCharBytePos(uint32_t currentBytePos) const
{
    if (currentBytePos == 0)
        return 0;
    size_t pos = currentBytePos - 1;
    while (pos > 0 && ((*this)[pos] & 0xC0) == 0x80)
    {
        --pos;
    }
    return pos;
}

uint32_t TpString::getCharByteLength(uint32_t bytePos) const
{
    if (bytePos >= size())
        return 0;
    unsigned char ch = (*this)[bytePos];
    if ((ch & 0x80) == 0)
        return 1; // 单字节字符
    else if ((ch & 0xE0) == 0xC0)
        return 2; // 双字节字符
    else if ((ch & 0xF0) == 0xE0)
        return 3; // 三字节字符
    else if ((ch & 0xF8) == 0xF0)
        return 4; // 四字节字符
    return 1;     // 默认处理为单字节
}

uint32_t TpString::bytePosToLogicalPos(uint32_t bytePos) const
{
    if (bytePos >= size())
    {
        return logicalLength();
    }
    uint32_t logicalPos = 0;
    uint32_t currentByte = 0;
    while (currentByte < bytePos)
    {
        currentByte += getCharByteLength(currentByte);
        logicalPos++;
    }
    return logicalPos;
}
