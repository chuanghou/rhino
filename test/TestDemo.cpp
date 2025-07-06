//
// Created by houch on 2025/6/22.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/filesystem.hpp>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "capnp/myProto.capnp.h"

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


kj::Array<capnp::word> writeAddressBook() {
    ::capnp::MallocMessageBuilder message;

    AddressBook::Builder addressBook = message.initRoot<AddressBook>();
    ::capnp::List<Person>::Builder people = addressBook.initPeople(2);

    Person::Builder alice = people[0];
    alice.setId(123);
    alice.setName("Alice");
    alice.setEmail("alice@examp   le.com");
    // Type shown for explanation purposes; normally you'd use auto.
    ::capnp::List<Person::PhoneNumber>::Builder alicePhones =
            alice.initPhones(1);
    alicePhones[0].setNumber("555-1212");
    alicePhones[0].setType(Person::PhoneNumber::Type::MOBILE);
    alice.getEmployment().setSchool("MIT");

    Person::Builder bob = people[1];
    bob.setId(456);
    bob.setName("Bob");
    bob.setEmail("bob@example.com");
    auto bobPhones = bob.initPhones(2);
    bobPhones[0].setNumber("555-4567");
    bobPhones[0].setType(Person::PhoneNumber::Type::HOME);
    bobPhones[1].setNumber("555-7654");
    bobPhones[1].setType(Person::PhoneNumber::Type::WORK);
    bob.getEmployment().setUnemployed();

    return capnp::messageToFlatArray(message);
}

void printAddressBook(kj::Array<capnp::word> &words) {
    capnp::FlatArrayMessageReader msgReader(words);
    AddressBook::Reader addressBook = msgReader.getRoot<AddressBook>();

    for (Person::Reader person: addressBook.getPeople()) {
        std::cout << person.getName().cStr() << ": "
                << person.getEmail().cStr() << std::endl;
        for (Person::PhoneNumber::Reader phone: person.getPhones()) {
            const char *typeName = "UNKNOWN";
            switch (phone.getType()) {
                case Person::PhoneNumber::Type::MOBILE: typeName = "mobile";
                    break;
                case Person::PhoneNumber::Type::HOME: typeName = "home";
                    break;
                case Person::PhoneNumber::Type::WORK: typeName = "work";
                    break;
            }
            std::cout << "  " << typeName << " phone: "
                    << phone.getNumber().cStr() << std::endl;
        }
        Person::Employment::Reader employment = person.getEmployment();
        switch (employment.which()) {
            case Person::Employment::UNEMPLOYED:
                std::cout << "  unemployed" << std::endl;
                break;
            case Person::Employment::EMPLOYER:
                std::cout << "  employer: "
                        << employment.getEmployer().cStr() << std::endl;
                break;
            case Person::Employment::SCHOOL:
                std::cout << "  student at: "
                        << employment.getSchool().cStr() << std::endl;
                break;
            case Person::Employment::SELF_EMPLOYED:
                std::cout << "  self-employed" << std::endl;
                break;
        }
    }
}

// capnp 从github拉分支，cmake -S . -B build  && cmake --build build && cmake --install build
// 通过这种方式安装可以直接使用cmake findpackage convenience!
TEST(HelloTest, CapnpTest) {
    auto array = writeAddressBook();

    printAddressBook(array);

}
