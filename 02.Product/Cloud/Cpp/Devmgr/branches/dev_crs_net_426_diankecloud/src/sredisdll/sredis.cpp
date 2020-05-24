
#if defined WIN32
#include "stdafx.h"
#include <string.h>
#elif defined LINUX
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#endif

#include <stdlib.h>
#include "sredis.h"

#if defined WIN32
#include "winsock2.h"
#pragma warning(disable: 4996)  
#endif

//#define REDIS_REPLY_STRING 1
//#define REDIS_REPLY_ARRAY 2
//#define REDIS_REPLY_INTEGER 3
//#define REDIS_REPLY_NIL 4
//#define REDIS_REPLY_STATUS 5
//#define REDIS_REPLY_ERROR 6

CSRedis::CSRedis(ICsredisSink* pSink)
{
	Init();
}

CSRedis::~CSRedis(void)
{
	Fini();
}

bool CSRedis::Init()
{
#if defined WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,1), &wsaData);
#endif

	_host.clear();
	_port = 6379;
	m_bconnectok = false;
	this->_connect = NULL;
	this->_reply = NULL;
	this->_reply1 = NULL;
	m_ulldisconnetcnt = 0;

	m_bauthok = false;
	_password.clear();

	return true;
}

void CSRedis::Fini()
{
	_host.clear();
	_port = 6379;
	m_bconnectok = false;
	m_bauthok = false;
	_password.clear();
	this->_connect = NULL;  
	this->_reply = NULL;  
	this->_reply1 = NULL;
}

bool CSRedis::connect(const char* host, int port)
{
	m_bconnectok = false;

	_host.assign(host);
	_port = port;

	disconnect();

	//int timeout = 10000;
	//struct timeval tv;
	//tv.tv_sec = timeout / 1000;
	//tv.tv_usec = timeout * 1000;
	this->_connect = redisConnect(host, port);
	if (this->_connect != NULL && this->_connect->err)
	{
		printf("redisConnect (%s, %d) Error:[%d, %s]\n", host, port, this->_connect->err, this->_connect->errstr);
		disconnect();
		return m_bconnectok;
	}

	m_ulldisconnetcnt = 0;
	m_bconnectok = true;
	return m_bconnectok;
}

bool CSRedis::isconnectok()
{
	return m_bconnectok;
}

void CSRedis::disconnect()
{
	if (this->_connect != NULL)
	{
		redisFree(this->_connect);
		this->_connect = NULL;
	}
}

bool  CSRedis::auth(const char* password)
{
	m_bauthok = false;
	std::string str_password;
	str_password.assign(password);

	_password.assign(str_password);

	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::auth== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::auth== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "AUTH %s", _password.c_str());

		printf("*************  CSRedis::auth result:_connect[%d, %s], this->_reply=%p\n", this->_connect->err, this->_connect->errstr, this->_reply);

		if (this->_reply != NULL)
		{
			//printf("*************  CSRedis::auth result:_connect[%d, %s], this->_reply=%p, this->_reply->type=%d, this->_reply->str=%s\n", this->_connect->err, this->_connect->errstr, this->_reply, this->_reply->type, this->_reply->str);

			// this->_connect->err��this->_reply��this->_reply->type��this->_reply->str
			//         ��0             NULL            ����                  ����                                       <---> ���Ӳ�����
			//          0             not NULL            5                  OK                                         <---> ��������,����AUTH�����Ҳ����
			//          0             not NULL            6          ERR invalid password                               <---> ��������,��AUTH����ش���,�ó�����redis-server����������ͻ���auth����Я�����������
			//          0             not NULL            6          ERR Client sent AUTH, but no password is set       <---> ��������,��AUTH����ش���,�ó�����redis-serverδ��������,���ͻ���auth����Я��������

#ifdef WIN32
			if (!(this->_reply != NULL && this->_reply->type == REDIS_REPLY_STATUS && stricmp(this->_reply->str, "OK") == 0))
#elif defined LINUX
			if (!(this->_reply != NULL && this->_reply->type == REDIS_REPLY_STATUS && strcasecmp(this->_reply->str, "OK") == 0))
#endif
			{
				printf("xxxxxxxxxxxxxx  CSRedis::auth result: this->_reply=%p, this->_reply->type=%d, this->_reply->str=%s\n", this->_reply, this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
				return false;
			}
			else
			{
				printf("oooooooooooooo  CSRedis::auth result:this->_reply=%p, this->_reply->type=%d, this->_reply->str=%s\n", this->_reply, this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
				m_bauthok = true;
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	return false;
}

void  CSRedis::flushall()		//���
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::flushall== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(),_port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::flushall== m_bconnectok == false reconnect error !!!----->> \n");
			return;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "flushall");
		if (this->_reply != NULL)
		{
//
//#ifdef WIN32
//			if (this->_reply->type == REDIS_REPLY_STATUS && stricmp(this->_reply->str, "OK") == 0)
//#elif defined LINUX
//			if (this->_reply->type == REDIS_REPLY_STATUS && strcasecmp(this->_reply->str, "OK") == 0)
//#endif
//			{
//			}
//			else
//			{
//			}
			freeReplyObject(this->_reply);
		}
	}
	return;
}

bool CSRedis::pingpong()
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::pingpong()== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::pingpong()== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "PING");

		printf("#############  CSRedis::pingpong result:_connect[%d, %s], this->_reply=%p\n", this->_connect->err, this->_connect->errstr, this->_reply);

		// this->_connect->err��this->_reply��this->_reply->type��this->_reply->str
		//         ��0             NULL            ����                  ����                                        <---> ���Ӳ�����
		//          0             not NULL            5                  PONG                                       <---> ��������,����PING�����Ҳ����
		//          0             not NULL            6          NOAUTH Authentication required                     <---> ��������,��PING����ش���,����ԭ����"δ����֤"


#ifdef WIN32
		if ((this->_reply == NULL && this->_connect->err != 0)
			|| (!(this->_reply != NULL && this->_reply->type == REDIS_REPLY_STATUS && stricmp(this->_reply->str, "PONG") == 0)))
#elif defined LINUX
		if ((this->_reply == NULL && this->_connect->err != 0)
			|| (!(this->_reply != NULL && this->_reply->type == REDIS_REPLY_STATUS && strcasecmp(this->_reply->str, "PONG") == 0)))
#endif
		{
			

#ifdef WIN32
			if (this->_reply != NULL && this->_reply->type == REDIS_REPLY_ERROR && stricmp(this->_reply->str, "NOAUTH Authentication required") == 0)
#elif defined LINUX
			if (this->_reply != NULL && this->_reply->type == REDIS_REPLY_ERROR && strcasecmp(this->_reply->str, "NOAUTH Authentication required") == 0)
#endif
			{
				// ��֤ʧ�ܵ�PING����
				if (_password.length() > 0)
				{
					// ������֤
					auth(_password.c_str());
				}
			}
			else
			{
				m_ulldisconnetcnt++;
				if (m_ulldisconnetcnt % 3 == 0)
				{
					m_ulldisconnetcnt = 0;
					m_bconnectok = false;
				}
			}

			if (this->_reply != NULL)
			{
				printf("xxxxxxxxxxx  CSRedis::pingpong this->_reply=%p, this->_reply->type=%d, this->_reply->str=%s\n", this->_reply, this->_reply->type, this->_reply->str);

				freeReplyObject(this->_reply); // ���ͷŴ����ڴ�й©
			}
			return false;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("oooooooooooo  CSRedis::pingpong this->_reply=%p, this->_reply->type=%d, this->_reply->str=%s\n", this->_reply, this->_reply->type, this->_reply->str);

				freeReplyObject(this->_reply); // ���ͷŴ����ڴ�й©
			}
			return true;
		}

		//if (this->_reply != NULL)
		//{
		//	printf("xxxxxxxxxxx  CSRedis::pingpong this->_reply=0x%x, this->_reply->type=%d, this->_reply->str=%s\n", this->_reply, this->_reply->type, this->_reply->str);
		//	if (this->_reply->type == REDIS_REPLY_STATUS
		//		&& stricmp(this->_reply->str, "PONG") == 0)
		//	{
		//	}
		//	freeReplyObject(this->_reply); // ���ͷŴ����ڴ�й©
		//	return true;
		//}
	}

	return false;
}

bool CSRedis::selectdb(const int dbnum)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::selectdb(const int dbnum)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::selectdb(const int dbnum)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "SELECT %d", dbnum);

		//printf("CSRedis::selectdb Error:[%d, %s], this->_reply=%p\n", this->_connect->err, this->_connect->errstr, this->_reply);
		//if (this->_reply == NULL
		//	&& this->_connect->err != 0)
		//{
		//	m_bconnectok = false;
		//}

		
		if (this->_reply != NULL)
		{
			printf("====CSRedis::selectdb(%d)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", dbnum, this->_reply->type, this->_reply->str);

#ifdef WIN32
			if (this->_reply->type == REDIS_REPLY_STATUS && stricmp(this->_reply->str, "OK") == 0)
#elif defined LINUX
			if (this->_reply->type == REDIS_REPLY_STATUS && strcasecmp(this->_reply->str, "OK") == 0)
#endif
			{
				freeReplyObject(this->_reply); // ���ͷŴ����ڴ�й©
				return true;
			}
			else
			{
				freeReplyObject(this->_reply); // ���ͷŴ����ڴ�й©
			}
		}
		
		//printf("=====this->_reply == NULL========= CSRedis::selectdb(%d)=====------------------------------>>>>>>>>>>\n", dbnum);
	}

	return false;
}

bool CSRedis::existskey(const char* key)		//ɾ��ֵ
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::existskey(const char* key)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::existskey(const char* key)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "EXISTS %s", key);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			if (this->_reply->integer == 1)
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::existskey(const char* key)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool  CSRedis::setexpiretime(const char* key, unsigned long long seconds)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::setexpiretime(const char* key, unsigned long long seconds)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::setexpiretime(const char* key, unsigned long long seconds)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
#ifdef WIN32
		this->_reply = (redisReply*)redisCommand(this->_connect, "expire %s %I64d", key, seconds);
#elif defined LINUX
		this->_reply = (redisReply*)redisCommand(this->_connect, "expire %s %lld", key, seconds);
#endif
		
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			if (this->_reply->integer == 1)
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool  CSRedis::setpexpiretime(const char* key, unsigned long long milliseconds)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::setpexpiretime(const char* key, unsigned long long milliseconds)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::setpexpiretime(const char* key, unsigned long long milliseconds)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
#ifdef WIN32
		this->_reply = (redisReply*)redisCommand(this->_connect, "pexpire %s %I64d", key, milliseconds);
#elif defined LINUX
		this->_reply = (redisReply*)redisCommand(this->_connect, "pexpire %s %lld", key, milliseconds);
#endif

		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			if (this->_reply->integer == 1)
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool  CSRedis::persistkey(const char* key)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::persistkey(const char* key)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::persistkey(const char* key)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "persist %s", key);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			if (this->_reply->integer == 1)
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

long long  CSRedis::ttlkey(const char* key)
{
	long long llret = -3;
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::ttlkey(const char* key)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::ttlkey(const char* key)== m_bconnectok == false reconnect error !!!----->> \n");
			return llret;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "ttl %s", key);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			llret = this->_reply->integer;
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				freeReplyObject(this->_reply);
			}
		}
	}

	return llret;
}

long long  CSRedis::pttlkey(const char* key)
{
	long long llret = -3;
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::pttlkey(const char* key)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::pttlkey(const char* key)== m_bconnectok == false reconnect error !!!----->> \n");
			return llret;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "pttl %s", key);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			llret = this->_reply->integer;
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				freeReplyObject(this->_reply);
			}
		}
	}

	return llret;
}

char* CSRedis::getvalue(const char* key)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::getvalue(const char* key)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::getvalue(const char* key)== m_bconnectok == false reconnect error !!!----->> \n");
			return NULL;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "GET %s", key);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_STRING)
		{
			char* str = new char[this->_reply->len + 1];
			memset(str, 0, this->_reply->len + 1);
			memcpy(str, this->_reply->str, this->_reply->len);
			freeReplyObject(this->_reply);
			return str;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::getvalue(const char* key)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}

	}
	return NULL;
}

char* CSRedis::getvalue(const char* key, const char* number)		//��ȡָ��keyֵ
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::getvalue(const char* key, const char* number)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::getvalue(const char* key, const char* number)== m_bconnectok == false reconnect error !!!----->> \n");
			return NULL;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "ZSCORE %s %s", key, number);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_STRING)
		{
			char* str = new char[this->_reply->len + 1];
			memset(str, 0, this->_reply->len + 1);
			memcpy(str, this->_reply->str, this->_reply->len);
			freeReplyObject(this->_reply);
			return str;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::getvalue(const char* key, const char* number)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return NULL;
}

bool CSRedis::setvalue(const char* key, const char* value)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::setvalue(const char* key, const char* value)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::setvalue(const char* key, const char* value)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "SET %s %s", key, value);
		if (this->_reply != NULL && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			if (this->_reply->integer == 1)
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::setvalue(const char* key, const char* value)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool CSRedis::setvalue(const char* key, const char* score, const char* number)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::setvalue(const char* key, const char* score, const char* number)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::setvalue(const char* key, const char* score, const char* number)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "ZADD %s %s %s", key, score, number);
		if (this->_reply != NULL && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			if (this->_reply->integer == 1)
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::setvalue(const char* key, const char* score, const char* number)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool CSRedis::sethashvalue(const char* key, const char* score, const char* number)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::sethashvalue(const char* key, const char* score, const char* number)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::sethashvalue(const char* key, const char* score, const char* number)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "HSET %s %s %s", key, number, score);
		if (this->_reply != NULL && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			if (this->_reply->integer == 1) // ˵����key��number�򲻴��ڣ���ʱ�´�����Ϊ0��ʾnumber���Ѿ����ڣ���ʱ���Ǿɵ�score
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::sethashvalue==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

char* CSRedis::gethashvalue(const char* key, const char* number)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::gethashvalue(const char* key, const char* number)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::gethashvalue(const char* key, const char* number)== m_bconnectok == false reconnect error !!!----->> \n");
			return NULL;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "HGET %s %s", key, number);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_STRING)
		{
			char* str = new char[this->_reply->len + 1];
			memset(str, 0, this->_reply->len + 1);
			memcpy(str, this->_reply->str, this->_reply->len);
			freeReplyObject(this->_reply);

			//printf("=============== CSRedis::gethashvalue =====------------return str----------->>> %p\n", str);
			return str;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::gethashvalue==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return NULL;
}

bool CSRedis::deletevalue(const char* key)		//ɾ��ֵ
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::deletevalue(const char* key)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::deletevalue(const char* key)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "DEL %s", key);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			if (this->_reply->integer == 1) // ˵����key���ڣ�Ϊ0��ʾkey������
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::deletevalue(const char* key)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool CSRedis::deletevalue(const char* key, const char* number)		//ɾ��ֵ
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::deletevalue(const char* key, const char* number)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::deletevalue(const char* key, const char* number)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "ZREM %s %s", key, number);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			if (this->_reply->integer == 1)
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::deletevalue(const char* key, const char* number)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool CSRedis::deletehashvalue(const char* key, const char* number)		//ɾ��ֵ
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::deletehashvalue(const char* key, const char* number)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::deletehashvalue(const char* key, const char* number)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "hdel %s %s", key, number);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			if (this->_reply->integer == 1)
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::deletehashvalue==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

long long CSRedis::getdbnum(const char* key)		//��ȡָ�����ݿ�ؼ������ݳ���
{
	long long nLen = 0;
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::getdbnum(const char* key)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::getdbnum(const char* key)== m_bconnectok == false reconnect error !!!----->> \n");
			return nLen;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "ZCARD %s", key);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			nLen = this->_reply->integer;
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::getdbnum==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return nLen;
}

int CSRedis::getdbdata(const char* key, int start, int stop, CNetMpLoad** load, const int getvcnum)		//��ȡָ�����ݿ�ؼ�������
{
	int nLen = 0;
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::getdbdata(const char* key, int start, int stop, CNetMpLoad** load, const int getvcnum)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::getdbdata(const char* key, int start, int stop, CNetMpLoad** load, const int getvcnum)== m_bconnectok == false reconnect error !!!----->> \n");
			return 0;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "ZRANGE %s %d %d", key, start, stop);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_ARRAY)
		{
			nLen = this->_reply->elements;
			if (nLen > getvcnum
				&& getvcnum > 0)
			{
				nLen = getvcnum;
			}
			for (int i = 0; i < nLen; i++)
			{
				if (this->_reply->element[i] != NULL && this->_reply->element[i]->type == REDIS_REPLY_STRING)
				{
					//load[i]->token = new char[this->_reply->element[i]->len + 1];
					//memset(load[i]->token, 0, this->_reply->element[i]->len + 1);
					//memcpy(load[i]->token, this->_reply->element[i]->str, this->_reply->element[i]->len);
					load[i]->deviceid = atoi(this->_reply->element[i]->str);

					this->_reply1 = (redisReply*)redisCommand(this->_connect, "ZSCORE %s %s", key, this->_reply->element[i]->str);
					if (this->_reply1 != NULL  && this->_reply1->type == REDIS_REPLY_STRING)
					{
						//load[i]->load = atoi(this->_reply1->str);

#ifdef WIN32
						load[i]->load = _atoi64(this->_reply1->str);
#elif defined LINUX
						load[i]->load = strtoll(this->_reply1->str, NULL, 10);
#endif

						freeReplyObject(this->_reply1);
					}
					else
					{
						load[i]->load = 0;
						freeReplyObject(this->_reply);
						return 0;
					}
				}
				else
				{
					freeReplyObject(this->_reply);
					return 0;
				}
			}
			freeReplyObject(this->_reply);
			return nLen;
		}
		else
		{
			if (this->_reply != NULL)
			{
				freeReplyObject(this->_reply);
			}
		}
	}

	return 0;
}

int CSRedis::getzsetdata(const char* key, int start, int stop, ValueScores** valuescores, const int getvcnum)//��ȡָ�����ݿ�ؼ���zset����
{
	int nLen = 0;
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::getzsetdata(const char* key, int start, int stop, ValueScores** valuescores, const int getvcnum)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::getzsetdata(const char* key, int start, int stop, ValueScores** valuescores, const int getvcnum)== m_bconnectok == false reconnect error !!!----->> \n");
			return 0;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "ZRANGE %s %d %d", key, start, stop);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_ARRAY)
		{
			nLen = this->_reply->elements;
			if (nLen > getvcnum
				&& getvcnum > 0)
			{
				nLen = getvcnum;
			}
			for (int i = 0; i < nLen; i++)
			{
				if (this->_reply->element[i] != NULL && this->_reply->element[i]->type == REDIS_REPLY_STRING)
				{
					memset(valuescores[i]->value, 0, 256);
					memcpy(valuescores[i]->value, this->_reply->element[i]->str, this->_reply->element[i]->len);

					this->_reply1 = (redisReply*)redisCommand(this->_connect, "ZSCORE %s %s", key, this->_reply->element[i]->str);
					if (this->_reply1 != NULL  && this->_reply1->type == REDIS_REPLY_STRING)
					{
						memset(valuescores[i]->score, 0, 256);
						memcpy(valuescores[i]->score, this->_reply1->str, this->_reply1->len);

						freeReplyObject(this->_reply1);
					}
					else
					{
						memset(valuescores[i]->score, 0, 256);

						freeReplyObject(this->_reply);
						return 0;
					}
				}
				else
				{
					freeReplyObject(this->_reply);
					return 0;
				}
			}
			freeReplyObject(this->_reply);
			return nLen;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("==1==CSRedis::getzsetdata==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return 0;
}

int CSRedis::getdbdata(const char* key, char* id, unsigned long long& load)		//��ȡָ�����ݿ�ؼ�������
{
	int nLen = 0;

	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::getdbdata(const char* key, char* id, unsigned long long& load)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::getdbdata(const char* key, char* id, unsigned long long& load)== m_bconnectok == false reconnect error !!!----->> \n");
			return nLen;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply1 = (redisReply*)redisCommand(this->_connect, "ZSCORE %s %s", key, id);
		if (this->_reply1 != NULL  && this->_reply1->type == REDIS_REPLY_STRING)
		{
			nLen = 1;
			//load = atoi(this->_reply1->str);

#ifdef WIN32
			load = _atoi64(this->_reply1->str);
#elif defined LINUX
			load = strtoll(this->_reply1->str, NULL, 10);
#endif

			freeReplyObject(this->_reply1);
		}
		else
		{
			if (this->_reply1 != NULL)
			{
				printf("====CSRedis::getdbdata(const char* key, char* id, unsigned long long& load)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply1);
			}
		}
	}
	
	return nLen;
}

int CSRedis::getdbdata(const char* key, CDeviceID** id, const int getvcnum)		//��ȡָ�����ݿ�ؼ�������
{
	int nLen = 0;

	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::getdbdata(const char* key, CDeviceID** id, const int getvcnum)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::getdbdata(const char* key, CDeviceID** id, const int getvcnum)== m_bconnectok == false reconnect error !!!----->> \n");
			return 0;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "ZRANGE %s %d %d", key, 0, -1);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_ARRAY)
		{
			nLen = this->_reply->elements;
			if (nLen > getvcnum
				&& getvcnum > 0)
			{
				nLen = getvcnum;
			}
			for (int i = 0; i < nLen; i++)
			{
				if (this->_reply->element[i] != NULL && this->_reply->element[i]->type == REDIS_REPLY_STRING)
				{
					//load[i]->token = new char[this->_reply->element[i]->len + 1];
					//memset(load[i]->token, 0, this->_reply->element[i]->len + 1);
					//memcpy(load[i]->token, this->_reply->element[i]->str, this->_reply->element[i]->len);
					id[i]->deviceid = atoi(this->_reply->element[i]->str);
				}
				else
				{
					freeReplyObject(this->_reply);
					return 0;
				}
			}
			freeReplyObject(this->_reply);
			return nLen;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::getdbdata(const char* key, CDeviceID** id, const int getvcnum)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return 0;
}

long long CSRedis::gethashfiledsnum(const char* key)		//��ȡָ�����ݿ�ؼ������ݳ���
{
	long long nLen = 0;

	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::gethashfiledsnum(const char* key)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::gethashfiledsnum(const char* key)== m_bconnectok == false reconnect error !!!----->> \n");
			return nLen;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "hlen %s", key);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			nLen = this->_reply->integer;
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::gethashfiledsnum(const char* key)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return nLen;
}

int CSRedis::gethashdbdata(const char* key, CHKeys** fileds, const int filedsnum)		//��ȡָ�����ݿ�ؼ�������
{
	int nLen = 0;

	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::gethashdbdata(const char* key, CHKeys** fileds, const int filedsnum)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::gethashdbdata(const char* key, CHKeys** fileds, const int filedsnum)== m_bconnectok == false reconnect error !!!----->> \n");
			return nLen;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "hkeys %s", key);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_ARRAY)
		{
			nLen = this->_reply->elements;
			if (nLen > filedsnum
				&& filedsnum > 0)
			{
				nLen = filedsnum;
			}

			for (int i = 0; i < nLen; i++)
			{
				if (this->_reply->element[i] != NULL && this->_reply->element[i]->type == REDIS_REPLY_STRING)
				{
					memset(fileds[i]->filed, 0, 256);
					memcpy(fileds[i]->filed, this->_reply->element[i]->str, this->_reply->element[i]->len);
				}
				else
				{
					freeReplyObject(this->_reply); // �˳�ʱ�ǵ��ͷ��ڴ�
					return 0;
				}
			}
			freeReplyObject(this->_reply);
			return nLen;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::gethashdbdata(const char* key, CHKeys** fileds, const int filedsnum)==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return 0;
}

bool CSRedis::listLPush(const char* key, const char* value)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::listLPush(const char* key, const char* value)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::listLPush(const char* key, const char* value)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "LPUSH %s %s", key, value);
		if (this->_reply != NULL && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			//printf("====== CSRedis::listLPush =====3-----_reply--type--> %d,len--> %lld\n", this->_reply->type, this->_reply->integer);
			if (this->_reply->integer > 0)//push�󷵻ض���Ԫ�صĸ���
			{
				freeReplyObject(this->_reply);
				return true;
			}
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				//printf("====== CSRedis::listLPush =====1-----_reply--type--> %d,len--> %lld\n", this->_reply->type, this->_reply->integer);
				printf("====CSRedis::listLPush==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
			//else
			//{
			//	printf("====== CSRedis::listLPush =====error-----_reply--type == NULL\n");
			//}
		}
	}

	return false;
}

char* CSRedis::listRPop(const char* key)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::listRPop(const char* key)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::listRPop(const char* key)== m_bconnectok == false reconnect error !!!----->> \n");
			return NULL;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "RPOP %s", key);
		if (this->_reply != NULL && this->_reply->type == REDIS_REPLY_STRING)
		{
			//printf("====== CSRedis::listRPop =====1-----_reply--type--> %d,len--> %d\n", this->_reply->type, this->_reply->len);

			char* str = new char[this->_reply->len + 1];
			memset(str, 0, this->_reply->len + 1);
			memcpy(str, this->_reply->str, this->_reply->len);
			freeReplyObject(this->_reply);
			return str;
		}
		else /*if (this->_reply != NULL && this->_reply->type == REDIS_REPLY_NIL)*/
		{
			if (this->_reply != NULL)
			{
				printf("====== CSRedis::listRPop =====4-----_reply--type--> %d,len--> %d\n", this->_reply->type, this->_reply->len);
				freeReplyObject(this->_reply);
			}
		}
	}
	return NULL;
}

long long CSRedis::listLLen(const char* key)
{
	long long nLen = 0;

	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::listLLen(const char* key)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::listLLen(const char* key)== m_bconnectok == false reconnect error !!!----->> \n");
			return nLen;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "LLEN %s", key);

		//printf("CSRedis::listLLen Error:[%d, %s], this->_reply=0x%x\n", this->_connect->err, this->_connect->errstr, this->_reply);

		//if (this->_reply == NULL
		//	&& this->_connect->err != 0)
		//{
		//	m_bconnectok = false;
		//}

		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_INTEGER)
		{
			nLen = this->_reply->integer;
			freeReplyObject(this->_reply);
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====CSRedis::listLLen==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return nLen;
}

bool CSRedis::command_array(RedisReplyArray& rArray, const char* cmd, ...)
//bool CSRedis::command_array(RedisReplyArray& rArray, const char* cmd)
{
	bool bRet = false;

	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::command_array(RedisReplyArray& rArray, const char* cmd, ...)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::command_array(RedisReplyArray& rArray, const char* cmd, ...)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		va_list args;
		va_start(args, cmd);
		this->_reply = (redisReply*)redisCommand(this->_connect, cmd, args);
		va_end(args);

		//this->_reply = (redisReply*)redisCommand(this->_connect, cmd, args);

		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_ARRAY)
		{
			size_t nLen = this->_reply->elements;

			printf("==1==command_array==this->_reply->type==-->>> %d, nLen =%d\n", this->_reply->type, nLen);

			for (size_t i = 0; i < nLen; i++)
			{
				printf("=1=command_array==this->_reply->elements[%d]==-->> type:%d, str:%s, len:%d\n", i, this->_reply->element[i]->type, this->_reply->element[i]->str, this->_reply->element[i]->len);

				if (this->_reply->element[i] != NULL && this->_reply->element[i]->type == REDIS_REPLY_STRING)
				{
					RedisDataItem item;
					item.type = this->_reply->element[i]->type;
					item.str.assign(this->_reply->element[i]->str, this->_reply->element[i]->len);
					//item.str = (NULL == reply->element[i]->str) ? string("") : reply->element[i]->str;
					rArray.push_back(item);
				}
				else
				{
					continue;
				}

			}

			freeReplyObject(this->_reply);
			return true;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("==2==command_array==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);

				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool CSRedis::commandargv_status(const REDISVDATA& vData)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::commandargv_status(const REDISVDATA& vData)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::commandargv_status(const REDISVDATA& vData)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		vector<const char *> argv(vData.size()); //argv���� vData.size()��const char *��������
		vector<size_t> argvlen(vData.size()); //argvlen���� vData.size()��size_t��������
		unsigned int j = 0;
		for (REDISVDATA::const_iterator iter = vData.begin(); iter != vData.end(); ++iter, ++j)
		{
			argv[j] = iter->c_str(), argvlen[j] = iter->size();
		}

		this->_reply = (redisReply*)redisCommandArgv(this->_connect, argv.size(), &(argv[0]), &(argvlen[0]));

#ifdef WIN32
		if (this->_reply != NULL
			&& this->_reply->type == REDIS_REPLY_STATUS
			&& stricmp(this->_reply->str, "OK") == 0)
#elif defined LINUX
		if (this->_reply != NULL
			&& this->_reply->type == REDIS_REPLY_STATUS
			&& strcasecmp(this->_reply->str, "OK") == 0)
#endif
		{
			//printf("==commandargv_status==this->_reply->type==-->>> %d:%s\n", this->_reply->type, this->_reply->str);

			freeReplyObject(this->_reply);

			return true;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("====commandargv_status==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);
				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool CSRedis::commandargv_array(const REDISVDATA& vDataIn, RedisReplyArray& rArray)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::commandargv_array(const REDISVDATA& vDataIn, RedisReplyArray& rArray)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::commandargv_array(const REDISVDATA& vDataIn, RedisReplyArray& rArray)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		vector<const char*> argv(vDataIn.size());//argv���� vData.size()��const char *��������
		vector<size_t> argvlen(vDataIn.size()); //argvlen���� vData.size()��size_t��������
		unsigned int j = 0;
		for (REDISVDATA::const_iterator iter = vDataIn.begin(); iter != vDataIn.end(); ++iter, ++j)
		{
			argv[j] = iter->c_str(), argvlen[j] = iter->size();
		}

		this->_reply = (redisReply*)redisCommandArgv(this->_connect, argv.size(), &(argv[0]), &(argvlen[0]));

		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_ARRAY)
		{
			//printf("=1=commandargv_array==this->_reply->type==-->>> %d\n", this->_reply->type);

			size_t nLen = this->_reply->elements;

			for (size_t i = 0; i < nLen; i++)
			{
				//printf("=1=commandargv_array==this->_reply->elements[%d]==-->> type:%d, str:%s, len:%d\n", i, this->_reply->element[i]->type, this->_reply->element[i]->str, this->_reply->element[i]->len);

				if (this->_reply->element[i] != NULL 
					&& (this->_reply->element[i]->type == REDIS_REPLY_STRING
					|| this->_reply->element[i]->type == REDIS_REPLY_NIL))
				{
					RedisDataItem item;
					item.type = this->_reply->element[i]->type;
					item.str.assign(this->_reply->element[i]->str, this->_reply->element[i]->len);
					//item.str = (NULL == reply->element[i]->str) ? string("") : reply->element[i]->str;
					rArray.push_back(item);
				}
				else
				{
					continue;
				}

			}

			freeReplyObject(this->_reply);
			return true;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("==2==commandargv_array==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);

				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool CSRedis::commandargv_pair(const REDISVDATA& vDataIn, RedisReplyKeyValuePair& mPair)
{
	bool bres = false;
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::commandargv_pair(const REDISVDATA& vDataIn, RedisReplyKeyValuePair& mPair)== m_bconnectok == false so reconnect redis----->>\n");

		bool bconn = connect(_host.c_str(), _port);
		if (!bconn)
		{
			printf(">>>>>>>CSRedis::commandargv_pair(const REDISVDATA& vDataIn, RedisReplyKeyValuePair& mPair)== m_bconnectok == false reconnect error !!!----->> \n");
			return bres;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	// 2��ָ�������ƺͱ���
	unsigned int uiInputSize = vDataIn.size();
	if (uiInputSize <= 2)
	{
		printf("xxxxxxxx CSRedis::commandargv_pair(const REDISVDATA& vDataIn, RedisReplyKeyValuePair& mPair)== input data size(%d) error !!!----->> \n", uiInputSize);
		return bres;
	}

	if (this->_connect != NULL)
	{
		vector<const char*> argv(uiInputSize);//argv���� vData.size()��const char *��������
		vector<size_t> argvlen(uiInputSize); //argvlen���� vData.size()��size_t��������
		unsigned int j = 0;
		for (REDISVDATA::const_iterator iter = vDataIn.begin(); iter != vDataIn.end(); ++iter, ++j)
		{
			argv[j] = iter->c_str(), argvlen[j] = iter->size();
		}

		this->_reply = (redisReply*)redisCommandArgv(this->_connect, argv.size(), &(argv[0]), &(argvlen[0]));

		if (this->_reply != NULL && this->_reply->type == REDIS_REPLY_ARRAY)
		{
			//printf("=1=commandargv_pair==this->_reply->type==-->>> %d, this->_reply->elements==-->>> %d\n", this->_reply->type, this->_reply->elements);//2

			size_t nLen = this->_reply->elements;
			if (nLen == (uiInputSize - 2))// ��2���ٳ�ָ�������ƺͱ���
			{
				bres = true;

				for (size_t i = 0; i < nLen; i++)
				{
					//printf("=1=commandargv_pair==this->_reply->elements[%d]==-->> type:%d, str:%s, len:%d\n", i, this->_reply->element[i]->type, this->_reply->element[i]->str, this->_reply->element[i]->len);

					if (this->_reply->element[i] != NULL 
						&& (this->_reply->element[i]->type == REDIS_REPLY_STRING
						|| this->_reply->element[i]->type == REDIS_REPLY_NIL))
					{
						RedisDataItem item;
						item.type = this->_reply->element[i]->type;
						item.str.assign(this->_reply->element[i]->str, this->_reply->element[i]->len);

						mPair.insert(std::make_pair(vDataIn[i + 2], item));// ǰ�����ַ������������ƺͱ���
					}
					else
					{
						continue;
					}
				}
			}
			else
			{
				//printf("=3=commandargv_pair==nLen==-->>> %d, vDataIn.size()=%d\n", nLen, vDataIn.size());
				bres = false;
			}

			freeReplyObject(this->_reply);
			return bres;
		}
		else
		{
			if (this->_reply != NULL)
			{
				//printf("==2==commandargv_pair==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type, this->_reply->str);

				freeReplyObject(this->_reply);
			}
		}
	}

	return false;
}

bool CSRedis::hashHMSet(const string& key, const REDISVDATA& vData)
{
	//CCriticalAutoLock myAutoLock(m_csRedis);

	REDISVDATA vCmdData;
	vCmdData.clear();
	vCmdData.push_back("HMSET");
	vCmdData.push_back(key);
	addparam(vCmdData, vData);
	return commandargv_status(vCmdData);
}

bool CSRedis::hashHMGet(const string& key, const REDISVKEYS& fileds, RedisReplyArray& rArray)
{
	//CCriticalAutoLock myAutoLock(m_csRedis);

	REDISVDATA vCmdData;
	vCmdData.clear();
	vCmdData.push_back("HMGET");
	vCmdData.push_back(key);
	addparam(vCmdData, fileds);
	return commandargv_array(vCmdData, rArray);
}

bool CSRedis::hashHMGet(const string& key, const REDISVKEYS& fileds, RedisReplyKeyValuePair& mPair)
{
	REDISVDATA vCmdData;
	vCmdData.clear();
	vCmdData.push_back("HMGET");
	vCmdData.push_back(key);
	addparam(vCmdData, fileds);
	return commandargv_pair(vCmdData, mPair);
}

//bool CSRedis::hashHGetAll(const string& key, RedisReplyArray& rArray)
//{
//	//CCriticalAutoLock myAutoLock(m_csRedis);
//
//	return command_array(rArray, "HGETALL %s", key.c_str());
//}

bool CSRedis::hashHGetAll(const char* key, RedisReplyArray& rArray)
{
	if (!m_bconnectok)
	{
		printf(">>>>>>>CSRedis::hashHGetAll(const char* key, RedisReplyArray& rArray)== m_bconnectok == false so reconnect redis----->>\n");

		bool bres = connect(_host.c_str(), _port);
		if (!bres)
		{
			printf(">>>>>>>CSRedis::hashHGetAll(const char* key, RedisReplyArray& rArray)== m_bconnectok == false reconnect error !!!----->> \n");
			return false;
		}
		else
		{
			if (_password.length() > 0)
			{
				// ��������������֤
				auth(_password.c_str());
			}
		}
	}

	if (this->_connect != NULL)
	{
		size_t nLen = 0;
		this->_reply = (redisReply*)redisCommand(this->_connect, "HGETALL %s", key);
		if (this->_reply != NULL  && this->_reply->type == REDIS_REPLY_ARRAY)
		{
			nLen = this->_reply->elements;

			//printf("==1==hashHGetAll==this->_reply->type==-->>> %d, nLen =%d\n", this->_reply->type, nLen);

			for (size_t i = 0; i < nLen; i++)
			{
				if (this->_reply->element[i] != NULL && this->_reply->element[i]->type == REDIS_REPLY_STRING)
				{
					RedisDataItem item;
					item.type = this->_reply->element[i]->type;
					item.str.assign(this->_reply->element[i]->str, this->_reply->element[i]->len);
					//item.str = (NULL == reply->element[i]->str) ? string("") : reply->element[i]->str;
					rArray.push_back(item);
				}
				else
				{
					continue;
				}
			}

			freeReplyObject(this->_reply);
			return true;
		}
		else
		{
			if (this->_reply != NULL)
			{
				printf("==2==hashHGetAll==this->_reply->type=->>> %d,this->_reply->str=->>>%s\n", this->_reply->type,this->_reply->str);

				freeReplyObject(this->_reply);
			}
		}
	}
	
	return false;
}