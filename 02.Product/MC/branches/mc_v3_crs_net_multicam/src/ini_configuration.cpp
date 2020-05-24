/***********************************************************
*   �ļ���: configuration.cpp
*     ��Ȩ: �人������ɽ 2015-2020
*     ����: Marvin Zhai
* �ļ�˵��: configuration���ʵ��
* ��������: 2015.05.04
* �޸�����: 2015.08.22
*   �޸���:
* �޸�����: 1.����ʱmap��ʱ��ʹ�õ������������β���(ͬʱʹ��count��find)
2.����str��Ĭ��ֵʱ��ʹ��strdup��ͳһ����ֵ������(ʹ�ú�free)
*************************************************************/
#include <cstdlib>
#include <string.h> /* for strchr() */

#include "ini_configuration.h"

#define MAX_PANNE_NUM 25
namespace SRMC{
/* UTF-8  UTF-16  UTF-32 ������ļ�ǰ����д�С�˱�ʶ(bit-order-mark)*/
inline static SR_bool stripBOM(SR_char** pstr)
{
	return (pstr) && (*pstr) && ((*pstr)[0] == '\357') && ((*pstr)[1] == '\273') && ((*pstr)[2] == '\277') && ((*pstr) += 3);
}

std::string& trimBlanks(std::string& str)
{
	if (0 != str.length())
	{
		const SR_char *s = str.c_str();
		while (*s == ' ' || *s == '\t')
			s++;

		const SR_char *e = s;
		for (const SR_char *p = e; *p; p++)
			if (*p != ' ' && *p != '\t')
				e = p + 1;

		str.assign(s, e - s);
	}
	return str;
}


Configuration::Configuration(const SR_char* filename)
{
	if (0 == filename)
	{
		//sr_printf(SR_PRINT_ERROR, "must have filename\n");
		printf("must have filename\n");
		return;
	}
	m_filename.assign(filename);
	FILE *pfile = ::fopen(filename, "r");;
	if (0 == pfile)
	{
		//sr_printf(SR_PRINT_ERROR, "cannot open %s\n", filename);
		printf("cannot open %s\n", filename);
		return;
	}

	m_sec_key_value.clear();

	std::string sect;
	SR_bool start = true;
	SR_char buf[1024];

	while (1)
	{
		/* ʧ�ܻ���������� 0 ;
		* �в���bufsize���ַ����������оͽ�����
		* �������У��������һ�����з������ַ�������bufsize-1��
		* ��fgetsֻ����һ�����������У�������������NULL�ַ���β
		*/
		if (!::fgets(buf, sizeof(buf), pfile))
			break;

		SR_char *pc = ::strchr(buf, '\r');
		if (pc)
			*pc = 0;
		pc = ::strchr(buf, '\n');
		if (pc)
			*pc = 0;
		pc = buf;

		// skip over an initial UTF-8 BOM
		if (start)
		{
			stripBOM(&pc);
			start = false;
		}

		while (*pc == ' ' || *pc == '\t')
			pc++;

		switch (*pc)
		{
		case 0:
		case ';':
			continue;
		}

		std::string s(pc);
		if (s[0] == '[')
		{
			SR_int32 r = s.find(']');
			if (r > 0)
			{
				sect.clear();
				sect = s.substr(1, r - 1);
				trimBlanks(sect);
				/* ��֮ǰ�Ѽ����Ӹ� section,��insert nothing */
			}
			continue;
		}

		SR_uint32 q = s.find('=');
		if (0 == q)
			continue;
		std::string key = s.substr(0, q);
		trimBlanks(key);

		if (0 == key.length())
			continue;

		s = s.substr(q + 1);
		trimBlanks(s);
		/* �� section+key �ϲ���һ��"KEY" ,�������������key��*/
		key.insert(0, sect);

		/* ��֧����չ������ */
		m_sec_key_value.insert(make_pair(key, s));
	}

	::fclose(pfile);
	pfile = 0;
}

#define SR_CONFIGURATION_MAP_FIND(sec,key,defvalue)\
if((0 == section)||(0 == key))\
	return defvalue;\
std::string secstr(sec);\
std::string keystr(key);\
trimBlanks(secstr);\
trimBlanks(keystr);\
secstr.append(keystr);\
const std::map<std::string,std::string>::const_iterator itor = m_sec_key_value.find(secstr);\
do{}while(0)	


const std::string Configuration::getStrValue(const SR_char* section, const SR_char* key, const SR_char* defvalue_cstr) const
{
	std::string defvalue;
	if (defvalue_cstr)
		defvalue.assign(defvalue_cstr);
	else
		defvalue.clear();

	SR_CONFIGURATION_MAP_FIND(section, key, defvalue);
	if (m_sec_key_value.end() == itor)
		return defvalue;
	else
	{
		std::string findstr(itor->second);
		return findstr;
	}
}

SR_int32 Configuration::getIntValue(const SR_char* section, const SR_char* key, SR_int32 defvalue) const
{
	SR_CONFIGURATION_MAP_FIND(section, key, defvalue);
	if (m_sec_key_value.end() == itor)
	{
		return defvalue;
	}
	else
	{
		const SR_char* findstr = (itor->second).c_str();
		SR_int32 ret = ::strtol(findstr, 0, 10);
		return ret;
	}
}

SR_int64 Configuration::getInt64Value(const SR_char* section, const SR_char* key, SR_int64 defvalue) const
{
	SR_CONFIGURATION_MAP_FIND(section, key, defvalue);
	if (m_sec_key_value.end() == itor)
	{
		return defvalue;
	}
	else
	{
		const SR_char* findstr = (itor->second).c_str();
		SR_int64 ret = ::strtoll(findstr, 0, 10);
		return ret;
	}

}
SR_bool Configuration::getBoolValue(const SR_char* section, const SR_char* key, SR_bool defvalue) const
{
	SR_CONFIGURATION_MAP_FIND(section, key, defvalue);
	if (m_sec_key_value.end() == itor)
	{
		return defvalue;
	}
	else
	{
		const SR_char* findstr = (itor->second).c_str();
		if ((0 == ::strncmp(findstr, "true", 4)) || (0 == ::strncmp(findstr, "TRUE", 4)))
			return true;
		else
			return false;
	}
}

std::string MCCfgInfo::m_filename;
MCCfgInfo*  MCCfgInfo::m_instance = (MCCfgInfo*)0;
std::string MCCfgInfo::m_listenip;
SR_uint32   MCCfgInfo::m_listenport;
SR_uint32   MCCfgInfo::m_nettype;
std::string MCCfgInfo::m_devmgrip;
SR_uint32   MCCfgInfo::m_devmgrport;
SR_uint64   MCCfgInfo::m_detect_tertime;//����ն��������
SR_uint64   MCCfgInfo::m_detect_netmptime;
SR_uint64   MCCfgInfo::m_beatto_ter_time;
SR_uint64   MCCfgInfo::m_beatto_netmp_time;
SR_uint64   MCCfgInfo::m_beatto_devmgr_time;
SR_uint64   MCCfgInfo::m_update_syscurload_time;
SR_uint64   MCCfgInfo::m_check_devmgr_timer;
SR_uint64   MCCfgInfo::m_update_netmpinfo_time;
SR_uint32   MCCfgInfo::m_netmp_highload;
SR_uint32   MCCfgInfo::m_netmp_lowload;
SR_uint32   MCCfgInfo::m_max_connected_with_netmp;
SR_uint32   MCCfgInfo::m_idle_netmp_nums;
SR_uint32   MCCfgInfo::m_maxters_in_netmp;
std::string MCCfgInfo::m_register_password;
SR_uint32   MCCfgInfo::m_mc_svr_type;
std::string MCCfgInfo::m_mctoken;
SR_uint32   MCCfgInfo::m_mcdeviceid = 0u;
SR_uint32   MCCfgInfo::m_mcgroupid = 0u;
std::string MCCfgInfo::m_mcdomainname;
SR_uint64 	MCCfgInfo::m_waitdevmgrtimeout;
SR_uint64 	MCCfgInfo::m_waitnetmptimeout;
SR_uint64 	MCCfgInfo::m_keep_conf_live;
SR_uint64 	MCCfgInfo::m_reqsecondvedio_timeout;
SR_uint64 	MCCfgInfo::m_reconnect_devmgr_time;
SR_uint32   MCCfgInfo::m_reconnect_devmgr_loopcnt;
SR_uint32  	MCCfgInfo::m_detecttimer_precision;
SR_uint32  	MCCfgInfo::m_usevideomixer;
SR_uint32  	MCCfgInfo::m_stdterselectvideomode;
SR_uint32  	MCCfgInfo::m_single_netmp_mp;
SR_uint32  	MCCfgInfo::m_callinvite;
SR_uint32  	MCCfgInfo::m_supportconfrecsvr;
SR_uint64 	MCCfgInfo::m_keep_conf_rollcall;
SR_uint32  	MCCfgInfo::m_livesvrtype;//ֱ����������,Ĭ��0-SRSֱ����1-������ֱ����2-��Ѷ��ֱ��
std::string MCCfgInfo::m_replacenetmp_regsrcipmapip;
SR_uint32  	MCCfgInfo::m_max_autopanne_nums;
SR_uint32  	MCCfgInfo::m_max_assignpanne_nums;

// logconf
SR_uint32 MCCfgInfo::m_nLoglevel;
SR_bool MCCfgInfo::m_bIsStdout;

static MCCfgInfo private_g_mcconfinfo;

MCCfgInfo* MCCfgInfo::instance()
{
	return &private_g_mcconfinfo;
}

MCCfgInfo::MCCfgInfo()
{
	MCCfgInfo::m_filename.assign("mc.conf");
	Parser(true);
}
std::string MCCfgInfo::get_replacenetmp_regsrcipmapip(std::string IPdotdec)
{
	std::string  m_replacenetmpip;
	m_replacenetmpip.clear();
	for (std::map<SR_uint32, std::string>::iterator checknetmpitor = m_checknetmp_regsrcipmap.begin();
		checknetmpitor != m_checknetmp_regsrcipmap.end(); checknetmpitor++)
	{
		if (checknetmpitor->first != 0)
		{
			/*sr_printf(SR_PRINT_INFO, "get_replacenetmp_regsrcipmapip,count = %d,checknetmpips=%s\n", checknetmpitor->first,checknetmpitor->second.c_str());*/
			if (checknetmpitor->second.find(IPdotdec) != std::string::npos)
			{
				std::map<SR_uint32, std::string>::iterator replacenetmpitor = m_replacenetmp_regsrcipmap.find(checknetmpitor->first);
				if (replacenetmpitor != m_replacenetmp_regsrcipmap.end())
				{
					m_replacenetmpip = replacenetmpitor->second;
					sr_printf(SR_PRINT_INFO, "get_replacenetmp_regsrcipmapip,count = %d,,m_replacenetmpip=%s\n", replacenetmpitor->first,m_replacenetmpip.c_str());
				}
			}
		}
	}
	return m_replacenetmpip;
}//���netmpע���ַ
bool MCCfgInfo::Parser(bool needreloadall/* = true*/)
{
	if (m_filename.length() > 0)
	{
		SRMC::Configuration mc_cfg(m_filename.c_str());

		if (needreloadall)
		{
			m_listenip = mc_cfg.getStrValue("mc", "mcip", "192.168.1.6");
			m_nettype = mc_cfg.getIntValue("mc", "nettype", 0);
			m_listenport = mc_cfg.getIntValue("generate", "mclistenport", 12345);
			m_devmgrip = mc_cfg.getStrValue("device", "deviceip", "192.168.1.4");
			m_devmgrport = mc_cfg.getIntValue("device", "deviceport", 54321);
			m_detect_tertime = mc_cfg.getInt64Value("mc", "detecttime", 12000);
			m_detect_netmptime = mc_cfg.getInt64Value("netmp", "detect_netmpheart_time", 30000);
			m_beatto_ter_time = mc_cfg.getInt64Value("mc", "heartbeattoter", 9000);
			m_beatto_netmp_time = mc_cfg.getInt64Value("netmp", "heartbeattonetmp", 9000);
			m_beatto_devmgr_time = mc_cfg.getInt64Value("device", "mc_to_devMgr_timer", 3000);
			m_update_syscurload_time = mc_cfg.getInt64Value("device", "update_syscurload_time", 3000);
			m_check_devmgr_timer = mc_cfg.getInt64Value("device", "mc_check_devmgr_timer", 9000);
			m_update_netmpinfo_time = mc_cfg.getInt64Value("device", "update_netmpinfo_time", 4000);
			m_netmp_highload = mc_cfg.getIntValue("netmp", "netmp_highload", 800);
			m_netmp_lowload = mc_cfg.getIntValue("netmp", "netmp_lowload", 12345);
			m_max_connected_with_netmp = mc_cfg.getIntValue("netmp", "max_connected_with_netmp", 12345);
			m_idle_netmp_nums = mc_cfg.getIntValue("netmp", "netmp_idle_tcp_nums", 2);
			m_maxters_in_netmp = mc_cfg.getIntValue("netmp", "maxternums", 12345);
			m_register_password = mc_cfg.getStrValue("mc", "auth_password", "mc_pd");
			m_mc_svr_type = mc_cfg.getIntValue("mc", "mc_svr_type", 1);
			m_waitdevmgrtimeout = mc_cfg.getInt64Value("mc", "waitdevmgrconfinfotime", 5000llu);
			m_waitnetmptimeout = mc_cfg.getInt64Value("mc", "waitnetmpcreateconftime", 5000llu);
			m_keep_conf_live = mc_cfg.getInt64Value("mc", "keep_conf_live", 300000llu);
			m_reqsecondvedio_timeout = mc_cfg.getInt64Value("mc", "reqsecondvedio_timeout", 5000llu);
			m_reconnect_devmgr_time = mc_cfg.getInt64Value("mc", "reconnectdevmgrtime", 3000llu);
			m_reconnect_devmgr_loopcnt = mc_cfg.getIntValue("mc", "reconnectdevmgrloopcnt", 1);
			m_detecttimer_precision = mc_cfg.getIntValue("mc", "detecttimer_precision", 1000);
			m_usevideomixer = mc_cfg.getIntValue("mc", "usevideomixer", 0);
			m_stdterselectvideomode = mc_cfg.getIntValue("mc", "stdterselectvideomode", 0);
			m_single_netmp_mp = mc_cfg.getIntValue("mc", "single_netmp_mp", 1);
			m_callinvite = mc_cfg.getIntValue("mc", "callinvite", 0);
			m_supportconfrecsvr = mc_cfg.getIntValue("mc", "supportconfrecsvr", 0);
			m_keep_conf_rollcall = mc_cfg.getInt64Value("mc", "keep_conf_rollcall", 40000llu);
			m_livesvrtype = mc_cfg.getIntValue("mc", "livesvrtype", 0);//ֱ����������,Ĭ��0-SRSֱ����1-������ֱ����2-��Ѷ��ֱ��
			m_checknetmp_regsrcips = mc_cfg.getStrValue("netmp", "checknetmp_regsrcips", " ");
			m_replacenetmp_regsrcip = mc_cfg.getStrValue("netmp", "replacenetmp_regsrcip", " ");
			m_max_autopanne_nums = mc_cfg.getIntValue("mc","max_autopanne_nums",25);
			m_max_assignpanne_nums = mc_cfg.getIntValue("mc", "max_assignpanne_nums", 25);
			if (m_max_autopanne_nums <= 0
				|| m_max_autopanne_nums > 25 )
			{
				m_max_autopanne_nums = MAX_PANNE_NUM;
			}
			if (m_max_assignpanne_nums <= 0
				|| m_max_assignpanne_nums > 25)
			{
				m_max_assignpanne_nums = MAX_PANNE_NUM;
			}
			char * checknetmpresult = NULL;
			char *checknetmpbuf = new char[strlen(m_checknetmp_regsrcips.c_str()) + 1];
			strcpy(checknetmpbuf,m_checknetmp_regsrcips.c_str());
			SR_uint32 checknetmpcount = 1;
			while ((checknetmpresult = strstr(checknetmpbuf, ";")) != NULL)
			{
				checknetmpresult[0] = '\0';
				m_checknetmp_regsrcipmap.insert(std::make_pair(checknetmpcount, std::string(checknetmpbuf)));
				checknetmpbuf = checknetmpresult + strlen(";");
				++checknetmpcount;
			}
			m_checknetmp_regsrcipmap.insert(std::make_pair(checknetmpcount, std::string(checknetmpbuf)));
			char * replacenetmpresult = NULL;
			char *replacenetmpbuf = new char[strlen(m_replacenetmp_regsrcip.c_str()) + 1];
			strcpy(replacenetmpbuf, m_replacenetmp_regsrcip.c_str());
			SR_uint32 replacenetmpcount = 1;
			while ((replacenetmpresult = strstr(replacenetmpbuf, ";")) != NULL)
			{
				replacenetmpresult[0] = '\0';
				m_replacenetmp_regsrcipmap.insert(std::make_pair(replacenetmpcount, std::string(replacenetmpbuf)));
				replacenetmpbuf = replacenetmpresult + strlen(";");
				++replacenetmpcount;
			}
			m_replacenetmp_regsrcipmap.insert(std::make_pair(replacenetmpcount, std::string(replacenetmpbuf)));
		}
		
		// LOG��־�ȼ����Ƿ��ӡ���ն˿���̨
		m_nLoglevel = mc_cfg.getIntValue("mc", "loglevel", 0);
		int isStdout = mc_cfg.getIntValue("mc", "isstdout", 1);
		m_bIsStdout = (isStdout != 0) ? true : false;
	}
	else
	{
		return false;
	}

	return true;
}

}/* namespace SRMC */
