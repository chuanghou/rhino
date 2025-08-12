
#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

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

namespace rhino {
    template<typename T>
    std::string toJson(const T &obj, bool pretty = false) {
        nlohmann::json j = obj;
        return pretty ? j.dump(4) : j.dump();
    }

    template<typename T>
    T fromJson(const std::string &value) {
        return nlohmann::json::parse(value).get<T>();
    }

    inline std::string fromJsonTree(const nlohmann::json &j, bool pretty = false) {
        return pretty ? j.dump(4) : j.dump();
    }

    inline nlohmann::json toJsonTree(const std::string &value) {
        return nlohmann::json::parse(value);
    }

    template<typename T>
    nlohmann::json toJsonTree(const T &obj) {
        return obj;
    }

    template<typename T>
    T fromJsonTree(const nlohmann::json &j) {
        return j.get<T>();
    }
}