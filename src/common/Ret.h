
#pragma once

#include "nlohmann/json.hpp"
#include <sstream>
#include <string>
#include <utility>

#define SUCCESS_OF_NULLPTR com::Ret<void *>::successOfNullptr

namespace rhino {

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

    bool succeed() const { return success; }

    bool sysErr() const { return sysErr; }

    T getData() const { return data; }

    bool failed() const { return !success; }

    EGrp getEGrp() const { return eGrp; }

    Err getErr() const { return err; }

    static Ret successWith(T data) { return Ret(data); }

    static Ret<void *> successOfNullptr;

    static Ret sysErr(EGrp &eGrp, Err &err) {
        return Ret(false, true, eGrp, err);
    }

    static Ret sysErr(EGrp &eGrp, Err &err, const std::string &errMsg) {
        err.msg = errMsg;
        return Ret(false, true, eGrp, err);
    }

    static Ret bizErr(EGrp &eGrp, Err &err) {
        return Ret(false, false, eGrp, err);
    }

    static Ret bizErr(EGrp &eGrp, Err &err, const std::string &errMsg) {
        err.msg = errMsg;
        return Ret(false, false, eGrp, err);
    }

    template <typename S> static Ret<T> FailureTransfer(Ret<S> source) {
        return Ret(source.succeed(), source.sysErr, source.eGrp, source.err);
    }
};
} // namespace rhino
