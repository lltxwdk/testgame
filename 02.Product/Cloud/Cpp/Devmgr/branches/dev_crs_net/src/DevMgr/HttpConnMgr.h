#ifndef _HTTPCONNMGR_H_
#define _HTTPCONNMGR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//libevent http server header files

#ifdef WIN32
#include "../../../../libevent-2.1.8-stable\win\vs2013\include/event.h" // 与00.Common\common下的文件有冲突,所以加上相对路径
#elif defined LINUX
#include "event.h"
#endif
#include "evhttp.h"
#include "event2/http.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"
#include "event2/http_struct.h"
#include "event2/http_compat.h"
#include "event2/util.h"
#include "event2/listener.h"

//#endif
#include "sr_type.h"
#include "SafeData.h"

class CAsyncThread;
class CDevMgr;

struct httpcbarg
{
	void* m_arg1;
	void* m_arg2;
};

class HttpConnMgr
{
public:
	HttpConnMgr(CAsyncThread* pThread, CDevMgr* devMgr);
	~HttpConnMgr();

	bool Init(const char* listen_ip, uint16_t listen_port);

private:
	CDevMgr* m_devMgr;
	CAsyncThread* m_pMainThread;
	uint64_t    http_conn_num_;

};

#endif