#include "gtest/gtest.h"

using namespace std;

#define Postive(msg) \
    if (value <= 0) throw std::invalid_argument(msg);

#define LessThan(limit, msg) \
    if (value >= limit) throw std::invalid_argument(msg);

#define Val(type, name, ...) \
private: \
    type name##_; \
public: \
    void set_##name(type value) { \
        __VA_ARGS__; \
        name##_ = value; \
    } \
    type get_##name() const { return name##_; }

// 使用示例
class Entity {
    
    Val(int32_t, score,
        Postive("Score must be positive");
        LessThan(100, "Score must be < 100");
    );
};

TEST(ValidationTest, protoType) {

    Entity en;

    bool ret = false;
    try
    {
    en.set_score(100);
    }
    catch(const std::exception& e)
    {
        ret = true;
        std::cerr << e.what() << '\n';
    }
    

    EXPECT_TRUE(ret);
    
}