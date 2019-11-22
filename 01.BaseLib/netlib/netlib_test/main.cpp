#include "test.h"

int main(int argc, char* argv[])
{
    TraceLog::GetInstance().OpenLog("netlib.log", TL_DEBUG, true);
    LogINFO("Test start\n");

    IUdpTransLib* udp_translib = CreateUdpTransLib();
    udp_translib->Init(1, ASYNC_MODEL_EPOLL);
    test_normal(argc, argv);
    //test_udp_echoserver_send(udp_translib);
    //test_multithread_send(udp_translib);
    
    LogINFO("\nTest start");
	return 0;
}

