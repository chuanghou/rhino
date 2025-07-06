//
// Created by houchuang on 2025/7/6.
//

#ifndef JSON_H
#define JSON_H


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

namespace rhino::util::json {
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

#endif //JSON_H
