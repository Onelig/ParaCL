#include "simulator.hpp"
#include <gtest/gtest.h>
#include <sstream>

class SimulatorTest : public ::testing::Test
{
protected:
    ParaCL::Simulator simulator_;
};


TEST_F(SimulatorTest, TestCreateValue)
{
    std::string code("value = 0;");

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());
}


TEST_F(SimulatorTest, TestPrintOper)
{
    std::string code("value = 312; print value;");

    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf());

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());

    std::cout.rdbuf(old_buf);

    ASSERT_EQ(ss.str(), "312\n");
}


TEST_F(SimulatorTest, TestArithmeticOpers)
{
    std::string code("a = 10 + 5; b = 20 - 3; c = 7 * 6; d = 19 / 2; e = 17 % 4; print a; print b; print c; print d; print e;");

    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf());

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());

    std::cout.rdbuf(old_buf);

    ASSERT_EQ(ss.str(), "15\n17\n42\n9\n1\n");
}


TEST_F(SimulatorTest, TestComparisonOpers)
{
    std::string code("x = 5; y = 8; print x < y; print x <= y; print x > y; print x >= y; print x == 5; print x != 5;");

    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf());

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());

    std::cout.rdbuf(old_buf);

    ASSERT_EQ(ss.str(), "1\n1\n0\n0\n1\n0\n");
}


TEST_F(SimulatorTest, TestAssignmentOpers)
{
    std::string code("val = 10; val += 5; print val; val -= 3; print val; val *= 2; print val; val /= 4; print val; val %= 4; print val;");

    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf());

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());

    std::cout.rdbuf(old_buf);

    ASSERT_EQ(ss.str(), "15\n12\n24\n6\n2\n");
}


TEST_F(SimulatorTest, TestIncDecOpers)
{
    std::string code("counter = 5; ++counter; print counter; --counter; print counter;");

    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf());

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());

    std::cout.rdbuf(old_buf);

    ASSERT_EQ(ss.str(), "6\n5\n");
}


TEST_F(SimulatorTest, TestUNMinLogNOT)
{
    std::string code("num = 5; print -num; flag = 0; print !flag; flag = 1; print !flag;");

    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf());

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());

    std::cout.rdbuf(old_buf);

    ASSERT_EQ(ss.str(), "-5\n1\n0\n");
}


TEST_F(SimulatorTest, TestIfElseOper)
{
    std::string code("value_p = 981; if (value_p >= 100) { print value_p; } else { print 100; }");

    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf());

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());

    std::cout.rdbuf(old_buf);

    ASSERT_EQ(ss.str(), "981\n");
}


TEST_F(SimulatorTest, TestWhileOper)
{
    std::string code("myNum = 100; while(myNum <= 110) { print myNum++; } print myNum; ");

    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf());

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());

    std::cout.rdbuf(old_buf);

    ASSERT_EQ(ss.str(), "100\n101\n102\n103\n104\n105\n106\n107\n108\n109\n110\n111\n");
}


TEST_F(SimulatorTest, TestIfWhileOper)
{
    std::string code("myNum = 100; while(myNum <= 110) { if (myNum % 2) { print myNum; } else { print myNum / 2; } ++myNum; } print myNum; ");

    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf());

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());

    std::cout.rdbuf(old_buf);

    ASSERT_EQ(ss.str(), "50\n101\n51\n103\n52\n105\n53\n107\n54\n109\n55\n111\n");
}

TEST_F(SimulatorTest, Combinedtest)
{
    std::string code("myNum = 100; while (myNum <= 105) { if (myNum % 2) { myNum *= 2; } else { myNum += 3; } print myNum; --myNum; }");

    std::stringstream ss;
    std::streambuf* old_buf = std::cout.rdbuf(ss.rdbuf());

    ASSERT_NO_THROW(simulator_.set(code));
    ASSERT_NO_THROW(simulator_.run());

    std::cout.rdbuf(old_buf);

    ASSERT_EQ(ss.str(), "103\n105\n107\n");
}