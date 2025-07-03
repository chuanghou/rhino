//
// Created by houch on 2025/6/22.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/filesystem.hpp>

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