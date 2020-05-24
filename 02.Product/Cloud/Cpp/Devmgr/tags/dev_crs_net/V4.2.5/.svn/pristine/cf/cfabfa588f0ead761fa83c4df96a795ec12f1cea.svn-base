
#ifndef _SR_SREDIS_H
#define _SR_SREDIS_H

#include "sredisdll.h"
#if defined WIN32
#include "..\redis-3.0\deps\hiredis\hiredis.h"
#elif defined LINUX
#include "hiredis.h"
#endif
using namespace ISREDIS;

class CSRedis:public ICsredis
{
public:

	CSRedis(ICsredisSink* pSink);

	virtual ~CSRedis(void);

public:

	virtual bool connect(const char* host, int port = 6379);
	virtual bool isconnectok();
	virtual void disconnect();
	virtual void flushall();//清空所有库
	virtual bool pingpong(); //探测连接是否正常
	virtual bool selectdb(const int dbnum);//切换数据库
	virtual bool existskey(const char* key);//判断key是否存在

	virtual bool setexpiretime(const char* key, unsigned long long seconds);//设置失效时间(秒),设置成功返回 1,当key不存在或者不能为key设置生存时间时返回 0 。
	virtual bool setpexpiretime(const char* key, unsigned long long milliseconds);//设置失效时间(毫秒),设置成功，返回 1,key 不存在或设置失败，返回 0

	virtual bool persistkey(const char* key);//取消失效时间(即持久的,永不过期的),当生存时间移除成功时，返回 1 .如果 key 不存在或 key 没有设置生存时间，返回 0 。

	virtual long long ttlkey(const char* key);//以秒为单位，返回给定 key 的剩余生存时间(TTL, time to live),当 key 不存在时，返回 -2 。当 key 存在但没有设置剩余生存时间时，返回 -1 。程序内部错误,返回 -3
	virtual long long pttlkey(const char* key);//以毫秒为单位，返回给定 key 的剩余生存时间(TTL, time to live),当 key 不存在时，返回 -2 。当 key 存在但没有设置剩余生存时间时，返回 -1 。程序内部错误,返回 -3


	virtual char* getvalue(const char* key);
	virtual char* getvalue(const char* key, const char* number);//获取指定key值
	virtual bool  setvalue(const char* key, const char* value);
	virtual bool  setvalue(const char* key, const char* score, const char* number);
	virtual bool  sethashvalue(const char* key, const char* score, const char* number);
	virtual char*  gethashvalue(const char* key, const char* score);
	
	virtual bool  deletevalue(const char* key);		//删除值
	virtual bool  deletevalue(const char* key, const char* number);		//删除值
	virtual bool  deletehashvalue(const char* key, const char* number);		//删除值
	virtual long long getdbnum(const char* key);		//获取指定数据库关键词内容长度
	virtual int	  getdbdata(const char* key, int start, int stop, CNetMpLoad** load, const int getvcnum);		//获取指定数据库关键词内容
	virtual int	  getzsetdata(const char* key, int start, int stop, ValueScores** valuescores, const int getvcnum);	//获取指定数据库关键词zset内容
	virtual int   getdbdata(const char* key, char* id, unsigned long long& load);		//获取指定数据库关键词内容
	virtual int	  getdbdata(const char* key, CDeviceID** id, const int getvcnum);		//获取指定数据库关键词内容
	virtual long long gethashfiledsnum(const char* key);		//获取指定数据库关键词内容长度
	virtual int   gethashdbdata(const char* key, CHKeys** fileds, const int getfiledsnum);		//获取指定数据库关键词内容

	virtual bool  listLPush(const char* key, const char* value);//
	virtual char* listRPop(const char* key);//
	virtual long long listLLen(const char* key);

	virtual bool hashHMSet(const string& key, const REDISVDATA& vData);
	virtual bool hashHMGet(const string& key, const REDISVKEYS& fileds, RedisReplyArray& rArray);
	virtual bool hashHMGet(const string& key, const REDISFILEDS& fileds, RedisReplyKeyValuePair& mPair);
	//virtual bool hashHGetAll(const string& key, RedisReplyArray& rArray);
	virtual bool hashHGetAll(const char* key, RedisReplyArray& rArray);

protected:

	bool Init();
	void Fini();

private:

	std::string _host;
	int _port;

	bool m_bconnectok;
	unsigned long long m_ulldisconnetcnt;

	redisContext* _connect;  
	redisReply* _reply;  
	redisReply* _reply1;

	void addparam(REDISVDATA& vDes, const REDISVDATA& vSrc)
	{
		for (REDISVDATA::const_iterator iter = vSrc.begin(); iter != vSrc.end(); ++iter)
		{
			vDes.push_back(*iter);
		}
	}

	bool command_array(RedisReplyArray& rArray, const char* cmd, ...);
	//bool command_array(RedisReplyArray& rArray, const char* cmd);

	bool commandargv_status(const REDISVDATA& vData);
	bool commandargv_array(const REDISVDATA& vDataIn, RedisReplyArray& rArray);
	bool commandargv_pair(const REDISVDATA& vDataIn, RedisReplyKeyValuePair& mPair);

};

#endif