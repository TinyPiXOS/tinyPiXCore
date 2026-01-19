#include "TpString.h"
#include <iostream>
#include "TpObject.h"
#include "TpVariant.h"
#include "TpProperty.h"

class TestClass : public TpObject
{
public:
    TestClass()
    {
        // TpObject::registerProperty("count", [=](const TpVariant &) {}, [=](const TpVariant &input)
        //                            { setCount(parsePropertyValue<int32_t>(input)); });
        TP_PROPERTY(int, count, count, setCount);
        TP_PROPERTY(TpString, name, getName, setName);
    };

    int m_count;
    TpString m_name;

    void setCount(int value)
    {
        std::cout << "setCount: " << value << std::endl;
        m_count = value;
    }
    int count() const { return m_count; }

    void setName(const TpString &value)
    {
        std::cout << "setName: " << value << std::endl;
        m_name = value;
    }
    TpString getName() const { return m_name; }
};

int main(int argc, char *argv[])
{
    TestClass obj;

    // 测试属性设置
    obj.setProperty("count", TpVariant(66));
    obj.setProperty("name", TpVariant(std::string("Test Object")));

    // 测试属性获取
    // std::cout << "Count: " << obj.property("count").toInt32() << std::endl;
    // std::cout << "Name: " << obj.property("name").toString() << std::endl;

    return 0;
}
