//
// Created by houch on 2025/11/8.
//
#include "gtest/gtest.h"
#include <chrono>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

class SyncWriteBenchmark {
private:
    std::string filename_;
    size_t total_size_;
    size_t buffer_size_;

public:
    SyncWriteBenchmark(const std::string& filename, size_t total_size = 100 * 1024 * 1024, size_t buffer_size = 4096)
        : filename_(filename), total_size_(total_size), buffer_size_(buffer_size) {}

    // 测试1: 使用O_SYNC标志进行同步写入
    double test_osync() {
        auto start = std::chrono::high_resolution_clock::now();

        int fd = open(filename_.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);
        if (fd == -1) {
            std::cerr << "无法打开文件: " << filename_ << std::endl;
            return -1;
        }

        std::vector<char> buffer(buffer_size_, 'A');
        size_t bytes_written = 0;

        while (bytes_written < total_size_) {
            size_t write_size = std::min(buffer_size_, total_size_ - bytes_written);
            ssize_t result = write(fd, buffer.data(), write_size);
            if (result != static_cast<ssize_t>(write_size)) {
                std::cerr << "写入错误" << std::endl;
                close(fd);
                return -1;
            }
            bytes_written += result;
        }

        // 确保所有数据都刷到磁盘
        fsync(fd);
        close(fd);
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration<double>(end - start).count();
    }

    // 测试2: 使用O_DSYNC标志进行同步写入（只同步数据，不同步元数据）
    double test_odsync() {
        auto start = std::chrono::high_resolution_clock::now();

        int fd = open(filename_.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_DSYNC, 0644);
        if (fd == -1) {
            std::cerr << "无法打开文件: " << filename_ << std::endl;
            return -1;
        }

        std::vector<char> buffer(buffer_size_, 'B');
        size_t bytes_written = 0;

        while (bytes_written < total_size_) {
            size_t write_size = std::min(buffer_size_, total_size_ - bytes_written);
            ssize_t result = write(fd, buffer.data(), write_size);
            if (result != static_cast<ssize_t>(write_size)) {
                std::cerr << "写入错误" << std::endl;
                close(fd);
                return -1;
            }
            bytes_written += result;
        }

        close(fd);
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration<double>(end - start).count();
    }

    // 测试3: 使用fsync进行手动同步
    double test_fsync() {
        auto start = std::chrono::high_resolution_clock::now();

        int fd = open(filename_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            std::cerr << "无法打开文件: " << filename_ << std::endl;
            return -1;
        }

        std::vector<char> buffer(buffer_size_, 'C');
        size_t bytes_written = 0;

        while (bytes_written < total_size_) {
            size_t write_size = std::min(buffer_size_, total_size_ - bytes_written);
            ssize_t result = write(fd, buffer.data(), write_size);
            if (result != static_cast<ssize_t>(write_size)) {
                std::cerr << "写入错误" << std::endl;
                close(fd);
                return -1;
            }
            bytes_written += result;

            // 每次写入后都调用fsync（最严格的同步）
            fsync(fd);
        }

        close(fd);
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration<double>(end - start).count();
    }

    // 测试4: 使用fdatasync（只同步数据，不同步元数据）
    double test_fdatasync() {
        auto start = std::chrono::high_resolution_clock::now();

        int fd = open(filename_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            std::cerr << "无法打开文件: " << filename_ << std::endl;
            return -1;
        }

        std::vector<char> buffer(buffer_size_, 'D');
        size_t bytes_written = 0;

        while (bytes_written < total_size_) {
            size_t write_size = std::min(buffer_size_, total_size_ - bytes_written);
            ssize_t result = write(fd, buffer.data(), write_size);
            if (result != static_cast<ssize_t>(write_size)) {
                std::cerr << "写入错误" << std::endl;
                close(fd);
                return -1;
            }
            bytes_written += result;

            // 使用fdatasync，比fsync更快
            fdatasync(fd);
        }

        close(fd);
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration<double>(end - start).count();
    }

    // 测试5: 异步写入作为对比
    double test_async() {
        auto start = std::chrono::high_resolution_clock::now();

        int fd = open(filename_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            std::cerr << "无法打开文件: " << filename_ << std::endl;
            return -1;
        }

        std::vector<char> buffer(buffer_size_, 'E');
        size_t bytes_written = 0;

        while (bytes_written < total_size_) {
            size_t write_size = std::min(buffer_size_, total_size_ - bytes_written);
            ssize_t result = write(fd, buffer.data(), write_size);
            if (result != static_cast<ssize_t>(write_size)) {
                std::cerr << "写入错误" << std::endl;
                close(fd);
                return -1;
            }
            bytes_written += result;
        }

        close(fd);
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration<double>(end - start).count();
    }

    void run_benchmarks() {
        std::cout << "=== 同步写入磁盘性能测试 ===" << std::endl;
        std::cout << "测试文件大小: " << (total_size_ / (1024 * 1024)) << " MB" << std::endl;
        std::cout << "缓冲区大小: " << (buffer_size_ / 1024) << " KB" << std::endl;
        std::cout << "============================" << std::endl;

        // 测试各种同步方式
        std::cout << "\n1. O_SYNC (最严格的同步):" << std::endl;
        double time1 = test_osync();
        if (time1 > 0) {
            double speed1 = (total_size_ / (1024.0 * 1024.0)) / time1;
            std::cout << "   时间: " << time1 << " 秒" << std::endl;
            std::cout << "   速度: " << speed1 << " MB/s" << std::endl;
        }

        std::cout << "\n2. O_DSYNC (只同步数据):" << std::endl;
        double time2 = test_odsync();
        if (time2 > 0) {
            double speed2 = (total_size_ / (1024.0 * 1024.0)) / time2;
            std::cout << "   时间: " << time2 << " 秒" << std::endl;
            std::cout << "   速度: " << speed2 << " MB/s" << std::endl;
        }

        std::cout << "\n3. 每次写入后fsync (最慢):" << std::endl;
        double time3 = test_fsync();
        if (time3 > 0) {
            double speed3 = (total_size_ / (1024.0 * 1024.0)) / time3;
            std::cout << "   时间: " << time3 << " 秒" << std::endl;
            std::cout << "   速度: " << speed3 << " MB/s" << std::endl;
        }

        std::cout << "\n4. 每次写入后fdatasync:" << std::endl;
        double time4 = test_fdatasync();
        if (time4 > 0) {
            double speed4 = (total_size_ / (1024.0 * 1024.0)) / time4;
            std::cout << "   时间: " << time4 << " 秒" << std::endl;
            std::cout << "   速度: " << speed4 << " MB/s" << std::endl;
        }

        std::cout << "\n5. 异步写入 (作为对比):" << std::endl;
        double time5 = test_async();
        if (time5 > 0) {
            double speed5 = (total_size_ / (1024.0 * 1024.0)) / time5;
            std::cout << "   时间: " << time5 << " 秒" << std::endl;
            std::cout << "   速度: " << speed5 << " MB/s" << std::endl;
        }

        // 清理测试文件
        remove(filename_.c_str());
    }
};

TEST(FileWritePerfomance, demoTest) {
    // 测试参数：文件名，总大小(100MB)，缓冲区大小(4KB)
    SyncWriteBenchmark benchmark("sync_test_file.bin", 100 * 1024 * 1024, 4096);
    benchmark.run_benchmarks();
}