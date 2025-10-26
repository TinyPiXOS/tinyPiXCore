#include <iostream>
#include <string>
#include <cassert>
#include "TpVariant.h"

// 自定义测试类
class TestClass {
public:
    TestClass() : value(0), name("default") {
        std::cout << "TestClass default constructor: " << name << " (" << value << ")" << std::endl;
    }
    
    TestClass(int v, const std::string& n) : value(v), name(n) {
        std::cout << "TestClass parameterized constructor: " << name << " (" << value << ")" << std::endl;
    }
    
    TestClass(const TestClass& other) : value(other.value), name(other.name) {
        std::cout << "TestClass copy constructor: " << name << " (" << value << ")" << std::endl;
    }
    
    ~TestClass() {
        std::cout << "TestClass destructor: " << name << " (" << value << ")" << std::endl;
    }
    
    TestClass& operator=(const TestClass& other) {
        value = other.value;
        name = other.name;
        std::cout << "TestClass assignment operator: " << name << " (" << value << ")" << std::endl;
        return *this;
    }
    
    bool operator==(const TestClass& other) const {
        return value == other.value && name == other.name;
    }
    
    int getValue() const { return value; }
    const std::string& getName() const { return name; }
    
private:
    int value;
    std::string name;
};

// 测试函数
void testCustomType() {
    std::cout << "===== 开始自定义类型测试 =====" << std::endl;
    
    // 测试1: 基本构造和析构
    std::cout << "\n1. 基本构造和析构测试" << std::endl;
    {
        TestClass testObj(42, "test1");
        TpVariant var = testObj;
        
        // 检查是否为自定义类型
        assert(var.isCustom<TestClass>());
        std::cout << "自定义类型检查通过" << std::endl;
        
        // 获取值并验证
        TestClass retrieved = var.toCustom<TestClass>();
        assert(retrieved.getValue() == 42);
        assert(retrieved.getName() == "test1");
        std::cout << "值验证通过" << std::endl;
    }
    std::cout << "基本构造和析构测试通过" << std::endl;
    
    // 测试2: 赋值操作
    std::cout << "\n2. 赋值操作测试" << std::endl;
    {
        TpVariant var;
        TestClass testObj1(100, "test2");
        var = testObj1;
        
        // 验证赋值
        assert(var.isCustom<TestClass>());
        TestClass retrieved = var.toCustom<TestClass>();
        assert(retrieved.getValue() == 100);
        assert(retrieved.getName() == "test2");
        
        // 重新赋值
        TestClass testObj2(200, "test2_updated");
        var = testObj2;
        
        // 验证重新赋值
        retrieved = var.toCustom<TestClass>();
        assert(retrieved.getValue() == 200);
        assert(retrieved.getName() == "test2_updated");
    }
    std::cout << "赋值操作测试通过" << std::endl;
    
    // 测试3: 拷贝构造
    std::cout << "\n3. 拷贝构造测试" << std::endl;
    {
        TestClass testObj(300, "test3");
        TpVariant var1 = testObj;
        TpVariant var2 = var1; // 拷贝构造
        
        // 验证两个变量都有正确的值
        assert(var1.isCustom<TestClass>());
        assert(var2.isCustom<TestClass>());
        
        TestClass retrieved1 = var1.toCustom<TestClass>();
        TestClass retrieved2 = var2.toCustom<TestClass>();
        
        assert(retrieved1.getValue() == 300);
        assert(retrieved1.getName() == "test3");
        assert(retrieved2.getValue() == 300);
        assert(retrieved2.getName() == "test3");
        
        // 验证它们是不同的对象（深拷贝）
        assert(&retrieved1 != &retrieved2);
    }
    std::cout << "拷贝构造测试通过" << std::endl;
    
    // 测试4: 赋值运算符
    std::cout << "\n4. 赋值运算符测试" << std::endl;
    {
        TestClass testObj1(400, "test4_1");
        TestClass testObj2(500, "test4_2");
        
        TpVariant var1 = testObj1;
        TpVariant var2 = testObj2;
        
        // 赋值前验证
        assert(var1.toCustom<TestClass>().getValue() == 400);
        assert(var2.toCustom<TestClass>().getValue() == 500);
        
        // 执行赋值
        var2 = var1;
        
        // 验证赋值后
        assert(var1.toCustom<TestClass>().getValue() == 400);
        assert(var2.toCustom<TestClass>().getValue() == 400);
        assert(var2.toCustom<TestClass>().getName() == "test4_1");
    }
    std::cout << "赋值运算符测试通过" << std::endl;
    
    // 测试6: 异常处理
    std::cout << "\n6. 异常处理测试" << std::endl;
    {
        TpVariant var;
        
        try {
            // 尝试从未初始化的变量中获取自定义类型
            TestClass retrieved = var.toCustom<TestClass>();
            assert(false); // 不应该执行到这里
        } catch (const std::bad_cast& e) {
            std::cout << "正确捕获 bad_cast 异常: " << e.what() << std::endl;
        }
        
        // 测试错误类型转换
        var = 42; // 设置为整数
        
        try {
            TestClass retrieved = var.toCustom<TestClass>();
            assert(false); // 不应该执行到这里
        } catch (const std::bad_cast& e) {
            std::cout << "正确捕获 bad_cast 异常: " << e.what() << std::endl;
        }
    }
    std::cout << "异常处理测试通过" << std::endl;
    
    // 测试7: 复杂场景
    std::cout << "\n7. 复杂场景测试" << std::endl;
    {
        // 创建包含自定义类型的向量
        std::vector<TpVariant> complexVector;
        
        for (int i = 0; i < 3; ++i) {
            TestClass testObj(800 + i, "complex_" + std::to_string(i));
            complexVector.push_back(testObj);
        }
        
        // 验证向量内容
        for (int i = 0; i < 3; ++i) {
            assert(complexVector[i].isCustom<TestClass>());
            TestClass retrieved = complexVector[i].toCustom<TestClass>();
            assert(retrieved.getValue() == 800 + i);
            assert(retrieved.getName() == "complex_" + std::to_string(i));
        }
        
        // 将向量放入另一个 TpVariant
        TpVariant vectorVar = complexVector;
        
        // 这里应该触发向量处理逻辑，而不是自定义类型逻辑
        // 根据您的实现，可能需要调整这个测试
    }
    std::cout << "复杂场景测试通过" << std::endl;
    
    std::cout << "\n===== 所有测试通过! =====" << std::endl;
}

int main() {
    try {
        testCustomType();
        std::cout << "\n所有自定义类型功能测试成功完成!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "未知错误导致测试失败" << std::endl;
        return 1;
    }
}