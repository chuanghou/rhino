#pragma once

#include <mutex>
#include "common/Version.h"

namespace rhino {
RHINO_INLINE_NAMESPACE_BEGIN
template <typename T> class Locked {

    T obj;
    mutable std::mutex mtx;

  public:
    template <typename Func> auto operator()(Func f) const -> decltype(f(obj)) {
        std::lock_guard<std::mutex> lock(mtx);
        return f(obj);
    }

    template <typename Func> auto operator()(Func f) -> decltype(f(obj)) {
        std::lock_guard<std::mutex> lock(mtx);
        return f(obj);
    }
};
RHINO_INLINE_NAMESPACE_END
} // namespace rhino
