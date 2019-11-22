#include "gtest/gtest.h"
#include <stdarg.h>
#include "Utils.h"

class UtilsTest : public testing::Test 
{
protected:
};

class Hello
{
public:
    Hello(){}
    ~Hello(){}
};

TEST_F(UtilsTest, utils)
{
  printf("Get Utils Time%lu \r\n", Utils::Time());
  printf("Get TimeNanos Time%lld \r\n", Utils::TimeNanos());
  std::string strData1 = Utils::String::Format("%s", "hello,world");
  EXPECT_STREQ("hello,world", strData1.c_str());
  
  std::string strData2 = "hello";
  Utils::String::AppendFormat(strData2, "%s", ",world");
  EXPECT_STREQ("hello,world", strData2.c_str());
  
  std::string strKey = ",";
  std::string strContent = "hello,world";
  Utils::StringArray nValues;
  Utils::String::Split(strContent, nValues, strKey);
  EXPECT_EQ(2, nValues.size());
  EXPECT_STREQ("hello", nValues[0].c_str());
  EXPECT_STREQ("world", nValues[1].c_str());
}

TEST_F(UtilsTest, DelayRelease)
{
  DelayRelease<Hello> nDelayRelease;
  Hello *pHello = new Hello();
  nDelayRelease.Attach(pHello);
  nDelayRelease.RelayExpireObj(2000);
  EXPECT_TRUE(true);
}
