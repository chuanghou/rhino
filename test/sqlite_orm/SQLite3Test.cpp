//
// Created by houch on 2025/11/7.
//

// 编译时需要链接sqlite3库，例如：g++ -std=c++14 main.cpp -lsqlite3 -o sqlite_orm_demo
#include "sqlite_orm/sqlite_orm.h"

#include "gtest/gtest.h"
#include <iostream>
#include <memory>

using std::cout;
using std::endl;
using namespace sqlite_orm;

// 定义User类，映射到users表
struct User {
    int id{};
    std::string name;
    int age{};
};

// 创建内存数据库存储实例
inline auto create_storage(const std::string &dbFile) {
    return make_storage(dbFile,
        make_table("users",
            make_column("id", &User::id, primary_key()),
            make_column("name", &User::name),
            make_column("age", &User::age)
        )
    );
}

// 类型别名，方便使用
using Storage = decltype(create_storage(""));

void sqlite_orm_demo() {

    const std::string dbFile = "demo.db";
    // 启动前删除之前的数据库文件
    std::remove(dbFile.c_str());
    std::cout << "已清理旧数据库文件" << std::endl;

    // 1. 创建内存数据库和表结构
    auto storage = create_storage(dbFile);
    storage.sync_schema(); // 同步表结构，如果表不存在则创建

    // 2. 插入数据
    User alice{0, "Alice", 30}; // id设为0，自增字段在插入时会被忽略
    User bob{0, "Bob", 25};
    User charlie{0, "Charlie", 35};

    alice.id = storage.insert(alice); // 插入并获取自增ID
    bob.id = storage.insert(bob);
    charlie.id = storage.insert(charlie);

    // 3. 查询所有用户
    cout << "所有用户:" << endl;
    auto all_users = storage.get_all<User>();
    for (const auto& user : all_users) {
        cout << "ID: " << user.id << ", Name: " << user.name << ", Age: " << user.age << endl;
    }

    // 4. 条件查询
    cout << "\n年龄大于30的用户:" << endl;
    auto older_users = storage.get_all<User>(where(c(&User::age) > 30));
    for (const auto& user : older_users) {
        cout << "Name: " << user.name << ", Age: " << user.age << endl;
    }

    // 5. 更新数据
    auto user_to_update = storage.get<User>(alice.id); // 根据ID获取用户
    user_to_update.age = 31;
    storage.update(user_to_update); // 更新到数据库

    cout << "\n更新Alice年龄后:" << endl;
    auto updated_alice = storage.get<User>(alice.id);
    cout << "Alice的新年龄: " << updated_alice.age << endl;

    // 6. 删除数据
    storage.remove<User>(charlie.id); // 根据ID删除用户

    cout << "\n删除Charlie后的用户:" << endl;
    auto remaining_users = storage.get_all<User>();
    for (const auto& user : remaining_users) {
        cout << "ID: " << user.id << ", Name: " << user.name << ", Age: " << user.age << endl;
    }
}

TEST(SQLite3Test, demo_test) {
    sqlite_orm_demo();
}