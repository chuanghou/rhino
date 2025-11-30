#pragma once

#include "nlohmann/json.hpp"
#include <cstdint>
#include <string>
#include "common/Version.h"

namespace rhino {
RHINO_INLINE_NAMESPACE_BEGIN

// error group, abstract for reduce code length
struct EGrp {

    int32_t code{};
    std::string semanticCode;

    EGrp() = default;

    EGrp(const int32_t code, std::string semanticCode)
        : code(code), semanticCode(std::move(semanticCode)) {
            if (code == 0) {
                throw std::runtime_error("you should not use zero as a error group code");
            }
        }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(EGrp, code, semanticCode);

    static EGrp INTERNAL;
};

struct Err {

    bool sysErr{};
    int32_t code{};
    std::string semanticCode;
    std::string msg;

    Err(const int32_t code, std::string semanticCode, std::string defaultMsg)
        : code(code), semanticCode(std::move(semanticCode)),
          msg(std::move(defaultMsg)) {
        if (code == 0) {
            throw std::runtime_error("errCode should not be 0!");
        }
        sysErr = code < 0;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Err, sysErr, code, semanticCode, msg);

    static Err SYS_ERR;

    static Err UNREACHABLE;

    static Err FORMAT_ERR;

    static Err QUEUE_FULL;
};
RHINO_INLINE_NAMESPACE_END
} // namespace rhino
