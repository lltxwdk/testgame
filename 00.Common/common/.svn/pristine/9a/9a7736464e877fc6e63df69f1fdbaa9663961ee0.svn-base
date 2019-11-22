#include "gtest/gtest.h"
#include "Utils.h"
#include "SafeData.h"
#include <sys/timeb.h>

static int static_count = 0;
class TestData
{
public:
    TestData()
    {
        static_count++;
    }
    ~TestData()
    {
        static_count--;
    }
};

class SafeData_UTest : public testing::Test
{
protected:

    // Sets up the test fixture.
    virtual void SetUp()
    {
        
    }

    // Tears down the test fixture.
    virtual void TearDown()
    {

    }


protected:
    void UT_Test_1();
    void UT_Test_2();
    void UT_Test_3();

private:
    SafeMap<uint32_t, TestData> safe_map_;
};

TEST_F(SafeData_UTest, UT_Test_1){ UT_Test_1(); }
void SafeData_UTest::UT_Test_1()
{
    for (uint32_t i = 0; i < 1000; i++)
    {
        safe_map_.Insert(i, new TestData);
    }

    for (uint32_t i = 0; i < 1000; i++)
    {
        safe_map_.Delete(i);
    }

    Utils::Sleep(2 * 1000);
    safe_map_.RelayExpireObj(1000);

    EXPECT_EQ(static_count, 0);

    for (uint32_t i = 0; i < 1000; i++)
    {
        safe_map_.Insert(i, new TestData);
    }
}

TEST_F(SafeData_UTest, UT_Test_2){ UT_Test_2(); }
void SafeData_UTest::UT_Test_2()
{
    for (uint32_t i = 0; i < 1000; i++)
    {
        safe_map_.Insert(i, new TestData);
    }

    for (uint32_t i = 0; i < 1000; i++)
    {
        safe_map_.Delete(i);
    }

    safe_map_.RelayExpireObj(0);

    EXPECT_EQ(static_count, 0);
}


TEST_F(SafeData_UTest, UT_Test_3){ UT_Test_3(); }
void SafeData_UTest::UT_Test_3()
{
    EXPECT_EQ(static_count, 0);
}
