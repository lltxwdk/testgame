/***********************************************************
*   文件名: configuration.h
*     版权: 武汉随锐亿山 2015-2020
*     作者: Marvin Zhai
* 文件说明: configuration类的实现
* 创建日期: 2015.05.04
* 修改日期: 2015.08.22
*   修改人:
* 修改内容: 1.查找时map的时候，使用迭代器。避免多次查找(同时使用count和find)
2.返回str的默认值时，使用strdup。统一返回值的类型(使用后free)
3.修改返回str类型为std::string
*************************************************************/

#ifndef SRMC_CONFIGURATION_H
#define SRMC_CONFIGURATION_H

#include <map>
#include <string>
#include "sr_type.h"


namespace SRMC{
	/***************************************************************************
	INI配置文件解析类
	1)格式,一行开头使用";"表示注释(前面可以有空格,制表符).每个section中的key唯一
	[section]
	key=value
	2)目前value支持的类型 int,long long int,char*,bool
	3)弱错误检测:getXXXValue在任何情况下均有返回值,若有重复section或key,则后续的无效
	4)所有操作无阻塞
	***************************************************************************/
	class Configuration
	{
	public:
		explicit Configuration(const SR_char* filename);
		~Configuration()
		{
			m_sec_key_value.clear();
		}

		/* if no such section or key ,return defvalue */
		const std::string getStrValue(const SR_char* section, const SR_char* key, const SR_char* defvalue = 0) const;
		SR_int32 getIntValue(const SR_char* section, const SR_char* key, SR_int32 defvalue = 0) const;
		SR_int64 getInt64Value(const SR_char* section, const SR_char* key, SR_int64 defvalue = 0) const;
		SR_bool getBoolValue(const SR_char* section, const SR_char* key, SR_bool defvalue = false) const;
		std::string getFileName() const { return m_filename; }
	private:
		Configuration(){}
		Configuration(const Configuration&){}
		std::string	m_filename;
		/* string 内置的比较函数可保证key的唯一性 */
		std::map<std::string, std::string> m_sec_key_value;
	};

	class MCCfgInfo
	{
	public:
		static MCCfgInfo* instance();
		bool Parser(bool needreloadall = true);
		std::map<SR_uint32, std::string> m_checknetmp_regsrcipmap;
		std::map<SR_uint32, std::string> m_replacenetmp_regsrcipmap;
		std::string m_checknetmp_regsrcips;//检测netmp注册地址
		std::string m_replacenetmp_regsrcip;//替换netmp的地址
		SR_void 	set_mctoken(const std::string& mctoken){ m_mctoken.assign(mctoken); }
		SR_void     set_mcdeviceid(SR_uint32 mcdeviceid) { m_mcdeviceid = mcdeviceid; }
		SR_void     set_mcgroupid(SR_uint32 mcgroupid) { m_mcgroupid = mcgroupid; }
		SR_void 	set_mcdomainname(const std::string& mcdomainname){ m_mcdomainname.assign(mcdomainname); }
		const std::string& get_fileName() const { return m_filename; }
		std::string get_listenip() const { return m_listenip; }
		SR_uint32   get_listenport() const { return m_listenport; }
		SR_uint32   get_nettype() const { return m_nettype; }
		std::string get_devmgrip() const { return m_devmgrip; }
		SR_uint32   get_devmgrport() const { return m_devmgrport; }
		SR_uint64   get_detect_tertime() const{ return m_detect_tertime; }//检测终端心跳间隔
		SR_uint32   get_detecttimer_precision() const {return m_detecttimer_precision;}
		SR_uint64   get_detect_netmptime() const{ return m_detect_tertime; }
		SR_uint64   get_beatto_ter_time() const{ return m_beatto_ter_time; }
		SR_uint64   get_beatto_netmp_time() const{ return m_beatto_netmp_time; }
		SR_uint64   get_beatto_devmgr_time() const{ return m_beatto_devmgr_time; }
		SR_uint64   get_update_syscurload_time() const{ return m_update_syscurload_time; }
		SR_uint64   get_update_netmpinfo_time() const { return m_update_netmpinfo_time; }
		SR_uint32   get_netmp_highload() const{ return m_netmp_highload; }
		SR_uint32   get_netmp_lowload() const{ return m_netmp_lowload; }
		SR_uint32   get_max_connected_with_netmp() const{ return m_max_connected_with_netmp; }
		SR_uint32   get_idle_netmp_nums() const{ return m_idle_netmp_nums; }
		SR_uint32   get_maxters_in_netmp() const{ return m_maxters_in_netmp; }
		std::string get_register_password() const{ return m_register_password; }
		SR_uint32   get_mc_svr_type() const{ return m_mc_svr_type; }
		std::string get_mctoken() const{ return m_mctoken; }
		SR_uint32   get_mcdeviceid() const{ return m_mcdeviceid; }
		SR_uint32   get_mcgroupid() const{ return m_mcgroupid; }
		std::string get_mcdomainname() const{ return m_mcdomainname; }
		SR_uint64   get_waitdevmgrconfinfotime() const { return m_waitdevmgrtimeout; }
		SR_uint64   get_waitnetmpcreateconftime() const { return m_waitnetmptimeout; }
		SR_uint64   get_keep_conf_live() const { return m_keep_conf_live; }
		SR_uint64   get_reqsecondvedio_timeout() const { return m_reqsecondvedio_timeout; }
		SR_uint64   get_reconnect_devmgr_time() const { return m_reconnect_devmgr_time; }
		SR_uint32   get_reconnect_devmgr_loopcnt() const { return m_reconnect_devmgr_loopcnt; }
		SR_uint64   get_check_devmgr_time() const {return m_check_devmgr_timer;}
		SR_uint32   get_use_videomixer() const { return m_usevideomixer; }
		SR_uint32   get_stdterselectvideomode() const { return m_stdterselectvideomode; }
		SR_uint32   get_single_netmp_mp() const { return m_single_netmp_mp; }
		SR_uint32   get_callinvite() const { return m_callinvite; }
		SR_uint32   get_sup_confrecsvr() const { return m_supportconfrecsvr; }
		SR_uint64   get_keep_conf_rollcall() const { return m_keep_conf_rollcall; }
		SR_uint32   get_live_svrtype() const { return m_livesvrtype; }
		SR_uint32   get_loglevel() const { return m_nLoglevel; }
		SR_bool     get_isstdout() const { return m_bIsStdout; }
		std::string get_checknetmp_regsrcips()const { return m_checknetmp_regsrcips; }//检测netmp注册地址
		std::string get_replacenetmp_regsrcip() const { return m_replacenetmp_regsrcip; }//检测netmp注册地址
		SR_uint32   get_max_autopanne_nums() const { return m_max_autopanne_nums; }
		SR_uint32   get_max_assignpanne_nums() const { return m_max_assignpanne_nums; }
		std::string get_replacenetmp_regsrcipmapip(std::string IPdotdec);//检测netmp注册地址
		MCCfgInfo();
	private:
		static std::string m_filename;
		static std::string m_szFileUrl;
		static MCCfgInfo*  m_instance;
		static std::string m_listenip;
		static SR_uint32   m_listenport;
		static SR_uint32   m_nettype;
		static std::string m_devmgrip;
		static SR_uint32   m_devmgrport;
		static SR_uint64   m_detect_tertime;//检测终端心跳间隔
		static SR_uint64   m_detect_netmptime;
		static SR_uint64   m_beatto_ter_time;
		static SR_uint64   m_beatto_netmp_time;
		static SR_uint64   m_beatto_devmgr_time;
		static SR_uint64   m_update_syscurload_time;
		static SR_uint64   m_check_devmgr_timer;
		static SR_uint64   m_update_netmpinfo_time;
		static SR_uint32   m_netmp_highload;
		static SR_uint32   m_netmp_lowload;
		static SR_uint32   m_max_connected_with_netmp;
		static SR_uint32   m_idle_netmp_nums;
		static SR_uint32   m_maxters_in_netmp;
		static std::string m_register_password;
		static SR_uint32   m_mc_svr_type;
		static std::string m_mctoken;
		static SR_uint32   m_mcdeviceid;
		static SR_uint32   m_mcgroupid;
		static std::string m_mcdomainname;
		static SR_uint64   m_waitdevmgrtimeout;
		static SR_uint64   m_waitnetmptimeout;
		static SR_uint64   m_keep_conf_live;
		static SR_uint64   m_reqsecondvedio_timeout;
		static SR_uint64   m_reconnect_devmgr_time;
		static SR_uint32   m_reconnect_devmgr_loopcnt;
		static SR_uint32   m_detecttimer_precision;
		static SR_uint32   m_usevideomixer;
		static SR_uint32   m_stdterselectvideomode;
		static SR_uint32   m_single_netmp_mp;
		static SR_uint32   m_callinvite;
		static SR_uint32   m_supportconfrecsvr;
		static SR_uint64   m_keep_conf_rollcall;
		static SR_uint32   m_livesvrtype;//直播服务类型,默认0-SRS直播、1-阿里云直播、2-腾讯云直播
		// logconf
		static SR_uint32 m_nLoglevel;
		static SR_bool m_bIsStdout;
		static std::string m_replacenetmp_regsrcipmapip;
		static SR_uint32  	m_max_autopanne_nums;
		static SR_uint32  	m_max_assignpanne_nums;
	};

} // namespace SRMC
#endif // SRMC_CONFIGURATION_H