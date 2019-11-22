#include "HttpConnMgr.h"
#include "asyncthread.h"
#include "DevMgr.h"

#include "cbuffer.h"

//int init_win_socket()
//{
//	WSADATA wsaData;
//	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//	{
//		return -1;
//	}
//	return 0;
//}

#define BUF_MAX 1024*16

//#ifdef WIN32
//#elif defined LINUX
//解析post请求数据
void get_post_message(char *buf, struct evhttp_request *req)
{
//#ifdef WIN32
//#elif defined LINUX
	size_t post_size = 0;

	post_size = evbuffer_get_length(req->input_buffer);//获取数据长度
	printf("==%s==line:%d,post len:%d\n", __FUNCTION__, __LINE__, post_size);
	if (post_size <= 0)
	{
		printf("==%s==line:%d,post msg is empty!\n", __FUNCTION__, __LINE__);
		return;
	}
	else
	{
		size_t copy_len = post_size > BUF_MAX ? BUF_MAX : post_size;
		printf("==%s==line:%d,post len:%d, copy_len:%d\n", __FUNCTION__, __LINE__, post_size, copy_len);
		memcpy(buf, evbuffer_pullup(req->input_buffer, -1), copy_len);
		buf[post_size] = '\0';
		printf("==%s==line:%d,post msg:%s\n", __FUNCTION__, __LINE__, buf);
	}
//#endif	
}

//解析http头，主要用于get请求时解析uri和请求参数
char *find_http_header(struct evhttp_request *req, struct evkeyvalq *params, const char *query_char)
{
	if (req == NULL || params == NULL || query_char == NULL)
	{
		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "input params is null.");
		return NULL;
	}

	char *query = NULL;
	char *query_result = NULL;

//#ifdef WIN32
//#elif defined LINUX
	struct evhttp_uri *decoded = NULL;
	const char *path;
	const char *uri = evhttp_request_get_uri(req);//获取请求uri

	if (uri == NULL)
	{
		printf("==%s==line:%d,evhttp_request_get_uri return null\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	else
	{
		printf("==%s==line:%d,Got a GET request for <%s>\n", __FUNCTION__, __LINE__, uri);
	}

	//解码uri
	decoded = evhttp_uri_parse(uri);
	if (!decoded)
	{
		printf("==%s==line:%d,It's not a good URI. Sending BADREQUEST\n", __FUNCTION__, __LINE__);
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		return NULL;
	}

	//获取uri中的path部分
	path = evhttp_uri_get_path(decoded);
	if (path == NULL)
	{
		path = "/";
	}
	else
	{
		printf("==%s==line:%d,path is:%s\n", __FUNCTION__, __LINE__, path);
	}

	//获取uri中的参数部分
	query = (char*)evhttp_uri_get_query(decoded);
	if (query == NULL)
	{
		printf("==%s==line:%d,evhttp_uri_get_query return null\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	//查询指定参数的值
	evhttp_parse_query_str(query, params);
	query_result = (char*)evhttp_find_header(params, query_char);
//#endif	

	return query_result;
}

//处理get请求
void handler_http_hjget_msg_cb(struct evhttp_request *req, void *arg)
{
	if (req == NULL)
	{
		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "input param req is null.");
		return;
	}

//	char *sign = NULL;
//	char *data = NULL;
//	struct evkeyvalq sign_params = { 0 };
//	sign = find_http_header(req, &sign_params, "sign");//获取get请求uri中的sign参数
//	if (sign == NULL)
//	{
//		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "request uri no param sign.");
//	}
//	else
//	{
//		printf("==%s==line:%d,get request param: sign=[%s]\n", __FUNCTION__, __LINE__, sign);
//	}
//
//	data = find_http_header(req, &sign_params, "data");//获取get请求uri中的data参数
//	if (data == NULL)
//	{
//		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "request uri no param data.");
//	}
//	else
//	{
//		printf("==%s==line:%d,get request param: data=[%s]\n", __FUNCTION__, __LINE__, data);
//	}
//	printf("\n");
//
//	//回响应
//	struct evbuffer *retbuff = NULL;
//	retbuff = evbuffer_new();
//	if (retbuff == NULL)
//	{
//		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "retbuff is null.");
//		return;
//	}
//	evbuffer_add_printf(retbuff, "Receive get request,Thanks for the request!");
//	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);
//	evbuffer_free(retbuff);
}

//处理post请求
void handler_http_hjpost_msg_cb(struct evhttp_request *req, void *arg)
{
	if (req == NULL)
	{
		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "input param req is null.");
		return;
	}

	//char buf[BUF_MAX] = { 0 };
	//get_post_message(buf, req);//获取请求数据，一般是json格式的数据
	//if (buf == NULL)
	//{
	//	printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "get_post_message return null.");
	//	return;
	//}
	//else
	//{
	//	//可以使用json库解析需要的数据
	//	printf("==%s==line:%d,request data:%s\n", __FUNCTION__, __LINE__, buf);
	//}

//#ifdef WIN32
//#elif defined LINUX
//
//	//回响应
//	struct evbuffer *retbuff = NULL;
//	retbuff = evbuffer_new();
//	if (retbuff == NULL)
//	{
//		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "retbuff is null.");
//		return;
//	}
//	evbuffer_add_printf(retbuff, "Receive post request,Thanks for the request!");
//	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);
//	evbuffer_free(retbuff);
//#endif	
}

//处理操作会见网关控制监控的http请求
void handler_http_cmdhjgw_opmonitor_msg_cb(struct evhttp_request *req, void *arg)
{
	httpcbarg* pcbarg = NULL;
	pcbarg = (httpcbarg*)arg;
	if (req == NULL
		|| pcbarg == NULL)
	{
		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "input param req or arg is null.");
		return;
	}

	CDevMgr* pdevMgr = NULL;
	CAsyncThread* pThread = NULL;
	pdevMgr = (CDevMgr*)(pcbarg->m_arg1);
	pThread = (CAsyncThread*)(pcbarg->m_arg2);
	if (pdevMgr == NULL
		|| pThread == NULL)
	{
		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "input param pdevMgr or pThread is null.");
		return;
	}

//#ifdef WIN32
//#elif defined LINUX

	switch (req->type)
	{
	case EVHTTP_REQ_GET:
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req type is EVHTTP_REQ_GET(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_POST:
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req type is EVHTTP_REQ_POST(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_HEAD:
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req type is EVHTTP_REQ_HEAD(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_PUT:
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req type is EVHTTP_REQ_PUT(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_DELETE:
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req type is EVHTTP_REQ_DELETE(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_OPTIONS:
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req type is EVHTTP_REQ_OPTIONS(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_TRACE:
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req type is EVHTTP_REQ_TRACE(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_CONNECT:
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req type is EVHTTP_REQ_CONNECT(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_PATCH:
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req type is EVHTTP_REQ_PATCH(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	default:
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req type is unknown(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	}

	char url[8192] = { 0 };
	char buf[BUF_MAX] = { 0 };
	const struct evhttp_uri* evhttp_uri = evhttp_request_get_evhttp_uri(req);
	evhttp_uri_join(const_cast<struct evhttp_uri*>(evhttp_uri), url, 8192);
	sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,accept req url:%s\n", __FUNCTION__, __LINE__, url);

	size_t post_size = 0;
	post_size = evbuffer_get_length(req->input_buffer);//获取数据长度
	printf("==%s==line:%d,post len:%d\n", __FUNCTION__, __LINE__, post_size);
	if (post_size <= 0)
	{
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,post msg is empty!\n", __FUNCTION__, __LINE__);
		return;
	}
	else
	{
		size_t copy_len = post_size > BUF_MAX ? BUF_MAX : post_size;
		//printf("==%s==line:%d,post len:%d, copy_len:%d\n", __FUNCTION__, __LINE__, post_size, copy_len);
		memcpy(buf, evbuffer_pullup(req->input_buffer, -1), copy_len);
		buf[copy_len] = '\0';
		sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,post len:%d, copy_len:%d,post msg:%s\n", __FUNCTION__, __LINE__, post_size, copy_len, buf);
	}

	//char *post_data = (char *)EVBUFFER_DATA(req->input_buffer);
	//sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,post info:%s\n", __FUNCTION__, __LINE__, post_data);

	std::string strtmp = buf;
	int index;
	std::map<std::string, std::string> cmd_key_val;
	cmd_key_val.clear();
	char *sep1 = { "&" };
	char *sep2 = { "=" };
	int seplen1 = strlen(sep1);
	int seplen2 = strlen(sep2);

	//sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,strpostdata->>%s\n", __FUNCTION__, __LINE__, strtmp.c_str());

	int lastindx = 0;
	index = strtmp.find(sep1);
	while (index != strtmp.npos)
	{
		//printf("lastindx=%d, curindex=%d\n", lastindx, index);

		std::string srcsubstr = strtmp.substr(lastindx, index - lastindx);

		//printf("srcsubstr = %s\n", srcsubstr.c_str());

		int subindex = srcsubstr.find(sep2);
		if (subindex > 0)
		{
			cmd_key_val.insert(std::pair<std::string, std::string>(srcsubstr.substr(0, subindex), srcsubstr.substr(subindex + seplen2, srcsubstr.length())));
		}

		lastindx = index;

		index = strtmp.find(sep1, index + 1);

		//printf("next index=%d\n", index);
		if (index > 0)
		{
			lastindx += seplen1;

			//printf("next lastindx=%d\n", lastindx);
		}
		else
		{
			std::string lastsubstr = strtmp.substr(lastindx + seplen1, strtmp.length());

			//printf("lastsubstr = %s\n", lastsubstr.c_str());

			int lastsubindex = lastsubstr.find(sep2);
			if (lastsubindex > 0)
			{
				cmd_key_val.insert(std::pair<std::string, std::string>(lastsubstr.substr(0, lastsubindex), lastsubstr.substr(lastsubindex + seplen2, lastsubstr.length())));
			}
		}
	}

	for (std::map<std::string, std::string>::iterator ms_itor = cmd_key_val.begin();
		ms_itor != cmd_key_val.end(); ms_itor++)
	{
		sr_printf(SR_LOGLEVEL_DEBUG,"cmd_key_val<key:%s, value:%s>\n", ms_itor->first.c_str(), ms_itor->second.c_str());
	}


	std::string strError;
	strError.clear();

	unsigned int  uigwid = 0;
	unsigned int uiOPtype = 0;
	std::string strplatformcode;
	strplatformcode.clear();
	std::string strdsturl;
	strdsturl.clear();
	std::string strplatfnewcode;
	strplatfnewcode.clear();
	std::string strplatfnewkey;
	strplatfnewkey.clear();
	std::map<std::string, std::string>::iterator gw_itor = cmd_key_val.find("gwid");
	if (gw_itor != cmd_key_val.end())
	{
		uigwid = atoi(gw_itor->second.c_str());
		
		if (uigwid != 0)
		{
			std::map<std::string, std::string>::iterator op_itor = cmd_key_val.find("optype");// sync、modify、del
			if (op_itor != cmd_key_val.end())
			{
#ifdef WIN32
				if (_stricmp(op_itor->second.c_str(), "sync") == 0)
				{
					uiOPtype = 1;
				}
				else if (_stricmp(op_itor->second.c_str(), "modify") == 0)
				{
					uiOPtype = 2;
				}
				else if (_stricmp(op_itor->second.c_str(), "del") == 0)
				{
					uiOPtype = 3;
				}
#elif defined LINUX
				if (strcasecmp(op_itor->second.c_str(), "sync") == 0)
				{
					uiOPtype = 1;
				}
				else if (strcasecmp(op_itor->second.c_str(), "modify") == 0)
				{
					uiOPtype = 2;
				}
				else if (strcasecmp(op_itor->second.c_str(), "del") == 0)
				{
					uiOPtype = 3;
				}
#endif
				
				if (uiOPtype != 0)
				{
					std::map<std::string, std::string>::iterator pc_itor = cmd_key_val.find("platformcode");
					if (pc_itor != cmd_key_val.end())
					{
						strplatformcode = pc_itor->second;
						//if (strplatformcode.length() == 20)
						if (strplatformcode.length() > 0)
						{
							if (uiOPtype == 1) // sync
							{
								std::map<std::string, std::string>::iterator dsturl_itor = cmd_key_val.find("dsturl");
								if (dsturl_itor != cmd_key_val.end())
								{
									//decoded url
									char *decoded_dst_url = NULL;
									decoded_dst_url = evhttp_decode_uri(dsturl_itor->second.c_str());
									if (decoded_dst_url != NULL)
									{
										printf("decoded_dst_uri len=%d, dsturl=%s\n", strlen(decoded_dst_url), decoded_dst_url);
										if (strlen(decoded_dst_url) > 0)
										{
											strdsturl = decoded_dst_url;
											//typedef CBufferT<unsigned int, unsigned int, void*, void*, void*, void*, void*, void*> CParam;
											//CParam* pParam;
											//pParam = new CParam(decoded_dst_url, strlen(decoded_dst_url), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, uigwid, uiOPtype);
											//typedef void (CDevMgr::* ACTION)(void*);
											//pThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
											//	*pdevMgr, &CDevMgr::Handle_CmdHJGW_OPMonitor_msg, (void*)pParam));
										} 
										else
										{
											strError.assign("dsturl parm is null.");
										}
									}
									else
									{
										strError.assign("decode dsturl parm error.");
										//printf("evhttp_decode_uri return error.\n");
									}
								}
								else
								{
									strError.assign("dsturl parm not find.");
								}
							}
							else if (uiOPtype == 2) // modify
							{
								std::map<std::string, std::string>::iterator pnc_itor = cmd_key_val.find("platformnewcode");
								if (pnc_itor != cmd_key_val.end())
								{
									strplatfnewcode = pnc_itor->second;
									//if (strplatfnewcode.length() == 20)
									if (strplatfnewcode.length() > 0)
									{
										// 只有消息中明确有密码才使用新密码
										std::map<std::string, std::string>::iterator newkey_itor = cmd_key_val.find("platformnewkey");
										if (newkey_itor != cmd_key_val.end())
										{
											strplatfnewkey = newkey_itor->second;
										}

										//// 只要两者有一个就进行修改
										//typedef CBufferT<unsigned int, unsigned int, void*, void*, void*, void*, void*, void*> CParam;
										//CParam* pParam;
										//pParam = new CParam(strplatfnewcode.c_str(), strplatfnewcode.length(), strplatfnewkey.c_str(), strplatfnewkey.length(), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, uigwid, uiOPtype);
										//typedef void (CDevMgr::* ACTION)(void*);
										//pThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
										//	*pdevMgr, &CDevMgr::Handle_CmdHJGW_OPMonitor_msg, (void*)pParam));
									}
									else
									{
										strError.assign("platformnewcode is error.");
									}
								}
								else
								{
									strError.assign("platformnewcode parm not find.");
								}
							}
							else if (uiOPtype == 3) // del
							{
							}
						}
						else
						{
							strError.assign("platformcode is error.");
						}
					}
					else
					{
						strError.assign("platformcode parm not find.");
					}
				}
				else
				{
					strError.assign("optype is error.");
				}
			}
			else
			{
				strError.assign("optype parm not find.");
			}
		}
		else
		{
			strError.assign("gwid is error.");
		}
	}
	else
	{
		strError.assign("gwid parm not find.");
	}

	////HTTP header
	//evhttp_add_header(req->output_headers, "Server", "myhttpd v 0.0.1");
	//evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
	//evhttp_add_header(req->output_headers, "Connection", "close");

	//回响应
	struct evbuffer *retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR,"==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "retbuff is null.");
		return;
	}

	if (strError.length() > 0)
	{
		//evhttp_send_error(req, HTTP_NOTFOUND, strError.c_str());// 404

		evbuffer_add_printf(retbuff, "Receive opmonitor http request,%s",strError.c_str());
	}
	else
	{
		typedef CBufferT<unsigned int, unsigned int, void*, void*, void*, void*, void*, void*> CParam;
		CParam* pParam;
		pParam = new CParam((char *)strplatformcode.c_str(), strplatformcode.length(), (char *)strdsturl.c_str(), strdsturl.length(), (char *)strplatfnewcode.c_str(), strplatfnewcode.length(), (char *)strplatfnewkey.c_str(), strplatfnewkey.length(), NULL, 0, NULL, 0, NULL, 0, NULL, 0, uigwid, uiOPtype);

		typedef void (CDevMgr::* ACTION)(void*);
		pThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*pdevMgr, &CDevMgr::Handle_CmdHJGW_OPMonitor_msg, (void*)pParam));
		
		evbuffer_add_printf(retbuff, "Receive opmonitor http request,Thanks for the request!");
		//evhttp_send_reply(req, HTTP_OK, "Client", retbuff);// 200
		//evbuffer_free(retbuff);
	}

	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);// 200
	evbuffer_free(retbuff);
//#endif	
}

//处理操作会见网关音视频能力集配置信息的http请求
void handler_http_cmdhjgw_opavconfig_msg_cb(struct evhttp_request *req, void *arg)
{
	httpcbarg* pcbarg = NULL;
	pcbarg = (httpcbarg*)arg;
	if (req == NULL
		|| pcbarg == NULL)
	{
		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "input param req or arg is null.");
		return;
	}

	CDevMgr* pdevMgr = NULL;
	CAsyncThread* pThread = NULL;
	pdevMgr = (CDevMgr*)(pcbarg->m_arg1);
	pThread = (CAsyncThread*)(pcbarg->m_arg2);
	if (pdevMgr == NULL
		|| pThread == NULL)
	{
		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "input param pdevMgr or pThread is null.");
		return;
	}

	//#ifdef WIN32
	//#elif defined LINUX

	switch (req->type)
	{
	case EVHTTP_REQ_GET:
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req type is EVHTTP_REQ_GET(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_POST:
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req type is EVHTTP_REQ_POST(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_HEAD:
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req type is EVHTTP_REQ_HEAD(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_PUT:
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req type is EVHTTP_REQ_PUT(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_DELETE:
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req type is EVHTTP_REQ_DELETE(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_OPTIONS:
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req type is EVHTTP_REQ_OPTIONS(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_TRACE:
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req type is EVHTTP_REQ_TRACE(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_CONNECT:
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req type is EVHTTP_REQ_CONNECT(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	case EVHTTP_REQ_PATCH:
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req type is EVHTTP_REQ_PATCH(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	default:
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req type is unknown(%d)\n", __FUNCTION__, __LINE__, req->type);
		break;
	}

	char url[8192] = { 0 };
	char buf[BUF_MAX] = { 0 };
	const struct evhttp_uri* evhttp_uri = evhttp_request_get_evhttp_uri(req);
	evhttp_uri_join(const_cast<struct evhttp_uri*>(evhttp_uri), url, 8192);
	sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,accept req url:%s\n", __FUNCTION__, __LINE__, url);

	size_t post_size = 0;
	post_size = evbuffer_get_length(req->input_buffer);//获取数据长度
	printf("==%s==line:%d,post len:%d\n", __FUNCTION__, __LINE__, post_size);
	if (post_size <= 0)
	{
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,post msg is empty!\n", __FUNCTION__, __LINE__);
		return;
	}
	else
	{
		size_t copy_len = post_size > BUF_MAX ? BUF_MAX : post_size;
		//printf("==%s==line:%d,post len:%d, copy_len:%d\n", __FUNCTION__, __LINE__, post_size, copy_len);
		memcpy(buf, evbuffer_pullup(req->input_buffer, -1), copy_len);
		buf[copy_len] = '\0';
		sr_printf(SR_LOGLEVEL_DEBUG, "==%s==line:%d,post len:%d, copy_len:%d,post msg:%s\n", __FUNCTION__, __LINE__, post_size, copy_len, buf);
	}

	//char *post_data = (char *)EVBUFFER_DATA(req->input_buffer);
	//sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,post info:%s\n", __FUNCTION__, __LINE__, post_data);

	std::string strtmp = buf;
	int index;
	std::map<std::string, std::string> cmd_key_val;
	cmd_key_val.clear();
	char *sep1 = { "&" };
	char *sep2 = { "=" };
	int seplen1 = strlen(sep1);
	int seplen2 = strlen(sep2);

	//sr_printf(SR_LOGLEVEL_DEBUG,"==%s==line:%d,strpostdata->>%s\n", __FUNCTION__, __LINE__, strtmp.c_str());

	int lastindx = 0;
	index = strtmp.find(sep1);
	while (index != strtmp.npos)
	{
		//printf("lastindx=%d, curindex=%d\n", lastindx, index);

		std::string srcsubstr = strtmp.substr(lastindx, index - lastindx);

		//printf("srcsubstr = %s\n", srcsubstr.c_str());

		int subindex = srcsubstr.find(sep2);
		if (subindex > 0)
		{
			cmd_key_val.insert(std::pair<std::string, std::string>(srcsubstr.substr(0, subindex), srcsubstr.substr(subindex + seplen2, srcsubstr.length())));
		}

		lastindx = index;

		index = strtmp.find(sep1, index + 1);

		//printf("next index=%d\n", index);
		if (index > 0)
		{
			lastindx += seplen1;

			//printf("next lastindx=%d\n", lastindx);
		}
		else
		{
			std::string lastsubstr = strtmp.substr(lastindx + seplen1, strtmp.length());

			//printf("lastsubstr = %s\n", lastsubstr.c_str());

			int lastsubindex = lastsubstr.find(sep2);
			if (lastsubindex > 0)
			{
				cmd_key_val.insert(std::pair<std::string, std::string>(lastsubstr.substr(0, lastsubindex), lastsubstr.substr(lastsubindex + seplen2, lastsubstr.length())));
			}
		}
	}

	for (std::map<std::string, std::string>::iterator ms_itor = cmd_key_val.begin();
		ms_itor != cmd_key_val.end(); ms_itor++)
	{
		sr_printf(SR_LOGLEVEL_DEBUG, "cmd_key_val<key:%s, value:%s>\n", ms_itor->first.c_str(), ms_itor->second.c_str());
	}


	std::string strError;
	strError.clear();

	unsigned int uiOPtype = 0; // 1 - 增加、2 - 删除、3 - 修改
	std::string straudio;
	straudio.clear();
	std::string strvideo;
	strvideo.clear();

	std::map<std::string, std::string>::iterator op_itor = cmd_key_val.find("optype");// add、del、modify
	if (op_itor != cmd_key_val.end())
	{
#ifdef WIN32
		if (_stricmp(op_itor->second.c_str(), "add") == 0)
		{
			uiOPtype = 1;
		}
		else if (_stricmp(op_itor->second.c_str(), "del") == 0)
		{
			uiOPtype = 2;
		}
		else if (_stricmp(op_itor->second.c_str(), "modify") == 0)
		{
			uiOPtype = 3;
		}
#elif defined LINUX
		if (strcasecmp(op_itor->second.c_str(), "add") == 0)
		{
			uiOPtype = 1;
		}
		else if (strcasecmp(op_itor->second.c_str(), "del") == 0)
		{
			uiOPtype = 2;
		}
		else if (strcasecmp(op_itor->second.c_str(), "modify") == 0)
		{
			uiOPtype = 3;
		}
#endif

		if (uiOPtype != 0)
		{
			std::map<std::string, std::string>::iterator a_itor = cmd_key_val.find("audio");
			if (a_itor != cmd_key_val.end())
			{
				straudio = a_itor->second;

				std::map<std::string, std::string>::iterator v_itor = cmd_key_val.find("video");
				if (v_itor != cmd_key_val.end())
				{
					strvideo = v_itor->second;
				}
				else
				{
					strError.assign("video parm not find.");
				}
			}
			else
			{
				strError.assign("audio parm not find.");
			}
			// 有音视频配置项,进一步判断其值是否都为空
			if (strError.length() == 0)
			{
				if (straudio.length() == 0
					&& strvideo.length() == 0)
				{
					strError.assign("audio and video value is null.");
				}
			} 
		}
		else
		{
			strError.assign("optype is error.");
		}
	}
	else
	{
		strError.assign("optype parm not find.");
	}


	////HTTP header
	//evhttp_add_header(req->output_headers, "Server", "myhttpd v 0.0.1");
	//evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
	//evhttp_add_header(req->output_headers, "Connection", "close");

	//回响应
	struct evbuffer *rspbuff = NULL;
	rspbuff = evbuffer_new();
	if (rspbuff == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "rspbuff is null.");
		return;
	}

	if (strError.length() > 0)
	{
		//evhttp_send_error(req, HTTP_NOTFOUND, strError.c_str());// 404

		evbuffer_add_printf(rspbuff, "Receive opavconfig http request,%s", strError.c_str());
	}
	else
	{
		typedef CBufferT<unsigned int, void*, void*, void*, void*, void*, void*, void*> CParam;
		CParam* pParam;
		pParam = new CParam((char *)straudio.c_str(), straudio.length(), (char *)strvideo.c_str(), strvideo.length(), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, uiOPtype);

		typedef void (CDevMgr::* ACTION)(void*);
		pThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*pdevMgr, &CDevMgr::Handle_CmdHJGW_OPAVConfig_msg, (void*)pParam));

		evbuffer_add_printf(rspbuff, "Receive opavconfig http request,Thanks for the request!");
		//evhttp_send_reply(req, HTTP_OK, "Client", retbuff);// 200
		//evbuffer_free(retbuff);
	}

	evhttp_send_reply(req, HTTP_OK, "Client", rspbuff);// 200
	evbuffer_free(rspbuff);
	//#endif	
}

//处理其它http请求
void handler_http_generic_msg_cb(struct evhttp_request *req, void *arg)
{
	if (req == NULL)
	{
		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "input param req is null.");
		return;
	}

//#ifdef WIN32
//#elif defined LINUX

	//char* s = "This is the generic buf";
	const struct evhttp_uri* evhttp_uri = evhttp_request_get_evhttp_uri(req);
	char url[8192] = { 0 };
	evhttp_uri_join(const_cast<struct evhttp_uri*>(evhttp_uri), url, 8192);
	printf("==%s==line:%d,accept req url:%s\n", __FUNCTION__, __LINE__, url);

	char *post_data = (char *)EVBUFFER_DATA(req->input_buffer);
	printf("==%s==line:%d,post info\n :%s\n", __FUNCTION__, __LINE__, post_data);

	////HTTP header
	//evhttp_add_header(req->output_headers, "Server", "myhttpd v 0.0.1");
	//evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
	//evhttp_add_header(req->output_headers, "Connection", "close");

	//回响应
	struct evbuffer *retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("==%s==line:%d,%s\n", __FUNCTION__, __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add_printf(retbuff, "Receive http request,Thanks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);
	evbuffer_free(retbuff);
//#endif	
}

//#endif


HttpConnMgr::HttpConnMgr(CAsyncThread* pThread, CDevMgr* devMgr)
{
	m_pMainThread = pThread;
	m_devMgr = devMgr;
	http_conn_num_ = 0;
}

HttpConnMgr::~HttpConnMgr()
{
	m_pMainThread = NULL;
	m_devMgr = NULL;
	http_conn_num_ = 0;
}
bool HttpConnMgr::Init(const char* listen_ip, uint16_t listen_port)
{
//#ifdef WIN32
//#elif defined LINUX
	struct evhttp *http_server = NULL;
	short httpsvr_port = 9527;
	char httpsvr_addr[64] = "0.0.0.0";
	httpsvr_port = listen_port;
	//memcpy(httpsvr_addr, listen_ip, sizeof(listen_ip));// 指定某IP会出现绑定失败的情况

	struct event_base* evbase = event_base_new();
	http_server = evhttp_new(evbase);
	if (NULL == http_server)
	{
		return false;
	}

	int ret = evhttp_bind_socket(http_server, httpsvr_addr, httpsvr_port);
	if (ret != 0)
	{
		typedef CBufferT<int, void*, void*, void*, void*, void*, void*, void*> CParam;
		CParam* pParam;
		pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, ret);

		typedef void (CDevMgr::* ACTION)(void*);
		m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*m_devMgr, &CDevMgr::Handle_UpdateHttpSvrLisentState, (void*)pParam));

		return false;
	}

	////设置请求超时时间(s)
	//evhttp_set_timeout(http_server, 5);

	//设置事件处理函数，evhttp_set_cb针对每一个事件(请求)注册一个处理函数，
	//区别于evhttp_set_gencb函数，是对所有请求设置一个统一的处理函数
	//evhttp_set_cb(http_server, "/hj/get", handler_http_hjget_msg_cb, NULL);
	//evhttp_set_cb(http_server, "/hj/post", handler_http_hjpost_msg_cb, NULL);
	httpcbarg stcbarg;
	stcbarg.m_arg1 = m_devMgr;
	stcbarg.m_arg2 = m_pMainThread;
	evhttp_set_cb(http_server, "/devmgr/hjgw/opmonitor", handler_http_cmdhjgw_opmonitor_msg_cb, &stcbarg);
	httpcbarg stcbarg_opavconfig;
	stcbarg_opavconfig.m_arg1 = m_devMgr;
	stcbarg_opavconfig.m_arg2 = m_pMainThread;
	evhttp_set_cb(http_server, "/devmgr/hjgw/opavconfig", handler_http_cmdhjgw_opavconfig_msg_cb, &stcbarg_opavconfig);
	evhttp_set_gencb(http_server, handler_http_generic_msg_cb, NULL);

	printf("http server start OK!\n");
	typedef CBufferT<int, void*, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, ret);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*m_devMgr, &CDevMgr::Handle_UpdateHttpSvrLisentState, (void*)pParam));

	//循环监听
	event_base_dispatch(evbase);
	//实际上不会释放，代码不会运行到这一步
	evhttp_free(http_server);
//#endif	
	
	printf("HttpConnMgr::Init OK!\n");
	return true;
}
//HttpConnMgr::HttpConnMgr(ITcpTransLib* tcp_trans_mod, CDevMgr* devMgr)
//{
//	tcp_trans_mod_ = tcp_trans_mod;
//	m_devMgr_ = devMgr;
//	http_conn_num_ = 0;
//
//	//device_connect_map_.Clear();
//	//get_last_map_.clear();
//
//	//tcp_listener_ = NULL;
//
//	//m_devmgrinfomanager = new SRMC::DevmgrCoonManager();
//}
//
//HttpConnMgr::~HttpConnMgr()
//{
//	//device_connect_map_.Clear();
//	//get_last_map_.clear();
//}
//
//bool HttpConnMgr::Init(const char* listen_ip, uint16_t listen_port)
//{
//	tcp_listener_ = tcp_trans_mod_->CreateTcpListenner(this);
//	if (NULL == tcp_listener_)
//	{
//		return false;
//	}
//	if (!tcp_listener_->Listen(listen_ip, listen_port))
//	{
//		return false;
//	}
//	return true;
//}
////// 主动关闭连接
////void HttpConnMgr::HttpOnClose(DeviceConnect * pDeviceConnet)
////{
////}
//
//void HttpConnMgr::OnAccept(ITcpConn* new_tcpconn, char* remote_ip, unsigned short remote_port)
//{
//	//INetAddr addr(remote_ip, remote_port);
//	//sr_printf(SR_LOGLEVEL_NORMAL, "Accept new tcp connection from device:%s, new_tcpconn=%p\n", addr.AsString().c_str(), new_tcpconn);
//	//DeviceConnect *device_cnct = new DeviceConnect(new_tcpconn, dev_seq_num_++, m_devMgr_);
//	//device_cnct->Init(remote_ip, remote_port);
//
//	//assert(device_connect_map_.Find(device_cnct) == NULL);
//	//device_connect_map_.Insert(device_cnct, device_cnct);
//	//sr_printf(SR_LOGLEVEL_NORMAL, "++device size = %d, device_cnct=%p\n", device_connect_map_.Size(), device_cnct);
//}
//
//void HttpConnMgr::OnNetError(int err_code)
//{
//	assert(false);
//}
