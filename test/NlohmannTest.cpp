//
// Created by 侯创 on 2025/7/10.
//

#include <iostream>
#include <ostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <utility>

template<typename T>
struct nlohmann::adl_serializer<std::optional<T> > {
    static void to_json(json &j, const std::optional<T> &opt) {
        if (opt) {
            j = *opt;
        } else {
            j = nullptr;
        }
    }

    static void from_json(const json &j, std::optional<T> &opt) {
        if (j.is_null()) {
            opt = std::nullopt;
        } else {
            opt = j.get<T>();
        }
    }
};

namespace util::json {
    template<typename T>
    std::string to_json(const T &obj, bool pretty = false) {
        nlohmann::json j = obj;
        return pretty ? j.dump(4) : j.dump();
    }

    template<typename T>
    T from_json(const std::string &value) {
        return nlohmann::json::parse(value).get<T>();
    }

    inline std::string from_json_tree(const nlohmann::json &j, bool pretty = false) {
        return pretty ? j.dump(4) : j.dump();
    }

    inline nlohmann::json to_json_tree(const std::string &value) {
        return nlohmann::json::parse(value);
    }

    template<typename T>
    nlohmann::json to_json_tree(const T &obj) {
        return obj;
    }

    template<typename T>
    T from_json_tree(const nlohmann::json &j) {
        return j.get<T>();
    }
}

struct BoolConfig {
    std::string type = "bool";
    bool value{};
    std::string desc;

    explicit BoolConfig(std::string desc) : desc(std::move(desc)) {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(BoolConfig, type, value, desc);
};

struct Int32Config {
    std::string type = "int32";
    int32_t value{};
    std::string desc;

    explicit Int32Config(std::string desc) : desc(std::move(desc)) {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Int32Config, type, value, desc);
};

struct StringConfig {
    std::string type = "string";
    std::string value;
    std::string desc;

    explicit StringConfig(std::string desc) : desc(std::move(desc)) {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(StringConfig, type, value, desc);
};


struct HotConfig {
    BoolConfig boolConfig1{"aaa"};
    Int32Config int32Config1{"bbbb"};
    StringConfig stringConfig1{"cccc"};

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(HotConfig, boolConfig1, int32Config1, stringConfig1);
};

TEST(NlohmannTest, Serialize) {
    HotConfig hotConfig;
    auto jsonRet = util::json::to_json(hotConfig, true);

    std::string s = R"({
    "boolConfig1": {
        "desc": "aaa",
        "type": "bool",
        "value": false
    },
    "int32Config1": {
        "desc": "bbbb",
        "type": "int32",
        "value": 0
    },
    "stringConfig1": {
        "desc": "cccc",
        "type": "string",
        "value": ""
    }
})";

    auto newConfig = util::json::from_json<HotConfig>(s);

    std::cout << util::json::to_json(hotConfig, true) << std::endl;


}
