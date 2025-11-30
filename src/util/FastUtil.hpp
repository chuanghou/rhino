#pragma once

#include <cstdint>
#include <string>
#include <cstring>
#include <emmintrin.h>

namespace rhino {

/**
 * 快速字符串转整数（不安全版本，不做参数校验，为了性能）
 * 
 * 限制和要求：
 * - val 必须是非空指针，指向有效的以 '\0' 结尾的字符串
 * - 字符串必须只包含数字字符 '0'-'9'，不能有其他字符（包括空格、负号、小数点等）
 * - 仅支持非负整数，不支持负数
 * - 不做溢出检查，如果结果超出 int32_t 范围，行为未定义
 * - 空字符串返回 0
 * 
 * 性能优化：使用位运算 (number << 3) + (number << 1) 代替 number * 10
 * 
 * @param val 指向数字字符串的指针，必须以 '\0' 结尾
 * @return 转换后的整数，如果输入无效则结果未定义
 */
inline int32_t FastStoiNotSafely(const char *val) {
    int32_t number = 0;
    for (size_t i = 0;; i++) {
        if (val[i] == '\0') {
            return number;
        }
        number = (number << 3) + (number << 1) + val[i] - '0';
    }
}

/**
 * 快速字符串转整数（std::string 重载版本）
 * 
 * @param val 包含数字的字符串
 * @return 转换后的整数
 */
inline int32_t FastStoiNotSafely(const std::string &val) {
    return FastStoiNotSafely(val.c_str());
}

/**
 * SIMD 优化的内存拷贝（不安全版本，不做参数校验，为了性能）
 * 
 * 使用 SSE2 指令集进行批量拷贝，每次处理 16 字节，剩余部分使用 memcpy
 * 
 * 限制和要求：
 * - dst 和 src 必须非空指针
 * - length 必须有效（可以为 0，此时不执行任何操作）
 * - 内存区域不能重叠（如果需要处理重叠，请使用 memmove）
 * - 需要 CPU 支持 SSE2 指令集
 * 
 * 性能优化：
 * - 使用 _mm_loadu_si128 和 _mm_storeu_si128 进行 16 字节批量拷贝
 * - 剩余不足 16 字节的部分使用 memcpy
 * 
 * Attention!, if you really know the length, you delete the for loop
 * just write it!
 * 
 * @param dst 目标内存地址
 * @param src 源内存地址
 * @param length 要拷贝的字节数
 */
inline void fastCopy(void *dst, const void *src, size_t length) {
    // 快速返回：如果长度为 0，直接返回
    if (length == 0) {
        return;
    }
    
    auto d = reinterpret_cast<__m128i*>(dst);
    auto s = reinterpret_cast<const __m128i*>(src);

    size_t partCount = length >> 4;  // 计算 16 字节块的数量
    for (size_t i = 0; i < partCount; i++) {
        _mm_storeu_si128(d + i, _mm_loadu_si128(s + i));
    }
    size_t copied = partCount << 4;  // 已拷贝的字节数（16 字节对齐部分）
    auto dstChar = reinterpret_cast<char*>(dst);
    auto srcChar = reinterpret_cast<const char*>(src);
    std::memcpy(dstChar + copied, srcChar + copied, length - copied);
}
} // namespace rhino
