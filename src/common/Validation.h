
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <regex>
/*
Validation.h offer a bunch of common used macro use to validate entity field. The idea comes 
from JSR 303, but c++ does not offer annotation or reflection until now. User Var macro and validations
offered, setter and getter will be added to your source file, you the set method will trigger valdation,
so easy. You can offer your only specific validation macro like we offered, just enjoy it.

*/
// class Entity {
//     Var(int32_t, score, Positive("Score must be positive");
//         LessThan(100, "Score must be < 100"););
// };

// TEST(ValidationTest, protoType) {

//     Entity en;

//     bool ret = false;
//     try {
//         en.set_score(100);
//     } catch (const std::invalid_argument &e) {
//         ret = true;
//         std::cerr << e.what() << '\n';
//     }

//     EXPECT_TRUE(ret);
// }


#define Var(type, name, ...)                                                   \
  private:                                                                     \
    type name##_;                                                              \
                                                                               \
  public:                                                                      \
    void set_##name(type value) {                                              \
        __VA_ARGS__;                                                           \
        name##_ = value;                                                       \
    }                                                                          \
    type get_##name() const { return name##_; }

#define Positive(msg)                                                           \
    {                                                                          \
        if (value <= 0)                                                        \
            throw std::invalid_argument(msg);                                  \
    }

#define PositiveOrZero(msg)                                                     \
    {                                                                          \
        if (value <= 0)                                                        \
            throw std::invalid_argument(msg);                                  \
    }

#define Negative(msg)                                                          \
    {                                                                          \
        if (value <= 0)                                                        \
            throw std::invalid_argument(msg);                                  \
    }

#define NegativeOrZero(msg)                                                    \
    {                                                                          \
        if (value <= 0)                                                        \
            throw std::invalid_argument(msg);                                  \
    }

#define NonZero(msg)                                                           \
    {                                                                          \
        if (value <= 0)                                                        \
            throw std::invalid_argument(msg);                                  \
    }

#define NotBlank(msg)                                                          \
    {                                                                          \
        bool blank = false;                                                    \
        if (std::all_of(value.begin(), value.end(),                            \
                        [](const char &c) { return std::isspace(c); })) {      \
            blank = true;                                                      \
        }                                                                      \
        if (blank) {                                                           \
            throw std::invalid_argument(msg);                                  \
        }                                                                      \
    }

#define LessOrEqualThan(limit, msg)                                            \
    {                                                                          \
        if (value > limit)                                                     \
            throw std::invalid_argument(msg);                                  \
    }

#define LessThan(limit, msg)                                                   \
    {                                                                          \
        if (value >= limit)                                                    \
            throw std::invalid_argument(msg);                                  \
    }

#define GreaterOrEqualThan(limit, msg)                                         \
    {                                                                          \
        if (value < limit)                                                     \
            throw std::invalid_argument(msg);                                  \
    }

#define GreaterThan(limit, msg)                                                \
    {                                                                          \
        if (value <= limit)                                                    \
            throw std::invalid_argument(msg);                                  \
    }

#define NotEqual(limit, msg)                                                   \
    {                                                                          \
        if (value == limit)                                                    \
            throw std::invalid_argument(msg);                                  \
    }

#define NotEmpty(msg)                                                          \
    {                                                                          \
        if (value.empty())                                                     \
            throw std::invalid_argument(msg);                                  \
    }

#define SizeLessThan(limit, msg)                                               \
    {                                                                          \
        if (value.size() >= limit)                                             \
            throw std::invalid_argument(msg);                                  \
    }

#define SizeLessOrEqualThan(limit, msg)                                        \
    {                                                                          \
        if (value.size() > limit)                                              \
            throw std::invalid_argument(msg);                                  \
    }

#define SizeGreaterThan(limit, msg)                                            \
    {                                                                          \
        if (value.size() <= limit)                                             \
            throw std::invalid_argument(msg);                                  \
    }

#define SizeGreaterOrEqualThan(limit, msg)                                     \
    {                                                                          \
        if (value.size() < limit)                                              \
            throw std::invalid_argument(msg);                                  \
    }

#define SizeEqual(limit, msg)                                                  \
    {                                                                          \
        if (value.size() != limit)                                             \
            throw std::invalid_argument(msg);                                  \
    }

#define SizeNotEqual(limit, msg)                                               \
    {                                                                          \
        if (value.size() == limit)                                             \
            throw std::invalid_argument(msg);                                  \
    }