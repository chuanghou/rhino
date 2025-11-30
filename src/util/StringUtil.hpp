#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <cstring>
#include "common/Ret.h"
#include "common/Errs.h"
#include "common/Version.h"

// SIMD support detection
#if defined(__SSE2__) || (defined(_M_IX86) || defined(_M_X64))
#define RHINO_USE_SIMD 1
#include <emmintrin.h>
#if defined(_MSC_VER)
#include <intrin.h>
#endif
#endif

namespace rhino {
RHINO_INLINE_NAMESPACE_BEGIN

inline bool isBlank(const std::string &val) {
    bool blank = false;
    if (std::all_of(val.begin(), val.end(),
                    [](const char &c) { return std::isspace(c); })) {
        blank = true;
    }
    return blank;
}

inline std::vector<std::string> split(const std::string &s,
                               const std::string &delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }

    tokens.push_back(s.substr(start));

    return tokens;
}

/**
 * Copy string pointer to array (SIMD optimized version: uses SSE2 instructions for batch copying)
 * @param src Source string pointer (const char*)
 * @param dest Destination array (array reference, size auto-deduced)
 * @return Ret<size_t> Returns number of copied characters (excluding null terminator) on success, contains error information on failure
 */
template <size_t N>
inline Ret<size_t> copyStringToArray(const char* src, char (&dest)[N]) {
    // Compile-time check: array size must be greater than 0
    static_assert(N > 0, "Array size must be greater than 0");
    
    // Check if source pointer is null
    if (src == nullptr) {
        EGrp eGrp = EGrp::INTERNAL;
        Err err = Err::FORMAT_ERR;
        return Ret<size_t>::with(eGrp, err, "Source string pointer is nullptr");
    }
    
    const size_t maxCopy = N - 1; // Reserve space for null terminator
    size_t copied = 0;
    
#if defined(RHINO_USE_SIMD) && (N >= 16)
    // Use SIMD optimization: use SSE2 for batch copying on larger arrays
    constexpr size_t SIMD_SIZE = 16; // SSE2 processes 16 bytes at a time
    const size_t simdChunks = maxCopy / SIMD_SIZE;
    
    // Batch copy 16-byte chunks
    for (size_t i = 0; i < simdChunks; ++i) {
        // Load 16 bytes
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + copied));
        
        // Check if contains null byte (0x00)
        // Use _mm_movemask_epi8 to create mask, if null byte found, corresponding bit is 1
        __m128i zero = _mm_setzero_si128();
        __m128i cmp = _mm_cmpeq_epi8(chunk, zero);
        int mask = _mm_movemask_epi8(cmp);
        
        if (mask != 0) {
            // Found null byte, find position of first null
            // Use cross-platform bit operations to find first set bit
            int nullPos;
#if defined(__GNUC__) || defined(__clang__)
            nullPos = __builtin_ctz(mask); // Count trailing zeros (position of first 1)
#elif defined(_MSC_VER)
            unsigned long index;
            _BitScanForward(&index, mask);
            nullPos = static_cast<int>(index);
#else
            // Fallback to manual search
            nullPos = 0;
            while ((mask & (1 << nullPos)) == 0) {
                ++nullPos;
            }
#endif
            // Copy up to null byte
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dest + copied), chunk);
            copied += nullPos;
            dest[copied] = '\0';
            
            // Check if stopped early due to array being too small
            if (src[copied] != '\0') {
                EGrp eGrp = EGrp::INTERNAL;
                Err err = Err::FORMAT_ERR;
                size_t srcLen = copied;
                while (src[srcLen] != '\0') {
                    ++srcLen;
                }
                std::string errMsg = "Source string length (" + std::to_string(srcLen) + 
                                    ") exceeds destination array size (" + std::to_string(N) + ")";
                return Ret<size_t>::with(eGrp, err, errMsg);
            }
            
            return Ret<size_t>::with(copied);
        }
        
        // No null byte found, copy entire chunk
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dest + copied), chunk);
        copied += SIMD_SIZE;
    }
    
    // Handle remaining portion less than 16 bytes
    while (copied < maxCopy && src[copied] != '\0') {
        dest[copied] = src[copied];
        ++copied;
    }
#else
    // Fallback to normal copy (short strings or no SIMD support)
    while (copied < maxCopy && src[copied] != '\0') {
        dest[copied] = src[copied];
        ++copied;
    }
#endif
    
    // Ensure null terminator
    dest[copied] = '\0';
    
    // Check if stopped early due to array being too small (source string has remaining characters)
    if (src[copied] != '\0') {
        EGrp eGrp = EGrp::INTERNAL;
        Err err = Err::FORMAT_ERR;
        // Calculate full length only in error case (for error message)
        size_t srcLen = copied;
        while (src[srcLen] != '\0') {
            ++srcLen;
        }
        std::string errMsg = "Source string length (" + std::to_string(srcLen) + 
                            ") exceeds destination array size (" + std::to_string(N) + ")";
        return Ret<size_t>::with(eGrp, err, errMsg);
    }
    
    // Return success result with number of copied characters
    return Ret<size_t>::with(copied);
}
RHINO_INLINE_NAMESPACE_END
} // namespace rhino
