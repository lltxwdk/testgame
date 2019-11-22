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
#include <string.h>
#include <stdio.h>
#include "SelectEventLoop.h"
#include "EventHandler.h"
class TestEventHandler : public EventHandler
{
public:
    virtual int GetEventFd(){ return 0; }
    virtual bool IsNeedWriteEvent(){ return false; }
    virtual void HandleReadEvent(){}
    virtual void HandleWriteEvent(){}
    virtual void HandleErrorEvent(){}
    virtual void HandleEvent(bool ev_err, bool ev_read, bool ev_write){}
};

class SelectEventLoop_UTest : public testing::Test
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
    void UT_SyncHandlerSet_01();
    void UT_EstimateBitrate_01();

};

TEST_F(SelectEventLoop_UTest, UT_SyncHandlerSet_01){ UT_SyncHandlerSet_01(); }
void SelectEventLoop_UTest::UT_SyncHandlerSet_01()
{
    SelectEventLoop select_loop;
    TestEventHandler handls[10];
    for (size_t i = 0; i < 10; i++)
    {
        select_loop.AddEventFd(FD_TCP_NORMAL, &handls[i]);
    }
    EXPECT_EQ(select_loop.pending_handlers.size(), 10);
    EXPECT_TRUE(select_loop.handler_set_.empty());
    EXPECT_TRUE(select_loop.handler_arr_.empty());

    select_loop.SyncHandlerSet();
    EXPECT_EQ(select_loop.handler_arr_.size(), 10);
    EXPECT_EQ(select_loop.handler_set_.size(), 10);
    EXPECT_TRUE(select_loop.pending_handlers.empty());


    for (size_t i = 0; i < 10; i++)
    {
        select_loop.DelEventFd(&handls[0]);
    }
    EXPECT_EQ(select_loop.pending_handlers.size(), 10);
    EXPECT_EQ(select_loop.handler_arr_.size(), 10);
    EXPECT_EQ(select_loop.handler_set_.size(), 10);

    select_loop.SyncHandlerSet();
    EXPECT_EQ(select_loop.handler_arr_.size(), 9);
    EXPECT_EQ(select_loop.handler_set_.size(), 9);
    EXPECT_TRUE(select_loop.pending_handlers.empty());

}


