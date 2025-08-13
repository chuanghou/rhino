#include "common/Errs.h"
#include "common/Ret.h"
#include "gtest/gtest.h"

using namespace rhino;
using namespace std;

TEST(CommonTest, ErrsTest) {
    auto ret = Ret<void *>::with(EGrp::INTERNAL, Err::FORMAT_ERR, "格式化错误");
    EXPECT_EQ(ret.success, false);
}