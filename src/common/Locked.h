#pragma once

#include <mutex>

namespace rhino {
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
} // namespace rhino
