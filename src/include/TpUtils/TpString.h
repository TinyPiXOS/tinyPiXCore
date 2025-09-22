#ifndef TP_STRING_H
#define TP_STRING_H

#include <string>
#include "TpRegex.h"
#include "TpList.h"

/**
 * @class TpString
 * @brief 继承自 std::string 的字符串封装类，提供了丰富的扩展操作方法
 *
 * 该类扩展了标准字符串的功能，包括查找、分割、转换、替换等实用方法，
 * 并支持基于逻辑位置（考虑UTF-8字符）的操作。
 */
class TpString : public std::string
{
public:
    /**
     * @brief 默认构造函数
     */
    TpString() {}

    /**
     * @brief 通过单个字符构造 TpString
     * @param ch 用于构造的字符
     */
    TpString(const char &ch)
    {
        std::string::append(1, ch);
    }

    /**
     * @brief 通过C风格字符串构造 TpString
     * @param value C风格字符串指针
     */
    TpString(const char *value)
        : std::string(value)
    {
    }

    /**
     * @brief 通过 std::string 对象构造 TpString
     * @param value std::string 对象
     */
    TpString(const std::string &value)
        : std::string(value)
    {
    }

    /**
     * @brief 赋值运算符重载（std::string）
     * @param _others 源 std::string 对象
     * @return 赋值后的 TpString 对象
     */
    TpString operator=(const std::string &_others)
    {
        *this = TpString(_others);
        return *this;
    }

    /**
     * @brief 赋值运算符重载（C风格字符串）
     * @param _others 源C风格字符串指针
     * @return 赋值后的 TpString 对象
     */
    TpString operator=(const char *_others)
    {
        *this = TpString(_others);
        return *this;
    }

    /**
     * @brief 相等运算符重载
     * @param _others 要比较的 TpString 对象
     * @return 如果字符串相等返回 true，否则返回 false
     */
    bool operator==(const TpString &_others) const
    {
        return (this->compare(_others) == 0) ? true : false;
    }

    /// @brief 查找子字符串的位置
    /// @param str 要查找的子字符串
    /// @param from 开始查找的位置（默认为0）
    /// @return 子字符串的位置索引，如果未找到则返回-1
    int32_t indexOf(const TpString &str, int32_t from = 0) const;

    /// @brief 查找字符的位置
    /// @param ch 要查找的字符
    /// @param from 开始查找的位置（默认为0）
    /// @return 字符的位置索引，如果未找到则返回-1
    int32_t indexOf(char ch, int32_t from = 0) const;

    /// @brief 从字符串末尾开始查找子字符串的位置
    /// @param str 要查找的子字符串
    /// @param from 开始查找的位置（默认为0）
    /// @return 子字符串的位置索引，如果未找到则返回-1
    int32_t lastIndexOf(const TpString &str, int32_t from = 0) const;

    /// @brief 从字符串末尾开始查找字符的位置
    /// @param ch 要查找的字符
    /// @param from 开始查找的位置（默认为0）
    /// @return 字符的位置索引，如果未找到则返回-1
    int32_t lastIndexOf(char ch, int32_t from = 0) const;

    /// @brief 将数字转换为字符串
    /// @param num 数字
    /// @return 返回转换后的字符串
    static TpString number(int32_t num, int32_t width = 0, char fillChar = '0');
    static TpString number(uint32_t num, int32_t width = 0, char fillChar = '0');
    static TpString number(uint64_t num, int32_t width = 0, char fillChar = '0');
    static TpString number(double num, int32_t precision = 2, int32_t width = 0, char fillChar = '0');

    /**
     * @brief 指定字符分割字符串
     * @param _ch 分隔符
     * @return 返回分割后的字符串列表
     */
    TpList<TpString> split(const char &_ch) const;

    /**
     * @brief 去除首尾空格，中间空格只保留一个
     * @return 返回处理后的字符串
     */
    TpString simplified() const;

    /**
     * @brief 去除字符串两端的空白字符
     * @return 返回处理后的字符串
     */
    TpString trimmed() const;

    /**
     * @brief 指定字符串替换
     * @param from 要被替换的字符串
     * @param to 替换后的字符串
     * @return 返回替换后的结果
     */
    TpString replace(const TpString &from, const TpString &to) const;

    /**
     * @brief 使用正则表达式进行字符串替换
     * @param regex 正则对象
     * @param to 匹配的字符串替换对象
     * @return 返回替换后的结果
     */
    TpString replace(const TpRegex &regex, const TpString &to) const;

    /**
     * @brief 替换字符串中指定位置的子串
     * @param pos 起始位置
     * @param len 要替换的长度
     * @param str 替换字符串
     * @return 替换后的字符串
     */
    TpString replace(uint64_t pos, uint64_t len, const TpString &str);

    /**
     * @brief 判断是否包含某个字符串
     * @param find 查询字符串
     * @return 当前字符串包含 find 返回 true，否则返回 false
     */
    bool contains(const TpString &find) const;

    /**
     * @brief 截取子字符串
     * @param _pos 起始索引（0开始）
     * @param _count 截取长度，-1 表示到字符串末尾
     * @return 截取后的字符串
     */
    TpString mid(const uint32_t &_pos, const int32_t &_count = -1) const;

    /**
     * @brief 移除指定位置的字符
     * @param _pos 起始索引
     * @param _count 要移除的字符数量
     */
    void remove(const uint32_t &_pos, const uint32_t &_count = 1);

    /**
     * @brief 转换为16位有符号整数
     * @param base 进制（2/8/10/16，默认为10）
     * @return 转换结果
     */
    int16_t toShort(int32_t base = 10) const;

    /**
     * @brief 转换为32位有符号整数
     * @param base 进制（2/8/10/16，默认为10）
     * @return 转换结果
     */
    int32_t toInt(int32_t base = 10) const;

    /**
     * @brief 转换为双精度浮点数
     * @return 转换结果
     */
    double toDouble() const;

    /**
     * @brief 转换为布尔值
     * @return 转换结果（"true"、"1"等返回true，其余返回false）
     */
    bool toBool() const;

    /**
     * @brief 转换为16位无符号整数
     * @param ok 可选，指示转换是否成功
     * @param base 进制（2/8/10/16，默认为10）
     * @return 转换结果
     */
    uint16_t toUShort(bool *ok = nullptr, int base = 10) const;

    /**
     * @brief 转换为32位无符号整数
     * @param ok 可选，指示转换是否成功
     * @param base 进制（2/8/10/16，默认为10）
     * @return 转换结果
     */
    uint32_t toUInt(bool *ok = nullptr, int base = 10) const;

    /**
     * @brief 转换为大写字符串
     * @return 大写字符串
     */
    TpString toUpper() const;

    /**
     * @brief 转换为小写字符串
     * @return 小写字符串
     */
    TpString toLower() const;

    /**
     * @brief 在字符串左侧填充字符至指定宽度
     * @param width 总宽度
     * @param fill 填充字符（默认为空格）
     * @return 处理后的字符串
     */
    TpString leftJustified(uint32_t width, char fill = ' ') const;

    /**
     * @brief 在字符串右侧填充字符至指定宽度
     * @param width 总宽度
     * @param fill 填充字符（默认为空格）
     * @return 处理后的字符串
     */
    TpString rightJustified(uint32_t width, char fill = ' ') const;

    /**
     * @brief 重复字符串指定次数
     * @param times 重复次数
     * @return 重复后的字符串
     */
    TpString repeated(uint32_t times) const;

    /**
     * @brief 获取字符的十六进制表示
     * @param separator 分隔符（默认为无）
     * @return 十六进制表示字符串
     */
    TpString toHex(char separator = '\0') const;

    /**
     * @brief 检查字符串是否包含指定字符集中的任何字符
     * @param charSet 字符集
     * @return 如果包含任何字符集中字符返回 true，否则返回 false
     */
    bool containsAnyOf(const TpString &charSet) const;

    /**
     * @brief 检查字符串是否仅包含指定字符集中的字符
     * @param charSet 字符集
     * @return 如果仅包含字符集中字符返回 true，否则返回 false
     */
    bool containsOnly(const TpString &charSet) const;

    /**
     * @brief 删除所有指定字符
     * @param ch 要删除的字符
     * @return 处理后的字符串
     */
    TpString removeChar(char ch) const;

    /**
     * @brief 反转字符串
     * @return 反转后的字符串
     */
    TpString reversed() const;

    /**
     * @brief 在指定位置插入字符串
     * @param pos 插入位置索引
     * @param str 要插入的字符串
     */
    void insert(const uint32_t &pos, const TpString &str);

    /**
     * @brief 在字符串末尾追加字符串
     * @param str 要追加的字符串
     */
    void append(const TpString &str);

    /**
     * @brief 从左侧截取指定长度字符串
     * @param _length 截取长度
     * @return 截取后的字符串
     */
    TpString left(const uint32_t &_length);

    /**
     * @brief 从右侧截取指定长度字符串
     * @param _length 截取长度
     * @return 截取后的字符串
     */
    TpString right(const uint32_t &_length);

    /**
     * @brief 判断字符串是否以指定前缀开头
     * @param prefix 前缀字符串
     * @return 如果以指定前缀开头返回 true，否则返回 false
     */
    bool startsWith(const TpString &prefix) const;

    /**
     * @brief 判断字符串是否以指定后缀结尾
     * @param suffix 后缀字符串
     * @return 如果以指定后缀结尾返回 true，否则返回 false
     */
    bool endsWith(const TpString &suffix) const;

    /**
     * @brief 获取字符串的逻辑长度（考虑UTF-8编码，中英文字符各算一个）
     * @return 逻辑长度
     */
    uint32_t logicalLength() const;

    /**
     * @brief 将逻辑位置转换为字节位置
     * @param logicalPos 逻辑位置
     * @return 对应的字节位置
     */
    uint32_t logicalPosToBytePos(uint32_t logicalPos) const;

    /**
     * @brief 获取下一个字符的字节位置
     * @param currentBytePos 当前字节位置
     * @return 下一个字符的字节位置
     */
    uint32_t nextCharBytePos(uint32_t currentBytePos) const;

    /**
     * @brief 获取前一个字符的字节位置
     * @param currentBytePos 当前字节位置
     * @return 前一个字符的字节位置
     */
    uint32_t prevCharBytePos(uint32_t currentBytePos) const;

private:
    /**
     * @brief 计算字符的字节长度（基于UTF-8编码）
     * @param bytePos 字节位置
     * @return 字符的字节长度
     */
    uint32_t getCharByteLength(uint32_t bytePos) const;

    /**
     * @brief 将字节位置转换为逻辑位置
     * @param bytePos 字节位置
     * @return 对应的逻辑位置
     */
    uint32_t bytePosToLogicalPos(uint32_t bytePos) const;
};

// 特化:hash模板为tpString类型
namespace std
{
    template <>
    struct hash<TpString>
    {
        /**
         * @brief 计算 TpString 的哈希值
         * @param str 要计算哈希值的字符串
         * @return 哈希值
         */
        std::size_t operator()(const TpString &str) const
        {
            // 使用std::string的哈希函数
            return hash<std::string>()(str);
        }
    };
}

#endif