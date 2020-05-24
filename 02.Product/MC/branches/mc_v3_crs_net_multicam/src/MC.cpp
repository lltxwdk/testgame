#include <map>
#include <time.h>

#include "terminalMgr.h"
#include "ini_configuration.h"
#include "deviceManager.h"


#include "hardwareInfo.h"

#include "sr_common.h"
#include "sr_decrypt.h"

#include <fstream>  
#include <string.h>  
#include <iostream>  
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include "Winsock2.h"
#else
#include<sys/types.h>  /*提供类型pid_t,size_t的定义*/
#include<sys/stat.h>
#include<fcntl.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/prctl.h>

#include <errno.h>
#endif

//  2.71828 18284 59045 23536 02874 71352 66249 77572 47093 69995 
//    95749 66967 62772 40766 30353 54759 45713 82178 52516 64274
#define MC_VERSION "2.71828"

#define McLicfilename "McLicence.ini"

//class Timer : public ITimerEvent
//{
//public:
//	Timer():m_pTermMgr(NULL),m_pdevMgr(NULL){}
//	~Timer(){}
//    void SetMonitors(TerminalMgr *pTermMgr, DeviceManager *pdevMgr){
//	    m_pTermMgr = pTermMgr;
//	    m_pdevMgr = pdevMgr;
//    }
//	void OnTimerEvent(){
//		if(NULL != m_pTermMgr)
//			m_pTermMgr->OnTimerPerSecond();
//		if(NULL != m_pdevMgr)
//			m_pdevMgr->OnTimerPerSecond();
//	}
//private:
//    TerminalMgr *m_pTermMgr;
//    DeviceManager *m_pdevMgr;
//	SUIRUI_DISALLOW_COPY_AND_ASSIGN(Timer);
//};

#if defined(_WIN32) || defined(_WIN64)
#define snprintf _snprintf
#endif 

//namespace Initial_Log{
//class InitLogFile
//{
//public:
//	InitLogFile(){
//		char filename[256] = {0};
//		time_t now = time(NULL);
//		struct tm tm_now;		
//#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
//		localtime_s(&tm_now, &now);
//#elif defined(LINUX) || defined(POSIX)
//		localtime_r(&now, &tm_now);
//#endif
//		snprintf(filename,sizeof(filename),"logmc_%02d-%02d-%02d",tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
//		TraceLog::GetInstance().OpenLog(filename,TL_DEBUG,true);
//	}
//	~InitLogFile(){}
//private:
//	SUIRUI_DISALLOW_COPY_AND_ASSIGN(InitLogFile);
//};
//static InitLogFile initlogfile;
//}

extern const char* SUIRUI_logo;
extern const char* MC_logo;
#define MAIN_THREAD_WHILE_SLEEP_SECOND (1)
#define TIMEOUT_WAIT_MICROSECOND (800)
#define MAIN_THREAD_CHECK_CONFIG_SECOND (5)

int watchdog_flag = 0;
int main_begin_flag = 0;

int g_iLoglevel = 2; // SR_PRINT_NORMAL等级
bool g_bisStdout = false;

void CheckConfig()
{
	// 重新读取配置文件配置
	SRMC::MCCfgInfo::instance()->Parser(false);
	
	int iNewLevel = SRMC::MCCfgInfo::instance()->get_loglevel();

	if (iNewLevel != g_iLoglevel) // 如果配置文件的日志级别发生变化重新设置日志级别
	{
		sr_printf(SR_PRINT_ALERT, "==Config is changed==SetLogLevel(g_iLoglevel)--- %d --->> %d\n", g_iLoglevel, iNewLevel);

		g_iLoglevel = iNewLevel;
		TraceLog::GetInstance().SetLogLevel(g_iLoglevel);
	}

	bool bisStdout = SRMC::MCCfgInfo::instance()->get_isstdout();

	if (bisStdout != g_bisStdout)
	{
		sr_printf(SR_PRINT_ALERT, "==Config is changed==EnabledStdout(g_bisStdout)--- %d --->> %d\n", g_bisStdout, bisStdout);

		g_bisStdout = bisStdout;
		TraceLog::GetInstance().EnabledStdout(g_bisStdout);
	}

	return;
}

int main(int argc, char* argv[])
{
#ifdef LINUX
		#define PIPE_INPUT  (0)
		#define PIPE_OUTPUT (1)
		int pipefd_descriptors[2]; 	
		pipe(pipefd_descriptors); // 创建无名管道
		
	#define FILEPATH_STR_MAX (256)
	#define PROGRAM_NAME (256)
		char file_path_getcwd[FILEPATH_STR_MAX] = {0};
		if(NULL == getcwd(file_path_getcwd,FILEPATH_STR_MAX))
		{
			printf("%d is small,filepath is too long\n",FILEPATH_STR_MAX);
			exit(3);
		}
		char program_name[PROGRAM_NAME] = {0};
		memcpy(program_name,argv[0],strlen(argv[0]));
		//char* execv_arg[] = {program_name,"-daemon -w",NULL};
		//printf("%s @@ %s\n",program_name,file_path_getcwd);
		char command_cd_and_run[512] = {0};
		snprintf(command_cd_and_run,sizeof(command_cd_and_run),"cd %s && %s -daemon -w\n",file_path_getcwd,program_name);
		
		int daemon_flag = 0;
		
		opterr = 0;
		int optch = getopt(argc,argv,"d:w");
		while(-1 != optch){
			switch(optch){
				case 'd':{
					if(0 != strncmp("aemon",optarg,6)){
						printf("usage error! -daemon\n");
						exit(1);
					}else{
						daemon_flag = 1;
					}
					break;
				}
				case 'w':{
					watchdog_flag = 1;
					break;
				}
				default:{
					printf("usage error! d[aemon]:w\n");
					exit(2);
				}
			}
			optch = getopt(argc,argv,"d:w::");
		}

		if(1 == watchdog_flag){ // 创建独立子进程监控MC进程
		    if (0 == fork()){
/////////////////////////////////////////////////////////////////////////////////				
				close(pipefd_descriptors[PIPE_OUTPUT]);
				memset(argv[0],0,strlen(argv[0]));
				memcpy(argv[0],"MC_watchdog",11);
			    if(-1 == setsid()){ // create a new session
					printf("setsid error(%d)\n",errno);
				}
				int fd;
			    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
			        dup2(fd, STDIN_FILENO);
			        dup2(fd, STDOUT_FILENO);
			        dup2(fd, STDERR_FILENO);
			        if (fd > STDERR_FILENO) close(fd);
			    }
#ifdef PR_SET_NAME
	// not available !!!!
				prctl(PR_SET_NAME,(unsigned long)"MC_watchdog",0,0,0);
#endif		
				struct timeval timeout_tv;
				
				const int timeout_nums = 2;
				const int error_nums = 10;
				int count_timeout = 0;
				int count_error = 0;
				char read_ch;
				int read_count = -1;
				const int read_fd = pipefd_descriptors[PIPE_INPUT];
				fd_set fds_read;
				while(1){
					FD_ZERO(&fds_read);
					FD_SET(read_fd, &fds_read);
					timeout_tv.tv_sec = 10;
					timeout_tv.tv_usec = TIMEOUT_WAIT_MICROSECOND;
					int result = select(read_fd + 1, &fds_read, NULL, NULL, &timeout_tv);
					if(result > 0){
						if (FD_ISSET(read_fd, &fds_read)){
							read_count = read(read_fd,&read_ch,sizeof(read_ch));
							if(1 == read_count){
								count_error = 0;
								count_timeout = 0;
								continue;
							}
							else{
								if(EINTR == errno) continue;
								++count_error;
							}
						}
					}
					else if(0 == result)// 超时
						++count_timeout;
					else
						++count_error;
					
					if((count_timeout == timeout_nums)||(count_error == error_nums)){
						system(command_cd_and_run);
						exit(0);
					}
				}
		    }
/////////////////////////////////////////////////////////////////////////////////
			close(pipefd_descriptors[PIPE_INPUT]);
		}
	// int daemon(int nochdir, int noclose);
	// If nochdir is zero, daemon() changes the process's current working
	//		 directory to the root directory ("/"); otherwise, the current working
	//		 directory is left unchanged.
	// If noclose is zero, daemon() redirects standard input, standard
	//		 output and standard error to /dev/null; otherwise, no changes are
	//		 made to these file descriptors.	
		if(1 == daemon_flag) ::daemon(1,0);
#endif

		char filename[256] = { 0 };
		time_t now = time(NULL);
		struct tm tm_now;
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		localtime_s(&tm_now, &now);
#elif defined(LINUX) || defined(POSIX)
		localtime_r(&now, &tm_now);
#endif
		snprintf(filename, sizeof(filename), "logmc_%02d-%02d-%02d", tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
		//TraceLog::GetInstance().OpenLog(filename, TL_DEBUG, true);

		//int iLoglevel = 2; // SR_LOGLEVEL_NORMAL等级
		//bool bisStdout = false;

		g_iLoglevel = SRMC::MCCfgInfo::instance()->get_loglevel();
		g_bisStdout = SRMC::MCCfgInfo::instance()->get_isstdout();

		TraceLog::GetInstance().OpenLog(filename, g_iLoglevel, g_bisStdout);

	char mc_logo_chars[1024] = {0};
	sr_printf(g_iLoglevel, "\n\n%s\n", SUIRUI_logo);
	snprintf(mc_logo_chars,sizeof(mc_logo_chars),MC_logo,
			((sizeof(long) == 8) ? "64" : "32"),
			__DATE__,__TIME__,
			MC_VERSION,
#if defined(_WIN32) || defined(_WIN64)
			GetCurrentProcessId()
#else
			(long) getpid()
#endif
			);
	sr_printf(g_iLoglevel, "\n\n%s\n", mc_logo_chars);
	sr_printf(g_iLoglevel, "cfgfilename:%s\nmc_listenip=%s,listenport=%d,use_videomixer=%d,isConfUseSingleNetmpMp=%d,g_iLoglevel=%d,g_bisStdout=%d\n"\
							"supportconfrecsvr=%d,livesvrtype=%d\n"\
							"devmgrip=%s,devmgrport=%d\n"\
							"mc_svr_type=%d,register_password=%s\nmc->ter heartbeat time=%lums,detect ters time=%lums\n"\
							"update netmpinfo=%lums,idle_tcp_connect_num=%d,max_tcp_connect_with_netmp=%d,"\
							"maxtersinnetmp=%d,lowload=%d,highload=%d\n"\
							"mc->devmgr heartbeat=%lums,waitdevmgrconfinfotime=%lums,keep_conf_live=%lums,getsyscurloadtime=%lums\n",
				SRMC::MCCfgInfo::instance()->get_fileName().c_str(),
				SRMC::MCCfgInfo::instance()->get_listenip().c_str(), SRMC::MCCfgInfo::instance()->get_listenport(), 
				SRMC::MCCfgInfo::instance()->get_use_videomixer(), SRMC::MCCfgInfo::instance()->get_single_netmp_mp(),
				SRMC::MCCfgInfo::instance()->get_loglevel(), SRMC::MCCfgInfo::instance()->get_isstdout(),
				SRMC::MCCfgInfo::instance()->get_sup_confrecsvr(), SRMC::MCCfgInfo::instance()->get_live_svrtype(),
				SRMC::MCCfgInfo::instance()->get_devmgrip().c_str(),SRMC::MCCfgInfo::instance()->get_devmgrport(),
				SRMC::MCCfgInfo::instance()->get_mc_svr_type(),SRMC::MCCfgInfo::instance()->get_register_password().c_str(),
				SRMC::MCCfgInfo::instance()->get_beatto_ter_time(),SRMC::MCCfgInfo::instance()->get_detect_tertime(),
				SRMC::MCCfgInfo::instance()->get_update_netmpinfo_time(),SRMC::MCCfgInfo::instance()->get_idle_netmp_nums(),
				SRMC::MCCfgInfo::instance()->get_max_connected_with_netmp(),SRMC::MCCfgInfo::instance()->get_maxters_in_netmp(),
				SRMC::MCCfgInfo::instance()->get_netmp_lowload(),SRMC::MCCfgInfo::instance()->get_netmp_highload(),
				SRMC::MCCfgInfo::instance()->get_beatto_devmgr_time(),SRMC::MCCfgInfo::instance()->get_waitdevmgrconfinfotime(),
				SRMC::MCCfgInfo::instance()->get_keep_conf_live(),SRMC::MCCfgInfo::instance()->get_update_syscurload_time());

	char cExpiringDate[30];
	memset(cExpiringDate, '\0', sizeof(cExpiringDate));
	int iMaxSRcount = 0; // 默认0,系统授权最大SR终端数
	int iMaxStdcount = 0; // 默认0,系统授权最大标准终端数
	int iMaxReccount = 0; // 默认0,系统授权最大录制路数
	int iMaxLivecount = 0;// 默认0,系统授权最大直播路数
	int iMaxConfNum = 0; // 默认0,系统授权最大会议数----目前还未增加该授权
	int iMaxTernumPerConf = 0; // 默认0,系统授权单会议最大终端数
	int iMaxVoicecount = 0;//默认0，系统授权最大语音终端数
	int iMaxMonitorcount = 0;//默认0，系统授权最大监控终端数
	char cStdExpiringDate[30];
	memset(cStdExpiringDate, '\0', sizeof(cStdExpiringDate));
	char cRecExpiringDate[30];
	memset(cRecExpiringDate, '\0', sizeof(cRecExpiringDate));
	char cLiveExpiringDate[30];
	memset(cLiveExpiringDate, '\0', sizeof(cLiveExpiringDate));
	char cVoiceExpiringDate[30];
	memset(cVoiceExpiringDate, '\0', sizeof(cVoiceExpiringDate));
	char cMonitorExpiringDate[30];
	memset(cMonitorExpiringDate, '\0', sizeof(cMonitorExpiringDate));
	char cMyVersion[256];
	memset(cMyVersion, '\0', sizeof(cMyVersion));

#ifdef LINUX

	if (access("./McLicence.ini", F_OK) == 0) // 与windows中PathFileExists功能一样
	{
		char cPrintInfo[2000];
		memset(cPrintInfo, '\0', sizeof(cPrintInfo));

		char cSystemId[30];
		memset(cSystemId, '\0', sizeof(cSystemId));

		//getSystemId(cSystemId, cPrintInfo);

		//sr_printf(SR_PRINT_NORMAL, "==-->>cSystemId=%s,cPrintInfo=%s\n", cSystemId, cPrintInfo);

		unsigned char macAddrlist[MAX_NET_CARDS * MAC_ADDR_LEN];
		unsigned char cpuid[MAX_NET_CARDS];
		memset(macAddrlist, 0, MAX_NET_CARDS * MAC_ADDR_LEN);
		memset(cpuid,0,MAX_NET_CARDS);
		int maclistlen = 0;
		int cpuidlen = 0;
		getMacList(macAddrlist, maclistlen);
		GetCpuId(cpuid,cpuidlen);
		char maclistStr[MAX_NET_CARDS * MAC_ADDR_LEN * 2];
		memset(maclistStr, 0, MAX_NET_CARDS * MAC_ADDR_LEN * 2);

		int i;
		for (i = 0; i < maclistlen; i++)
		{
			sprintf(&maclistStr[i * 2], "%02X", macAddrlist[i]);
		}

		char macAddr[MAX_NET_CARDS][MAC_ADDR_LEN * 2];
		memset(macAddr, 0, sizeof(macAddr));
		//memcpy(mac, maclistStr, strlen(maclistStr));

		int maclistStrlen = strlen(maclistStr);
		int maclen_hex = MAC_ADDR_LEN * 2;
		int line = maclistStrlen / maclen_hex;


		SR_bool bDecryptOK = false;
		char result[2048]={0};//注意授权文件的大小
		int j,k;
		for (j = 0; j < line; j++)
		{
			unsigned char mckey[100] = { 0 };
			unsigned char mckeycpy[100] = { 0 };
			//for (k = 0; k < maclen_hex; k++)
			//{
			//	sprintf(&macAddr[j][k], "%02X", maclistStr);
			//}


			//sprintf(mac, "%s", &maclistStr[j* MAC_ADDR_LEN * 2]);

			strncat((char*)mckey, &maclistStr[j* MAC_ADDR_LEN * 2], MAC_ADDR_LEN * 2);
			strncat((char*)mckey, (const char*)cpuid, cpuidlen);
			strncat((char*)mckey, PWD_3, PWD_3_CONST);
			strncpy((char *)mckeycpy,(char *)mckey,44);
			int mckeylen = strlen((const char*)mckeycpy);
			//sr_printf(SR_PRINT_NORMAL,"mckeycpy %s\n",mckeycpy);
			bDecryptOK = DecryptStream("./McLicence.ini", (const char*)mckeycpy, PWD_2, result,cPrintInfo);

			if (bDecryptOK)
			{
				//sr_printf(SR_PRINT_NORMAL,"cPrintInfo %s\n",cPrintInfo);

				//sr_printf(SR_PRINT_NORMAL, "==Decrypt McLicence.ini OK--result-->>\n%s\n", result);
				sr_printf(SR_PRINT_NORMAL, "==Decrypt McLicence.ini OK!\n");
				break;
			}

		}

		if (bDecryptOK == false)
		{
			//sr_printf(SR_PRINT_ERROR,"Decrypt McLicence.ini error, then exit this mc!\n");
			//sr_printf(SR_PRINT_ERROR,"cPrintInfo %s\n",cPrintInfo);
			sr_printf(SR_PRINT_ERROR,"Decrypt McLicence.ini error, please check licence file content!\n");

			//exit(1);
			SR_uint32 uiSleepNum = 0;
			while (true)
			{
				Utils::Sleep(1000*MAIN_THREAD_WHILE_SLEEP_SECOND);
				uiSleepNum++;

				if (uiSleepNum == MAIN_THREAD_CHECK_CONFIG_SECOND)
				{
					sr_printf(SR_PRINT_ERROR,"Decrypt McLicence.ini error, please check licence file content!\n");
					uiSleepNum = 0;
				}
			}
		}

		map_string ms;
		parsePair(ms,result,"\r\n","=");

		if (getOnePair(ms,D_EXPIRINGDATE,cExpiringDate) && strlen(cExpiringDate) > 0)
		{
			if(strlen(cExpiringDate) == 10)
			{
				char _year[10];
				memset(_year, '\0', sizeof(_year));
				strncpy(_year, cExpiringDate, 4);
				int year = atoi(_year);

				char _month[10];
				memset(_month, '\0', sizeof(_month));
				strncpy(_month, cExpiringDate + 5, 2);
				int month = atoi(_month);

				char _date[10];
				memset(_date, '\0', sizeof(_date));
				strncpy(_date, cExpiringDate + 8, 2);
				int date = atoi(_date);

				if(year > 2000 && year < 9999 && month > 0 && month < 13 && date > 0 && date < 32)
				{
					sprintf(cExpiringDate, "%s-%s-%s 23:59:59", _year, _month, _date);
				}
				else
				{
					time_t ltime;
					time( &ltime );
					struct tm *now = localtime( &ltime );
					strftime(cExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
				}
			}
			else
			{
				time_t ltime;
				time( &ltime );
				struct tm *now = localtime( &ltime );
				strftime(cExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
			}

			if(CompareCurrentEndTime(cExpiringDate) == Current_Is_Same_Or_Later)
			{
				//sr_printf(SR_PRINT_ERROR,"==-->> Licence file ExpiringDate expired Currenttime, then exit this mc!\n");
				sr_printf(SR_PRINT_ERROR,"==-->> Licence file ExpiringDate expired Currenttime, please contact suirui!\n");

				//exit(1);
				SR_uint32 uiSleepNum = 0;
				while (true)
				{
					Utils::Sleep(1000*MAIN_THREAD_WHILE_SLEEP_SECOND);
					uiSleepNum++;

					if (uiSleepNum == MAIN_THREAD_CHECK_CONFIG_SECOND)
					{
						sr_printf(SR_PRINT_ERROR,"==-->> Licence file ExpiringDate expired Currenttime, please contact suirui!\n");
						uiSleepNum = 0;
					}
				}
			}
			else
			{
				sr_printf(SR_PRINT_NORMAL,"==-->> Licence file ExpiringDate=%s\n", cExpiringDate);
			}
		}
		else
		{
			sr_printf(SR_PRINT_WARN,"==-->> Licence file ExpiringDate not exist or is Null!\n");
		}
		//标准终端系统授权截止时间
		if (getOnePair(ms,D_STD_EXPIRINGDATE,cStdExpiringDate) && strlen(cStdExpiringDate) > 0)
		{
			if(strlen(cStdExpiringDate) == 10)
			{
				char _year[10];
				memset(_year, '\0', sizeof(_year));
				strncpy(_year, cStdExpiringDate, 4);
				int year = atoi(_year);

				char _month[10];
				memset(_month, '\0', sizeof(_month));
				strncpy(_month, cStdExpiringDate + 5, 2);
				int month = atoi(_month);

				char _date[10];
				memset(_date, '\0', sizeof(_date));
				strncpy(_date, cStdExpiringDate + 8, 2);
				int date = atoi(_date);

				if(year > 2000 && year < 9999 && month > 0 && month < 13 && date > 0 && date < 32)
				{
					sprintf(cStdExpiringDate, "%s-%s-%s 23:59:59", _year, _month, _date);
				}
				else
				{
					time_t ltime;
					time( &ltime );
					struct tm *now = localtime( &ltime );
					strftime(cStdExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
				}
			}
			else
			{
				time_t ltime;
				time( &ltime );
				struct tm *now = localtime( &ltime );
				strftime(cStdExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
			}
		}
		//录制系统授权截止时间
		if (getOnePair(ms,D_REC_EXPIRINGDATE,cRecExpiringDate) && strlen(cRecExpiringDate) > 0)
		{
			if(strlen(cRecExpiringDate) == 10)
			{
				char _year[10];
				memset(_year, '\0', sizeof(_year));
				strncpy(_year, cRecExpiringDate, 4);
				int year = atoi(_year);

				char _month[10];
				memset(_month, '\0', sizeof(_month));
				strncpy(_month, cRecExpiringDate + 5, 2);
				int month = atoi(_month);

				char _date[10];
				memset(_date, '\0', sizeof(_date));
				strncpy(_date, cRecExpiringDate + 8, 2);
				int date = atoi(_date);

				if(year > 2000 && year < 9999 && month > 0 && month < 13 && date > 0 && date < 32)
				{
					sprintf(cRecExpiringDate, "%s-%s-%s 23:59:59", _year, _month, _date);
				}
				else
				{
					time_t ltime;
					time( &ltime );
					struct tm *now = localtime( &ltime );
					strftime(cRecExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
				}
			}
			else
			{
				time_t ltime;
				time( &ltime );
				struct tm *now = localtime( &ltime );
				strftime(cRecExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
			}
		}
		//直播系统授权截止时间
		if (getOnePair(ms,D_LIVE_EXPIRINGDATE,cLiveExpiringDate) && strlen(cLiveExpiringDate) > 0)
		{
			if(strlen(cLiveExpiringDate) == 10)
			{
				char _year[10];
				memset(_year, '\0', sizeof(_year));
				strncpy(_year, cLiveExpiringDate, 4);
				int year = atoi(_year);

				char _month[10];
				memset(_month, '\0', sizeof(_month));
				strncpy(_month, cLiveExpiringDate + 5, 2);
				int month = atoi(_month);

				char _date[10];
				memset(_date, '\0', sizeof(_date));
				strncpy(_date, cLiveExpiringDate + 8, 2);
				int date = atoi(_date);

				if(year > 2000 && year < 9999 && month > 0 && month < 13 && date > 0 && date < 32)
				{
					sprintf(cLiveExpiringDate, "%s-%s-%s 23:59:59", _year, _month, _date);
				}
				else
				{
					time_t ltime;
					time( &ltime );
					struct tm *now = localtime( &ltime );
					strftime(cLiveExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
				}
			}
			else
			{
				time_t ltime;
				time( &ltime );
				struct tm *now = localtime( &ltime );
				strftime(cLiveExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
			}
		}
		//监控系统授权截止时间
		if (getOnePair(ms,D_MONITOR_EXPIRINGDATE,cMonitorExpiringDate) && strlen(cMonitorExpiringDate) > 0)
		{
			if(strlen(cMonitorExpiringDate) == 10)
			{
				char _year[10];
				memset(_year, '\0', sizeof(_year));
				strncpy(_year, cMonitorExpiringDate, 4);
				int year = atoi(_year);

				char _month[10];
				memset(_month, '\0', sizeof(_month));
				strncpy(_month, cMonitorExpiringDate + 5, 2);
				int month = atoi(_month);

				char _date[10];
				memset(_date, '\0', sizeof(_date));
				strncpy(_date, cMonitorExpiringDate + 8, 2);
				int date = atoi(_date);

				if(year > 2000 && year < 9999 && month > 0 && month < 13 && date > 0 && date < 32)
				{
					sprintf(cMonitorExpiringDate, "%s-%s-%s 23:59:59", _year, _month, _date);
				}
				else
				{
					time_t ltime;
					time( &ltime );
					struct tm *now = localtime( &ltime );
					strftime(cMonitorExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
				}
			}
			else
			{
				time_t ltime;
				time( &ltime );
				struct tm *now = localtime( &ltime );
				strftime(cMonitorExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
			}
		}
		//语音系统授权截止时间
		if (getOnePair(ms,D_VOICE_EXPIRINGDATE,cVoiceExpiringDate) && strlen(cVoiceExpiringDate) > 0)
		{
			if(strlen(cVoiceExpiringDate) == 10)
			{
				char _year[10];
				memset(_year, '\0', sizeof(_year));
				strncpy(_year, cVoiceExpiringDate, 4);
				int year = atoi(_year);

				char _month[10];
				memset(_month, '\0', sizeof(_month));
				strncpy(_month, cVoiceExpiringDate + 5, 2);
				int month = atoi(_month);

				char _date[10];
				memset(_date, '\0', sizeof(_date));
				strncpy(_date, cVoiceExpiringDate + 8, 2);
				int date = atoi(_date);

				if(year > 2000 && year < 9999 && month > 0 && month < 13 && date > 0 && date < 32)
				{
					sprintf(cVoiceExpiringDate, "%s-%s-%s 23:59:59", _year, _month, _date);
				}
				else
				{
					time_t ltime;
					time( &ltime );
					struct tm *now = localtime( &ltime );
					strftime(cVoiceExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
				}
			}
			else
			{
				time_t ltime;
				time( &ltime );
				struct tm *now = localtime( &ltime );
				strftime(cVoiceExpiringDate, 128, "%Y-%m-%d %H:%M:%S", now );
			}
		}
		char temp[100];
		memset(temp, 0, sizeof(temp));
		if (getOnePair(ms,D_SR_COUNT,temp) && strlen(temp) > 0)
		{
			iMaxSRcount = atoi(temp);
		}
		//sr_printf(SR_PRINT_NORMAL,"==-->> Licence file SR_COUNT=%d\n",iMaxSRcount);
		memset(temp, 0, sizeof(temp));
		if (getOnePair(ms,D_STANDARD_COUNT,temp) && strlen(temp) > 0)
		{
			iMaxStdcount = atoi(temp);
		}
		//sr_printf(SR_PRINT_NORMAL,"==-->> Licence file STANDARD_COUNT=%d\n",iMaxStdcount);
		memset(temp, 0, sizeof(temp));
		if (getOnePair(ms,D_RECORD_COUNT,temp) && strlen(temp) > 0)
		{
			iMaxReccount = atoi(temp);
		}
		//sr_printf(SR_PRINT_NORMAL,"==-->> Licence file RECORD_COUNT=%d\n",iMaxReccount);
		memset(temp, 0, sizeof(temp));
		if (getOnePair(ms,D_MAX_LIVE_COUNT,temp) && strlen(temp) > 0)
		{
			iMaxLivecount = atoi(temp);
		}
		//sr_printf(SR_PRINT_NORMAL,"==-->> Licence file LIVE_COUNT=%d\n",iMaxLivecount);
		memset(temp, 0, sizeof(temp));
		if (getOnePair(ms,D_CONFERENCE_MAX_COUNT,temp) && strlen(temp) > 0)
		{
			iMaxTernumPerConf = atoi(temp);
		}
		memset(temp, 0, sizeof(temp));
		if (getOnePair(ms,D_MAX_MONITOR_COUNT,temp) && strlen(temp) > 0)
		{
			iMaxMonitorcount = atoi(temp);
		}
		memset(temp, 0, sizeof(temp));
		if (getOnePair(ms,D_MAX_VOICE_COUNT,temp) && strlen(temp) > 0)
		{
			iMaxVoicecount = atoi(temp);
		}
		//sr_printf(SR_PRINT_NORMAL,"==-->> Licence file CONFERENCE_MAX_COUNT=%d\n",iMaxTernumPerConf);
	}
	else
	{
		//sr_printf(SR_PRINT_NORMAL, "== cur dir not have McLicence.ini file, so MAX_PORT use default value(3).\n");
		//sr_printf(SR_PRINT_NORMAL, "== cur dir not have McLicence.ini file, then exit this mc!\n");
		sr_printf(SR_PRINT_NORMAL, "== cur dir not have McLicence.ini file, please contact suirui!\n");
		//exit(1);
		SR_uint32 uiSleepNum = 0;
		while (true)
		{
			Utils::Sleep(1000 * MAIN_THREAD_WHILE_SLEEP_SECOND);
			uiSleepNum++;

			if (uiSleepNum == MAIN_THREAD_CHECK_CONFIG_SECOND)
			{
				sr_printf(SR_PRINT_NORMAL, "== cur dir not have McLicence.ini file, please contact suirui!\n");
				uiSleepNum = 0;
			}
		}
	}
	
	char cVersion[256] = { 0 };
	char *sep1 = { ":" };
	sprintf(cVersion, "%s", "./version");
	ifstream infver;
	infver.open(cVersion);

	string strversionnname;
	strversionnname.assign("version");
	string strline;
	strline.clear();

	if (infver.is_open())
	{
		while (getline(infver, strline))
		{
			if (strline.length() > 0
				&& strversionnname.length() > 0)
			{
				// 不处理注释行
				if (strline[0] != '#'
					&& strline[0] != ';')
				{
					int pos;
					pos = strline.find(strversionnname);
					if (pos != -1)
					{
						// 下面的解析取决于编译脚本生成version内容格式
						string strsubend;
						strsubend.clear();

						int itmppos = pos + strversionnname.length();
						if (strline[itmppos] == ':')
						{
							if (strline[itmppos + 1] == ' ')
							{
								strsubend = strline.substr(itmppos + 2, strline.length());
								sprintf(cMyVersion, "%s", strsubend.c_str());
								break;
							}
							else
							{
								strsubend = strline.substr(itmppos + 1, strline.length());
								sprintf(cMyVersion, "%s", strsubend.c_str());
								break;
							}
						}
						else if (strline[itmppos] == ' ' && strline[itmppos + 1] == ':')
						{
							strsubend = strline.substr(itmppos + 2, strline.length());
							sprintf(cMyVersion, "%s", strsubend.c_str());
							break;
						}
					}
				}
			}
		}
	}

	infver.close();
#endif

    //ITcpNetEventLoop* event_loop = NULL;
    ////DefaultNetUdpEventHandler udp_handler;
    //DefaultNetTcpEventHandler tcp_handler;
    //Timer timer;
    //event_loop = CreateTcpEventLoop(T_LOOP_DEFAULT);
    //event_loop->SetThreadNum(8);
    //event_loop->Start(&tcp_handler, &timer);

	ITcpTransLib* m_tcp_trans_module = CreateTcpTransLib();
	if (!m_tcp_trans_module->Init(8, ASYNC_MODEL_EPOLL))
	{
		LogERR("tcp trans module init failed!!");
		return -1;
	}

	TerminalMgr	*ptermMgr = new TerminalMgr(m_tcp_trans_module);
	ptermMgr->Init();
	DeviceManager *pdevMgr = new DeviceManager(m_tcp_trans_module);

	pdevMgr->m_devmpprocess->m_terprocess = ptermMgr->GetThreadProcess();
	pdevMgr->m_devmpprocess->m_terthreadhandle = ptermMgr->GetProcessThread();
	pdevMgr->m_devmpprocess->m_deviceMan = pdevMgr;
	pdevMgr->m_devmpprocess->m_timermanger = ptermMgr->GetThreadProcess()->m_timermanger;//共享一个定时器管理队列
    pdevMgr->Init();
	//pdevMgr->m_devmpprocess->startUp();//连接devmgr

	if (pdevMgr != NULL)
	{
		pdevMgr->startUp();
	}

	if (ptermMgr != NULL)
	{
		ptermMgr->SaveLicence(cExpiringDate, iMaxSRcount, iMaxStdcount, cStdExpiringDate, iMaxReccount, cRecExpiringDate, iMaxLivecount,cLiveExpiringDate ,iMaxTernumPerConf, iMaxConfNum, iMaxMonitorcount,cMonitorExpiringDate, iMaxVoicecount, cVoiceExpiringDate,cMyVersion);

		sr_printf(g_iLoglevel, "cExpiringDate:%s,iMaxSRcount=%d,iMaxStdcount=%d,cStdExpiringDate:%s,iMaxReccount=%d,cRecExpiringDate:%s,iMaxLivecount=%d,cLiveExpiringDate:%s,iMaxTernumPerConf=%d,iMaxConfNum=%d,iMaxMonitorcount=%d, cMonitorExpiringDate:%s,iMaxVoicecount=%d,cVoiceExpiringDate:%s,cMyVersion:%s\n",
			cExpiringDate, iMaxSRcount, iMaxStdcount, cStdExpiringDate, iMaxReccount, cRecExpiringDate, iMaxLivecount, cLiveExpiringDate, iMaxTernumPerConf, iMaxConfNum, iMaxMonitorcount, cMonitorExpiringDate, iMaxVoicecount, cVoiceExpiringDate, cMyVersion);
	}


    //timer.SetMonitors(ptermMgr, pdevMgr);
	unsigned int uiCheckConfigNum = 0;
    char char_write = 'W';
    while (true)
    {
#ifdef LINUX
    	if(1 == watchdog_flag)
			write(pipefd_descriptors[PIPE_OUTPUT],&char_write,1);
#endif	
        Utils::Sleep(1000*MAIN_THREAD_WHILE_SLEEP_SECOND);
        //event_loop->WakeUpEvent();
        if (ptermMgr != NULL)
        {
            ptermMgr->OnTimerPerSecond();
        }
        if (pdevMgr != NULL)
        {
            pdevMgr->OnTimerPerSecond();
        }

		uiCheckConfigNum++;

		if (uiCheckConfigNum == MAIN_THREAD_CHECK_CONFIG_SECOND)
		{
			CheckConfig();
			uiCheckConfigNum = 0;
		}
    }
  
    return 0;
}

const char* SUIRUI_logo = 
"   SSSSSSSSSSSSSSS UUUUUUUU     UUUUUUUUIIIIIIIIIIRRRRRRRRRRRRRRRRR   UUUUUUUU     UUUUUUUUIIIIIIIIII \n"
" SS:::::::::::::::SU::::::U     U::::::UI::::::::IR::::::::::::::::R  U::::::U     U::::::UI::::::::I \n"
"S:::::SSSSSS::::::SU::::::U     U::::::UI::::::::IR::::::RRRRRR:::::R U::::::U     U::::::UI::::::::I \n"
"S:::::S     SSSSSSSUU:::::U     U:::::UUII::::::IIRR:::::R     R:::::RUU:::::U     U:::::UUII::::::II \n"
"S:::::S             U:::::U     U:::::U   I::::I    R::::R     R:::::R U:::::U     U:::::U   I::::I   \n"
"S:::::S             U:::::D     D:::::U   I::::I    R::::R     R:::::R U:::::D     D:::::U   I::::I   \n"
" S::::SSSS          U:::::D     D:::::U   I::::I    R::::RRRRRR:::::R  U:::::D     D:::::U   I::::I   \n"
"  SS::::::SSSSS     U:::::D     D:::::U   I::::I    R:::::::::::::RR   U:::::D     D:::::U   I::::I   \n"
"    SSS::::::::SS   U:::::D     D:::::U   I::::I    R::::RRRRRR:::::R  U:::::D     D:::::U   I::::I   \n"
"       SSSSSS::::S  U:::::D     D:::::U   I::::I    R::::R     R:::::R U:::::D     D:::::U   I::::I   \n"
"            S:::::S U:::::D     D:::::U   I::::I    R::::R     R:::::R U:::::D     D:::::U   I::::I   \n"
"            S:::::S U::::::U   U::::::U   I::::I    R::::R     R:::::R U::::::U   U::::::U   I::::I   \n"
"SSSSSSS     S:::::S U:::::::UUU:::::::U II::::::IIRR:::::R     R:::::R U:::::::UUU:::::::U II::::::II \n"
"S::::::SSSSSS:::::S  UU:::::::::::::UU  I::::::::IR::::::R     R:::::R  UU:::::::::::::UU  I::::::::I \n"
"S:::::::::::::::SS     UU:::::::::UU    I::::::::IR::::::R     R:::::R    UU:::::::::UU    I::::::::I \n"
" SSSSSSSSSSSSSSS         UUUUUUUUU      IIIIIIIIIIRRRRRRRR     RRRRRRR      UUUUUUUUU      IIIIIIIIII \n";

const char* MC_logo =
"    MMMMMMMMMM   CCCCCC \n"
"   MMM MMM MMM CCC           running      : %s bit\n"
"  MMM MMM MMM CCC            compile time : %s,%s\n"
" MMM MMM MMM CCC             version      : %s\n"
"MMM MMM MMM  CCCCCCC         pid          : %ld\n";
