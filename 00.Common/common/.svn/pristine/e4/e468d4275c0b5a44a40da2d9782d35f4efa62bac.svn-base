#include "gtest/gtest.h"
#include "TraceLog.h"

class TraceLogTest : public testing::Test 
{
protected:
};

TEST_F(TraceLogTest, Log)
{
    EXPECT_TRUE(TraceLog::GetInstance().OpenLog(NULL, 0, true));
    TraceLog::GetInstance().EnabledFile(true);
    LogDEBUG("TraceLog default");
    LogINFO("TraceLog default");
    LogNOTICE("TraceLog default");
    LogWARNING("TraceLog default");
    LogCRIT("TraceLog default");
    LogALERT("TraceLog default");
    LogEMERG("TraceLog default");

    EXPECT_TRUE(TraceLog::GetInstance().OpenLog("hello-1.log", 0, true));
    LogDEBUG("TraceLog log");
    LogINFO("TraceLog log");
    LogNOTICE("TraceLog log");
    LogWARNING("TraceLog log");
    LogCRIT("TraceLog log");
    LogALERT("TraceLog log");
    LogEMERG("TraceLog log");
    
    TraceLog::GetInstance().TotalLog("total_log", "hello total log");
    TraceLog::GetInstance().TotalLog(NULL, "hello total log");
    TraceLog::GetInstance().CloseLog();
}
