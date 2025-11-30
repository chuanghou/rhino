#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <vector>
#include <cstdint>
#include "common/Version.h"

// Platform-specific includes
#if defined(_WIN32) && (defined(_M_IX86) || defined(_M_X64))
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <immintrin.h>
#endif

namespace rhino {
RHINO_INLINE_NAMESPACE_BEGIN

// 辅助函数：获取高精度时钟的纳秒计数（用于非 x86 平台）
inline uint64_t get_chrono_nanoseconds() noexcept {
    using clock = std::chrono::high_resolution_clock;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               clock::now().time_since_epoch())
        .count();
}

/**
 * 获取 TSC 计数（带序列化，原始计数）
 * 
 * rdtscp 指令会序列化所有先前的指令，确保在读取 TSC 之前所有指令都已完成
 * 这对于精确测量代码段的执行时间很重要
 * 
 * @return TSC 计数值，非 x86 平台返回高精度时钟的纳秒数
 */
inline uint64_t rdtscp() noexcept {
#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
#if defined(_WIN32)
    unsigned int aux;
    return __rdtscp(&aux);
#elif defined(__GNUC__) || defined(__clang__)
    unsigned int lo, hi, aux;
    __asm__ __volatile__("rdtscp" : "=a"(lo), "=d"(hi), "=c"(aux) : : "memory");
    return (static_cast<uint64_t>(hi) << 32) | lo;
#else
    return get_chrono_nanoseconds();
#endif
#else
    return get_chrono_nanoseconds();
#endif
}

/**
 * 获取 TSC 计数（原始计数）
 * 
 * rdtsc 指令不会序列化，可能被乱序执行，但性能稍好
 * 适用于不需要严格顺序的场景
 * 
 * @return TSC 计数值，非 x86 平台返回高精度时钟的纳秒数
 */
inline uint64_t rdtsc() noexcept {
#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
#if defined(_WIN32)
    return __rdtsc();
#elif defined(__GNUC__) || defined(__clang__)
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi) : : "memory");
    return (static_cast<uint64_t>(hi) << 32) | lo;
#else
    return get_chrono_nanoseconds();
#endif
#else
    return get_chrono_nanoseconds();
#endif
}

/**
 * TSC 频率校准（跨平台实现，假设已绑核）
 * 
 * 通过测量固定时间内的 TSC 计数来计算 CPU 频率
 * 使用中位数减少异常值影响，使用方差检查确保校准质量
 * 
 * 注意：
 * - 建议在程序启动时调用一次，结果缓存使用
 * - 需要 CPU 支持 TSC 且频率稳定（现代 CPU 通常满足）
 * - 在多核系统上建议绑定到特定核心
 * 
 * @return TSC 频率（GHz），校准失败或非 x86 平台返回 0.0
 */
inline double calibrate_tsc_frequency() noexcept {
#if !defined(__i386__) && !defined(__x86_64__) && !defined(_M_IX86) && \
    !defined(_M_X64)
    return 0.0;
#endif

    // 校准参数
    constexpr int CALIBRATION_TIME_MS = 200;    // 校准时间（毫秒）
    constexpr int MIN_ITERATIONS = 5;            // 最小迭代次数
    constexpr int MAX_ITERATIONS = 20;          // 最大迭代次数（防止无限循环）
    constexpr double MAX_VARIANCE_RATIO = 0.005; // 最大允许方差比率 (0.5%)

    std::vector<double> measurements;
    measurements.reserve(MAX_ITERATIONS);

    for (int i = 0; i < MAX_ITERATIONS; i++) {
        auto start_chrono = std::chrono::steady_clock::now();
        uint64_t start_tsc = rdtscp();

        // 精确等待 - 使用高精度时钟自旋
        auto target_time = start_chrono + std::chrono::milliseconds(CALIBRATION_TIME_MS);
        while (std::chrono::steady_clock::now() < target_time) {
            // 使用 pause 指令减少循环开销和功耗
#if defined(__GNUC__) || defined(__clang__)
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

        // 检查数据有效性（允许 5% 的误差）
        constexpr int64_t min_elapsed_ns = static_cast<int64_t>(CALIBRATION_TIME_MS * 1e6 * 0.95);
        if (elapsed_ns < min_elapsed_ns || elapsed_ticks == 0) {
            continue;
        }

        // 计算频率 (GHz)
        double freq_ghz = static_cast<double>(elapsed_ticks) / static_cast<double>(elapsed_ns);
        measurements.push_back(freq_ghz);

        // 检查方差是否达标（需要至少 MIN_ITERATIONS 个样本）
        if (measurements.size() >= MIN_ITERATIONS) {
            double sum = 0.0;
            double sum_sq = 0.0;
            for (double f : measurements) {
                sum += f;
                sum_sq += f * f;
            }

            double mean = sum / measurements.size();
            // 计算方差：Var(X) = E[X^2] - (E[X])^2
            double variance = (sum_sq / measurements.size()) - (mean * mean);

            // 检查变异系数（标准差/均值）是否在允许范围内
            if (variance >= 0.0 && variance < MAX_VARIANCE_RATIO * mean * mean) {
                break;
            }
        }
    }

    // 返回中位数减少异常值影响
    if (measurements.empty()) {
        return 0.0;
    }

    // 使用 nth_element 优化：只需要部分排序
    size_t median_idx = measurements.size() / 2;
    std::nth_element(measurements.begin(), measurements.begin() + median_idx,
                     measurements.end());
    return measurements[median_idx];
}

/**
 * 计算高精度时间差（纳秒）
 * 
 * 使用缓存的 TSC 频率进行转换，首次调用时会进行校准
 * 
 * @param time_begin 开始时间（TSC 计数或纳秒）
 * @param time_end 结束时间（TSC 计数或纳秒）
 * @return 时间差（纳秒）
 */
inline uint64_t calc_time_nano(uint64_t time_begin, uint64_t time_end) noexcept {
#if !defined(__i386__) && !defined(__x86_64__) && !defined(_M_IX86) && \
    !defined(_M_X64)
    // 非 x86 平台直接返回差值（已经是纳秒）
    return time_end - time_begin;
#endif

    // 使用静态局部变量缓存校准结果（线程安全，C++11 保证）
    static const double tsc_freq = calibrate_tsc_frequency();

    // 在 x86 架构且校准成功时使用 TSC
    if (tsc_freq > 0.0) {
        uint64_t ticks = time_end - time_begin;
        // 使用浮点运算提高精度，然后转换为整数
        return static_cast<uint64_t>(static_cast<double>(ticks) / tsc_freq);
    }

    // 非 x86 架构或校准失败时直接返回差值
    return time_end - time_begin;
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