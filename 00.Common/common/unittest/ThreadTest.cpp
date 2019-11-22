#include "gtest/gtest.h"
#include "BaseThread.h"

class ThreadTest : public testing::Test 
{
protected:
	CBaseThread m_nThread;
    
    static unsigned int m_nThreadId;
    static uint32_t Run( void *arg );
};

unsigned int ThreadTest::m_nThreadId = 0;

uint32_t ThreadTest::Run( void *arg )
{
    printf("ThreadTest::Run-ok:thread id is %u\r\n", m_nThreadId);
    CBaseThread::Sleep(1000);
    return 0;
}

TEST_F(ThreadTest, run)
{
    EXPECT_FALSE(m_nThread.IsRunning());
    m_nThread.BeginThread(Run, (void*)&m_nThread, m_nThreadId, "ThreadTest");
    EXPECT_NE(0, m_nThreadId);
    CBaseThread::Sleep(500);
    EXPECT_TRUE(m_nThread.IsRunning());
    CBaseThread::Sleep(2000);
    EXPECT_FALSE(m_nThread.IsRunning());
}
