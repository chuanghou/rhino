//
// Created by houch on 2025/6/22.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/filesystem.hpp>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "capnp/myProto.capnp.h"

#include <optional>
#include <nlohmann/json.hpp>

#include "util/JsonUtil.h"

using namespace rhino;
using namespace std;

TEST(BoostTest, FileSystemTest) {
    namespace fs = boost::filesystem;
    // 1. 测试当前工作目录
    fs::path current_path = fs::current_path();
    std::cout << "当前工作目录: " << current_path << std::endl;
}

// 定义一个接口
class MyInterface {
public:
    virtual ~MyInterface() = default;

    virtual int Foo(int x) = 0;
};

// 使用gmock生成Mock类
class MockMyInterface : public MyInterface {
public:
    MOCK_METHOD(int, Foo, (int x), (override));
};

TEST(MockTestSuite, MockTestCase) {
    MockMyInterface mock;
    EXPECT_CALL(mock, Foo(5)).Times(1).WillOnce(testing::Return(10));

    ASSERT_EQ(mock.Foo(5), 10);
}

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}


// ====================== 1. 为optional添加支持 ======================


// ====================== 1. 定义业务结构体（完全不修改原始定义） ======================
struct Address {
    std::string street;
    std::string city;
    std::string country;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Address, city, country);
};

struct Employee {
    std::string name;
    int age{};
    std::vector<std::string> hobbies;
    Address address;
    std::optional<std::string> nickname; // 可选字段

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Employee, name, age, hobbies, address, nickname);
};

TEST(HelloTest, nlohmann_json) {

    Address addr{"123 Main St", "New York", "USA"};
    Employee employee{"Alice", 25, {"reading", "coding"}, addr, "Ali"};
    // 1. 完整转换流程
    std::cout << "===== 完整转换流程 =====" << std::endl;
    std::string jsonStr = toJson(employee, true);
    std::cout << "序列化结果:\n" << jsonStr << std::endl;

    auto restored = fromJson<Employee>(jsonStr);
    std::cout << "\n反序列化结果name: " << restored.name << std::endl;

    auto json_tree = toJsonTree(jsonStr);
    auto t = fromJsonTree<Employee>(json_tree);

}
