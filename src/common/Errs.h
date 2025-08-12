#pragma once

#include "nlohmann/json.hpp"
#include <cstdint>
#include <string>

namespace rhino {

// error group, abstract for reduce code length
struct EGrp {

    int32_t code{};
    std::string semanticCode;

    EGrp() = default;

    EGrp(const int32_t code, std::string semanticCode)
        : code(code), semanticCode(std::move(semanticCode)) {}

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(EGrp, code, semanticCode);

    static EGrp INTERNAL;
};

struct Err {

    int32_t code{};
    std::string semanticCode;
    std::string msg;

    Err() = default;

    Err(const int32_t code, std::string semanticCode, std::string defaultMsg)
        : code(code), semanticCode(std::move(semanticCode)),
          msg(std::move(defaultMsg)) {}

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Err, code, semanticCode, msg);

    static Err SYS_ERR;

    static Err UNREACHABLE;

    static Err FORMAT_ERR;

    static Err QUEUE_FULL;
};
} // namespace rhino
