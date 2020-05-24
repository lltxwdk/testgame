// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SREDISDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SREDISDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef __SR_REDISDLL_I_H__
#define __SR_REDISDLL_I_H__

#include <vector>
#include <set>
#include <string.h>
#include <string>
#include <map>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LINUX
#define __stdcall
#endif

#ifdef  SREDISDLL_EXPORTS
#define SREDISDLL_API     __declspec(dllexport)
#else
#define SREDISDLL_API
#endif

// This class is exported from the sredisdll.dll
	namespace ISREDIS
	{		
		typedef struct CNETMPLOAD
		{
			int deviceid;
			unsigned long long load;
		}CNetMpLoad;

		typedef struct CVALUESCORE
		{
			char value[256];
			char score[256];
		}ValueScores;

		typedef struct CHKEYS
		{
			char filed[256];
		}CHKeys;

		typedef struct CDEVICEID
		{
			int deviceid;
		}CDeviceID;

		//串口观察者接口
		class ICsredisSink
		{
		public:
		};

		typedef struct REDIS_DATA_ITEM_
		{
			int type;
			std::string str;

			REDIS_DATA_ITEM_& operator=(const REDIS_DATA_ITEM_ &data) 
			{
				type = data.type;
				str = data.str;
				return *this;
			}

		}RedisDataItem;

		typedef std::vector<RedisDataItem>  RedisReplyData;
		typedef RedisReplyData              RedisReplyArray;

		typedef std::string              REDISKEY;
		typedef std::string              REDISVALUE;
		typedef std::vector<REDISKEY>    REDISVKEYS;
		typedef REDISVKEYS	             REDISFILEDS;
		typedef std::vector<REDISVALUE>  REDISVALUES;
		typedef std::vector<std::string> REDISVDATA;
		typedef std::set<std::string>	 REDISSETDATA;
		typedef std::map<std::string, RedisDataItem> RedisReplyKeyValuePair;

		class ICsredis {
		public:
			// TODO: add your methods here.
			virtual bool connect(const char* host, int port = 6379) = 0;
			virtual bool auth(const char* password) = 0;
			virtual bool isconnectok() = 0;
			virtual void disconnect() = 0;
			virtual void flushall() = 0;//清空所有库
			virtual bool pingpong() = 0;//探测连接是否正常
			virtual bool selectdb(const int dbnum) = 0;//切换数据库
			virtual bool existskey(const char* key) = 0;//判断key是否存在

			virtual bool setexpiretime(const char* key, unsigned long long seconds) = 0;//设置失效时间(秒),设置成功返回 1,当key不存在或者不能为key设置生存时间时返回 0 。
			virtual bool setpexpiretime(const char* key, unsigned long long milliseconds) = 0;//设置失效时间(毫秒),设置成功，返回 1,key 不存在或设置失败，返回 0

			virtual bool persistkey(const char* key) = 0;//取消失效时间(即持久的,永不过期的),当生存时间移除成功时，返回 1 .如果 key 不存在或 key 没有设置生存时间，返回 0 。

			virtual long long ttlkey(const char* key) = 0;//以秒为单位，返回给定 key 的剩余生存时间(TTL, time to live),当 key 不存在时，返回 -2 。当 key 存在但没有设置剩余生存时间时，返回 -1 。程序内部错误,返回 -3
			virtual long long pttlkey(const char* key) = 0;//以毫秒为单位，返回给定 key 的剩余生存时间(TTL, time to live),当 key 不存在时，返回 -2 。当 key 存在但没有设置剩余生存时间时，返回 -1 。程序内部错误,返回 -3
			

			virtual char* getvalue(const char* key) = 0;//获取指定key值
			virtual char* getvalue(const char* key, const char* number) = 0;//获取指定key值
			virtual bool  setvalue(const char* key, const char* value) = 0;	//设置指定key值
			virtual bool  setvalue(const char* key, const char* score, const char* number) = 0; //设定key sort set值
			virtual bool  sethashvalue(const char* key, const char* score, const char* number) = 0; //设定key sort set值
			virtual char*  gethashvalue(const char* key, const char* score) = 0;
			
			virtual bool  deletevalue(const char* key) = 0;		//删除值
			virtual bool  deletevalue(const char* key, const char* number) = 0;		//删除值
			virtual bool  deletehashvalue(const char* key, const char* number) = 0;		//删除值
			virtual long long getdbnum(const char* key) = 0;		//获取指定数据库关键词内容长度
			virtual int	  getdbdata(const char* key, int start, int stop, CNetMpLoad** load, const int getvcnum) = 0;		//获取指定数据库关键词内容
			virtual int	  getzsetdata(const char* key, int start, int stop, ValueScores** valuescores, const int getvcnum) = 0;	//获取指定数据库关键词zset内容
			virtual int   getdbdata(const char* key, char* id, unsigned long long& load) = 0;		//获取指定数据库关键词内容
			virtual int	  getdbdata(const char* key, CDeviceID** id, const int getvcnum) = 0;		//获取指定数据库关键词内容
			virtual long long gethashfiledsnum(const char* key) = 0;		//获取指定数据库关键词内容长度
			virtual int   gethashdbdata(const char* key, CHKeys** fileds, const int filedsnum) = 0;		//获取指定数据库关键词内容

			virtual bool  listLPush(const char* key, const char* value) = 0;//
			virtual char* listRPop(const char* key) = 0;
			virtual long long listLLen(const char* key) = 0;

			virtual bool hashHMSet(const string& key, const REDISVDATA& vData) = 0; // HMSET
			virtual bool hashHMGet(const string& key, const REDISFILEDS& fileds, RedisReplyArray& rArray) = 0; // HMGET
			virtual bool hashHMGet(const string& key, const REDISFILEDS& fileds, RedisReplyKeyValuePair& mPair) = 0; // HMGET
			//virtual bool hashHGetAll(const string& key, RedisReplyArray& rArray) = 0; // HGETALL
			virtual bool hashHGetAll(const char* key, RedisReplyArray& rArray) = 0;
						
			virtual ~ICsredis(){}

		};

		/**
		* @function			CreateIRedis
		* @abstract			创建对象
		* @param				
		* @pSink			
		* @return		    ISREDIS*
		*/
		SREDISDLL_API ICsredis* CreateIRedis(ICsredisSink* pcSink);

		/**
		* @function			CreateRedisInstanceList
		* @abstract			创建对象
		* @param
		* @pSink
		* @return		    ISREDIS*
		*/
		SREDISDLL_API ICsredis** CreateRedisInstanceList(ICsredisSink* pcSink, unsigned int uiRedisConnNum);

		/**
		* @function			DeleteIRedis
		* @abstract			删除对象
		* @param				
		* @return		    无
		*/
		SREDISDLL_API void DeleteIRedis(ICsredis* pIRedis);

		/**
		* @function			DeleteRedisInstanceList
		* @abstract			删除对象
		* @param
		* @return		    无
		*/
		SREDISDLL_API void DeleteRedisInstanceList(ICsredis** pIRedis, unsigned int uiRedisConnNum);

	}
	
#ifdef __cplusplus
}
#endif

#endif //__SR_REDISDLL_I_H__

