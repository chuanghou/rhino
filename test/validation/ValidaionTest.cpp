#include "gtest/gtest.h"
#include "common/Validation.h"
using namespace std;

class Entity {
    Var(int32_t, score,
        Positive("Score must be positive");
        LessThan(100, "Score must be < 100"););
};

TEST(ValidationTest, protoType) {

    Entity en{};

    bool ret = false;
    try {
        en.set_score(100);
    } catch (const std::invalid_argument &e) {
        ret = true;
        std::cerr << e.what() << '\n';
    }

    EXPECT_TRUE(ret);
}