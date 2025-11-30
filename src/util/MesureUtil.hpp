#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>
#include "common/Version.h"

namespace rhino {
RHINO_INLINE_NAMESPACE_BEGIN
// 获取TSC计算（原始计数）
inline uint64_t rdtscp() noexcept {
// x86/x64架构实现
#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
#if defined(_WIN32)
    unsigned int aux;
    return __rdtscp(&aux);
#elif defined(__linux__)
    unsigned int lo, hi, aux;
    __asm__ __volatile__("rdtscp" : "=a"(lo), "=d"(hi), "=c"(aux));
    return (static_cast<uint64_t>(hi) << 32) | lo;
#else
    // 其他平台使用chrono
    using clock = std::chrono::high_resolution_clock;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               clock::now().time_since_epoch())
        .count();
#endif
// 非x86架构使用chrono高精度时钟
#else
    using clock = std::chrono::high_resolution_clock;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               clock::now().time_since_epoch())
        .count();
#endif
}

// 获取TSC计算（原始计数）
inline uint64_t rdtsc() noexcept {
// x86/x64架构实现
#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
#if defined(_WIN32)
    return __rdtsc(); // Windows 直接使用 rdtsc
#elif defined(__linux__)
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
#else
    // 其他平台使用chrono
    using clock = std::chrono::high_resolution_clock;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               clock::now().time_since_epoch())
        .count();
#endif
// 非x86架构使用chrono高精度时钟
#else
    using clock = std::chrono::high_resolution_clock;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               clock::now().time_since_epoch())
        .count();
#endif
}

// TSC频率校准（跨平台实现，假设已绑核）
inline double calibrate_tsc_frequency() {
// 非x86架构返回0（不需要TSC频率）
#if !defined(__i386__) && !defined(__x86_64__) && !defined(_M_IX86) &&         \
    !defined(_M_X64)
    return 0.0;
#endif

    // 校准参数优化
    constexpr int CALIBRATION_TIME_MS = 200; // 增加校准时间提高精度
    constexpr int MIN_ITERATIONS = 5;        // 增加最小迭代次数
    constexpr double MAX_VARIANCE = 0.005;   // 更严格的最大允许方差 (0.5%)

    // 移除 MIN/MAX_ACCEPTED_FREQ 检查，现代 CPU 频率范围更广
    std::vector<double> measurements;
    measurements.reserve(MIN_ITERATIONS);

    for (int i = 0; i < MIN_ITERATIONS; i++) {
        auto start_chrono = std::chrono::steady_clock::now();
        uint64_t start_tsc = rdtscp();

        // 精确等待 - 使用高精度时钟自旋
        auto target_time =
            start_chrono + std::chrono::milliseconds(CALIBRATION_TIME_MS);
        while (std::chrono::steady_clock::now() <
               target_time) { // 使用pause指令减少循环开销和功耗
#if defined(__GNUC__)
            __builtin_ia32_pause();
#elif defined(_MSC_VER)
            _mm_pause();
#endif
        }

        auto end_chrono = std::chrono::steady_clock::now();
        uint64_t end_tsc = rdtscp();

        // 计算经过时间
        auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                              end_chrono - start_chrono)
                              .count();
        uint64_t elapsed_ticks = end_tsc - start_tsc;

        // 检查数据有效性
        if (elapsed_ns < CALIBRATION_TIME_MS * 1e6 * 0.95 ||
            elapsed_ticks == 0) {
            continue;
        }

        // 计算频率 (GHz)
        double freq_ghz = static_cast<double>(elapsed_ticks) / elapsed_ns;
        measurements.push_back(freq_ghz);

        // 检查方差是否达标
        if (measurements.size() >= MIN_ITERATIONS) {
            double sum = 0.0;
            double sum_sq = 0.0;
            for (double f : measurements) {
                sum += f;
                sum_sq += f * f;
            }

            double mean = sum / measurements.size();
            double variance = (sum_sq - (sum * sum) / measurements.size()) /
                              measurements.size();

            if (variance < MAX_VARIANCE * mean * mean) {
                break;
            }
        }
    }

    // 返回中位数减少异常值影响
    if (measurements.empty())
        return 0.0;

    std::sort(measurements.begin(), measurements.end());
    return measurements[measurements.size() / 2];
}

// 计算高精度时间差(纳秒)
inline uint64_t calc_time_nano(const uint64_t uzTimeBegin,
                               const uint64_t uzTimeEnd) {
#if !defined(__i386__) && !defined(__x86_64__) && !defined(_M_IX86) &&         \
    !defined(_M_X64)
    return uzTimeEnd - uzTimeBegin;
#endif

    static const double tsc_freq = calibrate_tsc_frequency();

    // 在x86架构且校准成功时使用TSC
    if (tsc_freq > 0) {
        uint64_t ticks = uzTimeEnd - uzTimeBegin; // 转换为相对值，避免溢出问题
        // 转换为纳秒
        return static_cast<uint64_t>(ticks / tsc_freq);
    }

    // 非x86架构或校准失败时使用std::chrono
    return uzTimeEnd - uzTimeBegin;
}

// 计算高精度时间差(纳秒)
inline uint64_t calc_time_nano(const uint64_t uzTimeBegin,
                               const uint64_t uzTimeEnd,
                               const double dbTscFreq) {
#if !defined(__i386__) && !defined(__x86_64__) && !defined(_M_IX86) &&         \
    !defined(_M_X64)
    return uzTimeEnd - uzTimeBegin;
#endif

    // 在x86架构且校准成功时使用TSC
    if (dbTscFreq > 0.0) {
        uint64_t ticks = uzTimeEnd - uzTimeBegin; // 转换为相对值，避免溢出问题
        // 转换为纳秒
        return static_cast<uint64_t>(ticks / dbTscFreq);
    }

    // 非x86架构或校准失败时使用std::chrono
    return uzTimeEnd - uzTimeBegin;
}

// 计算高精度时间差(纳秒)
inline uint64_t cvt_tsc_to_nano(const uint64_t uzTscTick,
                                const double dbTscFreq) {
#if !defined(__i386__) && !defined(__x86_64__) && !defined(_M_IX86) &&         \
    !defined(_M_X64)
    return uzTscTick;
#endif

    // 在x86架构且校准成功时使用TSC
    if (dbTscFreq > 0.0) {
        // 转换为纳秒
        return static_cast<uint64_t>(uzTscTick / dbTscFreq);
    }

    // 非x86架构或校准失败时使用std::chrono
    return uzTscTick;
}

// ======================== 使用示例 ========================
// int main()
// {
//   // 一次性校准
//   const static double uzTscFreq = calibrate_tsc_frequency();
//   const uint64_t uzTimeBegin = rdtsc();
//   // 模拟工作负载
//   volatile double sum = 0;

//   for (int i = 0; i < 1000000; i++)
//   {
//     sum += std::sin(i) * std::cos(i);
//   }
//   const uint64_t uzTimeEnd = rdtsc();

//   const uint64_t duration_ns = calc_time_nano(uzTimeBegin, uzTimeEnd,
//   uzTscFreq);

//   std::cout << "总耗时:" << duration_ns << " ns;平均耗时:" << duration_ns /
//   1000000 << " ns\n";
// }
RHINO_INLINE_NAMESPACE_END
} // namespace rhino