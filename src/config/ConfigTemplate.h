#pragma once
#include <string>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

namespace rhino {

class PostProcessor {
  public:
    virtual void PostProcess() {}
};

class StringCfg : public PostProcessor {
  public:
    std::string desc;
    std::string val;
    std::string type = "string";
};

struct Int32Cfg : public PostProcessor {
  public:
    std::string desc;
    int32_t val;
    std::string type = "int32";
};

struct BoolCfg : public PostProcessor {
  public:
    std::string desc;
    bool val;
    std::string type = "bool";
};

} // namespace rhino