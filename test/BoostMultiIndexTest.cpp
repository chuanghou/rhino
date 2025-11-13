//
// Created by houch on 2025/11/8.
//

#include "gtest/gtest.h"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

// Boost.MultiIndex
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
using namespace std;
using namespace std::chrono;
namespace bmi = boost::multi_index;

// 测试数据结构
struct Data {
    int id;
    std::string value;

    Data(int i, const std::string& v) : id(i), value(v) {}
};

// Boost.MultiIndex 容器定义（仅使用hash索引）
using BoostMultiIndexContainer = bmi::multi_index_container<
    Data,
    bmi::indexed_by<
        bmi::hashed_unique<
            bmi::member<Data, int, &Data::id>
        >
    >
>;

// 性能测试类
class PerformanceTest {
private:
    std::unordered_map<int, Data> unordered_map_;
    BoostMultiIndexContainer multi_index_;
    std::vector<int> test_keys_;

public:
    PerformanceTest(int data_size) {
        generate_test_data(data_size);
    }

    // 生成测试数据
    void generate_test_data(int data_size) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, data_size * 10);

        // 生成不重复的随机键
        while (test_keys_.size() < data_size) {
            int key = dis(gen);
            if (std::find(test_keys_.begin(), test_keys_.end(), key) == test_keys_.end()) {
                test_keys_.push_back(key);
            }
        }
    }

    // 测试unordered_map插入性能
    long long test_unordered_map_insert() {
        auto start = high_resolution_clock::now();

        for (int key : test_keys_) {
            unordered_map_.emplace(key, Data{key, "value_" + std::to_string(key)});
        }

        auto end = high_resolution_clock::now();
        return duration_cast<microseconds>(end - start).count();
    }

    // 测试multi_index插入性能
    long long test_multi_index_insert() {
        auto start = high_resolution_clock::now();

        for (int key : test_keys_) {
            multi_index_.insert(Data{key, "value_" + std::to_string(key)});
        }

        auto end = high_resolution_clock::now();
        return duration_cast<microseconds>(end - start).count();
    }

    // 测试unordered_map查找性能
    long long test_unordered_map_find() {
        auto start = high_resolution_clock::now();

        size_t found_count = 0;
        for (int key : test_keys_) {
            auto it = unordered_map_.find(key);
            if (it != unordered_map_.end()) {
                ++found_count;
            }
        }

        auto end = high_resolution_clock::now();
        // 验证查找结果
        std::cout << "unordered_map查找验证: " << found_count << "/" << test_keys_.size()
                  << " 成功找到" << std::endl;
        return duration_cast<microseconds>(end - start).count();
    }

    // 测试multi_index查找性能
    long long test_multi_index_find() {
        auto& index = multi_index_.get<0>();
        auto start = high_resolution_clock::now();

        size_t found_count = 0;
        for (int key : test_keys_) {
            auto it = index.find(key);
            if (it != index.end()) {
                ++found_count;
            }
        }

        auto end = high_resolution_clock::now();
        // 验证查找结果
        std::cout << "multi_index查找验证: " << found_count << "/" << test_keys_.size()
                  << " 成功找到" << std::endl;
        return duration_cast<microseconds>(end - start).count();
    }

    // 运行完整测试
    void run_complete_test() {
        std::cout << "=== 性能测试开始 ===" << std::endl;
        std::cout << "数据量: " << test_keys_.size() << " 个元素" << std::endl;

        // 插入性能测试
        std::cout << "\n--- 插入性能测试 ---" << std::endl;
        auto um_insert_time = test_unordered_map_insert();
        auto mi_insert_time = test_multi_index_insert();

        std::cout << "std::unordered_map 插入时间: " << um_insert_time << " μs" << std::endl;
        std::cout << "Boost.MultiIndex 插入时间: " << mi_insert_time << " μs" << std::endl;
        std::cout << "性能差异: " << (mi_insert_time * 100.0 / um_insert_time - 100) << "%" << std::endl;

        // 查找性能测试
        std::cout << "\n--- 查找性能测试 ---" << std::endl;
        auto um_find_time = test_unordered_map_find();
        auto mi_find_time = test_multi_index_find();

        std::cout << "std::unordered_map 查找时间: " << um_find_time << " μs" << std::endl;
        std::cout << "Boost.MultiIndex 查找时间: " << mi_find_time << " μs" << std::endl;
        std::cout << "性能差异: " << (mi_find_time * 100.0 / um_find_time - 100) << "%" << std::endl;

        // 内存使用情况（估算）
        std::cout << "\n--- 内存使用估算 ---" << std::endl;
        std::cout << "std::unordered_map 大小: " << unordered_map_.size() << std::endl;
        std::cout << "Boost.MultiIndex 大小: " << multi_index_.size() << std::endl;
    }
};

// 不同数据量下的测试
void run_scalability_test() {
    std::vector<int> test_sizes = {1000, 10000, 50000, 100000};

    for (int size : test_sizes) {
        std::cout << "\n\n==================================" << std::endl;
        std::cout << "数据量: " << size << std::endl;
        std::cout << "==================================" << std::endl;

        PerformanceTest test(size);
        test.run_complete_test();
    }
}


TEST(BoostMultiIndexTest, performanceWithStdUnoderedMap) {
    // 单次测试
    PerformanceTest test(50000);
    test.run_complete_test();

    // 可扩展性测试（取消注释运行）
    run_scalability_test();
}