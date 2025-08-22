#include <cstdint>
#include <string>
#include <cstring>
#include <emmintrin.h>

namespace rhhno {

int32_t fastStoiNotSafely(const char *val) {
    int32_t number = 0;
    for (size_t i = 0;; i++) {
        if (val[i] == '\0') {
            return number;
        }
        number = (number << 3) + (number << 1) + val[i] - '0';
    }
}
int32_t fastStoiNotSafely(const std::string &val) {
    return fastStoiNotSafely(val.c_str());
}

/**
 * Attention!, if you really know the length, you delete the for loop
 * just write it!
 */
inline int32_t fastCopy(void *dst, const void *src, size_t length) {
    auto d = reinterpret_cast<__m128i*>(dst);
    auto s = reinterpret_cast<const __m128i*>(src);

    size_t partCount = length >> 4;
    for(int i = 0; i < partCount; i++) {
        _mm_storeu_si128(d + i, _mm_loadu_si128(s + i));
    }
    size_t copyed = partCount << 4;
    std::memcpy(dst + copyed, src + copyed, length - copyed);
}
} // namespace rhhno
