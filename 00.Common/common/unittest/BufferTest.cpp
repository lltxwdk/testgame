/*
 * =====================================================================================
 *
 *       Filename:  buffer_unittest.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/07/2014 10:22:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *        Company:
 *
 * =====================================================================================
 */
#include "gtest/gtest.h"
#include "Buffer.h"
#include "CriticalSection.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//any fixed tpye data
typedef struct tagMyBuf 
{   
    char buf_data[1024];
}MyBuf;

class BufferTest : public testing::Test
{
protected:
    virtual void SetUp()
    {
        pNull = NULL;
    }
    
    Buffer<MyBuf, CCriticalSection> m_nBufferList;
    MemPool<MyBuf, CCriticalSection> m_nMemPool;
    MyBuf *pNull;
};

//memlist test
TEST_F(BufferTest, MemList)
{
    /*EXPECT_TRUE(m_nBufferList.isEmpty());
    MyBuf *pBuf1 = m_nMemPool.GetMemery();
    m_nBufferList.Push(pBuf1);
    EXPECT_FALSE(m_nBufferList.isEmpty());
    
    MyBuf *pBuf2 = m_nBufferList.pop();
    EXPECT_NE(pNull, pBuf2);
    m_nMemPool.Release(pBuf2);
    
    MyBuf *pBuf3 = m_nBufferList.pop();
    EXPECT_EQ(pNull, pBuf3);
    
    EXPECT_TRUE(m_nBufferList.isEmpty());*/
}

//mempool test
TEST_F(BufferTest, MemPoll)
{
    /*MyBuf *pushmem = NULL;
    MyBuf *popmem = NULL;
    EXPECT_TRUE(m_nBufferList.isEmpty());
    for(int n = 0; n < 10; n++) 
    {
        pushmem = m_nMemPool.GetMemery();
        if ( !n ) 
        {
            popmem = pushmem;
        }
        sprintf(pushmem->buf_data, "%d:string", n);
        m_nBufferList.Push(pushmem);
    }
    EXPECT_EQ(popmem, m_nBufferList.pop());
    
    popmem = m_nBufferList.pop();
    while( popmem != NULL )
    {
        m_nMemPool.Release(popmem);
        popmem = m_nBufferList.pop();
    }
    
    popmem = m_nBufferList.pop();
    EXPECT_EQ(pNull, popmem);*/
}


