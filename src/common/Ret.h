
#pragma once

#include "nlohmann/json.hpp"
#include <sstream>
#include <string>
#include <utility>
#include "common/Version.h"
#include "common/Errs.h"

#define SUCCESS_OF_NULLPTR rhino::Ret<void *>::successOfNullptr

namespace rhino {
RHINO_INLINE_NAMESPACE_BEGIN

template <class T> class Ret {
  public:
    bool success = true;
    T data{};
    bool sysErr = false;
    EGrp eGrp{};
    Err err{};
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Ret<T>, success, data, sysErr, eGrp, err)

    Ret() = default;

    explicit Ret(T data) : data(data) {}

    Ret(const bool success, const bool sysErr, EGrp eGrp, Err err)
        : success(success), sysErr(sysErr), eGrp(std::move(eGrp)),
          err(std::move(err)) {}

    bool failed() const { return !success; }

    EGrp getEGrp() const { return eGrp; }

    Err getErr() const { return err; }

    static Ret with(T data) { return Ret(data); }

    static Ret<void *> successOfNullptr;

    static Ret with(EGrp &eGrp, Err &err) {
        return Ret(false, err.sysErr, eGrp, err);
    }

    static Ret with(EGrp &eGrp, Err &err, const std::string &errMsg) {
        err.msg = errMsg;
        return Ret(false, err.sysErr, eGrp, err);
    }

    template <typename S> static Ret<T> FailureTransfer(Ret<S> source) {
        return Ret(source.success, source.sysErr, source.eGrp, source.err);
    }
};
RHINO_INLINE_NAMESPACE_END
} // namespace rhino
