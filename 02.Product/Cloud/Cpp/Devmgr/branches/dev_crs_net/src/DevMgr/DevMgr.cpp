// DevMgr.cpp : �������̨Ӧ�ó������ڵ㡣
//

#ifdef WIN32
#include "stdafx.h"
#endif
#include <errno.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>  
#include <stdio.h>

#include "DevMgr.h"
#include "DBconnector.h"
#include "list_device.h"
#include "list_mediagroup.h"
#include "list_deviceconfig.h"
#include "list_deviceconfdetail.h"
#include "list_userconfdetail.h"
#include "list_conference.h"
#include "list_confreport.h"
#include "list_confparticipant.h"
#include "list_confrecord.h"
#include "list_recordfile.h"
#include "list_confliveinfo.h"
#include "list_conflivesetting.h"
#include "list_confrollcallinfo.h"
#include "list_rollcalllist.h"
#include "list_confpollinfo.h"
#include "list_polllist.h"
#include "list_datadictionary.h"
#include "parser.h"
#include "wrapper_msg.h"
#include "cbuffer.h"
#include "asyncthread.h"
#include "AutoTrace.h"

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h> 

#define REDIS_DEF
#define MYSQL_DEF

#define g_Trace 1

// ��ʱ���ʱ��5s
#define CHECK_TIME 5

// ������ʱʱ��15s
#define HEARTBEAT_TIMEOUT 15

#define TOLERANCE_TIME 20

// ÿ�ֲ������ݿ�100��
#define OP_DB_NUM 100
// �����ϴβ������ݿ�ʱ����15��
#define WRITE_DB_TIMESPAN 15

#if defined(_WIN32) || defined(_WIN64)
#define snprintf _snprintf
#endif 

//namespace Initial_Log{
//	class InitLogFile
//	{
//	public:
//		InitLogFile(){
//			char filename[256] = { 0 };
//			time_t now = time(NULL);
//			struct tm tm_now;
//#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
//			localtime_s(&tm_now, &now);
//#elif defined(LINUX) || defined(POSIX)
//			localtime_r(&now, &tm_now);
//#endif
//			snprintf(filename, sizeof(filename), "logdevmgr_%02d:%02d:%02d", tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
//			TraceLog::GetInstance().OpenLog(filename, TL_DEBUG, true);
//		}
//		~InitLogFile(){}
//	private:
//		SUIRUI_DISALLOW_COPY_AND_ASSIGN(InitLogFile);
//	};
//	static InitLogFile initlogfile;
//}

unsigned long getTickCount(void)
{
	unsigned long currentTime = 0;
#if defined(WIN32)
	currentTime = GetTickCount();
#endif
#if defined(LINUX)
	struct timeval current;
	gettimeofday(&current, NULL);
	//currentTime = current.tv_sec * 1000 + current.tv_usec / 1000;
	currentTime = current.tv_sec * 1000000 + current.tv_usec;
#endif
	return currentTime;
}

//#ifdef WIN32
//
//void PASCAL CheckHeartbeatTimerCallbackFunc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
//{
//	CDevMgr *pDevmgr = (CDevMgr *)dwUser;
//	if (pDevmgr != NULL)
//	{
//
//		typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
//		CParam* pParam;
//		pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pDevmgr->m_pMainThread);
//
//		typedef void (CDevMgr::* ACTION)(void*);
//		pDevmgr->m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
//			*pDevmgr, &CDevMgr::Handle_CheckDeviceHeartbeat, /*0*/(void*)pParam));
//	}
//	else
//	{
//		//printf("======= Win CheckHeartbeatTimerCallbackFunc =====pDevmgr==NULL=====Error--->>> \n");
//		return;
//	}
//}
//#elif defined LINUX
//// ��ʱ�ص�������������ʱ����豸����
//void CheckHeartbeatTimerCallbackFunc(union sigval v)
//{
//	CDevMgr * pDevmgr = (CDevMgr *) (v.sival_ptr);
//	if (pDevmgr->m_pMainThread == NULL)
//	{
//		//printf("======pDevmgr->m_pMainThread == NULL====-return--->>>>\n");
//		return;
//	}
//
//	typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
//	CParam* pParam;
//	pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0,NULL, 0, NULL, 0, pDevmgr->m_pMainThread);
//
//	typedef void (CDevMgr::* ACTION)(void*);
//	pDevmgr->m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
//		*pDevmgr, &CDevMgr::Handle_CheckDeviceHeartbeat, /*0*/(void*)pParam));
//}
//#endif
//

void CDevMgr::GetSoftwareVersion()
{
	m_softwareversion.clear();
	
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
				// ������ע����
				if (strline[0] != '#'
					&& strline[0] != ';')
				{
					int pos;
					pos = strline.find(strversionnname);
					if (pos != -1)
					{
						// ����Ľ���ȡ���ڱ���ű�����version���ݸ�ʽ

						int itmppos = pos + strversionnname.length();
						if (strline[itmppos] == ':')
						{
							if (strline[itmppos + 1] == ' ')
							{
								m_softwareversion = strline.substr(itmppos + 2, strline.length());
								break;
							}
							else
							{
								m_softwareversion = strline.substr(itmppos + 1, strline.length());
								break;
							}
						}
						else if (strline[itmppos] == ' ' && strline[itmppos + 1] == ':')
						{
							m_softwareversion = strline.substr(itmppos + 2, strline.length());
							break;
						}
					}
				}
			}
		}
	}

	infver.close();
}


void CDevMgr::LoadSystemDomainInfo()
{
	// ÿ�μ��������ļ����������д��
	m_mapSystemDomainInfo.clear();

	char cHosts_orig[256] = { 0 };
	char *sep1 = { " " };

	ifstream inf;
#ifdef WIN32
	sprintf(cHosts_orig, "%s", "C:\\Windows\\System32\\drivers\\etc\\hosts");
#elif defined LINUX
	sprintf(cHosts_orig, "%s", "/etc/hosts");
#endif

	inf.open(cHosts_orig);

	string strline;
	strline.clear();

	if (inf.is_open())
	{
		while (getline(inf, strline))
		{
			//printf("%s\n", strline.c_str());

			if (strline.length() > 0)
			{
				// ������ע����
				if (strline[0] != '#')
				{
					std::string strDomainname;
					std::string strDomainip;
					strDomainname.clear();
					strDomainip.clear();
					//char cDomainip[128] = { 0 };
					//char cDomainname[128] = { 0 };
					unsigned int uicount = 0;
					char *token = strtok((char*)strline.c_str(), sep1);
					while (token != NULL)
					{
						//str = token;
						printf("%s\n", token);

						uicount++;
						// ip
						if (uicount == 1)
						{
							//sprintf(cDomainip, "%s", token);
							strDomainip = token;
						}
						// ����
						if (uicount == 2)
						{
							//sprintf(cDomainname, "%s", token);
							strDomainname = token;
							break;
						}
						token = strtok(NULL, sep1);
					}

					//if (strlen(cDomainname) > 0)
					if (strDomainname.length() > 0)
					{
						//printf("<cDomainname:%s, cDomainip:%s>\n", cDomainname, cDomainip);
						sr_printf(SR_LOGLEVEL_ALERT, "<cDomainname:%s, cDomainip:%s>\n", strDomainname.c_str(), strDomainip.c_str());

						m_mapSystemDomainInfo.insert(std::make_pair(strDomainname, strDomainip));
					}
				}
				else
				{
					printf("this line frist char is #,so do nothing.\n");
				}
			}
		}
	}

	inf.close();
}

int CDevMgr::UpDomainIPToHostsFile(char* newdomainip, char* domainname)
{
	bool bfind = false;
	bool bhavechange = false;

	char cHosts_orig[256] = { 0 };
	char cHosts_bak[256] = { 0 };

	ifstream inf;
	ofstream outf;
#ifdef WIN32
	sprintf(cHosts_orig, "%s", "C:\\Windows\\System32\\drivers\\etc\\hosts");
	sprintf(cHosts_bak, "%s", "D:\\hosts");
#elif defined LINUX
	sprintf(cHosts_orig, "%s", "/etc/hosts");
	sprintf(cHosts_bak, "%s", "./hosts");
#endif

	inf.open(cHosts_orig);
	outf.open(cHosts_bak);

	string strline;
	strline.clear();

	string strnewdomainip;
	strnewdomainip.assign(newdomainip);
	string strdomainname;
	strdomainname.assign(domainname);

	if (inf.is_open()
		&& outf.is_open())
	{
		while (getline(inf, strline))
		{
			//printf("%s\n", strline.c_str());

			if (strline.length() > 0
				&& strdomainname.length() > 0)
			{
				// ������ע����
				if (strline[0] != '#')
				{
					int pos;
					pos = strline.find(strdomainname);
					if (pos != -1
						&& pos >= 1)
					{
						int itmppos = pos + strdomainname.length();
						sr_printf(SR_LOGLEVEL_ALERT, "UpDomainIPToHostsFile find newdomainname(%s) in strline(%s) start at strline[%d]=%c and compare newdomainname len get next char is strline[%d]=%c,%d\n", strdomainname.c_str(), strline.c_str(), pos, strline[pos], itmppos, strline[itmppos], strline[itmppos]);
						if ((strline[pos - 1] == ' ')
							&& (strline[itmppos] == '\0' || strline[itmppos] == ' '))
						{
							bfind = true;
							string strorigip;
							strorigip.clear();
							strorigip = strline.substr(0, pos - 1);
							sr_printf(SR_LOGLEVEL_ALERT, "UpDomainIPToHostsFile origip is:%s, len=%d\n", strorigip.c_str(), strorigip.length());

							if (0 != strorigip.compare(0, std::string::npos, strnewdomainip))
							{
								bhavechange = true;
								string strsubend;
								strsubend.clear();
								strsubend = strline.substr(pos, strline.length());
								sr_printf(SR_LOGLEVEL_ALERT, "UpDomainIPToHostsFile origip is:%s,not equal strnewdomainip is:%s\n", strorigip.c_str(), strnewdomainip.c_str());

								char cTmp[256] = { 0 };
								//sprintf(cTmp, "%s %s", strnewdomainip.c_str(), strdomainname.c_str());
								sprintf(cTmp, "%s %s", strnewdomainip.c_str(), strsubend.c_str());
								strline.assign(cTmp);
							}
						}
					}
				}
				else
				{
					sr_printf(SR_LOGLEVEL_ALERT, "UpDomainIPToHostsFile this line frist char is #,so do nothing.\n");
				}
			}
			outf << strline << '\n';// �ǵ�һ��Ҫ�ӻس�����
		}
	}

	inf.close();
	outf.close();

	// ��ϵͳhosts�ļ����ҵ�������
	if (bfind)
	{
		if (bhavechange)
		{
			sr_printf(SR_LOGLEVEL_ALERT, "UpDomainIPToHostsFile domainname(%s)'s ip have change.\n", domainname);

			char cHosts_dst[256] = { 0 };
			char *sep1 = { " " };
			ifstream inf;
			ofstream outf;
#ifdef WIN32
			//sprintf(cChangeflag_f, "%s", "D:\\changeflag");
			sprintf(cHosts_bak, "%s", "D:\\hosts");
			sprintf(cHosts_dst, "%s", "C:\\Windows\\System32\\drivers\\etc\\hosts");
#elif defined LINUX
			//sprintf(cChangeflag_f, "%s", "./changeflag");
			sprintf(cHosts_bak, "%s", "./hosts");
			sprintf(cHosts_dst, "%s", "/etc/hosts");
#endif

			//outflagf << '0' << '\n';

			inf.open(cHosts_bak);
			outf.open(cHosts_dst);
			if (inf.is_open()
				&& outf.is_open())
			{
				while (getline(inf, strline))
				{
					printf("%s\n", strline.c_str());
					outf << strline << '\n';// ���ݻ�д,�ǵ�һ��Ҫ�ӻس�����
				}
			}

			inf.close();
			outf.close();

			return 1;  // ���ҵ���������ip���³ɹ�
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ALERT, "UpDomainIPToHostsFile domainname(%s)'s ip not change.\n", domainname);
			return 0; // ���ҵ�����������ipû�仯
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ALERT, "UpDomainIPToHostsFile not find domainname(%s).\n", domainname);
		return -1; // δ���ҵ�������
	}
}

int CDevMgr::ReloadNginx(char* domainname, char* regip, char* connip, unsigned int port)
{
#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif

	int iresult = -1;// -1-ʧ�ܣ� 0-�ɹ�

	char cCMD[256] = { 0 };
	sprintf(cCMD, "nginx -s reload");

#ifdef WIN32
	sr_printf(SR_LOGLEVEL_ALERT, "==ReloadNginx==--os:win32-->[domainname:%s, devregip:%s, devconnip:%s, uidevport]\n", domainname, regip, connip, port);

	iresult = system(cCMD);
#elif defined LINUX
	sr_printf(SR_LOGLEVEL_ALERT, "==ReloadNginx==--os:linux-->[domainname:%s, devregip:%s, devconnip:%s, uidevport]\n", domainname, regip, connip, port);

	pid_t status;
	status = system(cCMD);

	if (-1 == status)
	{
		sr_printf(SR_LOGLEVEL_ALERT, "==ReloadNginx==--execute-->system[%s] cmd error.\n", cCMD);
		iresult = -1;
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ALERT, "==ReloadNginx==--execute-->system[%s] cmd return status:0x%x, WIFEXITED(status)=%d, WEXITSTATUS(status)=%d\n", cCMD, status, WIFEXITED(status), WEXITSTATUS(status));

		if (WIFEXITED(status))
		{
			if (0 == WEXITSTATUS(status))
			{
				sr_printf(SR_LOGLEVEL_ALERT, "==ReloadNginx==--execute-->system[%s] cmd success.\n", cCMD);
				iresult = 0;
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ALERT, "==ReloadNginx==--execute-->system[%s] cmd fail, script exit code: %d\n", cCMD, WEXITSTATUS(status));
				iresult = -1;
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ALERT, "==ReloadNginx==--execute-->system[%s] cmd fail, script exit status = [%d]\n", cCMD, WEXITSTATUS(status));
			iresult = -1;
		}
	}
#endif

#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_DEBUG, "********CDevMgr::ReloadNginx*****time_use** is: %lf us\n", time_use);

#endif

	return iresult;

	//if (!m_pUpMsqThread)
	//	return;

	//typedef CBufferT<unsigned int, void*, void*, void*, void*, void*, void*, void*> CParam;
	//CParam* pParam;
	//pParam = new CParam(domainname, strlen(domainname), regip, strlen(regip), connip, strlen(connip), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, port);

	//typedef void (CDevMgr::* ACTION)(void*);
	//m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
	//	*this, &CDevMgr::Handle_ReloadNginx, (void*)pParam));
	//return;
}

void CDevMgr::Handle_ReloadNginx(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif

	typedef CBufferT<unsigned int, void*, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	unsigned int uidevport = (unsigned int)(pParam->m_Arg1);
	char domainname[256] = { 0 };
	char devregip[64] = { 0 };
	char devconnip[64] = { 0 };
	sprintf(domainname, "%s", (char*)pParam->m_pData1);
	sprintf(devregip, "%s", (char*)pParam->m_pData2);
	sprintf(devconnip, "%s", (char*)pParam->m_pData3);

	delete pParam;
	pParam = NULL;

	char cCMD[256] = { 0 };
	//sprintf(cCMD, "copy /y %s %s", cHosts_bak, cHosts_orig);
	sprintf(cCMD, "nginx -s reload");

#ifdef WIN32
	sr_printf(SR_LOGLEVEL_ALERT, "==Handle_ReloadNginx==--os:win32-->[domainname:%s, devregip:%s, devconnip:%s, uidevport]\n", domainname, devregip, devconnip, uidevport);
	
	system(cCMD);
#elif defined LINUX
	sr_printf(SR_LOGLEVEL_ALERT, "==Handle_ReloadNginx==--os:linux-->[domainname:%s, devregip:%s, devconnip:%s, uidevport]\n", domainname, devregip, devconnip, uidevport);

	system(cCMD);
#endif

#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_DEBUG, "********CDevMgr::Handle_ReloadNginx*****time_use** is: %lf us\n", time_use);

#endif
}
/*
int CDevMgr::CheckDomainInfoHavechange(char* newdomainip, char* domainname, int checktype)
{
	bool bfind = false;
	bool bhavechange = false;

	char cHosts_orig[256] = { 0 };
	char cHosts_bak[256] = { 0 };

	ifstream inf;
	ofstream outf;
#ifdef WIN32
	sprintf(cHosts_orig, "%s", "C:\\Windows\\System32\\drivers\\etc\\hosts");
	sprintf(cHosts_bak, "%s", "D:\\hosts");
#elif defined LINUX
	sprintf(cHosts_orig, "%s", "/etc/hosts");
	sprintf(cHosts_bak, "%s", "./hosts");
#endif

	inf.open(cHosts_orig);
	outf.open(cHosts_bak);

	string strline;
	strline.clear();

	string strnewdomainip;
	strnewdomainip.assign(newdomainip);
	string strdomainname;
	strdomainname.assign(domainname);

	if (inf.is_open()
		&& outf.is_open())
	{
		while (getline(inf, strline))
		{
			//printf("%s\n", strline.c_str());

			if (strline.length() > 0
				&& strdomainname.length() > 0)
			{
				// ������ע����
				if (strline[0] != '#')
				{
					int pos;
					pos = strline.find(strdomainname);
					if (pos != -1)
					{
						bfind = true;
						printf("find this(%s) domainname pos=%d.\n", strdomainname.c_str(), pos);
						if (pos >= 1)
						{
							string strorigip;
							strorigip.clear();
							strorigip = strline.substr(0, pos - 1);
							printf("origip is:%s, len=%d\n", strorigip.c_str(), strorigip.length());

							if (0 != strorigip.compare(0, std::string::npos, strnewdomainip))
							{
								bhavechange = true;
								printf("origip is:%s,not equal strnewdomainip is:%s\n", strorigip.c_str(), strnewdomainip.c_str());

								char cTmp[256] = { 0 };
								sprintf(cTmp, "%s %s", strnewdomainip.c_str(), strdomainname.c_str());
								strline.assign(cTmp);
							}
						}
					}
				}
				else
				{
					printf("this line frist char is #,so do nothing.\n");
				}
			}
			outf << strline << '\n';// �ǵ�һ��Ҫ�ӻس�����
		}
	}

	inf.close();
	outf.close();

	// ��ϵͳhosts�ļ����ҵ�������
	if (bfind)
	{
		if (bhavechange)
		{
			printf("domain's ip have change.\n");

			//#ifdef WIN32
			//		char cCMD[256] = { 0 };
			//		sprintf(cCMD, "copy /y %s %s", cHosts_bak, cHosts_orig);
			//		system(cCMD);
			//		//memset(cCMD, 0, sizeof(cCMD));
			//		//sprintf(cCMD, "del /f %s", cHosts_orig);
			//		//system(cCMD);
			//#elif defined LINUX
			//		system("cp -f ./hosts /etc/");// ǿ�Ƹ���
			//#endif


			//char cChangeflag_f[256] = { 0 };
			//ofstream outflagf;

			char cHosts_dst[256] = { 0 };
			char *sep1 = { " " };
			ifstream inf;
			ofstream outf;
#ifdef WIN32
			//sprintf(cChangeflag_f, "%s", "D:\\changeflag");
			sprintf(cHosts_bak, "%s", "D:\\hosts");
			sprintf(cHosts_dst, "%s", "C:\\Windows\\System32\\drivers\\etc\\hosts");
#elif defined LINUX
			//sprintf(cChangeflag_f, "%s", "./changeflag");
			sprintf(cHosts_bak, "%s", "./hosts");
			sprintf(cHosts_dst, "%s", "/etc/hosts");
#endif

			//outflagf << '0' << '\n';

			inf.open(cHosts_bak);
			outf.open(cHosts_dst);
			if (inf.is_open()
				&& outf.is_open())
			{
				while (getline(inf, strline))
				{
					printf("%s\n", strline.c_str());
					outf << strline << '\n';// ���ݻ�д,�ǵ�һ��Ҫ�ӻس�����

					// 
					if (strline.length() > 0)
					{
						// ������ע����
						if (strline[0] != '#')
						{
							std::string strDomainname;
							std::string strDomainip;
							strDomainname.clear();
							strDomainip.clear();
							unsigned int uicount = 0;
							char *token = strtok((char*)strline.c_str(), sep1);
							while (token != NULL)
							{
								printf("%s\n", token);

								uicount++;
								// ip
								if (uicount == 1)
								{
									strDomainip = token;
								}
								// ����
								if (uicount == 2)
								{
									strDomainname = token;
									break;
								}
								token = strtok(NULL, sep1);
							}

							if (strDomainname.length() > 0)
							{
								printf("<cDomainname:%s, cDomainip:%s>\n", strDomainname.c_str(), strDomainip.c_str());

								m_mapSystemDomainInfo.insert(std::make_pair(strDomainname, strDomainip));
							}
						}
					}
				}
			}

			inf.close();
			outf.close();

			return 1;  // ���ҵ���������ipҲû�仯
		}
		else
		{
			printf("domain's ip not change.\n");
			return 0; // ���ҵ�����������ipû�仯
		}
	}
	else
	{
		return -1; // δ���ҵ�������
	}
}
*/
void CDevMgr::SerialProtoMsgAndSend(DeviceConnect *pClient, int proto_msguid, const google::protobuf::Message* msg)
{
	std::string strRsp = msg->SerializeAsString();

	//����Relayͷ
	SRRELAY::S_RelayMsgHeader rheader;
	rheader.m_s = 'S';
	rheader.m_r = 'R';
	rheader.m_version = '0';
	rheader.m_channelid = '0';
	rheader.m_data_len = 8 + strRsp.length();			//mcͷ+probuf����
	rheader.m_cmdtype = 202;						//mcͷ+probuf����
	char rHeader[28] = { 0 };
	SRRELAY::packPRH(&rheader, rHeader);

	//����MCͷ
	SRMC::S_ProtoMsgHeader header;
	header.m_msguid = proto_msguid;
	header.m_msglen = strRsp.length();
	char mcHeader[8] = { 0 };
	SRMC::packPMH(&header, mcHeader);

	//�����ݰ�
	//std::string strData = buf;
	char sendbuf[1024*50] = { 0 };//��ֹ����SRMsgId_RspUpdateDeviceInfo��Ϣ(��������豸��Ϣ)Խ��,����ռ�
	memcpy(sendbuf, rHeader, 8);
	memcpy(sendbuf + 8, mcHeader, 8);
	memcpy(sendbuf + 8 + 8, strRsp.c_str(), strRsp.length());

	//���ͳ�ȥ
	if (pClient != NULL)
	{
		bool sendret = pClient->SendPktToDevConnection(sendbuf, 16 + strRsp.length(), 202);

		// ������Ϣ����־����
		SR_uint32 uiLoglevel = SR_LOGLEVEL_DEBUG;
		if (SRMsgId_RspRegister == proto_msguid)
		{
			uiLoglevel = SR_LOGLEVEL_NORMAL;
		}
		else if ((SRMsgId_RspGetDeviceInfo == proto_msguid)
			|| (SRMsgId_IndDevtoserHeart == proto_msguid)
			|| (SRMsgId_RspGetSystemCurLoad == proto_msguid))
		{
			uiLoglevel = SR_LOGLEVEL_DEBUG;
		}
		else
		{
			uiLoglevel = SR_LOGLEVEL_INFO;
		}

		if (sendret)
		{
			sr_printf(uiLoglevel, "me(server)-> other(client) CDevMgr::SerialProtoMsgAndSend success, sockptr=%p,msg contxt:%s--\n%s\n", pClient, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
		else
		{
			sr_printf(uiLoglevel, "me(server)-> other(client) CDevMgr::SerialProtoMsgAndSend failed, sockptr=%p,msg contxt:%s--\n%s\n", pClient, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
	}
}

void CDevMgr::SerialAndSendDevmgr(AcitiveConnect *pSvrConn, int proto_msguid, const google::protobuf::Message* msg)
{
	if (pSvrConn)
	{
		std::string strRsp = msg->SerializeAsString();

		//����Relayͷ
		SRRELAY::S_RelayMsgHeader rheader;
		rheader.m_s = 'S';
		rheader.m_r = 'R';
		rheader.m_version = '0';
		rheader.m_channelid = '0';
		rheader.m_data_len = 8 + strRsp.length();
		rheader.m_cmdtype = SRMSG_CMDTYPE_DevMgr_MC;
		char srHeader[28] = { 0 };
		SRRELAY::packPRH(&rheader, srHeader);

		//����protoͷ
		SRMC::S_ProtoMsgHeader header;
		header.m_msguid = proto_msguid;
		header.m_msglen = strRsp.length();
		char protoHeader[8] = { 0 };
		SRMC::packPMH(&header, protoHeader);

		//�����ݰ�
		//std::string strData = buf;
		char sendbuf[1024 * 50] = { 0 };
		memcpy(sendbuf, srHeader, 8);
		memcpy(sendbuf + 8, protoHeader, 8);
		memcpy(sendbuf + 8 + 8, strRsp.c_str(), strRsp.length());

		// me(client)->devmgr(server)
		uint16_t cmd_type = SRMSG_CMDTYPE_DevMgr_MC;
		if (Msg_ID_DEVICE_SERVER_BASE_1 == (Msg_ID_DEVICE_SERVER_BASE_1 & proto_msguid))
			cmd_type = SRMSG_CMDTYPE_DevMgr_MC;

		//���ͳ�ȥ
		bool sendret = pSvrConn->SendPktToDevmgr(sendbuf, 16 + strRsp.length(), cmd_type);

		// ������Ϣ����־����
		SR_uint32 uiLoglevel = SR_LOGLEVEL_DEBUG;
		if (SRMsgId_ReqRegister == proto_msguid)
		{
			uiLoglevel = SR_LOGLEVEL_NORMAL;
		}
		else if ((SRMsgId_IndsertodevHeart == proto_msguid))
		{
			uiLoglevel = SR_LOGLEVEL_DEBUG;
		}
		else
		{
			uiLoglevel = SR_LOGLEVEL_INFO;
		}

		if (sendret)
		{
			sr_printf(uiLoglevel, "me(client)->devmgr(server) CDevMgr::SerialAndSendDevmgr success, sockptr=%p,msg contxt:%s--\n%s\n", pSvrConn, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
		else
		{
			sr_printf(uiLoglevel, "me(client)->devmgr(server) CDevMgr::SerialAndSendDevmgr failed, sockptr=%p,msg contxt:%s--\n%s\n", pSvrConn, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}

	}
}

void CDevMgr::CheckDeviceHeartbeat()
{
	if (m_pMainThread == NULL)
	{
		//printf("======pDevmgr->m_pMainThread == NULL====-return--->>>>\n");
		return;
	}

	typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, m_pMainThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_CheckDeviceHeartbeat, (void*)pParam));
}

void CDevMgr::Handle_CheckDeviceHeartbeat(void *pArg)
{
	//��ȡ��ǰʱ�䴮
	time_t timeNow;
	struct tm *ptmNow;
	char szTime[30];
	timeNow = time(NULL);
	ptmNow = localtime(&timeNow);
	sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

#ifdef WIN32
	sr_printf(SR_LOGLEVEL_DEBUG, "======CDevMgr::Handle_CheckDeviceHeartbeat=======timeNow---> %I64d, %s\n", timeNow, szTime);
#elif defined LINUX
	sr_printf(SR_LOGLEVEL_DEBUG, "======CDevMgr::Handle_CheckDeviceHeartbeat=======timeNow---> %lld, %s\n", timeNow, szTime);
#endif

	


	// 1����������SRMsgId_IndDevtoserHeart��������������豸������������
	// ////////Indication////
	//message IndDevtoserHeart{
	//	optional string token = 1;//token����Զ��壬Ψһ����
	//}	

	if (pArg == NULL || m_devConnMgr == NULL)
	{
		assert(0);
		return;
	}

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif

	typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;
	CAsyncThread * pMainThread = (CAsyncThread *)(pParam->m_Arg1);

	std::map<DeviceConnect *, DeviceConnect *> dev_connect_map;
	m_devConnMgr->GetLatestMap(dev_connect_map); // �˴�5s����һ�Σ�ע��ú����ڲ���ʹ���ӳ�ɾ��ʱ�䣬�����ڲ��ܴ����ӳ�ɾ��ʱ��

	//std::map<SR_uint32, AcitiveConnect*> svr_connect_map;
	//m_devConnMgr->GetLatestMap(svr_connect_map);

	sr_printf(SR_LOGLEVEL_DEBUG, " Handle_CheckDeviceHeartbeat PostSend SRMsgId_IndDevtoserHeart to dev_connect_map.size = %d\n", dev_connect_map.size());
	//sr_printf(SR_LOGLEVEL_DEBUG, " Handle_CheckDeviceHeartbeat PostSend SRMsgId_IndsertodevHeart to svr_connect_map.size = %d\n", svr_connect_map.size());

	// �����¼�devmgr
	std::map<DeviceConnect *, DeviceConnect *>::iterator iter_map_dev_connect = dev_connect_map.begin();
	while (iter_map_dev_connect != dev_connect_map.end())
	{
		int iDeviceid = 0;
		iDeviceid = iter_map_dev_connect->second->GetDeviceID();

		//printf(" === Handle_CheckDeviceHeartbeat iDeviceid = %d, map_dev_connect->first = %p, map_dev_connect->second = %p\n", iDeviceid, iter_map_dev_connect->first, iter_map_dev_connect->second);

		if (iDeviceid != 0)
		{
			iter_mapDeviceHeartbeatTime iter = m_MapDeviceHeartbeatTime.find(iDeviceid);
			if (iter != m_MapDeviceHeartbeatTime.end()) // �ҵ����豸��ȡ��token
			{
				char cToken[33] = { 0 };
				strcpy(cToken, iter->second.token.c_str());
				SendHeartbeatInd(iter_map_dev_connect->second, cToken);
			}
		}

		iter_map_dev_connect++;
	}

	//// �ϼ�devmgrֻ��һ������Ҫ��svr_connect_map	
	//if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr != NULL
	//	&& m_uiMyDeviceid != 0)
	//{
	//	SendHeartBeatToDevmgr();
	//}


	// 2���������ע��ɹ����豸�����Ƿ�ʱ

	//sr_printf(SR_LOGLEVEL_DEBUG, "===Handle_CheckDeviceHeartbeat ----- m_MapDeviceHeartbeatTime.size() --->> %d\n", m_MapDeviceHeartbeatTime.size());

	iter_mapDeviceHeartbeatTime iter = m_MapDeviceHeartbeatTime.begin();
	while (iter != m_MapDeviceHeartbeatTime.end())
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_DEBUG, "==device_id--->> %d, ==-->> %I64d\n", iter->first, iter->second.time);
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_DEBUG, "==device_id--->> %d, ==-->> %lld\n", iter->first, iter->second.time);
#endif

		if (timeNow - iter->second.time >= m_nGetDevHeartbeatTimeout) // ˵������15��δ�յ����豸������
		{
			iter_mapDeviceHeartbeatTime iter_next = iter; // ���潫Ҫ��ɾ��Ԫ�صı�����
			iter_next++;

			// redis::db2::deviceid_ipport�ļ�¼��deviceid�� ip+port����redis::db3::dev_deviceidandgroupid�ļ�¼��deviceid�� groupid��
			// ��Ҫ����Mysql::device��Ӧstatus��״ֵ̬

			int deviceid = iter->first;
			unsigned int uiDeviceType = iter->second.devicetype;

			sr_printf(SR_LOGLEVEL_ERROR, "==not recv (device_id = %d, uiDeviceType = %d)  Heartbeat in 15 s ===update status = 0--->>\n", deviceid, uiDeviceType);

			UpdateDeviceInfoInDB(deviceid, uiDeviceType, pMainThread, szTime, timeNow);


			if (m_bCloseTimeoutSocket)
			{
				// �����豸��Ӧ��socket�ر�
				std::map<DeviceConnect *, DeviceConnect *>::iterator map_dev_connect_iter = dev_connect_map.begin();
				while (map_dev_connect_iter != dev_connect_map.end())
				{
					int iDeviceid = 0;
					iDeviceid = map_dev_connect_iter->second->GetDeviceID();

					if (iDeviceid == deviceid)
					{
						sr_printf(SR_LOGLEVEL_ERROR, "==not recv heartbeat DeviceOnClose--->>\n");
						//m_devConnMgr->DeviceOnClose((DeviceConnect *)(map_dev_connect_iter->second), (LPSOCKET)(map_dev_connect_iter->first));
						m_devConnMgr->DeviceOnClose((DeviceConnect *)(map_dev_connect_iter->first));

						break;
					}

					map_dev_connect_iter++;
				}
			}

			m_MapDeviceHeartbeatTime.erase(iter);

			iter = iter_next;
		
		}
		else
		{
			iter++;
		}

		
	} // while

	//// 3����ʱ��m_sConfReport��m_sDeviceConfDetail��m_sUserConfInfo�ڴ�д�����ݿ�
	//CheckWriteDB();

	//// 4����ʱ��������ļ������·����仯��������
	//CheckConfig();


#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_DEBUG, "********CDevMgr::Handle_CheckDeviceHeartbeat*****time_use** is: %lf us\n", time_use);

#endif

	delete pParam;
	pParam = NULL;

}


void CDevMgr::Handle_UpdateHttpSvrLisentState(void *pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<int, void*, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;
	int ilisentret = pParam->m_Arg1;

	// http��������ɹ�
	if (ilisentret == 0)
	{
		m_HttpSvrListenISok = true;
	}
	else
	{
		m_HttpSvrListenISok = false;
	}

	sr_printf(SR_LOGLEVEL_CRIT, "======CDevMgr::Handle_UpdateHttpSvrLisentState====--->isok: %d, ilisentret: %d\n", m_HttpSvrListenISok, ilisentret);

	delete pParam;
	pParam = NULL;
}

void CDevMgr::Handle_CmdHJGW_OPMonitor_msg(void *pArg)
{
	//��ȡ��ǰʱ�䴮
	time_t timeNow;
	struct tm *ptmNow;
	char szTime[30];
	timeNow = time(NULL);
	ptmNow = localtime(&timeNow);
	sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

#ifdef WIN32
	sr_printf(SR_LOGLEVEL_DEBUG, "======CDevMgr::Handle_CmdHJGW_OPMonitor_msg=======timeNow---> %I64d, %s\n", timeNow, szTime);
#elif defined LINUX
	sr_printf(SR_LOGLEVEL_DEBUG, "======CDevMgr::Handle_CmdHJGW_OPMonitor_msg=======timeNow---> %lld, %s\n", timeNow, szTime);
#endif

	if (pArg == NULL || m_devConnMgr == NULL)
	{
		assert(0);
		return;
	}

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif

	unsigned int  uigwid = 0;
	unsigned int uiOPtype = 0;
	std::string strplatformcode;
	strplatformcode.clear();
	std::string strdsturl;
	strdsturl.clear();
	std::string strplatformnewcode;
	strplatformnewcode.clear();
	std::string strplatformnewkey;
	strplatformnewkey.clear();

	typedef CBufferT<unsigned int, unsigned int, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;
	uigwid = pParam->m_Arg1;
	uiOPtype = pParam->m_Arg2;

	if (uigwid == 0
		|| uiOPtype == 0)
	{
		delete pParam;
		pParam = NULL;
		return;
	}

	if (pParam->m_nLen1 > 0)
	{
		strplatformcode = pParam->m_pData1;
	}
	if (pParam->m_nLen2 > 0)
	{
		strdsturl = pParam->m_pData2;
	}
	if (pParam->m_nLen3 > 0)
	{
		strplatformnewcode = pParam->m_pData3;
	}
	if (pParam->m_nLen4 > 0)
	{
		strplatformnewkey = pParam->m_pData4;
	}

	std::map<DeviceConnect *, DeviceConnect *> dev_connect_map;
	m_devConnMgr->GetLatestMap(dev_connect_map); // �˴�5s����һ�Σ�ע��ú����ڲ���ʹ���ӳ�ɾ��ʱ�䣬�����ڲ��ܴ����ӳ�ɾ��ʱ��

	sr_printf(SR_LOGLEVEL_INFO, " Handle_CmdHJGW_OPMonitor_msg cur dev_connect_map.size = %d\n", dev_connect_map.size());

	// �����¼�devmgr
	std::map<DeviceConnect *, DeviceConnect *>::iterator iter_map_dev_connect = dev_connect_map.begin();
	while (iter_map_dev_connect != dev_connect_map.end())
	{
		if (iter_map_dev_connect->second != NULL)
		{
			int iDeviceid = 0;
			iDeviceid = iter_map_dev_connect->second->GetDeviceID();

			if (iDeviceid != 0
				&& iDeviceid == uigwid)
			{
				// �㲥���������ߵ�gw
				iter_mapDeviceHeartbeatTime iter = m_MapDeviceHeartbeatTime.find(uigwid);
				if (iter != m_MapDeviceHeartbeatTime.end()) // �ҵ����豸��ȡ��token
				{
					char cToken[33] = { 0 };
					strcpy(cToken, iter->second.token.c_str());

					SRMsgs::CmdGWOPMonitor cmdgw;
					cmdgw.set_token(cToken);
					cmdgw.set_optype(uiOPtype);
					cmdgw.set_platformcode(strplatformcode.c_str());
					cmdgw.set_dsturl(strdsturl.c_str());
					cmdgw.set_platformnewcode(strplatformnewcode.c_str());
					cmdgw.set_platformnewkey(strplatformnewkey.c_str());

					SerialProtoMsgAndSend(iter_map_dev_connect->second, SRMsgId_CmdGWOPMonitor, &cmdgw);
				}
			}
		}

		iter_map_dev_connect++;
	}

#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_DEBUG, "********CDevMgr::Handle_CmdHJGW_OPMonitor_msg*****time_use** is: %lf us\n", time_use);

#endif

	delete pParam;
	pParam = NULL;

}

void CDevMgr::Handle_CmdHJGW_OPAVConfig_msg(void *pArg)
{
	//��ȡ��ǰʱ�䴮
	time_t timeNow;
	struct tm *ptmNow;
	char szTime[30];
	timeNow = time(NULL);
	ptmNow = localtime(&timeNow);
	sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

#ifdef WIN32
	sr_printf(SR_LOGLEVEL_DEBUG, "======CDevMgr::Handle_CmdHJGW_OPAVConfig_msg=======timeNow---> %I64d, %s\n", timeNow, szTime);
#elif defined LINUX
	sr_printf(SR_LOGLEVEL_DEBUG, "======CDevMgr::Handle_CmdHJGW_OPAVConfig_msg=======timeNow---> %lld, %s\n", timeNow, szTime);
#endif

	if (pArg == NULL || m_devConnMgr == NULL)
	{
		assert(0);
		return;
	}

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif

	unsigned int uiOPtype = 0; // 1 - ���ӡ�2 - ɾ����3 - �޸�
	std::string straudio;
	straudio.clear();
	std::string strvideo;
	strvideo.clear();

	typedef CBufferT<unsigned int, void*, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;
	uiOPtype = pParam->m_Arg1;

	if (uiOPtype == 0)
	{
		delete pParam;
		pParam = NULL;
		return;
	}

	if (pParam->m_nLen1 > 0)
	{
		straudio = pParam->m_pData1;
	}
	if (pParam->m_nLen2 > 0)
	{
		strvideo = pParam->m_pData2;
	}

	std::map<DeviceConnect *, DeviceConnect *> dev_connect_map;
	m_devConnMgr->GetLatestMap(dev_connect_map); // �˴�5s����һ�Σ�ע��ú����ڲ���ʹ���ӳ�ɾ��ʱ�䣬�����ڲ��ܴ����ӳ�ɾ��ʱ��

	sr_printf(SR_LOGLEVEL_INFO, " Handle_CmdHJGW_OPMonitor_msg cur dev_connect_map.size = %d\n", dev_connect_map.size());

	SRMsgs::CmdGWOPAVConfig cmdgw;
	//cmdgw.set_token(cToken);
	cmdgw.set_optype(uiOPtype);

	SRMsgs::CmdGWOPAVConfig_CapInfo* paddcapinfo = 0;

	char *sep1 = { "," };
	if (straudio.length() > 0)
	{
		paddcapinfo = cmdgw.add_capinfos();
		paddcapinfo->set_captype(1);

		SRMsgs::CmdGWOPAVConfig_Capset* paddcapset = 0;

		char *atoken = strtok((char*)straudio.c_str(), sep1);
		while (atoken != NULL)
		{
			//printf("%s\n", atoken);

			paddcapset = paddcapinfo->add_capsets();
			paddcapset->set_fmt(atoken);

			atoken = strtok(NULL, sep1);
		}

	}

	if (strvideo.length() > 0)
	{
		paddcapinfo = cmdgw.add_capinfos();
		paddcapinfo->set_captype(2);

		SRMsgs::CmdGWOPAVConfig_Capset* paddcapset = 0;

		char *vtoken = strtok((char*)strvideo.c_str(), sep1);
		while (vtoken != NULL)
		{
			//printf("%s\n", vtoken);

			paddcapset = paddcapinfo->add_capsets();
			paddcapset->set_fmt(vtoken);

			vtoken = strtok(NULL, sep1);
		}
	}

	if (paddcapinfo != 0)
	{
		// �����¼�devmgr
		std::map<DeviceConnect *, DeviceConnect *>::iterator iter_map_dev_connect = dev_connect_map.begin();
		while (iter_map_dev_connect != dev_connect_map.end())
		{
			if (iter_map_dev_connect->second != NULL)
			{
				int iDeviceid = 0;
				iDeviceid = iter_map_dev_connect->second->GetDeviceID();

				if (iDeviceid != 0)
				{
					// �㲥���������ߵ�gw
					iter_mapDeviceHeartbeatTime iter = m_MapDeviceHeartbeatTime.find(iDeviceid);
					if (iter != m_MapDeviceHeartbeatTime.end()) // �ҵ����豸��ȡ��token
					{
						if (iter->second.devicetype == DEVICE_SERVER::DEVICE_GW)
						{
							char cToken[33] = { 0 };
							strcpy(cToken, iter->second.token.c_str());

							cmdgw.set_token(cToken);

							SerialProtoMsgAndSend(iter_map_dev_connect->second, SRMsgId_CmdGWOPAVConfig, &cmdgw);
						}
					}
				}
			}

			iter_map_dev_connect++;
		}
	}


#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_DEBUG, "********CDevMgr::Handle_CmdHJGW_OPAVConfig_msg*****time_use** is: %lf us\n", time_use);

#endif

	delete pParam;
	pParam = NULL;

}

void CDevMgr::CheckWriteDB()
{
	if (!m_pUpMsqThread)
		return;

	//typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*> CParam;
	//CParam* pParam;
	//pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, m_pUpMsqThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_CheckWriteDB));
	return;
}

void CDevMgr::Handle_CheckWriteDB(void *pArg)
{
	//typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*> CParam;
	//CParam* pParam = (CParam*)pArg;

	if (!m_pWriteDBThread)
		return;

	typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, m_pWriteDBThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_WriteUserConfInfoToDB, (void*)pParam));

	//printf(" 55555555 Handle_CheckWriteDB  post Handle_WriteUserConfInfoToDB msg 5555555555\n");
}

void CDevMgr::CheckConfig()
{
	//if (!m_pUpConfigThread)
	//	return;

	//typedef void (CDevMgr::* ACTION)(void*);
	//m_pUpConfigThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
	//	*this, &CDevMgr::Handle_CheckConfig));

	if (!m_pMainThread)
		return;

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_CheckConfig));
	return;
}

void CDevMgr::Handle_CheckConfig(void *pArg)
{

	// ���¶�ȡ�����ļ�����
	CParser::GetInstance()->Parser();

	unsigned int uiOpDbNum = CParser::GetInstance()->GetOPDBNum();
	if (uiOpDbNum != m_op_db_num)
	{
		sr_printf(SR_LOGLEVEL_ALERT, "==Config is changed==op_db_num--- %d  --->> %d \n", m_op_db_num, uiOpDbNum);
		m_op_db_num = uiOpDbNum;
	}

	unsigned int uiNewDevHeartbeatTimeout = CParser::GetInstance()->GetDevHeartbeatTimeout();
	if (uiNewDevHeartbeatTimeout != m_nGetDevHeartbeatTimeout)
	{
		sr_printf(SR_LOGLEVEL_ALERT, "==Config is changed==DevHeartbeatTimeout--- %d s --->> %d s\n", m_nGetDevHeartbeatTimeout, uiNewDevHeartbeatTimeout);
		m_nGetDevHeartbeatTimeout = uiNewDevHeartbeatTimeout;
	}

	bool bisCloseTimeoutSocket = CParser::GetInstance()->IsCloseTimeoutSocket();
	if (bisCloseTimeoutSocket != m_bCloseTimeoutSocket)
	{
		sr_printf(SR_LOGLEVEL_ALERT, "==Config is changed==IsCloseTimeoutSocket--- %d --->> %d\n", m_bCloseTimeoutSocket, bisCloseTimeoutSocket);
		m_bCloseTimeoutSocket = bisCloseTimeoutSocket;
	}

	int iNewLevel = CParser::GetInstance()->GetLoglevel();

	if (iNewLevel != m_iLoglevel) // ��������ļ�����־�������仯����������־����
	{
		sr_printf(SR_LOGLEVEL_ALERT, "==Config is changed==SetLogLevel(m_iLoglevel)--- %d --->> %d\n", m_iLoglevel, iNewLevel);

		m_iLoglevel = iNewLevel;
		TraceLog::GetInstance().SetLogLevel(m_iLoglevel);
	}

	bool bisStdout = CParser::GetInstance()->GetIsStdout();

	if (bisStdout != m_bisStdout)
	{
		sr_printf(SR_LOGLEVEL_ALERT, "==Config is changed==EnabledStdout(m_bisStdout)--- %d --->> %d\n", m_bisStdout, bisStdout);

		m_bisStdout = bisStdout;
		TraceLog::GetInstance().EnabledStdout(m_bisStdout);
	}

	return;
}

void CDevMgr::CheckDBConn()
{
	if (!m_pMainThread)
		return;

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_CheckDBConn));
	return;
}


void CDevMgr::Handle_CheckDBConn(void *pArg)
{
	// ���redis����
	char redis_ip[64] = { 0 };
	sprintf(redis_ip, "%s", CParser::GetInstance()->GetRedisIpAddr().c_str());
	unsigned int redis_port = 0;
	redis_port = CParser::GetInstance()->GetRedisPort();

	if (m_pMainThread)
	{
		typedef CBufferT<int, unsigned int, void*, void*, void*, void*, void*, void*> CParam;
		CParam* pParam;
		pParam = new CParam(redis_ip, strlen(redis_ip), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, e_RC_TT_MainThread, redis_port);


		typedef void (CDevMgr::* ACTION)(void*);
		m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*this, &CDevMgr::Handle_CheckRedisConn, (void*)pParam));

		//sr_printf(SR_LOGLEVEL_ALERT, "==Handle_CheckDBConn--- e_RC_TT_MainThread --->>Handle_CheckRedisConn\n");

	}
	if (m_pUpMsqThread)
	{
		typedef CBufferT<int, unsigned int, void*, void*, void*, void*, void*, void*> CParam;
		CParam* pParam;
		pParam = new CParam(redis_ip, strlen(redis_ip), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, e_RC_TT_UpMsqThread, redis_port);

		typedef void (CDevMgr::* ACTION)(void*);
		m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*this, &CDevMgr::Handle_CheckRedisConn, (void*)pParam));

		//sr_printf(SR_LOGLEVEL_ALERT, "==Handle_CheckDBConn--- e_RC_TT_UpMsqThread --->>Handle_CheckRedisConn\n");
	}
	if (m_pWriteDBThread)
	{
		typedef CBufferT<int, unsigned int, void*, void*, void*, void*, void*, void*> CParam;
		CParam* pParam;
		pParam = new CParam(redis_ip, strlen(redis_ip), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, e_RC_TT_WriteDBThread, redis_port);

		typedef void (CDevMgr::* ACTION)(void*);
		m_pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*this, &CDevMgr::Handle_CheckRedisConn, (void*)pParam));


		//sr_printf(SR_LOGLEVEL_ALERT, "==Handle_CheckDBConn--- e_RC_TT_WriteDBThread --->>Handle_CheckRedisConn\n");
	}

	return;
}

void CDevMgr::Handle_CheckRedisConn(void *pArg)
{

	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<int, unsigned int, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	int ithreadtype = (int)(pParam->m_Arg1);
	unsigned int redis_port = (unsigned int)(pParam->m_Arg2);
	char* redis_ip = (char*)(pParam->m_pData1);

	//sr_printf(SR_LOGLEVEL_ALERT, "==Handle_CheckRedisConn--- ithreadtype=%d \n", ithreadtype);

	if (!(ithreadtype == e_RC_TT_MainThread
		|| ithreadtype == e_RC_TT_UpMsqThread
		|| ithreadtype == e_RC_TT_WriteDBThread))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "Handle_CheckRedisConn threadtype=%d error.\n", ithreadtype);

		delete pParam;
		pParam = NULL;
		return;
	}

	if (m_pRedisConnList[ithreadtype])
	{
		m_pRedisConnList[ithreadtype]->pingpong();

		if (m_pRedisConnList[ithreadtype]->isconnectok() == false)
		{
			m_pRedisConnList[ithreadtype]->connect(redis_ip, redis_port);
		}
	}

	delete pParam;
	pParam = NULL;
}

void CDevMgr::UpdateDeviceInfoInDB(int device_id, int device_type, CAsyncThread *pThread, char* ptime, time_t lltime)
{
	if (device_id == 0 || pThread == NULL)
	{
		return;
	}

	int deviceid = device_id;
	int devicetype = device_type;
	std::string strTime = ptime;

	CDevice* pDevice = NULL;
	std::map<CAsyncThread*, CDevice*>::iterator iter_mapAsyncThreadDevice = m_mapAsyncThreadDevice.find(pThread);
	if (iter_mapAsyncThreadDevice != m_mapAsyncThreadDevice.end())
	{
		pDevice = iter_mapAsyncThreadDevice->second;
	}


	if (pDevice == NULL)
	{
		return;
	}


	//time_t timeNow;
	//struct tm *ptmNow;
	//char szTime[30];
	//timeNow = time(NULL);
	//ptmNow = localtime(&timeNow);

	if (!m_pUpMsqThread)
		return;

	if (ptime == NULL)
		return;

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif 

	char traceBuf[1024] = { 0 };

	// һ �������豸device_id����device�豸״ֵ̬
	pDevice->SetDeviceID(deviceid);
	pDevice->SetStatus(0);
	if (pDevice->UpdateDB3())
	{
		sr_printf(SR_LOGLEVEL_INFO, "====CDevMgr::UpdateDeviceInfoInDB==update Mysql::device == (device_id = %d) status--->>0\n", deviceid);
	}
	else
	{
		sr_printf(SR_LOGLEVEL_INFO, "====CDevMgr::UpdateDeviceInfoInDB==update Mysql::device == (device_id = %d) status---- error !!!----->>0\n", deviceid);
	}

	if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
	{
		char deviceid_id[256] = { 0 };
		sprintf(deviceid_id, "deviceid_%d", deviceid);
		char deviceidbuf[128] = { 0 };
		sprintf(deviceidbuf, "%d", deviceid);

		// ��ŵ�ǰmc����netmp����confid
		RedisReplyArray vrra_db2_deviceid_devid;
		vrra_db2_deviceid_devid.clear();
		bool bhashHGetAll_ok = false;
		unsigned int nConfAndDevidNum = 0;

		// ��ŵ�ǰmcͬ�������netmp�µ�confid(ֻ���ڵ�ǰ�豸��mcʱ�Ż�ȡ)
		std::map< int, std::vector<unsigned long long> > tempmapRelatedNetmpidConfidset;
		tempmapRelatedNetmpidConfidset.clear();

		UpdateDeviceInfoData *pUpdateDeviceInfoData = NULL;


		// 1����ȡmc��netmp������Ϣ
		if (devicetype == DEVICE_SERVER::DEVICE_MC
			|| devicetype == DEVICE_SERVER::DEVICE_NETMP) //  device_conf_detail��ֻ����mc��netmp�����Ӧ������Ϣ
		{
			sr_printf(SR_LOGLEVEL_INFO, "=mc/netmp=CDevMgr::UpdateDeviceInfoInDB==update Mysql::device_conf_detail deviceid = %d realuseStoptime\n", deviceid);

			m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);
			bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(deviceid_id, vrra_db2_deviceid_devid);
			nConfAndDevidNum = vrra_db2_deviceid_devid.size();

			if (bhashHGetAll_ok
				&& (nConfAndDevidNum > 0)
				&& (nConfAndDevidNum % 2 == 0))
			{
				sr_printf(SR_LOGLEVEL_DEBUG, " ok mc/netmp CDevMgr::UpdateDeviceInfoInDB gethashdbdata redis db(2) Table:%s (bhashHGetAll_ok, nConfAndDevidNum)=(%d, %d) \n", deviceid_id, bhashHGetAll_ok, nConfAndDevidNum);


				// ��mc�����쳣����Ҫ�Ը�mc���ڻ����ϵ�netmp��Ӧ�������ʱ����и���
				if (devicetype == DEVICE_SERVER::DEVICE_MC)
				{
					// 1.1��ͨ��deviceid�ҵ�ͬһ��groupid�»��ŵ�netmp,��ͨ��db(1)��ö�Ӧnetmpid��
					// 1.2�������û�õ�netmpid��db(2)�ı�device_netmpid�������еĻ��飻
					// 1.3���ڱ�device_netmpid�������еĻ������ҵ������쳣mc�Ļ���id��Ȼ�����ø�netmp��deviceid��confid��mysql���в��������ҽ�device_netmpid��Ӧ�ļ�¼���
					m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
					char * pgroupid = m_pRedisConnList[e_RC_TT_MainThread]->getvalue("dev_deviceidandgroupid", deviceidbuf);
					if (pgroupid != NULL)
					{
						std::string netmp_groupid = "netmp_";
						netmp_groupid += pgroupid;

						delete pgroupid; // ��ɾ���ᵼ���ڴ�й©

						// ��ǰmcͬ���netmp��Ϣ[netmpid,heartbeat]
						RedisReplyArray vrra_db1_netmp_groupid;
						vrra_db1_netmp_groupid.clear();
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);
						bool bHGetAllNetmpBeat_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(netmp_groupid.c_str(), vrra_db1_netmp_groupid);

						unsigned int nNetmpidandBeatNum = vrra_db1_netmp_groupid.size();

						if (bHGetAllNetmpBeat_ok
							&& (nNetmpidandBeatNum > 0)
							&& (nNetmpidandBeatNum % 2 == 0))
						{
							sr_printf(SR_LOGLEVEL_DEBUG, " ok mc -> netmp CDevMgr::UpdateDeviceInfoInDB gethashdbdata redis db(1) Table:%s (bHGetAllNetmpBeat_ok, nNetmpidandBeatNum)=(%d, %d) \n", netmp_groupid.c_str(), bHGetAllNetmpBeat_ok, nNetmpidandBeatNum);

							for (int j = 0; j < nNetmpidandBeatNum;)
							{
								char netmpid_value[128] = { 0 };
								char heartbeat_value[128] = { 0 };
								sprintf(netmpid_value, "%s", vrra_db1_netmp_groupid[j].str.c_str());
								sprintf(heartbeat_value, "%s", vrra_db1_netmp_groupid[j + 1].str.c_str());

								char deviceid_netmpid[256] = { 0 };
								int iNetmpid = 0;
								iNetmpid = atoi(netmpid_value);//
								sprintf(deviceid_netmpid, "deviceid_%d", iNetmpid);

								RedisReplyArray vrra_db2_deviceid_netmpid;
								vrra_db2_deviceid_netmpid.clear();

								m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);
								bool bHGetAllNetmpConf_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(deviceid_netmpid, vrra_db2_deviceid_netmpid);
								unsigned int nNetmpConfAndDevidNum = vrra_db2_deviceid_netmpid.size();

								if (bHGetAllNetmpConf_ok
									&& (nNetmpConfAndDevidNum > 0)
									&& (nNetmpConfAndDevidNum % 2 == 0))
								{
									sr_printf(SR_LOGLEVEL_DEBUG, " ok mc -> netmp CDevMgr::UpdateDeviceInfoInDB gethashdbdata redis db(2) Table:%s (bHGetAllNetmpConf_ok, nNetmpConfAndDevidNum)=(%d, %d) \n", deviceid_netmpid, bHGetAllNetmpConf_ok, nNetmpConfAndDevidNum);

									std::vector<unsigned long long> vConfidset;
									vConfidset.clear();

									for (int k = 0; k < nNetmpConfAndDevidNum;)
									{
										char confid_value[128] = { 0 };
										char netmpid_value[128] = { 0 };
										unsigned long long ullConfid = 0;
										sprintf(confid_value, "%s", vrra_db2_deviceid_netmpid[k].str.c_str());
										sprintf(netmpid_value, "%s", vrra_db2_deviceid_netmpid[k + 1].str.c_str());

#ifdef WIN32
										ullConfid = _atoi64(confid_value);//
#elif defined LINUX
										ullConfid = strtoll(confid_value, NULL, 10);//
#endif

										vConfidset.push_back(ullConfid);
										k += 2;
									}

									if (iNetmpid != 0
										&& vConfidset.size() > 0)
									{
										tempmapRelatedNetmpidConfidset.insert(std::map< int, std::vector<unsigned long long> >::value_type(iNetmpid, vConfidset));
									}
								}
								else
								{
									sr_printf(SR_LOGLEVEL_ERROR, " mc -error-> netmp CDevMgr::UpdateDeviceInfoInDB gethashdbdata redis db(2) Table:%s (bHGetAllNetmpConf_ok, nNetmpConfAndDevidNum)=(%d, %d) \n", deviceid_netmpid, bHGetAllNetmpConf_ok, nNetmpConfAndDevidNum);
								}

								j += 2;
							}
						}
						else
						{
							sr_printf(SR_LOGLEVEL_ERROR, " mc -error-> netmp CDevMgr::UpdateDeviceInfoInDB gethashdbdata redis db(1) Table:%s (bHGetAllNetmpBeat_ok, nNetmpidandBeatNum)=(%d, %d) \n", netmp_groupid.c_str(), bHGetAllNetmpBeat_ok, nNetmpidandBeatNum);
						}
					}
					else
					{
						sr_printf(SR_LOGLEVEL_ERROR, " mc -error- > netmp_ CDevMgr::UpdateDeviceInfoInDB getvalue groupid redis db(3) from Table:dev_deviceidandgroupid (deviceid)=(%s) return NULL error!!! \n", deviceidbuf);
					}
				}
			}
			else
			{
				// ��db2 deviceid_##deviceid���л�ȡ[confid,deviceid]��ֵ��ʧ��
				sr_printf(SR_LOGLEVEL_ERROR, " error mc/netmp CDevMgr::UpdateDeviceInfoInDB gethashdbdata redis db(2) Table:%s (bhashHGetAll_ok, nConfAndDevidNum)=(%d, %d) \n", deviceid_id, bhashHGetAll_ok, nConfAndDevidNum);
			}
		}

		// 2��ɾ�����豸��Ӧ��load��������groupid��¼
		std::string svrname_groupid;
		svrname_groupid.clear();
		if (devicetype == DEVICE_SERVER::DEVICE_MC)
		{
			svrname_groupid = "mc_";
		}
		else if (devicetype == DEVICE_SERVER::DEVICE_NETMP)
		{
			svrname_groupid = "netmp_";
		}
		else if (devicetype == DEVICE_SERVER::DEVICE_RELAY_MC)
		{
			svrname_groupid = "relaymc_";
		}
		else if (devicetype == DEVICE_SERVER::DEVICE_RELAYSERVER)
		{
			svrname_groupid = "relayserver_";
		}
		else if (devicetype == DEVICE_SERVER::DEVICE_MP)
		{
			svrname_groupid = "mp_";
		}
		else if (devicetype == DEVICE_SERVER::DEVICE_GW)
		{
			svrname_groupid = "gw_";
		}
		else if (devicetype == DEVICE_SERVER::DEVICE_DEV)
		{
			svrname_groupid = "devmgr_";
		}
		else if (devicetype == DEVICE_SERVER::DEVICE_CRS)
		{
			svrname_groupid = "crs_";
		}
		else if (devicetype == DEVICE_SERVER::DEVICE_STUNSERVER)
		{
			svrname_groupid = "stunserver_";
		}

		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
		char * p = m_pRedisConnList[e_RC_TT_MainThread]->getvalue("dev_deviceidandgroupid", deviceidbuf);
		if (p != NULL)
		{
			svrname_groupid += p;
			m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);
			if (devicetype == DEVICE_SERVER::DEVICE_MC)
			{
				// mc�ĸ�����Ϣ��HASH�ṹ,�����ű�ɾ����
				char svrname_load_s[128] = { 0 };
				sprintf(svrname_load_s, "%s_%s", svrname_groupid.c_str(), deviceidbuf);
				m_pRedisConnList[e_RC_TT_MainThread]->deletevalue(svrname_load_s);
			}
			else
			{
				// �����豸�ĸ�����Ϣ��ZSET�ṹ
				m_pRedisConnList[e_RC_TT_MainThread]->deletevalue(svrname_groupid.c_str(), deviceidbuf);
			}

			m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);
			m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue(svrname_groupid.c_str(), deviceidbuf);

			m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
			m_pRedisConnList[e_RC_TT_MainThread]->deletevalue("dev_deviceidandgroupid", deviceidbuf);

			delete p; // ��ɾ���ᵼ���ڴ�й©

			sr_printf(SR_LOGLEVEL_DEBUG, " CDevMgr::UpdateDeviceInfoInDB getvalue groupid redis db(3) from Table:dev_deviceidandgroupid (deviceid, group_id)=(%s, %s) then deletevalue to redis db(0) db(1) db(3)\n", deviceidbuf, svrname_groupid.c_str());
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, " CDevMgr::UpdateDeviceInfoInDB getvalue groupid redis db(3) from Table:dev_deviceidandgroupid (deviceid)=(%s) return NULL error!!! \n", deviceidbuf);
		}

		// 3��ɾ��db3  device_ipport_##deviceidbuf��,��������Ϣ��HASH�ṹ,�����ű�ɾ����
		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
		char device_ipport_deviceid_s[256] = { 0 };
		sprintf(device_ipport_deviceid_s, "device_ipport_%s", deviceidbuf);
		m_pRedisConnList[e_RC_TT_MainThread]->deletevalue(device_ipport_deviceid_s);

		sr_printf(SR_LOGLEVEL_DEBUG, " CDevMgr::UpdateDeviceInfoInDB deletevalue redis db(3)  Table:%s \n", device_ipport_deviceid_s);

		// 4��ɾ��db3 local_domain_deviceinfo�ж�Ӧ����Ϣ,ͬʱ֪ͨ�ϼ�devmgr
		if (devicetype == DEVICE_SERVER::DEVICE_RELAY_MC
			|| devicetype == DEVICE_SERVER::DEVICE_RELAYSERVER
			|| devicetype == DEVICE_SERVER::DEVICE_STUNSERVER)
		{
			m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue("local_domain_deviceinfo", deviceidbuf);

			sr_printf(SR_LOGLEVEL_DEBUG, " CDevMgr::UpdateDeviceInfoInDB deletehashvalue redis db(3)  Table:local_domain_deviceinfo key->%s \n", device_ipport_deviceid_s, deviceidbuf);

			// ��ȡ��������
			RedisReplyArray vrra_domain_level;
			vrra_domain_level.clear();
			unsigned int nDomainAndLevel = 0;

			bool bHGetAllDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_domain_level);

			nDomainAndLevel = vrra_domain_level.size();

			if (bHGetAllDomain_ok
				&& (nDomainAndLevel > 0)
				&& (nDomainAndLevel % 2 == 0))
			{
				for (int idx = 0; idx < nDomainAndLevel;)
				{
					int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
					//vrra_domain_level[idx].str;// ����
					iLevel = atoi(vrra_domain_level[idx + 1].str.c_str());// level
					if (iLevel == 0)
					{
						if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr
							&& vrra_domain_level[idx].str.length() > 0)
						{
							SRMsgs::IndSubSvrInfoToDevmgr indsvrinfo;
							indsvrinfo.set_addordel(2);
							indsvrinfo.set_deviceid(m_uiMyDeviceid);
							indsvrinfo.set_token(m_szMyToken);

							SRMsgs::IndSubSvrInfoToDevmgr_DomainInfo* psvrdomaininfo = 0;
							{
								psvrdomaininfo = indsvrinfo.add_domaininfos();
								psvrdomaininfo->set_domainname(vrra_domain_level[idx].str);
								psvrdomaininfo->set_level(iLevel);

								SRMsgs::IndSubSvrInfoToDevmgr_ServerInfo* pSvrinfo = 0;
								{
									pSvrinfo = psvrdomaininfo->add_svrinfos();
									pSvrinfo->set_svrtype(devicetype);
									pSvrinfo->set_deviceid(deviceid);

									//SRMsgs::IndSubSvrInfoToDevmgr_IPPORTInfo* pSvripportinfo = new SRMsgs::IndSubSvrInfoToDevmgr_IPPORTInfo();
									//pSvripportinfo->set_inner(inner_ipprort_value);
									//pSvripportinfo->set_outer(outer_ipprort_value);
									//pSvrinfo->set_allocated_ipport(pSvripportinfo);
								}
							}

							SerialAndSendDevmgr((AcitiveConnect*)(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr), getMsgIdByClassName(IndSubSvrInfoToDevmgr), &indsvrinfo);
						}

						// ��(��)��������Ϊ��
						if (vrra_domain_level[idx].str.length() > 0)
						{
							SRMsgs::IndUpSvrInfoToDevmgr indsvrinfo;
							indsvrinfo.set_addordel(2);
							//indsvrinfo.set_deviceid(pDevice->GetDeviceID());// �¼�devmgr��id
							//indsvrinfo.set_token(pDevice->GetDeviceToken());// �¼�devmgr��token
							SRMsgs::IndUpSvrInfoToDevmgr_DomainInfo* psvrdomaininfo = 0;
							{
								psvrdomaininfo = indsvrinfo.add_domaininfos();
								psvrdomaininfo->set_domainname(vrra_domain_level[idx].str);
								psvrdomaininfo->set_level(iLevel);// ������ǿ��д0

								SRMsgs::IndUpSvrInfoToDevmgr_ServerInfo* pSvrinfo = 0;
								{
									pSvrinfo = psvrdomaininfo->add_svrinfos();
									pSvrinfo->set_svrtype(devicetype);
									pSvrinfo->set_deviceid(deviceid);

									//SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo* pSvripportinfo = new SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo();
									//pSvripportinfo->set_inner(inner_ipprort_value);
									//pSvripportinfo->set_outer(outer_ipprort_value);
									//pSvrinfo->set_allocated_ipport(pSvripportinfo);
								}
							}

							// 2������¼�devmgrָ��
							for (iter_mapDeviceHeartbeatTime dev_itor = m_MapDeviceHeartbeatTime.begin();
								dev_itor != m_MapDeviceHeartbeatTime.end(); dev_itor++)
							{
								if (dev_itor->second.devicetype == DEVICE_SERVER::DEVICE_DEV)
								{
									if (dev_itor->second.pClient)
									{
										indsvrinfo.set_deviceid(dev_itor->first);// �¼�devmgr��id
										indsvrinfo.set_token(dev_itor->second.token);// �¼�devmgr��token

										SerialProtoMsgAndSend(dev_itor->second.pClient, getMsgIdByClassName(IndUpSvrInfoToDevmgr), &indsvrinfo);
									}
								}
							}
						}

						break;
					}
					idx += 2;
				}
			}
		}
		

		// 4��ɾ��db2  conference_deviceid����conference��ؼ�¼���磺�Ȼ�ȡdeviceid_%d�������л��飨�������������id����ͨ������idɾ����conference_deviceid�ж�Ӧ��¼
		// ע�� conference_deviceid��ֻ����conferenceid��mcid��Ӧ��ϵ������ֻ��MC�쳣����Ŷ�conference_deviceid������
		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);
		if (devicetype == 1
			|| devicetype == 2) // mc
		{
			if (bhashHGetAll_ok
				&& (nConfAndDevidNum > 0)
				&& (nConfAndDevidNum % 2 == 0))
			{
				pUpdateDeviceInfoData = new UpdateDeviceInfoData();

				std::vector<unsigned long long> vConfidset;
				vConfidset.clear();

				for (int i = 0; i < nConfAndDevidNum;)
				{
					char confid_value[128] = { 0 };
					char deviceid_value[128] = { 0 };
					unsigned long long ullConfid = 0;

					sprintf(confid_value, "%s", vrra_db2_deviceid_devid[i].str.c_str());
					sprintf(deviceid_value, "%s", vrra_db2_deviceid_devid[i + 1].str.c_str());

#ifdef WIN32
					ullConfid = _atoi64(confid_value);//
#elif defined LINUX
					ullConfid = strtoll(confid_value, NULL, 10);//
#endif

					vConfidset.push_back(ullConfid);

					if (devicetype == 1)
					{
						m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue("conference_deviceid", confid_value);

						sr_printf(SR_LOGLEVEL_DEBUG, " mc CDevMgr::UpdateDeviceInfoInDB deletehashvalue redis db(2)  Table:conference_deviceid (conference,deviceid)=(%s, %s) \n", confid_value, deviceid_value);

						// 4.1�����˵�ǰ�쳣mcͬ�鲢��ͬ�����netmp�µĻ�����Ϣ
						if (tempmapRelatedNetmpidConfidset.size() > 0)
						{
							std::map< int, std::vector<unsigned long long> >::iterator tempmapRelatedNetmpidConfidset_iter = tempmapRelatedNetmpidConfidset.begin();
							while (tempmapRelatedNetmpidConfidset_iter != tempmapRelatedNetmpidConfidset.end())
							{
								std::vector<unsigned long long> vRelatedConfidset;
								vRelatedConfidset.clear();

								std::vector<unsigned long long>::iterator confid_iter = (tempmapRelatedNetmpidConfidset_iter->second).begin();
								while (confid_iter != (tempmapRelatedNetmpidConfidset_iter->second).end())
								{
									if (ullConfid == (*confid_iter)) // ͨ������id���й���
									{
										vRelatedConfidset.push_back(ullConfid);

										// ����Ӧ�ļ�¼��redis db(2) device_netmpid�������
										char deviceid_netmpid[256] = { 0 };
										sprintf(deviceid_netmpid, "deviceid_%d", tempmapRelatedNetmpidConfidset_iter->first);
										m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue(deviceid_netmpid, confid_value);

										sr_printf(SR_LOGLEVEL_DEBUG, " mc -> netmp CDevMgr::UpdateDeviceInfoInDB deletehashvalue redis db(2) netmp Table:%s (confid, deviceid)=(%s, %d) \n", deviceid_netmpid, confid_value, tempmapRelatedNetmpidConfidset_iter->first);
									}

									confid_iter++;
								}

								if (tempmapRelatedNetmpidConfidset_iter->first != 0
									&& vRelatedConfidset.size() > 0)
								{
									pUpdateDeviceInfoData->m_mapRelatedDeviceidandConfidset.insert(std::map< int, std::vector<unsigned long long> >::value_type(tempmapRelatedNetmpidConfidset_iter->first, vRelatedConfidset));
								}

								tempmapRelatedNetmpidConfidset_iter++;
							}
						}// 4.1�����˵�ǰ�쳣mcͬ�鲢��ͬ�����netmp�µĻ�����Ϣ
					}					

					i += 2;
				}

				if (deviceid != 0
					&& vConfidset.size() > 0)
				{
					pUpdateDeviceInfoData->m_mapDeviceidandConfidset.insert(std::map< int, std::vector<unsigned long long> >::value_type(deviceid, vConfidset));
				}
			}
		}

		// 5�����db2 deviceid_%d��
		m_pRedisConnList[e_RC_TT_MainThread]->deletevalue(deviceid_id);

		sr_printf(SR_LOGLEVEL_DEBUG, " CDevMgr::UpdateDeviceInfoInDB deletevalue redis db(2) clear Table: (deviceid_id)=(%s) \n", deviceid_id);

		// 6���������豸(Ŀǰֻ����mc��netmp�豸)�Ļ�����Ϣ
		if (devicetype == 1
			|| devicetype == 2)
		{

			if (pUpdateDeviceInfoData != NULL
				&& pUpdateDeviceInfoData->m_mapDeviceidandConfidset.size() > 0)
			{
				typedef CBufferT<int, int, CAsyncThread*, time_t, UpdateDeviceInfoData*, void*, void*, void*> CParam;
				CParam* pParam;
				pParam = new CParam(ptime, strlen(ptime), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, deviceid, device_type, m_pUpMsqThread, lltime, pUpdateDeviceInfoData);

				typedef void (CDevMgr::* ACTION)(void*);
				m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
					*this, &CDevMgr::Handle_UpdateDeviceInfoInDB, (void*)pParam));

				sr_printf(SR_LOGLEVEL_INFO, " CDevMgr::UpdateDeviceInfoInDB post (%d, %d) to CDevMgr::Handle_UpdateDeviceInfoInDB!!!!\n", deviceid, device_type);
			}
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, " ==error== CDevMgr::UpdateDeviceInfoInDB m_pRedis == NULL error!!! \n");
	}


#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_NORMAL, "==-->> CDevMgr::UpdateDeviceInfoInDB  ****time_use** is: %lf us \n", time_use);
#endif 

}

void CDevMgr::Handle_UpdateDeviceInfoInDB(void *pArg)
{

	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<int, int, CAsyncThread*, time_t, UpdateDeviceInfoData*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	char* ptime = (char*)pParam->m_pData1;
	std::string strTime = ptime;


	int deviceid = pParam->m_Arg1;
	int devicetype = pParam->m_Arg2;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg3);
	time_t lltime = (time_t)(pParam->m_Arg4);
	UpdateDeviceInfoData *pUpdateDeviceInfoData = (UpdateDeviceInfoData*)(pParam->m_Arg5);

	if (pUpMsgThread == NULL
		|| pUpdateDeviceInfoData == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL) // test delet by csh at 2016.10.21
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateDeviceInfoInDB pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif 


	if (devicetype == 1
		|| devicetype == 2)
	{
		// 1�����µ�ǰ�쳣�豸��mc����netmp���Ļ�����Ϣ
		std::map< int, std::vector<unsigned long long> >::iterator itor = pUpdateDeviceInfoData->m_mapDeviceidandConfidset.find(deviceid);
		if (itor != pUpdateDeviceInfoData->m_mapDeviceidandConfidset.end())
		{
			for (std::vector<unsigned long long>::iterator v_itor = itor->second.begin();
				v_itor != itor->second.end(); v_itor++)
			{
				unsigned long long ullConfid = *v_itor;

				// �����ǰ�쳣�豸��mc�����������л����������ն˵�״̬
				if (devicetype == 1)
				{
					m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);
					char conf_id_value[128] = { 0 };
					char confterlist_confid_key[128] = { 0 };
#ifdef WIN32
					sprintf(conf_id_value, "%I64d", ullConfid);
#elif defined LINUX
					sprintf(conf_id_value, "%lld", ullConfid);
#endif

					sprintf(confterlist_confid_key, "confterlist_%s", conf_id_value);

					// ����mysql�б�user_conf_detail�Ľ���ʱ��
					int ternum = m_pRedisConnList[e_RC_TT_UpMsqThread]->getdbnum(confterlist_confid_key);
					if (ternum > 0)
					{
						bool haseroor = false;
						CDeviceID** tersuid = NULL;
						typedef CDeviceID* TerSuidPtr;
						tersuid = new TerSuidPtr[ternum];
						if (tersuid != NULL)
						{
							for (int j = 0; j < ternum; j++)
							{
								tersuid[j] = NULL;
								CDeviceID* obj = new CDeviceID();
								if (obj != NULL)
								{
									tersuid[j] = obj;
								}
								else
								{
									haseroor = true;
								}
							}
						}
						else
						{
							haseroor = true;
						}

						if (haseroor) // �ڴ��������ֱ�ӷ���
						{
							sr_printf(SR_LOGLEVEL_ERROR, " mc CDevMgr::Handle_UpdateDeviceInfoInDB before getdbdata redis db(4) malloc memory for load error!!! \n");
						}
						else
						{
							int retTerNum = m_pRedisConnList[e_RC_TT_UpMsqThread]->getdbdata(confterlist_confid_key, tersuid, ternum); // ��ȡ����load�Ѿ��Ǵ�С����

							sr_printf(SR_LOGLEVEL_INFO, " mc CDevMgr::Handle_UpdateDeviceInfoInDB getdbdata redis db(4) confterlist_confid_key(%s), Record num:%d, retTerNum=%d\n", confterlist_confid_key, ternum, retTerNum);

							if (retTerNum == 0)
							{
								for (int kk = 0; kk < ternum; kk++)
								{
									if (tersuid[kk] != NULL)
									{
										delete (tersuid[kk]);
										tersuid[kk] = NULL;
									}
								}
								delete[]tersuid;
								tersuid = NULL;
							}
							else
							{
								for (int jj = 0; jj < retTerNum; jj++)
								{
									DelUserConfDetail(tersuid[jj]->deviceid, ullConfid, ptime, lltime);
#ifdef WIN32
									sr_printf(SR_LOGLEVEL_DEBUG, " mc CDevMgr::Handle_UpdateDeviceInfoInDB DelUserConfDetail [suid:%d, confid:%I64d, realleavetime=%s]\n", tersuid[jj]->deviceid, ullConfid, strTime.c_str());
#elif defined LINUX
									sr_printf(SR_LOGLEVEL_DEBUG, " mc CDevMgr::Handle_UpdateDeviceInfoInDB DelUserConfDetail [suid:%d, confid:%lld, realleavetime=%s]\n", tersuid[jj]->deviceid, ullConfid, strTime.c_str());
#endif
								}// for (int jj = 0; jj < retTerNum; jj++)
							}// if (retTerNum != 0)
						}// if (!haseroor)


						for (int kkk = 0; kkk < ternum; kkk++)
						{
							if (tersuid[kkk] != NULL)
							{
								delete (tersuid[kkk]);
								tersuid[kkk] = NULL;
							}
						}
						delete[]tersuid;
						tersuid = NULL;

					} //if (ternum > 0)

					UpdateConfReport(ullConfid, 0, ptime, lltime);

#ifdef WIN32
					sr_printf(SR_LOGLEVEL_INFO, " mc CDevMgr::Handle_UpdateDeviceInfoInDB UpdateConfReport [confid:%I64d, realleavetime=%s]\n", ullConfid, strTime.c_str());
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_INFO, " mc CDevMgr::Handle_UpdateDeviceInfoInDB UpdateConfReport [confid:%lld, realleavetime=%s]\n", ullConfid, strTime.c_str());
#endif
				}//if (devicetype == 1)

				UpdateDeviceConfDetail(ullConfid, deviceid, ptime, lltime);

				if (devicetype == 1)
				{
#ifdef WIN32
					sr_printf(SR_LOGLEVEL_INFO, " mc CDevMgr::Handle_UpdateDeviceInfoInDB update Mysql::device_conf_detail mc conf_id:%I64d, deviceid = %d, RealuseStoptime = %s\n", ullConfid, deviceid, strTime.c_str());
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_INFO, " mc CDevMgr::Handle_UpdateDeviceInfoInDB update Mysql::device_conf_detail mc conf_id:%lld, deviceid = %d, RealuseStoptime = %s\n", ullConfid, deviceid, strTime.c_str());
#endif
				}
				else if (devicetype == 2)
				{
#ifdef WIN32
					sr_printf(SR_LOGLEVEL_INFO, " netmp CDevMgr::Handle_UpdateDeviceInfoInDB update Mysql::device_conf_detail netmp conf_id:%I64d, deviceid = %d, RealuseStoptime = %s\n", ullConfid, deviceid, strTime.c_str());
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_INFO, " netmp CDevMgr::Handle_UpdateDeviceInfoInDB update Mysql::device_conf_detail netmp conf_id:%lld, deviceid = %d, RealuseStoptime = %s\n", ullConfid, deviceid, strTime.c_str());
#endif
				}
			}
		}

		// �Ե�ǰmcͬ��ͬ�����netmp���д���
		if (devicetype == 1)
		{
			for (std::map< int, std::vector<unsigned long long> >::iterator itor_r = pUpdateDeviceInfoData->m_mapRelatedDeviceidandConfidset.begin();
				itor_r != pUpdateDeviceInfoData->m_mapRelatedDeviceidandConfidset.end(); itor_r++)
			{
				int iNetmpid = itor_r->first;

				for (std::vector<unsigned long long>::iterator confid_itor = itor_r->second.begin();
					confid_itor != itor_r->second.end(); confid_itor++)
				{
					unsigned long long ullConfid = *confid_itor;
					UpdateDeviceConfDetail(ullConfid, iNetmpid, ptime, lltime);
#ifdef WIN32
					sr_printf(SR_LOGLEVEL_INFO, " mc -> netmp CDevMgr::Handle_UpdateDeviceInfoInDB update Mysql::device_conf_detail netmp conf_id:%I64d, iNetmpid = %d, RealuseStoptime = %s\n", ullConfid, iNetmpid, strTime.c_str());
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_INFO, " mc -> netmp CDevMgr::Handle_UpdateDeviceInfoInDB update Mysql::device_conf_detail netmp conf_id:%lld, iNetmpid = %d, RealuseStoptime = %s\n", ullConfid, iNetmpid, strTime.c_str());
#endif
				}
			}
		}

	} // if (devicetype == 1 || devicetype == 2)
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, " ==11== CDevMgr::Handle_UpdateDeviceInfoInDB devicetype error!!! \n");
	}

	delete pUpdateDeviceInfoData;
	pUpdateDeviceInfoData = NULL;

	delete pParam;
	pParam = NULL;

#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_NORMAL, "==-->> CDevMgr::Handle_UpdateDeviceInfoInDB  ****time_use** is: %lf us \n", time_use);
#endif 

}


void CDevMgr::SendHeartbeatInd(DeviceConnect *pClient, char* token)
{
	if (m_pMainThread == NULL)
	{
		return;
	}

	int itoken = atoi(token);
	//printf("    $$$$$$$$$$$  CDevMgr::SendHeartbeatInd token = %s, itoken = %d\n", token, itoken);

	typedef CBufferT<DeviceConnect*, int, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pClient, itoken);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_SendHeartbeatInd, (void*)pParam));

}

void CDevMgr::Handle_SendHeartbeatInd(void *pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<DeviceConnect*, int, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	do
	{
		DeviceConnect* pClient = (DeviceConnect*)(pParam->m_Arg1);
		//char* pToken = (char*)(pParam->m_Arg2);
		int itoken = (int)(pParam->m_Arg2);

		char stoken[33] = { 0 };
		sprintf(stoken, "%d", itoken);

		SRMsgs::IndDevtoserHeart Indsend;

		Indsend.set_token(stoken);
		//printf("===== Handle_SendHeartbeatInd ===IndDevtoserHeart ==set_token-->> %s\n", stoken);

		if (pClient != NULL)
		{
			SerialProtoMsgAndSend(pClient, SRMsgId_IndDevtoserHeart, &Indsend);
		}

	} while (0);

	delete pParam;
	pParam = NULL;
}

void CDevMgr::SendHeartBeatToDevmgr()
{
	if (m_pMainThread == NULL)
	{
		return;
	}

	if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr)
	{
		typedef CBufferT<void*, void*, void*, void*, void*, void*, void*, void*> CParam;
		CParam* pParam;
		pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0);

		typedef void (CDevMgr::* ACTION)(void*);
		m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*this, &CDevMgr::Handle_SendHeartBeatToDevmgr, (void*)pParam));
	}
	else
	{
		sr_printf(SR_LOGLEVEL_WARNING, "SendHeartBeatToDevmgr m_current_devmgr_sockptr is null.\n");
	}

}

void CDevMgr::Handle_SendHeartBeatToDevmgr(void *pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<void*, void*, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	if (m_uiMyDeviceid != 0)
	{
		SRMsgs::IndsertodevHeart hearbeattodevmgr;
		hearbeattodevmgr.set_svr_type(CParser::GetInstance()->GetMySvrType());
		hearbeattodevmgr.set_deviceid(m_uiMyDeviceid);
		hearbeattodevmgr.set_token(m_szMyToken);
		hearbeattodevmgr.set_load1(0u);
		hearbeattodevmgr.set_load2(0u);

		//SerialAndSendDevmgr_nobuf(getMsgIdByClassName(IndsertodevHeart), &hearbeattodevmgr);
		SerialAndSendDevmgr((AcitiveConnect*)(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr), getMsgIdByClassName(IndsertodevHeart), &hearbeattodevmgr);
	}

	delete pParam;
	pParam = NULL;
}

void CDevMgr::ReqUPServerInfoPeric(unsigned int reqgetsvrtype)
{
	if (m_pMainThread == NULL)
	{
		return;
	}

	if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr)
	{
		typedef CBufferT<unsigned int, void*, void*, void*, void*, void*, void*, void*> CParam;
		CParam* pParam;
		pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, reqgetsvrtype);

		typedef void (CDevMgr::* ACTION)(void*);
		m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*this, &CDevMgr::Handle_ReqUPServerInfoToDevmgr, (void*)pParam));
	}
	else
	{
		sr_printf(SR_LOGLEVEL_WARNING, "ReqUPServerInfoPeric m_current_devmgr_sockptr is null.\n");
	}

}

void CDevMgr::Handle_ReqUPServerInfoToDevmgr(void *pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned int, void*, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	unsigned int reqgetsvrtype = (unsigned int)(pParam->m_Arg1);
	// 1����ȡ��������
	char local_domain_name[128] = { 0 };
	char up_domain_name[128] = { 0 };
	RedisReplyArray vrra_domain_level;
	vrra_domain_level.clear();
	unsigned int nDomainAndLevel = 0;

	m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
	bool bHGetAllDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_domain_level);

	nDomainAndLevel = vrra_domain_level.size();

	if (bHGetAllDomain_ok
		&& (nDomainAndLevel > 0)
		&& (nDomainAndLevel % 2 == 0))
	{
		for (int idx = 0; idx < nDomainAndLevel;)
		{
			int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
			//vrra_domain_level[idx].str;// ����
			iLevel = atoi(vrra_domain_level[idx + 1].str.c_str());// level
			if (iLevel == 0) // ������
			{
				sprintf(local_domain_name, "%s", vrra_domain_level[idx].str.c_str());
			}
			else if (iLevel == -1) // �ϼ���
			{
				sprintf(up_domain_name, "%s", vrra_domain_level[idx].str.c_str());
			}
			idx += 2;
		}
	}


	if (m_uiMyDeviceid != 0
		&& reqgetsvrtype != 0
		&& strlen(local_domain_name) > 0)
	{
		SRMsgs::ReqGetSysDeviceInfo reqgsdi;
		reqgsdi.set_token(m_szMyToken);
		reqgsdi.set_deviceid(m_uiMyDeviceid);
		reqgsdi.set_svr_type(DEVICE_SERVER::DEVICE_DEV);
		reqgsdi.set_domainname(local_domain_name);
		reqgsdi.set_get_svr_type(reqgetsvrtype);//��ȡ�ϼ���relaymc��relayserver��stunserver�豸��Ϣ
		// Ŀǰֻ��ȡ�ϼ���
		if (strlen(up_domain_name) > 0)
		{
			reqgsdi.add_get_domainnames(up_domain_name);
		} 
		else
		{
			reqgsdi.add_get_domainnames("");
		}

		SerialAndSendDevmgr((AcitiveConnect*)(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr), getMsgIdByClassName(ReqGetSysDeviceInfo), &reqgsdi);
	}

	delete pParam;
	pParam = NULL;
}


int CDevMgr::OnServerReciveData(void* pclient, char* data, uint32_t data_len)//must return how many data Has been copied(recive)
{
	if (m_pMainThread == NULL)
		return 0;

	if (data == NULL || data_len <= 0)
		return 0;
	////typedef CBufferT<void*> CParam;
	//typedef CBufferT<void*, void*, void*, void*, void*, void*> CParam;
	//CParam* pParam;
	//pParam = new CParam(data, data_len, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pclient);

	unsigned long ulTimeNow = getTickCount(); // 

	typedef CBufferT<void*, CAsyncThread*, unsigned long, void*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(data, data_len, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pclient, m_pMainThread, ulTimeNow);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_ServerReciveData, (void*)pParam));
	return data_len;
}

void CDevMgr::Handle_ServerReciveData(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	sr_printf(SR_LOGLEVEL_DEBUG, "Handle_ServerReciveData.\n");

	typedef CBufferT<void*, CAsyncThread*, unsigned long, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	do
	{
		char* pData = (char*)pParam->m_pData1;
		int nLen = pParam->m_nLen1;
		//TcpClient* pClient = (TcpClient*)(pParam->m_Arg1);
        DeviceConnect* pClient = (DeviceConnect*)(pParam->m_Arg1);
		CAsyncThread* pMainThread = (CAsyncThread*)(pParam->m_Arg2);
		unsigned long recv_data_time = (unsigned long)(pParam->m_Arg3);

		//��������
		//printf("ParseClientData begin\n");
		ParseClientData(pClient, pMainThread, pData, nLen, recv_data_time);
		//printf("ParseClientData success\n");
	} while (0);

	delete pParam;
	pParam = NULL;
}

int CDevMgr::OnClientReciveData(void* psvr, char* data, uint32_t data_len)
{
	if (m_pMainThread == NULL)
		return 0;

	if (data == NULL || data_len <= 0)
		return 0;

	unsigned long ulTimeNow = getTickCount(); // 

	typedef CBufferT<void*, CAsyncThread*, unsigned long, void*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(data, data_len, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, psvr, m_pMainThread, ulTimeNow);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_ClientReciveData, (void*)pParam));
	return data_len;
}

void CDevMgr::Handle_ClientReciveData(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	sr_printf(SR_LOGLEVEL_DEBUG, "Handle_ClientReciveData.\n");

	typedef CBufferT<void*, CAsyncThread*, unsigned long, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	do
	{
		char* pData = (char*)pParam->m_pData1;
		int nLen = pParam->m_nLen1;
		AcitiveConnect* pSvr = (AcitiveConnect*)(pParam->m_Arg1);

		CAsyncThread* pMainThread = (CAsyncThread*)(pParam->m_Arg2);

		unsigned long recv_data_time = (unsigned long)(pParam->m_Arg3);

		//��������
		//printf("ParseServerData begin\n");
		ParseServerData(pSvr, pMainThread, pData, nLen, recv_data_time);
		//printf("ParseServerData success\n");
	} while (0);

	delete pParam;
	pParam = NULL;
}

void CDevMgr::OnConnect(const int isok)
{

}

void CDevMgr::OnError(const char* error)
{

}

void CDevMgr::OnClosed(void *pTcpConnet)
{
	if (m_pMainThread == NULL || pTcpConnet == NULL)
	{
		//printf("======m_pMainThread == NULL====-return--->>>>\n");
		sr_printf(SR_LOGLEVEL_ERROR, "===CDevMgr::OnClosed===m_pMainThread == NULL==pTcpConnet == NULL==-return--->>>>\n");
		return;
	}

	//typedef CBufferT<void*, LPSOCKET> CParam;
	//typedef CBufferT<void*, LPSOCKET, void*, void*, void*, void*> CParam;
	//CParam* pParam;
	//pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pTcpConnet, socket_fd);

	//sr_printf(SR_LOGLEVEL_ERROR, "===devmgr recv onclosed msg ===and post msg to m_pMainThread Handle_DeviceOnClose--->>>>\n");

	//��ȡ��ǰʱ�䴮
	time_t timeNow;
	struct tm *ptmNow;
	char szTime[30];
	timeNow = time(NULL);
	ptmNow = localtime(&timeNow);
	sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

	typedef CBufferT<void*, CAsyncThread*, time_t, void*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(szTime, strlen(szTime), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pTcpConnet, m_pMainThread, timeNow);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_DeviceOnClose, (void*)pParam));
}


void CDevMgr::Handle_DeviceOnClose(void *pArg)
{
	if (pArg == NULL)
	{
		assert(0);

		sr_printf(SR_LOGLEVEL_ERROR, "===CDevMgr::Handle_DeviceOnClose ===pArg == NULL ==-return--->>>>\n");

		return;
	}

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif

	typedef CBufferT<void*, CAsyncThread*, time_t, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	do
	{
		DeviceConnect* pClient = (DeviceConnect*)(pParam->m_Arg1);
		CAsyncThread * pMainThread = (CAsyncThread *)(pParam->m_Arg2);
		time_t lltime = (time_t)(pParam->m_Arg3);
		char* ptime = (char*)(pParam->m_pData1);

		sr_printf(SR_LOGLEVEL_ERROR, "==---->>>>>>>>Handle_DeviceOnClose==---->> %p\n", pClient);

		//// �˴�Ӧ�Ȳ�ص���socket�Ƿ񻹴��ڣ���������ڶ��ϱ����󱨣����ø��豸idȥ�������ݿⲻ��ѧ
		//
		//int deviceid = pClient->GetDeviceID();

		//iter_mapDeviceHeartbeatTime iter = m_MapDeviceHeartbeatTime.find(deviceid);
		//if (iter != m_MapDeviceHeartbeatTime.end())
		//{
		//	// �Ը��豸������ݿ�Mysql��Redis���и��´���
		//	unsigned int uiDeviceType = iter->second.devicetype;

		//	sr_printf(SR_LOGLEVEL_ERROR, "==Handle_DeviceOnColse (device_id = %d, uiDeviceType = %d)   ===update status = 0--->>\n", deviceid, uiDeviceType);

		//	UpdateDeviceInfoInDB(deviceid, uiDeviceType, pMainThread, ptime);
		//	//printf("==22222---->>>>Handle_DeviceOnColse===UpdateDeviceInfoInDB==deviceid-->>> %d,==uiDeviceType-->>> %d\n", deviceid, uiDeviceType);

		//	// ��Map�ж�Ӧ�ļ�¼����ɾ��
		//	m_MapDeviceHeartbeatTime.erase(iter);
		//}

		std::map<DeviceConnect *, DeviceConnect *> dev_connect_map;
		m_devConnMgr->GetLatestMap(dev_connect_map);

		sr_printf(SR_LOGLEVEL_DEBUG, " Handle_DeviceOnClose current dev_connect_map.size = %d\n", dev_connect_map.size());

		//std::map<LPSOCKET, DeviceConnect *>::iterator iter_map_dev_connect = dev_connect_map.begin();
		//while (iter_map_dev_connect != dev_connect_map.end())
		std::map<DeviceConnect *, DeviceConnect *>::iterator iter_map_dev_connect = dev_connect_map.find(pClient);
		if(iter_map_dev_connect != dev_connect_map.end())
		{
			int iDeviceid = 0;
			iDeviceid = iter_map_dev_connect->second->GetDeviceID();

			//sr_printf(SR_LOGLEVEL_ERROR, " === Handle_DeviceOnClose iDeviceid = %d, map_dev_connect->first = %p, map_dev_connect->second = %p\n", iDeviceid, iter_map_dev_connect->first, iter_map_dev_connect->second);

			if (iDeviceid != 0)
			{
				iter_mapDeviceHeartbeatTime iter = m_MapDeviceHeartbeatTime.find(iDeviceid);
				if (iter != m_MapDeviceHeartbeatTime.end())
				{
					// �Ը��豸������ݿ�Mysql��Redis���и��´���
					unsigned int uiDeviceType = iter->second.devicetype;

					sr_printf(SR_LOGLEVEL_ERROR, "==Handle_DeviceOnClose (device_id = %d, uiDeviceType = %d)   ===update status = 0--->>\n", iDeviceid, uiDeviceType);

					UpdateDeviceInfoInDB(iDeviceid, uiDeviceType, pMainThread, ptime, lltime);
					//printf("==22222---->>>>Handle_DeviceOnColse===UpdateDeviceInfoInDB==deviceid-->>> %d,==uiDeviceType-->>> %d\n", deviceid, uiDeviceType);

					// ��Map�ж�Ӧ�ļ�¼����ɾ��
					m_MapDeviceHeartbeatTime.erase(iter);
				}
			}
		}

		m_devConnMgr->DeviceOnClose(pClient); // 
		
	} while (0);


#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_DEBUG, "********CDevMgr::Handle_DeviceOnClose*****time_use** is: %lf us\n", time_use);

#endif

	delete pParam;
	pParam = NULL;
}

void CDevMgr::ActiveConnOnConnected(void *pActiveTcp)
{
	if (m_pMainThread == NULL || pActiveTcp == NULL)
	{
		//printf("======m_pMainThread == NULL====-return--->>>>\n");
		sr_printf(SR_LOGLEVEL_ERROR, "===CDevMgr::OnClosed===m_pMainThread == NULL==pTcpConnet == NULL==-return--->>>>\n");
		return;
	}

	//��ȡ��ǰʱ�䴮
	time_t timeNow;
	struct tm *ptmNow;
	char szTime[30];
	timeNow = time(NULL);
	ptmNow = localtime(&timeNow);
	sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

	typedef CBufferT<void*, CAsyncThread*, time_t, void*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(szTime, strlen(szTime), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pActiveTcp, m_pMainThread, timeNow);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_ActiveConnOnConnected, (void*)pParam));
}

void CDevMgr::Handle_ActiveConnOnConnected(void *pArg)
{
	if (pArg == NULL)
	{
		assert(0);

		sr_printf(SR_LOGLEVEL_ERROR, "===CDevMgr::Handle_ActiveConnOnClose ===pArg == NULL ==-return--->>>>\n");

		return;
	}
	
#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif

	typedef CBufferT<void*, CAsyncThread*, time_t, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	do
	{
		char* pData = (char*)pParam->m_pData1;
		int nLen = pParam->m_nLen1;

		AcitiveConnect* pActiveTcp = (AcitiveConnect*)(pParam->m_Arg1);

		CAsyncThread* pMainThread = (CAsyncThread*)(pParam->m_Arg2);

		unsigned long recv_msg_time = (unsigned long)(pParam->m_Arg3);

		m_devConnMgr->onActiveConnectSuccess(pActiveTcp);

	} while (0);

	delete pParam;
	pParam = NULL;
}

void CDevMgr::ActiveConnOnClosed(void *pActiveTcp)
{
	if (m_pMainThread == NULL || pActiveTcp == NULL)
	{
		//printf("======m_pMainThread == NULL====-return--->>>>\n");
		sr_printf(SR_LOGLEVEL_ERROR, "===CDevMgr::OnClosed===m_pMainThread == NULL==pTcpConnet == NULL==-return--->>>>\n");
		return;
	}

	//��ȡ��ǰʱ�䴮
	time_t timeNow;
	struct tm *ptmNow;
	char szTime[30];
	timeNow = time(NULL);
	ptmNow = localtime(&timeNow);
	sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

	typedef CBufferT<void*, CAsyncThread*, time_t, void*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(szTime, strlen(szTime), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pActiveTcp, m_pMainThread, timeNow);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_ActiveConnOnClose, (void*)pParam));
}

void CDevMgr::Handle_ActiveConnOnClose(void *pArg)
{
	if (pArg == NULL)
	{
		assert(0);

		sr_printf(SR_LOGLEVEL_ERROR, "===CDevMgr::Handle_ActiveConnOnClose ===pArg == NULL ==-return--->>>>\n");

		return;
	}

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif

	typedef CBufferT<void*, CAsyncThread*, time_t, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	do
	{
		char* pData = (char*)pParam->m_pData1;
		int nLen = pParam->m_nLen1;
		
		AcitiveConnect* pActiveTcp = (AcitiveConnect*)(pParam->m_Arg1);

		CAsyncThread* pMainThread = (CAsyncThread*)(pParam->m_Arg2);

		unsigned long recv_msg_time = (unsigned long)(pParam->m_Arg3);

		m_devConnMgr->onActiveConnSockError(pActiveTcp);

	} while (0);

	delete pParam;
	pParam = NULL;
}

void CDevMgr::RegisterToDevmgr()
{
	if (m_pMainThread == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "===CDevMgr::RegisterToDevmgr===m_pMainThread == NULL====-return--->>>>\n");
		return;
	}

	//��ȡ��ǰʱ�䴮
	time_t timeNow;
	struct tm *ptmNow;
	char szTime[30];
	timeNow = time(NULL);
	ptmNow = localtime(&timeNow);
	sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

	typedef CBufferT<CAsyncThread*, time_t, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(szTime, strlen(szTime), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, m_pMainThread, timeNow);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_RegisterToDevmgr, (void*)pParam));
}

void CDevMgr::Handle_RegisterToDevmgr(void *pArg)
{
	if (pArg == NULL)
	{
		assert(0);

		sr_printf(SR_LOGLEVEL_ERROR, "===CDevMgr::Handle_RegisterToDevmgr ===pArg == NULL ==-return--->>>>\n");

		return;
	}

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif

	typedef CBufferT<CAsyncThread*, time_t, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	// ��ȡ��������,��ȡʧ��(�磺redis����ʧ��)��Ҫ��������ע��
	std::string strlocaldomainname;
	strlocaldomainname.clear();
	if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
	{
		RedisReplyArray vrra_domain_level;
		vrra_domain_level.clear();
		unsigned int nDomainAndLevel = 0;
		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
		bool bHGetAllDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_domain_level);

		nDomainAndLevel = vrra_domain_level.size();

		if (bHGetAllDomain_ok
			&& (nDomainAndLevel > 0)
			&& (nDomainAndLevel % 2 == 0))
		{
			for (int idx = 0; idx < nDomainAndLevel;)
			{
				int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
				//vrra_domain_level[idx].str;// ����
				iLevel = atoi(vrra_domain_level[idx + 1].str.c_str());// level
				if (iLevel == 0)
				{
					strlocaldomainname.assign(vrra_domain_level[idx].str.c_str());
					break;
				}
				idx += 2;
			}
		}
	}

	if (strlocaldomainname.length() > 0)
	{
		char* pData = (char*)pParam->m_pData1;
		int nLen = pParam->m_nLen1;

		CAsyncThread* pMainThread = (CAsyncThread*)(pParam->m_Arg1);

		unsigned long recv_msg_time = (unsigned long)(pParam->m_Arg2);

		SRMsgs::ReqRegister devregister;
		struct in_addr addr;
		if (0 == inet_pton(AF_INET, CParser::GetInstance()->GetMyListenIpAddr().c_str(), &addr))
		{
			sr_printf(SR_LOGLEVEL_ERROR, "%s inet_pton error\n", __FUNCTION__);

			createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_re_register_timer, 0ull);
			delete pParam;
			pParam = NULL;
			return;
		}
		devregister.set_auth_password(CParser::GetInstance()->GetMySerialNumber());
		devregister.set_svr_type(CParser::GetInstance()->GetMySvrType());
		devregister.set_ip(addr.s_addr);
		devregister.set_port(CParser::GetInstance()->GetMyListenPort());
		devregister.set_cpunums(4);
		//devregister.set_nettype(0);// ����ָ����ַ����,0-˽(��)��,1-��(��)��
		devregister.set_nettype(1);// ����ָ����ַ����,0-˽(��)��,1-��(��)��
		devregister.set_version(m_softwareversion);
		devregister.set_domainname(strlocaldomainname);

		SerialAndSendDevmgr((AcitiveConnect*)(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr), getMsgIdByClassName(ReqRegister), &devregister);
	} 
	else
	{
		createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_re_register_timer, 0ull);
		delete pParam;
		pParam = NULL;
		return;
	}


	delete pParam;
	pParam = NULL;
}


CDevMgr::CDevMgr()
{
	//m_mapMainThreadUserConfDetial.clear();
	//m_mapUpmsgThreadUserConfDetial.clear();

	m_mapAsyncThreadDevice.clear();
	m_mapAsyncThreadDeviceConfig.clear();
	m_mapAsyncThreadConference.clear();
	m_mapAsyncThreadConfReport.clear();
	m_mapAsyncThreadUserConfDetial.clear();
	m_mapAsyncThreadDeviceConfDetail.clear();

	m_sConfReport.clear();
	m_sDeviceConfDetail.clear();
	m_sUserConfInfo.clear();

	time_t timeNow;
	timeNow = time(NULL);

	m_op_db_num = 1;
	m_write_db_timespan = 15;
	m_last_post_msg_time = timeNow;
	m_nGetDevHeartbeatTimeout = 15;
	//m_ullnum_per_post = 0;

	m_bCloseTimeoutSocket = false;

	m_iLoglevel = 2; // SR_LOGLEVEL_NORMAL�ȼ�
	m_bisStdout = false;
	m_uiDeviceNo = 0;

	m_uiMyDeviceid = 0;
	m_szMyToken.clear();
	m_uiMyGroupid = 0;

	m_bRegSelfok = false;
	m_iSelfDeviceid = 0;
	m_HttpSvrListenISok = false;

	m_checkclientheartbeat_periodtimer = (void*)0;
	m_upselfhb2db_periodtimer = (void*)0;
	m_writeredis2mysql_periodtimer = (void*)0;
	m_heart2devmgr_periodtimer = (void*)0;
	m_check_svr_heartbeat_timer = (void*)0;
	m_req_up_relaymcinfo_periodtimer = (void*)0;
	m_req_up_relayserverinfo_periodtimer = (void*)0;
	m_req_up_stunserverinfo_periodtimer = (void*)0;
	
	m_iLoglevel = CParser::GetInstance()->GetLoglevel();
	m_bisStdout = CParser::GetInstance()->GetIsStdout();
	m_uiDeviceNo = CParser::GetInstance()->GetDeviceNum();

	//���iDevmgrNum time_t
	m_pub_confreport_id = (((unsigned long long)(m_uiDeviceNo) << 32) + timeNow) * 1000; // ����1000�Ƿ�ֹ���Σ�����devmgr�л�ʱ�����϶̵���ID��ͻ
	//m_pub_device_detail_id = (((unsigned long long)(iDevmgrNum) << 32) + timeNow) * 1000; // ����1000�Ƿ�ֹ���Σ�����devmgr�л�ʱ�����϶̵���ID��ͻ
	//m_pub_userrpt_detail_id = (((unsigned long long)(iDevmgrNum) << 32) + timeNow) * 1000; // ����1000�Ƿ�ֹ���Σ�����devmgr�л�ʱ�����϶̵���ID��ͻ
	
	m_pub_device_detail_id = m_pub_confreport_id;
	m_pub_userrpt_detail_id = m_pub_confreport_id;

	m_pub_confrecord_id = m_pub_confreport_id;
	m_pub_recordfile_id = m_pub_confreport_id;
	m_pub_liveinfo_id = m_pub_confreport_id;

#ifdef WIN32
	printf(">>>>>>>>>In CDevMgr Class  iDevmgrNum>>> %u, timeNow>>> %I64d, m_confreport_id>>> %I64d\n", m_uiDeviceNo, timeNow, m_pub_confreport_id);
#elif defined LINUX
	printf(">>>>>>>>>In CDevMgr Class  iDevmgrNum>>> %u, timeNow>>> %lld, m_confreport_id>>> %lld\n", m_uiDeviceNo, timeNow, m_pub_confreport_id);
#endif

	//m_idCheckHeartbeatTimer = 0;

	m_devConnMgr = NULL;
	//m_HttpConnMgr = NULL;

	m_pRedisConnList = NULL;
	m_uiNeedRedisCoonNum = 0;
}

CDevMgr::~CDevMgr()
{
	//m_devConnMgr = NULL;

	//for (unsigned int k = 0; k < m_uiNeedRedisCoonNum; k++)
	//{
	//	if (m_pRedisConnList[k] != NULL)
	//	{
	//		m_pRedisConnList[k]->disconnect();
	//		sr_printf(SR_LOGLEVEL_NORMAL, "m_pRedisConnList[%d]->disconnect() success.\n", k);
	//		delete m_pRedisConnList[k];
	//	}
	//}
	//delete[] m_pRedisConnList;

	Fini();
}

bool CDevMgr::Init()
{
	m_fpLogFile = NULL;
	//GetProcessPath();
	//printf("===m_msgLogfile-->>>> %s\n", m_msgLogfile);
	//m_fpLogFile = fopen(m_msgLogfile, "w+t");

	char logfilename[256] = { 0 };
	time_t now = time(NULL);
	struct tm tm_now;
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
	localtime_s(&tm_now, &now);
#elif defined(LINUX) || defined(POSIX)
	localtime_r(&now, &tm_now);
#endif
	snprintf(logfilename, sizeof(logfilename), "logdevmgr_%02d:%02d:%02d", tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
	TraceLog::GetInstance().OpenLog(logfilename, m_iLoglevel, m_bisStdout);

	m_pMainThread = NULL;
	m_pUpMsqThread = NULL;
	m_pWriteDBThread = NULL;

	//unsigned int uiNeedRedisCoonNum = 0;
	m_uiNeedRedisCoonNum = 1;// Ĭ��Ϊ���̷߳��������

	//�û������߳�
	m_pMainThread = new CAsyncThread();
	if (m_pMainThread != NULL)
	{
		m_pMainThread->Start(false);

		m_selfThread = m_pMainThread;
	}
	
	m_timermanger = new TimerManager();
	
	m_pUpMsqThread = new CAsyncThread();
	if (m_pUpMsqThread != NULL)
	{
		m_pUpMsqThread->Start(false);
		m_uiNeedRedisCoonNum++;
	}
	

	m_pWriteDBThread = new CAsyncThread();
	if (m_pWriteDBThread != NULL)
	{
		m_pWriteDBThread->Start(false);
		m_uiNeedRedisCoonNum++;
	}
	

	m_pUpConfigThread = new CAsyncThread();
	m_pUpConfigThread->Start(false);

	m_pProcessHttpThread = new CAsyncThread();
	m_pProcessHttpThread->Start(false);

	//m_pTimer = new SR_Timer();
	m_curTime = 0;

	// ��ȡ�����汾��
	m_softwareversion.clear();
	GetSoftwareVersion();

	// ����ϵͳ��������
	m_mapSystemDomainInfo.clear();
	LoadSystemDomainInfo();

	unsigned int uisdicnt = 0;
	for (std::map<std::string, std::string>::iterator domain_itor = m_mapSystemDomainInfo.begin();
		domain_itor != m_mapSystemDomainInfo.end(); domain_itor++)
	{
		sr_printf(SR_LOGLEVEL_NORMAL, "m_mapSystemDomainInfo[%d] <%s, %s>\n", uisdicnt, domain_itor->first.c_str(), domain_itor->second.c_str());
		uisdicnt++;
	}


#ifdef REDIS_DEF

	sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Init() redisinit begin, m_uiNeedRedisCoonNum=%u.\n", m_uiNeedRedisCoonNum);


	//m_pRedis = CreateIRedis(this);
	////m_bConnRedis = m_pRedis->connect("192.168.90.37", 6379);
	//m_bConnRedis = m_pRedis->connect(CParser::GetInstance()->GetRedisIpAddr().c_str(), CParser::GetInstance()->GetRedisPort());
	//if (!m_bConnRedis)
	//{
	//	sr_printf(SR_LOGLEVEL_ERROR, "CDevMgr::Init() redis connect error.\n");
	//}
	//else
	//{
	//	sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Init() redisinit success.\n");
	//}
	

	m_pRedisConnList = CreateRedisInstanceList(this, m_uiNeedRedisCoonNum);

	if (m_pRedisConnList != NULL)
	{
		//��ʼ��Redis����
		for (unsigned int i = 0; i < m_uiNeedRedisCoonNum; i++)
		{
			if (m_pRedisConnList[i] != NULL)
			{
				if (!m_pRedisConnList[i]->connect(CParser::GetInstance()->GetRedisIpAddr().c_str(), CParser::GetInstance()->GetRedisPort()))
				{
					sr_printf(SR_LOGLEVEL_ERROR, "m_pRedisConnList[%d]->connect() redis connect error.\n", i);
					//return false;
				}
				else
				{
					sr_printf(SR_LOGLEVEL_NORMAL, "m_pRedisConnList[%d]->connect() redisinit success.\n", i);
				}
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "m_pRedisConnList[%d] == NULL error.\n", i);
				return false;
			}
		}
	}

#if 0
	time_t timeNow;
	struct tm *ptmNow;
	char szTime[30];
	timeNow = time(NULL);
	ptmNow = localtime(&timeNow);
	sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon+1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

	//m_pRedis->selectdb(1);
	//if (m_pRedis->existsvalue("foo1"))
	{ 
		//m_pRedis->deletevalue("foo1");
		//m_pRedis->setvalue("foo1", "hello zoujiaqing!");
		//m_pRedis->setvalue("mediagroup_2", "14", "1122211111111111111");	
		//printf("GET: %s\n", m_pRedis->getvalue("mediagroup_2"));
		//int num = m_pRedis->getdbnum("netmp");
		//if (num > 0)
		//{
		//	CNetMpLoad* load = new CNetMpLoad[num > 2 ? 2 : num];
		//	int start = 0;
		//	int stop = num > 2 ? (-1)*num + 1 : -1;
		//	m_pRedis->getdbdata("netmp", start, stop, load);
		//}
		//ZRANGE key start stop
	}
#endif

#endif

	sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Init() connect db begin.\n");

	sr_printf(SR_LOGLEVEL_NORMAL, "db info:[host=%s],[user=%s],[pwd=%s],[dbname=%s],[port=%d].\n", CParser::GetInstance()->GetMysqlIpAddr().c_str(),
		CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
		CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());

	//m_bConnMysql = CDBConnector::GetInstance()->ConnectDB("192.168.90.37", "root", "XBi5*nIzPc", "cloud", 3306);
	//m_bConnMysql = CDBConnector::GetInstance()->ConnectDB("127.0.0.1", "root", "admin123", "cloud", 3306);
	m_bConnMysql = CDBConnector::GetInstance()->ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
		CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
		CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
	if (!m_bConnMysql)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "CDevMgr::Init() connect db error.\n");
	}
	else
	{
		sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Init() connect db success.\n");
	}


	//////////////////////////////////////////////////////////////////////////
	sr_printf(SR_LOGLEVEL_NORMAL, ">>>>>>>>>>>>>>>>>>>>>>In Config File  GetDeviceNo>>>>>>>>>>>>>>>>>>> %d\n", m_uiDeviceNo);

	m_pMainUserConfDetail = new CUserConfDetail(m_uiDeviceNo);
	m_mapAsyncThreadUserConfDetial.insert(std::map<CAsyncThread*, CUserConfDetail*>::value_type(m_pMainThread, m_pMainUserConfDetail));
	m_pUpmsgUserConfDetail = new CUserConfDetail(m_uiDeviceNo);
	m_mapAsyncThreadUserConfDetial.insert(std::map<CAsyncThread*, CUserConfDetail*>::value_type(m_pUpMsqThread, m_pUpmsgUserConfDetail));
	m_pWriteDBUserConfDetail = new CUserConfDetail(m_uiDeviceNo);
	m_mapAsyncThreadUserConfDetial.insert(std::map<CAsyncThread*, CUserConfDetail*>::value_type(m_pWriteDBThread, m_pWriteDBUserConfDetail));

	m_pMainDevice = new CDevice(m_uiDeviceNo);
	m_mapAsyncThreadDevice.insert(std::map<CAsyncThread*, CDevice*>::value_type(m_pMainThread, m_pMainDevice));
	m_pUpmsgDevice = new CDevice(m_uiDeviceNo);
	m_mapAsyncThreadDevice.insert(std::map<CAsyncThread*, CDevice*>::value_type(m_pUpMsqThread, m_pUpmsgDevice));

	m_pMainDeviceConfig = new CDeviceConfig();
	m_mapAsyncThreadDeviceConfig.insert(std::map<CAsyncThread*, CDeviceConfig*>::value_type(m_pMainThread, m_pMainDeviceConfig));
	m_pUpmsgDeviceConfig = new CDeviceConfig();
	m_mapAsyncThreadDeviceConfig.insert(std::map<CAsyncThread*, CDeviceConfig*>::value_type(m_pUpMsqThread, m_pUpmsgDeviceConfig));

	m_pMainConference = new CConference();
	m_mapAsyncThreadConference.insert(std::map<CAsyncThread*, CConference*>::value_type(m_pMainThread, m_pMainConference));
	m_pUpmsgConference = new CConference();
	m_mapAsyncThreadConference.insert(std::map<CAsyncThread*, CConference*>::value_type(m_pUpMsqThread, m_pUpmsgConference));
	m_pWriteDBConference = new CConference();
	m_mapAsyncThreadConference.insert(std::map<CAsyncThread*, CConference*>::value_type(m_pWriteDBThread, m_pWriteDBConference));

	m_pMainConfReport = new CConfReport(m_uiDeviceNo);
	m_mapAsyncThreadConfReport.insert(std::map<CAsyncThread*, CConfReport*>::value_type(m_pMainThread, m_pMainConfReport));
	m_pUpmsgConfReport = new CConfReport(m_uiDeviceNo);
	m_mapAsyncThreadConfReport.insert(std::map<CAsyncThread*, CConfReport*>::value_type(m_pUpMsqThread, m_pUpmsgConfReport));
	m_pWriteDBConfReport = new CConfReport(m_uiDeviceNo);
	m_mapAsyncThreadConfReport.insert(std::map<CAsyncThread*, CConfReport*>::value_type(m_pWriteDBThread, m_pWriteDBConfReport));

	m_pMainConfRecord = new CConfRecord(m_uiDeviceNo);
	m_mapAsyncThreadConfRecord.insert(std::map<CAsyncThread*, CConfRecord*>::value_type(m_pMainThread, m_pMainConfRecord));
	m_pUpmsgConfRecord = new CConfRecord(m_uiDeviceNo);
	m_mapAsyncThreadConfRecord.insert(std::map<CAsyncThread*, CConfRecord*>::value_type(m_pUpMsqThread, m_pUpmsgConfRecord));
	m_pWriteDBConfRecord = new CConfRecord(m_uiDeviceNo);
	m_mapAsyncThreadConfRecord.insert(std::map<CAsyncThread*, CConfRecord*>::value_type(m_pWriteDBThread, m_pWriteDBConfRecord));

	m_pMainDeviceConfDetail = new CDeviceConfDetail(m_uiDeviceNo);
	m_mapAsyncThreadDeviceConfDetail.insert(std::map<CAsyncThread*, CDeviceConfDetail*>::value_type(m_pMainThread, m_pMainDeviceConfDetail));
	m_pUpmsgDeviceConfDetail = new CDeviceConfDetail(m_uiDeviceNo);
	m_mapAsyncThreadDeviceConfDetail.insert(std::map<CAsyncThread*, CDeviceConfDetail*>::value_type(m_pUpMsqThread, m_pUpmsgDeviceConfDetail));
	m_pWriteDBDeviceConfDetail = new CDeviceConfDetail(m_uiDeviceNo);
	m_mapAsyncThreadDeviceConfDetail.insert(std::map<CAsyncThread*, CDeviceConfDetail*>::value_type(m_pWriteDBThread, m_pWriteDBDeviceConfDetail));

	m_pMainRecordFile = new CRecordFile(m_uiDeviceNo);
	m_mapAsyncThreadRecordFile.insert(std::map<CAsyncThread*, CRecordFile*>::value_type(m_pMainThread, m_pMainRecordFile));
	m_pUpmsgRecordFile = new CRecordFile(m_uiDeviceNo);
	m_mapAsyncThreadRecordFile.insert(std::map<CAsyncThread*, CRecordFile*>::value_type(m_pUpMsqThread, m_pUpmsgRecordFile));
	m_pWriteDBRecordFile = new CRecordFile(m_uiDeviceNo);
	m_mapAsyncThreadRecordFile.insert(std::map<CAsyncThread*, CRecordFile*>::value_type(m_pWriteDBThread, m_pWriteDBRecordFile));

	m_pMainConfLiveInfo = new CConfLiveInfo(m_uiDeviceNo);
	m_mapAsyncThreadConfLiveInfo.insert(std::map<CAsyncThread*, CConfLiveInfo*>::value_type(m_pMainThread, m_pMainConfLiveInfo));
	m_pUpmsgConfLiveInfo = new CConfLiveInfo(m_uiDeviceNo);
	m_mapAsyncThreadConfLiveInfo.insert(std::map<CAsyncThread*, CConfLiveInfo*>::value_type(m_pUpMsqThread, m_pUpmsgConfLiveInfo));
	m_pWriteDBConfLiveInfo = new CConfLiveInfo(m_uiDeviceNo);
	m_mapAsyncThreadConfLiveInfo.insert(std::map<CAsyncThread*, CConfLiveInfo*>::value_type(m_pWriteDBThread, m_pWriteDBConfLiveInfo));

	m_pMainConfParticipant = new CConfParticipant();
	m_mapAsyncThreadConfParticipant.insert(std::map<CAsyncThread*, CConfParticipant*>::value_type(m_pMainThread, m_pMainConfParticipant));
	
	m_pMainConfLiveSetting = new CConfLiveSetting();
	m_mapAsyncThreadConfLiveSetting.insert(std::map<CAsyncThread*, CConfLiveSetting*>::value_type(m_pMainThread, m_pMainConfLiveSetting));

	m_pMainConfRollCallInfo = new CConfRollCallInfo();
	m_mapAsyncThreadConfRollCallInfo.insert(std::map<CAsyncThread*, CConfRollCallInfo*>::value_type(m_pMainThread, m_pMainConfRollCallInfo));
	m_pWriteDBConfRollCallInfo = new CConfRollCallInfo();
	m_mapAsyncThreadConfRollCallInfo.insert(std::map<CAsyncThread*, CConfRollCallInfo*>::value_type(m_pWriteDBThread, m_pWriteDBConfRollCallInfo));
	
	m_pMainRollCallList = new CRollCallList();
	m_mapAsyncThreadRollCallList.insert(std::map<CAsyncThread*, CRollCallList*>::value_type(m_pMainThread, m_pMainRollCallList));
	m_pWriteDBRollCallList = new CRollCallList();
	m_mapAsyncThreadRollCallList.insert(std::map<CAsyncThread*, CRollCallList*>::value_type(m_pWriteDBThread, m_pWriteDBRollCallList));

	m_pMainConfPollInfo = new CConfPollInfo();
	m_mapAsyncThreadConfPollInfo.insert(std::map<CAsyncThread*, CConfPollInfo*>::value_type(m_pMainThread, m_pMainConfPollInfo));
	m_pMainPollList = new CPollList();
	m_mapAsyncThreadPollList.insert(std::map<CAsyncThread*, CPollList*>::value_type(m_pMainThread, m_pMainPollList));

	m_pMainDataDictionary = new CDataDictionary();
	m_mapAsyncThreadDataDictionary.insert(std::map<CAsyncThread*, CDataDictionary*>::value_type(m_pMainThread, m_pMainDataDictionary));

	m_op_db_num = CParser::GetInstance()->GetOPDBNum();
	m_write_db_timespan = CParser::GetInstance()->GetWriteDBTimespan();
	m_nGetDevHeartbeatTimeout = CParser::GetInstance()->GetDevHeartbeatTimeout();
	m_bCloseTimeoutSocket = CParser::GetInstance()->IsCloseTimeoutSocket();

	sr_printf(SR_LOGLEVEL_NORMAL, " In config file: DeviceNo=%d, OperatDBNum=%d, WriteDBTimespan=%d, DevHeartbeatTimeout=%d, IsCloseTimeoutSocket=%d, LogLevel=%d, IsStdout=%d\n", m_uiDeviceNo, m_op_db_num, m_write_db_timespan, m_nGetDevHeartbeatTimeout, m_bCloseTimeoutSocket, m_iLoglevel, m_bisStdout);

	//////////////////////////////////////////////////////////////////////////
	if (!m_bRegSelfok)
	{
		OnRegisterSelf();// 

		//SRMsgs::ReqRegister selfregister;
		//struct in_addr addr;
		//if (0 == inet_pton(AF_INET, CParser::GetInstance()->GetMyListenIpAddr().c_str(), &addr))
		//{
		//	sr_printf(SR_LOGLEVEL_ERROR, "%s inet_pton LocalIpAddr=%s error\n", __FUNCTION__, CParser::GetInstance()->GetMyListenIpAddr().c_str());
		//	return false;
		//}
		//selfregister.set_auth_password(CParser::GetInstance()->GetMySerialNumber());
		//selfregister.set_svr_type(DEVICE_SERVER::DEVICE_DEV);// 
		//selfregister.set_ip(addr.s_addr);
		//selfregister.set_port(CParser::GetInstance()->GetMyListenPort());
		//selfregister.set_cpunums(4);
		//selfregister.set_version(m_softwareversion);
		////selfregister.set_httpport(CParser::GetInstance()->GetMyHTTPListenPort());

		////if (!RegisterSelf(&selfregister, m_pMainDeviceConfig, m_pMainDevice))
		////{
		////	sr_printf(SR_LOGLEVEL_ERROR, "%s ReqRegisterSelf error\n", __FUNCTION__);
		////	return false;
		////}

		//m_bRegSelfok = RegisterSelf(&selfregister, m_pMainDeviceConfig, m_pMainDevice);

		//if (!m_bRegSelfok)
		//{
		//	sr_printf(SR_LOGLEVEL_ERROR, "%s RegisterSelf error\n", __FUNCTION__);
		//	return false;
		//}
		//else
		//{
		//	m_iSelfDeviceid = m_pMainDevice->GetDeviceID();
		//	if (m_iSelfDeviceid != 0)
		//	{
		//		sr_printf(SR_LOGLEVEL_INFO, "%s RegisterSelf ok and m_iSelfDeviceid is:%d\n", __FUNCTION__, m_iSelfDeviceid);

		//		//char cMyToken[128] = { 0 };
		//		//sprintf(cMyToken, "%d", m_iSelfDeviceid);

		//		//SRMsgs::IndsertodevHeart selfhbind;
		//		//selfhbind.set_svr_type(DEVICE_SERVER::DEVICE_DEV);
		//		//selfhbind.set_deviceid(m_iSelfDeviceid);
		//		//selfhbind.set_token(cMyToken);
		//		//selfhbind.set_load1(0u);
		//		//selfhbind.set_load2(0u);

		//		time_t timeNow;
		//		timeNow = time(NULL);

		//		//sr_printf(SR_LOGLEVEL_NORMAL, "********UpdateSelfHeartbeatToDB (deviceid, timeNow)=(%d, %"SR_PRIu64")\n", selfhbind.deviceid(), timeNow);

		//		////��������������ݿ�
		//		//UpdatePeerHeartbeatToDB(&selfhbind, timeNow);

		//		UpdateSelfHeartbeatToDB(m_iSelfDeviceid, timeNow);
		//	}
		//	else
		//	{
		//		sr_printf(SR_LOGLEVEL_ERROR, "%s RegisterSelf ok but m_iSelfDeviceid is 0\n", __FUNCTION__);
		//		return false;
		//	}
		//}

	}

	//���������߳�
	ServerTcpStart(CParser::GetInstance()->GetMyListenPort());

	// ��HTTP�߳�������HTTP����
	if (m_pProcessHttpThread != NULL
		&& m_pMainThread != NULL
		&& CParser::GetInstance()->GetMyHTTPListenPort() != 0)
	{
		char strmyselfip[64] = { 0 };
		sprintf(strmyselfip, "%s", CParser::GetInstance()->GetMysqlIpAddr().c_str());

		typedef CBufferT<unsigned int, CAsyncThread*, CDevMgr*, void*, void*, void*, void*, void*> CParam;
		CParam* pParam;
		pParam = new CParam(strmyselfip, strlen(strmyselfip), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, CParser::GetInstance()->GetMyHTTPListenPort(), m_pMainThread, this);

		typedef void (CDevMgr::* ACTION)(void*);
		m_pProcessHttpThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*this, &CDevMgr::StartHttpServer, (void*)pParam));
	}

	if (m_devConnMgr)
	{
		//��ȡdevmgr.conf�����ļ�
		for (int i = 1; i <= MAX_DEVMGR_NUMS; ++i)
		{
			if (i==1)
			{
				if (CParser::GetInstance()->GetMaster1IpAddr().empty()
					|| (CParser::GetInstance()->GetMaster1Port() == 0))
				{
					continue;
				}
				else
				{
					SRMC::DevmgrSockInfo* pdsinfo = new SRMC::DevmgrSockInfo();
					pdsinfo->m_ip = CParser::GetInstance()->GetMaster1IpAddr();
					pdsinfo->m_port = CParser::GetInstance()->GetMaster1Port();
					m_devConnMgr->m_devmgrinfomanager->m_devmgrInfos.push_back(pdsinfo);
				}
			} 
			else if (i==2)
			{
				if (CParser::GetInstance()->GetMaster2IpAddr().empty()
					|| (CParser::GetInstance()->GetMaster2Port() == 0))
				{
					continue;
				}
				else
				{
					SRMC::DevmgrSockInfo* pdsinfo = new SRMC::DevmgrSockInfo();
					pdsinfo->m_ip = CParser::GetInstance()->GetMaster2IpAddr();
					pdsinfo->m_port = CParser::GetInstance()->GetMaster2Port();
					m_devConnMgr->m_devmgrinfomanager->m_devmgrInfos.push_back(pdsinfo);
				}
			} 
			else if (i==3)
			{
				if (CParser::GetInstance()->GetMaster3IpAddr().empty()
					|| (CParser::GetInstance()->GetMaster3Port() == 0))
				{
					continue;
				}
				else
				{
					SRMC::DevmgrSockInfo* pdsinfo = new SRMC::DevmgrSockInfo();
					pdsinfo->m_ip = CParser::GetInstance()->GetMaster3IpAddr();
					pdsinfo->m_port = CParser::GetInstance()->GetMaster3Port();
					m_devConnMgr->m_devmgrinfomanager->m_devmgrInfos.push_back(pdsinfo);
				}
			} 
			else if (i==4)
			{
				if (CParser::GetInstance()->GetMaster4IpAddr().empty()
					|| (CParser::GetInstance()->GetMaster4Port() == 0))
				{
					continue;
				}
				else
				{
					SRMC::DevmgrSockInfo* pdsinfo = new SRMC::DevmgrSockInfo();
					pdsinfo->m_ip = CParser::GetInstance()->GetMaster4IpAddr();
					pdsinfo->m_port = CParser::GetInstance()->GetMaster4Port();
					m_devConnMgr->m_devmgrinfomanager->m_devmgrInfos.push_back(pdsinfo);
				}
			}
		}

		if (m_devConnMgr->m_devmgrinfomanager->m_devmgrInfos.size() > 0)
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Init() master devmgr list not empty, my role is slave mode.\n");
			m_devConnMgr->ConnectDevmgr();
		}
		else
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Init() master devmgr list is empty, my role is single mode.\n");
		}
	}


	m_MapDeviceHeartbeatTime.clear();

//#ifdef WIN32
//	m_idCheckHeartbeatTimer = timeSetEvent(CHECK_TIME * 1000, 1, (LPTIMECALLBACK)CheckHeartbeatTimerCallbackFunc, (DWORD)this, TIME_PERIODIC);
//	if (NULL == m_idCheckHeartbeatTimer)
//	{
//		printf("timeSetEvent() failed with error %d\n", GetLastError());
//		return false;
//	}
//#elif defined LINUX
//	//������������ʱ��CheckHeartbeatTimer��ָ��ʱ����Ϊ5��
//	bool  bCreatTimerOk;
//	bCreatTimerOk = CreateTimer(this->m_idCheckHeartbeatTimer, CheckHeartbeatTimerCallbackFunc, this);
//
//	if (bCreatTimerOk)
//	{
//		//������ʱ��
//		bool bSetTimerOk = SetTimer(this->m_idCheckHeartbeatTimer, CHECK_TIME, 0);
//		if (!bSetTimerOk)
//		{
//			sr_printf(SR_LOGLEVEL_ERROR, "========------>>>>CheckHeartbeatTimer set failed\n");
//		}
//	}
//	else
//	{
//		sr_printf(SR_LOGLEVEL_ERROR, "==---------->>>CheckHeartbeatTimer Create failed!!!\n");
//	}
//#endif

	if (!m_checkclientheartbeat_periodtimer)
		m_checkclientheartbeat_periodtimer = createTimer(CHECK_CLIENT_HEARTBEAT_TIME, SRMC::e_periodicity_timer, e_check_client_heartbeat_timer, 0ull);

	if (!m_upselfhb2db_periodtimer)
		m_upselfhb2db_periodtimer = createTimer(UP_SELF_HEARTBEAT_TIME, SRMC::e_periodicity_timer, e_update_selfheartbeat2db_timer, 0ull); // ��ע������Լ���������ʱ��������ļ�����

	if (!m_writeredis2mysql_periodtimer)
		m_writeredis2mysql_periodtimer = createTimer(WRITE_DB_TIME, SRMC::e_periodicity_timer, e_write_redis2mysql_timer, 0ull);

	//if (!m_heart2devmgr_periodtimer)
	//	m_heart2devmgr_periodtimer = createTimer(SEND_HEARTBEAT_TO_DEVMGR_TIME, SRMC::e_periodicity_timer, e_hearttodevmgr_timer, 0ull);

	sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Init() success\n");

	//// �����ǲ���hashHMGet�������ؼ�ֵ�Ե�����
	//m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
	//char local_domain_authcode[128] = { 0 };
	//sprintf(local_domain_authcode, "%s", "local_domain_authcode");
	//
	//char cShengFieldName[128] = { 0 };
	//char cShiFieldName[128] = { 0 };
	//char cXianFieldName[128] = { 0 };
	//char cInnerFieldName[128] = { 0 };
	//char cOuterFieldName[128] = { 0 };
	//char cVPNFieldName[128] = { 0 };
	//sprintf(cInnerFieldName, "%s", "inner");
	//sprintf(cOuterFieldName, "%s", "outer");
	//sprintf(cVPNFieldName, "%s", "vpn");
	//sprintf(cShengFieldName, "%s", "sheng");
	//sprintf(cShiFieldName, "%s", "shi");
	//sprintf(cXianFieldName, "%s", "xian");
	//REDISKEY strlocaldomainauthcodekey = local_domain_authcode;
	//REDISFILEDS vGetFileds;
	//vGetFileds.clear();
	//RedisReplyArray vRedisReplyArray;
	//vRedisReplyArray.clear();
	//RedisReplyKeyValuePair vRedisReplyPair;
	//vRedisReplyPair.clear();
	//// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
	//REDISKEY strInnerFieldName = cInnerFieldName;
	//REDISKEY strOuterFieldName = cOuterFieldName;
	//REDISKEY strVPNFieldName = cVPNFieldName;
	//REDISKEY strShengFieldName = cShengFieldName;
	//REDISKEY strShiFieldName = cShiFieldName;
	//REDISKEY strXianFieldName = cXianFieldName;
	//vGetFileds.push_back(strInnerFieldName);
	//vGetFileds.push_back(strShengFieldName);
	//vGetFileds.push_back(strOuterFieldName);
	//vGetFileds.push_back(strShiFieldName);
	//vGetFileds.push_back(strVPNFieldName);
	//vGetFileds.push_back(strXianFieldName);
	////bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strlocaldomainauthcodekey, vGetFileds, vRedisReplyArray);
	////sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Init() bhashHMGet_ok=%d, vGetFileds.size()=%d, vRedisReplyArray.size()=%d\n", bhashHMGet_ok, vGetFileds.size(), vRedisReplyArray.size());
	////if (bhashHMGet_ok
	////	&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
	////{
	////	// ���ؽ��������Filed push_back˳�򷵻�
	////	for (RedisReplyArray::iterator reply_itor = vRedisReplyArray.begin();
	////		reply_itor != vRedisReplyArray.end(); reply_itor++)
	////	{
	////		sr_printf(SR_LOGLEVEL_NORMAL, "==>>[(*reply_itor).type=%d, (*reply_itor).str.c_str()=%s]\n", (*reply_itor).type, (*reply_itor).str.c_str());
	////		if ((*reply_itor).type == 1) //REDIS_REPLY_STRING
	////		{
	////		} 
	////		else if ((*reply_itor).type == 4) //REDIS_REPLY_NIL
	////		{
	////		}
	////	}
	////}
	//bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strlocaldomainauthcodekey, vGetFileds, vRedisReplyPair);
	//sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Init() bhashHMGet_ok=%d, vGetFileds.size()=%d, vRedisReplyPair.size()=%d\n", bhashHMGet_ok, vGetFileds.size(), vRedisReplyPair.size());
	//if (bhashHMGet_ok
	//	&& vRedisReplyPair.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
	//{
	//	// ���ؽ����һ������Filed push_back˳�򷵻�,�����map�Ĺؼ����ǰ�����ĸ�������е�
	//	for (RedisReplyKeyValuePair::iterator reply_itor = vRedisReplyPair.begin();
	//		reply_itor != vRedisReplyPair.end(); reply_itor++)
	//	{
	//		sr_printf(SR_LOGLEVEL_NORMAL, "==>>[reply_itor->first.c_str()=%s, (reply_itor->second.type=%d, reply_itor->second.str.c_str()=%s)]\n", reply_itor->first.c_str(), reply_itor->second.type, reply_itor->second.str.c_str());
	//	}
	//	// ���Բ����ؼ���˳���ȡֵ
	//	RedisReplyKeyValuePair::iterator reply_kv_itor = vRedisReplyPair.find(strShengFieldName);
	//	if (reply_kv_itor != vRedisReplyPair.end())
	//	{
	//		sr_printf(SR_LOGLEVEL_NORMAL, "==>>[key=%s, value=(type:%d, str:%s)]\n", reply_kv_itor->first.c_str(), reply_kv_itor->second.type, reply_kv_itor->second.str.c_str());
	//	}
	//}

	return true;
}

void CDevMgr::Fini()
{
	sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Fini() success\n");

#ifdef REDIS_DEF
	//if (m_bConnRedis)
	//{
	//	m_pRedis->disconnect();

	//	sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::CLOSE REDIS() success\n");
	//}
	for (unsigned int i = 0; i < m_uiNeedRedisCoonNum; i++)
	{
		if (m_pRedisConnList[i] != NULL)
		{
			m_pRedisConnList[i]->disconnect();
			sr_printf(SR_LOGLEVEL_NORMAL, "m_pRedisConnList[%d]->disconnect() success.\n", i);
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "m_pRedisConnList[%d] == NULL error.\n", i);
		}
	}
	delete[] m_pRedisConnList;
#endif

	if (m_bConnMysql)
	{
		CDBConnector::GetInstance()->CloseDB();

		sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::Fini CloseDB() success\n");
	}

//#ifdef WIN32
//	timeKillEvent(m_idCheckHeartbeatTimer);
//	m_idCheckHeartbeatTimer = 0;
//#elif defined LINUX
//	//ɾ����ʱ��
//	if (m_idCheckHeartbeatTimer != 0)
//	{
//		DeleteTimer(m_idCheckHeartbeatTimer);
//		m_idCheckHeartbeatTimer = 0;
//	}
//#endif

	//�߳��ͷ�
	if (NULL != m_pMainThread)
	{
		m_pMainThread->Stop();
		delete m_pMainThread;
		m_pMainThread = NULL;
	}
	if (NULL != m_pWriteDBThread)
	{
		m_pWriteDBThread->Stop();
		delete m_pWriteDBThread;
		m_pWriteDBThread = NULL;
	}	
	if (NULL != m_pUpMsqThread)
	{
		m_pUpMsqThread->Stop();
		delete m_pUpMsqThread;
		m_pUpMsqThread = NULL;
	}

	if (NULL != m_pUpConfigThread)
	{
		m_pUpConfigThread->Stop();
		delete m_pUpConfigThread;
		m_pUpConfigThread = NULL;
	}
	
	if (NULL != m_pProcessHttpThread)
	{
		m_pProcessHttpThread->Stop();
		delete m_pProcessHttpThread;
		m_pProcessHttpThread = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	if (NULL != m_pMainUserConfDetail)
	{
		delete m_pMainUserConfDetail;
		m_pMainUserConfDetail = NULL;
	}
	if (NULL != m_pUpmsgUserConfDetail)
	{
		delete m_pUpmsgUserConfDetail;
		m_pUpmsgUserConfDetail = NULL;
	}
	if (NULL != m_pWriteDBUserConfDetail)
	{
		delete m_pWriteDBUserConfDetail;
		m_pWriteDBUserConfDetail = NULL;
	}

	if (NULL != m_pMainDevice)
	{
		delete m_pMainDevice;
		m_pMainDevice = NULL;
	}
	if (NULL != m_pUpmsgDevice)
	{
		delete m_pUpmsgDevice;
		m_pUpmsgDevice = NULL;
	}

	if (NULL != m_pMainDeviceConfig)
	{
		delete m_pMainDeviceConfig;
		m_pMainDeviceConfig = NULL;
	}
	if (NULL != m_pUpmsgDeviceConfig)
	{
		delete m_pUpmsgDeviceConfig;
		m_pUpmsgDeviceConfig = NULL;
	}


	if (NULL != m_pMainConference)
	{
		delete m_pMainConference;
		m_pMainConference = NULL;
	}
	if (NULL != m_pUpmsgConference)
	{
		delete m_pUpmsgConference;
		m_pUpmsgConference = NULL;
	}


	if (NULL != m_pMainConfReport)
	{
		delete m_pMainConfReport;
		m_pMainConfReport = NULL;
	}
	if (NULL != m_pUpmsgConfReport)
	{
		delete m_pUpmsgConfReport;
		m_pUpmsgConfReport = NULL;
	}


	if (NULL != m_pMainDeviceConfDetail)
	{
		delete m_pMainDeviceConfDetail;
		m_pMainDeviceConfDetail = NULL;
	}
	if (NULL != m_pUpmsgDeviceConfDetail)
	{
		delete m_pUpmsgDeviceConfDetail;
		m_pUpmsgDeviceConfDetail = NULL;
	}


	m_mapAsyncThreadDevice.clear();
	m_mapAsyncThreadDeviceConfig.clear();
	m_mapAsyncThreadConference.clear();
	m_mapAsyncThreadConfReport.clear();
	m_mapAsyncThreadUserConfDetial.clear();
	m_mapAsyncThreadDeviceConfDetail.clear();

	if (NULL != m_devConnMgr)
	{
		delete m_devConnMgr;
		m_devConnMgr = NULL;
	}

	//if (NULL != m_HttpConnMgr)
	//{
	//	delete m_HttpConnMgr;
	//	m_HttpConnMgr = NULL;
	//}

	return;
}

bool CDevMgr::ServerTcpStart(const int listenport)
{
//#ifdef WIN32
//	m_pServerEvent_loop = CreateTcpEventLoop(T_LOOP_SELECT);
//#elif defined LINUX
//	m_pServerEvent_loop = CreateTcpEventLoop(T_LOOP_EPOLL);
//	m_pServerEvent_loop->SetThreadNum(16);//
//#endif
//
//	//�շ���ʼ�����������
//	m_pServerEvent_loop->Start(&g_tcp_handler, m_pTimer);    
//    m_devConnMgr = new DeviceConnMgr(m_pServerEvent_loop, this);
//	//m_ptcp_server = new TcpServer(this, m_fullpath, m_pServerEvent_loop);    
//	m_pserver_socket = m_pServerEvent_loop->CreateSocket();
//    //m_pServerEvent_loop->Listen(m_pserver_socket, m_ptcp_server, INADDR_ANY, listenport);
//    m_pServerEvent_loop->Listen(m_pserver_socket, m_devConnMgr, INADDR_ANY, listenport);


	//////////////////////////////////////////////////////////////////////////
	m_pTcp_trans_module = CreateTcpTransLib();
	if (!m_pTcp_trans_module->Init(16, ASYNC_MODEL_EPOLL))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "CDevMgr::ServerTcpStart() failed: tcp trans module init failed!!\n");
		return false;
	}

	m_devConnMgr = new DeviceConnMgr(m_pTcp_trans_module, this);
	if (!m_devConnMgr->Init(NULL, listenport))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "CDevMgr::ServerTcpStart() Listen failed!! port:%d\n", listenport);
		return false;
	}

	sr_printf(SR_LOGLEVEL_NORMAL, "CDevMgr::ServerTcpStart() success, listenport = %d\n", listenport);

	return true;
}

// ��HTTP�߳�������HTTP����
void CDevMgr::StartHttpServer(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned int, CAsyncThread*, CDevMgr*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;
	unsigned int uiihttplistenport = pParam->m_Arg1;
	CAsyncThread* pMainThread = pParam->m_Arg2;
	CDevMgr* pDevMgr = pParam->m_Arg3;

	
	char strmyselfip[64] = { 0 };
	if (pParam->m_nLen1 > 0)
	{
		memcpy(strmyselfip, pParam->m_pData1, pParam->m_nLen1);
	}

	m_HttpConnMgr = new HttpConnMgr(pMainThread, pDevMgr);
	m_HttpConnMgr->Init(strmyselfip, uiihttplistenport);
}

#if 0
int CDevMgr::GetProcessPath()
{
	//======================
	//��ȡ����·��
	//======================
	memset(m_fullpath, 0, sizeof(m_fullpath));
	char tcFullPath[256];
	GetModuleFileName(NULL, tcFullPath, 256);
	CString strFullPath(tcFullPath);
	int nPos = strFullPath.ReverseFind(_T('\\'));
	if (nPos > 0)
		strFullPath = strFullPath.Left(nPos);
	sprintf(m_fullpath, "%s\\devmgr.log", strFullPath);
	//======================
	//��ȡ����·������
	//======================
	return 0;
}
#endif
void CDevMgr::Splitpathex(const char *path, char *dirname, char *filename)
{
	char*	lastSlash;
	lastSlash = (char*)strrchr(path, '\\');
	char tmpChar = *lastSlash;
	*lastSlash = '\0';

	if (dirname)
		strcpy(dirname, path);
	if (filename)
		strcpy(filename, lastSlash + 1);

	*lastSlash = tmpChar;
}

int CDevMgr::GetProcessPath()
{
	char buf[1024] = { 0 };
#ifdef WIN32
	char pathname[1024];
	char filename[1024];
	GetModuleFileName(NULL, buf, 1024);
	Splitpathex(buf, pathname, filename);
	sprintf(m_fullpath, "%s\\devmgr.log", pathname);

	return 0;
#elif defined LINUX
	int i;
	int count = 1024;
	int rslt = readlink("/proc/self/exe", buf, count - 1);
	if (rslt < 0 || (rslt >= count - 1))
	{
		return NULL;
	}
	buf[rslt] = '\0';
	for (i = rslt; i >= 0; i--)
	{
		//printf("buf[%d] %c\n", i, buf[i]);
		if (buf[i] == '/')
		{
			buf[i + 1] = '\0';
			break;
		}
	}
	sprintf(m_fullpath, "%s/devmgr.log", buf);

	sprintf(m_msgLogfile, "%s/msgLog.log", buf);

	return 0;
#endif
}

int main(int argc, char* argv[])
{
	printf("main\n");

#ifdef LINUX
	bool use_daemon = false;
	for (int i = 0; i < argc; i++)
	{
		if (strcmp("-daemon", argv[i]) == 0 || strcmp("-d", argv[i]) == 0)
		{
			use_daemon = true;
			printf("main argv:%s\n", argv[i]);
			break;
		}
	}
	if (use_daemon)
	{
		printf("devmgr use daemon running mode:true\n");
	}
	else
	{
		printf("devmgr use daemon running mode:false\n");
	}
	if (use_daemon && 0 != daemon(1, 0))
	{
		printf("devmgr daemon start failed! errorcode:%d, desc:%s\n", errno, strerror(errno));
		return -1;
	}
#endif

	CDevMgr devmgr;
	devmgr.Init();

#if 1
	while (true) {
#ifdef _WIN32
		Sleep(1000);
#else
		usleep(1000 * 1000);
#endif
	}
#endif

	return 0;
}

void CDevMgr::UpdateConfRealtime(unsigned long long confid, int realtimetype, char* time, time_t lltime, int permanentenable/*=0*/)
{
	//sr_printf(SR_LOGLEVEL_INFO, "==into-->> UpdateConfRealtime realtimetype=%d.\n", realtimetype);

	if (!m_pUpMsqThread)
		return;

	if (time == NULL)
		return;

	typedef CBufferT<unsigned long long, int, CAsyncThread*, time_t, int, CAsyncThread*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, confid, realtimetype, m_pUpMsqThread, lltime, permanentenable, m_pWriteDBThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_UpdateConfRealtime, (void*)pParam));
	return;
}

void CDevMgr::Handle_UpdateConfRealtime(void* pArg)
{
	//sr_printf(SR_LOGLEVEL_INFO, "==into-->> Handle_UpdateConfRealtime .\n");

	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, int, CAsyncThread*, time_t, int, CAsyncThread*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	char* ptime = (char*)pParam->m_pData1;

	unsigned long long confid = pParam->m_Arg1;
	int realtimetype = pParam->m_Arg2;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg3);
	time_t add_time = (time_t)(pParam->m_Arg4);
	int permanentenable = pParam->m_Arg5;
	CAsyncThread* pWriteDBThread = (CAsyncThread*)(pParam->m_Arg6);

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateConfRealtime pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CConference* pConference = NULL;
	std::map<CAsyncThread*, CConference*>::iterator iter_mapAsyncThreadConference = m_mapAsyncThreadConference.find(pUpMsgThread);
	if (iter_mapAsyncThreadConference != m_mapAsyncThreadConference.end())
	{
		pConference = iter_mapAsyncThreadConference->second;
	}

	if (pConference == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateConfRealtime pConference is NULL.\n");

		return;
	}

	do
	{
		{
#ifdef LINUX
			float time_use = 0;
			struct timeval start;
			struct timeval end;
			gettimeofday(&start, NULL); // ��s level
#endif 

			m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);

			char update_confrealtime_confid_key[128] = { 0 };
			char cConfidFieldName[128] = { 0 };
			char conf_id_value[128] = { 0 };
			char cRealtimeFieldName[128] = { 0 };
			char real_time_value[128] = { 0 };
			char cPermanentEnableFieldName[32] = { 0 };
			char permanent_enable_value[32] = { 0 };

			if (realtimetype == 0) // real_starttime
			{
#ifdef WIN32
				sprintf(update_confrealtime_confid_key, "upconfrealstarttime_%I64d", confid);
#elif defined LINUX
				sprintf(update_confrealtime_confid_key, "upconfrealstarttime_%lld", confid);
#endif

				sprintf(cRealtimeFieldName, "%s", "real_starttime");
			} 
			else if (realtimetype == 1) // real_endtime
			{
#ifdef WIN32
				sprintf(update_confrealtime_confid_key, "upconfrealendtime_%I64d", confid);
#elif defined LINUX
				sprintf(update_confrealtime_confid_key, "upconfrealendtime_%lld", confid);
#endif

				sprintf(cRealtimeFieldName, "%s", "real_endtime");
			}

			sprintf(cConfidFieldName, "%s", "conf_id");
#ifdef WIN32
			sprintf(conf_id_value, "%I64d", confid);
#elif defined LINUX
			sprintf(conf_id_value, "%lld", confid);
#endif

			sprintf(cPermanentEnableFieldName, "%s", "permanent_enable");
			sprintf(permanent_enable_value, "%d", permanentenable);
			

			REDISKEY strRealtimeFieldName = cRealtimeFieldName;
			REDISVALUE strrealtimevalue = ptime;

			REDISKEY strUpdateConfRealtimeConfidkey = update_confrealtime_confid_key;

			REDISVDATA vConfRealtimeInfo;
			vConfRealtimeInfo.clear();

			REDISKEY strConfidFieldName = cConfidFieldName;
			REDISVALUE strconfidvalue = conf_id_value;
			vConfRealtimeInfo.push_back(strConfidFieldName);
			vConfRealtimeInfo.push_back(strconfidvalue);

			vConfRealtimeInfo.push_back(strRealtimeFieldName);
			vConfRealtimeInfo.push_back(strrealtimevalue);

			REDISKEY strPermanentEnableFieldName = cPermanentEnableFieldName;
			REDISVALUE strpermanentenablevalue = permanent_enable_value;
			vConfRealtimeInfo.push_back(strPermanentEnableFieldName);
			vConfRealtimeInfo.push_back(strpermanentenablevalue);

			/*bhashHMSet_ok =*/ m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strUpdateConfRealtimeConfidkey, vConfRealtimeInfo);

			bool bListLPushOK = false;
			bListLPushOK = m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", update_confrealtime_confid_key);
			sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateConfRealtime listLPush ** (%s) **bListLPushOK=%d\n", update_confrealtime_confid_key, bListLPushOK);

			if (pWriteDBThread)
			{
				typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
				CParam* pParam;
				pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pWriteDBThread);

				typedef void (CDevMgr::* ACTION)(void*);
				pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
					*this, &CDevMgr::Handle_WriteUserConfInfoToDB, (void*)pParam));
			}


#ifdef LINUX
			gettimeofday(&end, NULL); // ��s level
			time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

			sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateConfRealtime into db(4) ** (%s) **time_use** is: %lf us \n", update_confrealtime_confid_key, time_use);
#endif 

		}
	} while (0);



	delete pParam;
	pParam = NULL;
}

void CDevMgr::InsertConfReport(unsigned long long confid, unsigned long long confreportid, char* time, time_t lltime)
{
	if (!m_pUpMsqThread)
		return;

	if (time == NULL)
		return;

	typedef CBufferT<unsigned long long, unsigned long long, CAsyncThread*, time_t, CAsyncThread*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, confid, confreportid, m_pUpMsqThread, lltime, m_pWriteDBThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_InsertConfReport, (void*)pParam));
	return;
}

void CDevMgr::Handle_InsertConfReport(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, unsigned long long, CAsyncThread*, time_t, CAsyncThread*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	char* ptime = (char*)pParam->m_pData1;

	unsigned long long confid = pParam->m_Arg1;
	unsigned long long confreportid = pParam->m_Arg2;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg3);
	time_t add_time = (time_t)(pParam->m_Arg4);
	CAsyncThread* pWriteDBThread = (CAsyncThread*)(pParam->m_Arg5);

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_InsertConfReport pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}

	if (pConfReport == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_InsertConfReport pConfReport is NULL.\n");

		return;
	}
	
	do
	{
		{
#ifdef LINUX
			float time_use = 0;
			struct timeval start;
			struct timeval end;
			gettimeofday(&start, NULL); // ��s level
#endif 

			m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);
			char confstarttimerptid_confid_key[128] = { 0 };

			
			char cConfReportidFieldName[128] = { 0 };
			char conf_report_id_value[128] = { 0 };
			char cRealStarttimeFieldName[128] = { 0 };
			char real_starttime_value[128] = { 0 };
			
			sprintf(cConfReportidFieldName, "%s", "conf_report_id");
			sprintf(cRealStarttimeFieldName, "%s", "real_starttime");

#ifdef WIN32
			sprintf(confstarttimerptid_confid_key, "confstimerptidset_%I64d", confid);
			sprintf(conf_report_id_value, "%I64d", confreportid);
#elif defined LINUX
			sprintf(confstarttimerptid_confid_key, "confstimerptidset_%lld", confid);
			sprintf(conf_report_id_value, "%lld", confreportid);
#endif

			REDISKEY strConfReportidFieldName = cConfReportidFieldName;
			REDISVALUE strconfreportidvalue = conf_report_id_value;

			REDISKEY strRealStarttimeFieldName = cRealStarttimeFieldName;
			REDISVALUE strrealstarttimevalue = ptime;


			m_pRedisConnList[e_RC_TT_UpMsqThread]->sethashvalue(confstarttimerptid_confid_key, conf_report_id_value, ptime);// ��ֹ�����������ݻ�дʱ�û����ֱ������ٿ�

			//��������mysql�ᵼ���������Ҳ���conf_report_id
			char cConfidFieldName[128] = { 0 };
			char conf_id_value[128] = { 0 };
			sprintf(cConfidFieldName, "%s", "conf_id");
			char InsertConfreportInfo_confid_rptid_key[128] = { 0 };
#ifdef WIN32
			sprintf(conf_id_value, "%I64d", confid);
			sprintf(InsertConfreportInfo_confid_rptid_key, "insconfrptinfo_%I64d_%I64d", confid, confreportid);
#elif defined LINUX
			sprintf(conf_id_value, "%lld", confid);
			sprintf(InsertConfreportInfo_confid_rptid_key, "insconfrptinfo_%lld_%lld", confid, confreportid);
#endif
			REDISKEY strInsertConfreportInfoConfidRptidkey = InsertConfreportInfo_confid_rptid_key;

			REDISVDATA vConfreportConfidInfo;
			vConfreportConfidInfo.clear();

			REDISKEY strConfidFieldName = cConfidFieldName;
			REDISVALUE strconfidvalue = conf_id_value;
			vConfreportConfidInfo.push_back(strConfidFieldName);
			vConfreportConfidInfo.push_back(strconfidvalue);

			vConfreportConfidInfo.push_back(strConfReportidFieldName);
			vConfreportConfidInfo.push_back(strconfreportidvalue);

			vConfreportConfidInfo.push_back(strRealStarttimeFieldName);
			vConfreportConfidInfo.push_back(strrealstarttimevalue);

			char cRealEndtimeFieldName[128] = { 0 };
			char real_endtime_value[128] = { 0 };
			sprintf(cRealEndtimeFieldName, "%s", "real_endtime");
			sprintf(real_endtime_value, "%s", "19700101000000");

			REDISKEY strRealEndtimeFieldName = cRealEndtimeFieldName;
			REDISVALUE strrealendtimevalue = real_endtime_value;
			vConfreportConfidInfo.push_back(strRealEndtimeFieldName);
			vConfreportConfidInfo.push_back(strrealendtimevalue);

			/*bhashHMSet_ok =*/ m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strInsertConfreportInfoConfidRptidkey, vConfreportConfidInfo);

			bool bListLPushOK = false;
			bListLPushOK = m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", InsertConfreportInfo_confid_rptid_key);
			sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_InsertConfReport listLPush ** (%s) **bListLPushOK=%d\n", InsertConfreportInfo_confid_rptid_key, bListLPushOK);

			if (pWriteDBThread)
			{
				typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
				CParam* pParam;
				pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pWriteDBThread);

				typedef void (CDevMgr::* ACTION)(void*);
				pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
					*this, &CDevMgr::Handle_WriteUserConfInfoToDB, (void*)pParam));
			}

			//char real_endtime_value[128] = { 0 };
			//sprintf(real_endtime_value, "%s", "19700101000000");

			//pConfReport->SetConfID(confid);
			//pConfReport->SetConfReportID(confreportid);
			//pConfReport->SetStartTime(ptime);
			//pConfReport->SetStopTime(real_endtime_value);

			//bool bInsertOK = false;
			//bInsertOK = pConfReport->InsertConfReportRecordToDB();

			
			//time_t timeNow;
			//struct tm *ptmNow;
			//timeNow = time(NULL);
			//ptmNow = localtime(&timeNow);

			//time_t push_time = timeNow - add_time;

#ifdef LINUX
			gettimeofday(&end, NULL); // ��s level
			time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

			sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_InsertConfReport into db(4) ** (%s) **time_use** is: %lf us \n", confstarttimerptid_confid_key, time_use);
#endif 

		}
	} while (0);



	delete pParam;
	pParam = NULL;
}

void CDevMgr::UpdateConfReport(unsigned long long confid, unsigned long long confreportid, char* time, time_t lltime)
{
	if (!m_pUpMsqThread)
		return;

	if (time == NULL)
		return;

	typedef CBufferT<unsigned long long, unsigned long long, CAsyncThread*, time_t, CAsyncThread*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, confid, confreportid, m_pUpMsqThread, lltime, m_pWriteDBThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_UpdateConfReport, (void*)pParam));
	return;
}

void CDevMgr::Handle_UpdateConfReport(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, unsigned long long, CAsyncThread*, time_t, CAsyncThread*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	char* ptime = (char*)pParam->m_pData1;

	unsigned long long confid = pParam->m_Arg1;
	unsigned long long confreportid = pParam->m_Arg2;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg3);
	time_t add_time = (time_t)(pParam->m_Arg4);
	CAsyncThread* pWriteDBThread = (CAsyncThread*)(pParam->m_Arg5);

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateConfReport pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}

	if (pConfReport == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateConfReport pConfReport is NULL.\n");

		return;
	}

	do
	{
		{
#ifdef LINUX
			float time_use = 0;
			struct timeval start;
			struct timeval end;
			gettimeofday(&start, NULL); // ��s level
#endif 
			m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);
			char cConfidFieldName[128] = { 0 };
			char conf_id_value[128] = { 0 };
			char cConfReportidFieldName[128] = { 0 };
			char conf_report_id_value[128] = { 0 };
			char cRealStarttimeFieldName[128] = { 0 };
			char real_starttime_value[128] = { 0 };
			char cRealEndtimeFieldName[128] = { 0 };

			sprintf(cConfidFieldName, "%s", "conf_id");
			sprintf(cConfReportidFieldName, "%s", "conf_report_id");
			sprintf(cRealStarttimeFieldName, "%s", "real_starttime");
			sprintf(cRealEndtimeFieldName, "%s", "real_endtime");

#ifdef WIN32
			sprintf(conf_id_value, "%I64d", confid);
#elif defined LINUX
			sprintf(conf_id_value, "%lld", confid);
#endif

			char up_confreport_confid_reportid_key[128] = { 0 };

			char confstarttimerptid_confid_key[128] = { 0 };
			sprintf(confstarttimerptid_confid_key, "confstimerptidset_%s", conf_id_value);
			RedisReplyArray vRedisReplyArray;
			vRedisReplyArray.clear();

			bool bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll(confstarttimerptid_confid_key, vRedisReplyArray);

			unsigned int uiConfstrptinfosize = vRedisReplyArray.size();

			sr_printf(SR_LOGLEVEL_DEBUG, " m_pRedisConnList[Handle_UpdateConfReport]->hashHGetAll==%s-->> %d, size = %d\n", confstarttimerptid_confid_key, bhashHGetAll_ok, uiConfstrptinfosize);

			if (bhashHGetAll_ok
				&& (uiConfstrptinfosize > 0)
				&& (uiConfstrptinfosize % 2 == 0))
			{
				bool bFindConfReportID = false;

				for (unsigned int ii = 0; ii < uiConfstrptinfosize;)
				{
					sr_printf(SR_LOGLEVEL_DEBUG, "==Handle_UpdateConfReport==In redis db(4) key:%s ==>>> find starttime=%s, rptid=%s\n", confstarttimerptid_confid_key, vRedisReplyArray[ii].str.c_str(), vRedisReplyArray[ii + 1].str.c_str());
					ii += 2;
				}

				sprintf(real_starttime_value, "%s", vRedisReplyArray[uiConfstrptinfosize - 2].str.c_str());
				sprintf(conf_report_id_value, "%s", vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str());

				//char up_confreport_confid_reportid_key[128] = { 0 };
				sprintf(up_confreport_confid_reportid_key, "upconfreport_%s_%s", conf_id_value, conf_report_id_value);


				REDISKEY strupconfreportidkey = up_confreport_confid_reportid_key;
				REDISVDATA vConfreportdata;
				vConfreportdata.clear();

				REDISKEY strConfidFieldName = cConfidFieldName;
				REDISVALUE strconfidvalue = conf_id_value;
				vConfreportdata.push_back(strConfidFieldName);
				vConfreportdata.push_back(strconfidvalue);

				REDISKEY strConfReportidFieldName = cConfReportidFieldName;
				REDISVALUE strconfreportidvalue = conf_report_id_value;
				vConfreportdata.push_back(strConfReportidFieldName);
				vConfreportdata.push_back(strconfreportidvalue);

				REDISKEY strRealStarttimeFieldName = cRealStarttimeFieldName;
				REDISVALUE strrealstarttimevalue = real_starttime_value;
				vConfreportdata.push_back(strRealStarttimeFieldName);
				vConfreportdata.push_back(strrealstarttimevalue);

				REDISKEY strRealEndtimeFieldName = cRealEndtimeFieldName;
				REDISVALUE strrealendtimevalue = ptime;
				vConfreportdata.push_back(strRealEndtimeFieldName);
				vConfreportdata.push_back(strrealendtimevalue);

				bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strupconfreportidkey, vConfreportdata);

				//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet==confreport_confid_reportid_key----->>>> %d\n", bhashHMSet_ok);
				bool bListLPushOK = false;
				bListLPushOK =  m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", up_confreport_confid_reportid_key);
				sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateConfReport listLPush ** (%s) **bListLPushOK=%d\n", up_confreport_confid_reportid_key, bListLPushOK);

				if (pWriteDBThread)
				{
					typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
					CParam* pParam;
					pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pWriteDBThread);

					typedef void (CDevMgr::* ACTION)(void*);
					pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
						*this, &CDevMgr::Handle_WriteUserConfInfoToDB, (void*)pParam));
				}

				//if (bFindConfReportID)
				//{
				//	m_pRedisConnList[e_RC_TT_UpMsqThread]->deletevalue(confstarttimerptid_confid_key, real_starttime_value);
				//}
			}

#ifdef LINUX
			gettimeofday(&end, NULL); // ��s level
			time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

			sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateConfReport into db(4) ** (%s) **time_use** is: %lf us \n", up_confreport_confid_reportid_key, time_use);
#endif 

		}
	} while (0);



	delete pParam;
	pParam = NULL;

}

void CDevMgr::InsertDeviceConfDetail(unsigned long long confid, int deviceid, unsigned long long confreportid, unsigned long long devicedetailid, char* time, time_t lltime, bool bfromnetmp/* = false*/)
//void CDevMgr::InsertDeviceConfDetail(unsigned long long confid, int deviceid, char* time, time_t lltime, bool bfromnetmp/* = false*/)
{
	if (!m_pUpMsqThread)
		return;

	if (time == NULL)
		return;

	typedef CBufferT<unsigned long long, int, unsigned long long, unsigned long long, CAsyncThread*, time_t, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, confid, deviceid, confreportid, devicedetailid, m_pUpMsqThread, lltime);

	//typedef CBufferT<unsigned long long, int, CAsyncThread*, time_t, void*, void*> CParam;
	//CParam* pParam;
	//pParam = new CParam(time, strlen(time), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, confid, deviceid, m_pUpMsqThread, lltime);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_InsertDeviceConfDetail, (void*)pParam));
	return;
}

void CDevMgr::Handle_InsertDeviceConfDetail(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, int, unsigned long long, unsigned long long, CAsyncThread*, time_t, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	char* ptime = (char*)pParam->m_pData1;

	unsigned long long confid = pParam->m_Arg1;
	int deviceid = pParam->m_Arg2;
	unsigned long long confreportid = pParam->m_Arg3;
	unsigned long long devicedetailid = pParam->m_Arg4;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg5);
	time_t hand_add_time = (time_t)(pParam->m_Arg6);


	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL) // test delet by csh at 2016.10.21
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_InsertDeviceConfDetail pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}

	if (pConfReport == NULL)
	{
		delete pParam;
		pParam = NULL;
		return;
	}


	do
	{
		{
#ifdef LINUX
			float time_use = 0;
			struct timeval start;
			struct timeval end;
			gettimeofday(&start, NULL); // ��s level
#endif 

			char cConfidFieldName[128] = { 0 };
			char conf_id_value[128] = { 0 };
			char cConfReportidFieldName[128] = { 0 };
			char conf_report_id_value[128] = { 0 };
			char cDeviceidFieldName[128] = { 0 };
			char device_id_value[128] = { 0 };
			char cDeviceDetailidFieldName[128] = { 0 };
			char device_detail_id_value[128] = { 0 };
			char cRealuseStarttimeFieldName[128] = { 0 };
			char realuse_starttime_value[128] = { 0 };

			char devconfdetail_confid_deviceid_key[128] = { 0 };

			sprintf(cConfidFieldName, "%s", "conf_id");
			sprintf(cDeviceidFieldName, "%s", "device_id");
			sprintf(cConfReportidFieldName, "%s", "conf_report_id");
			sprintf(cDeviceDetailidFieldName, "%s", "device_detail_id");
			sprintf(cRealuseStarttimeFieldName, "%s", "realuse_starttime");

#ifdef WIN32
			sprintf(conf_id_value, "%I64d", confid);
			sprintf(devconfdetail_confid_deviceid_key, "devconfdtl_%I64d_%d", confid, deviceid);
#elif defined LINUX
			sprintf(conf_id_value, "%lld", confid);
			sprintf(devconfdetail_confid_deviceid_key, "devconfdtl_%lld_%d", confid, deviceid);
#endif

			m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);

			if (confreportid == 0)// ���������netmp����Ҫ��redis����
			{
				bool bFindConfReportID = false;

				char confstarttimerptid_confid_key[128] = { 0 };
				sprintf(confstarttimerptid_confid_key, "confstimerptidset_%s", conf_id_value);
				RedisReplyArray vRedisReplyArray;
				vRedisReplyArray.clear();

				bool bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll(confstarttimerptid_confid_key, vRedisReplyArray);

				unsigned int uiConfstrptinfosize = vRedisReplyArray.size();

				sr_printf(SR_LOGLEVEL_DEBUG, " m_pRedisConnList[Handle_InsertDeviceConfDetail]->hashHGetAll==%s-->> %d, size = %d\n", confstarttimerptid_confid_key, bhashHGetAll_ok, uiConfstrptinfosize);

				if (bhashHGetAll_ok
					&& (uiConfstrptinfosize > 0)
					&& (uiConfstrptinfosize % 2 == 0)) 
				{
					for (unsigned int ii = 0; ii < uiConfstrptinfosize;)
					{
						sr_printf(SR_LOGLEVEL_DEBUG, "==Handle_InsertDeviceConfDetail==In redis db(4) key:%s ==>>> find starttime=%s, rptid=%s\n", confstarttimerptid_confid_key, vRedisReplyArray[ii].str.c_str(), vRedisReplyArray[ii + 1].str.c_str());
						ii += 2;
					}

					// �ҵ�����ٿ������confreportid

#ifdef WIN32
					sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%I64d", &confreportid);
#elif defined LINUX
					sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%lld", &confreportid);
#endif

					bFindConfReportID = true;
				}

				if (bFindConfReportID == false) // ���ڴ���δ�ҵ��������ݿ�
				{
					pConfReport->SetConfID(confid);
					if (pConfReport->SelectDB())// 
					{
						confreportid = pConfReport->GetConfReportID();

#ifdef WIN32
						sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_InsertDeviceConfDetail == find confreportid from ConfReport table=== confid=%I64d, confreportid=%I64d\n", confid, confreportid);
#elif defined LINUX
						sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_InsertDeviceConfDetail == find confreportid from ConfReport table=== confid=%lld, confreportid=%lld\n", confid, confreportid);
#endif
					}
					else
					{
#ifdef WIN32
						sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_InsertDeviceConfDetail == Not find confreportid from ConfReport table=== confid=%I64d\n", confid);
#elif defined LINUX
						sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_InsertDeviceConfDetail == Not find confreportid from ConfReport table=== confid=%lld\n", confid);
#endif
					}

				}

				if (confreportid == 0)
				{
#ifdef WIN32
					sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_InsertDeviceConfDetail == Not find confreportid of Netmp === confid=%I64d\n", confid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_InsertDeviceConfDetail == Not find confreportid of Netmp === confid=%lld\n", confid);
#endif

					delete pParam;
					pParam = NULL;

					return;
				}
			}			

			sprintf(device_id_value, "%d", deviceid);			
			sprintf(realuse_starttime_value, "%s", ptime);

#ifdef WIN32
			sprintf(conf_report_id_value, "%I64d", confreportid);
			sprintf(device_detail_id_value, "%I64d", devicedetailid);
#elif defined LINUX
			sprintf(conf_report_id_value, "%lld", confreportid);
			sprintf(device_detail_id_value, "%lld", devicedetailid);
#endif


			REDISKEY strdevconfdetailconfiddeviceidkey = devconfdetail_confid_deviceid_key;
			REDISVDATA vDevconfdtlConfidDevidData;
			vDevconfdtlConfidDevidData.clear();

			REDISKEY strConfidFieldName = cConfidFieldName;
			REDISVALUE strconfidvalue = conf_id_value;
			vDevconfdtlConfidDevidData.push_back(strConfidFieldName);
			vDevconfdtlConfidDevidData.push_back(strconfidvalue);

			REDISKEY strConfReportidFieldName = cConfReportidFieldName;
			REDISVALUE strconfreportidvalue = conf_report_id_value;
			vDevconfdtlConfidDevidData.push_back(strConfReportidFieldName);
			vDevconfdtlConfidDevidData.push_back(strconfreportidvalue);

			REDISKEY strDeviceidFieldName = cDeviceidFieldName;
			REDISVALUE strdeviceidvalue = device_id_value;
			vDevconfdtlConfidDevidData.push_back(strDeviceidFieldName);
			vDevconfdtlConfidDevidData.push_back(strdeviceidvalue);

			REDISKEY strDeviceDetailidFieldName = cDeviceDetailidFieldName;
			REDISVALUE strdevicedetailidvalue = device_detail_id_value;
			vDevconfdtlConfidDevidData.push_back(strDeviceDetailidFieldName);
			vDevconfdtlConfidDevidData.push_back(strdevicedetailidvalue);

			REDISKEY strRealuseStarttimeFieldName = cRealuseStarttimeFieldName;
			REDISVALUE strrealusestarttimevalue = realuse_starttime_value;
			vDevconfdtlConfidDevidData.push_back(strRealuseStarttimeFieldName);
			vDevconfdtlConfidDevidData.push_back(strrealusestarttimevalue);

			bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strdevconfdetailconfiddeviceidkey, vDevconfdtlConfidDevidData);

			//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet==devconfdetail_confid_deviceid_key---->>>> %d\n", bhashHMSet_ok);

#ifdef LINUX
			gettimeofday(&end, NULL); // ��s level
			time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

			sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_InsertDeviceConfDetail into db(4) ** (%s) **time_use** is: %lf us \n", devconfdetail_confid_deviceid_key, time_use);
#endif 

		}
	} while (0);



	delete pParam;
	pParam = NULL;
}

void CDevMgr::UpdateDeviceConfDetail(unsigned long long confid, int deviceid, char* time, time_t lltime)
{
	if (!m_pUpMsqThread)
		return;

	if (time == NULL)
		return;

	typedef CBufferT<unsigned long long, int, CAsyncThread*, time_t, CAsyncThread*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, confid, deviceid, m_pUpMsqThread, lltime, m_pWriteDBThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_UpdateDeviceConfDetail, (void*)pParam));
	return;
}

void CDevMgr::Handle_UpdateDeviceConfDetail(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, int, CAsyncThread*, time_t, CAsyncThread*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	char* ptime = (char*)pParam->m_pData1;

	unsigned long long confid = pParam->m_Arg1;
	int deviceid = pParam->m_Arg2;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg3);
	time_t hand_add_time = (time_t)(pParam->m_Arg4);
	CAsyncThread* pWriteDBThread = (CAsyncThread*)(pParam->m_Arg5);

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL) // test delet by csh at 2016.10.21
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateDeviceConfDetail pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	// redis db4��devconfdtl_%confid_%deviceid + "realuse_stoptime" ת���� devconfdtl_%confid_%deviceid_%confreportid,ɾ��ǰ��������ӳ�д��mysql��ɾ��

	do
	{
		{
#ifdef LINUX
			float time_use = 0;
			struct timeval start;
			struct timeval end;
			gettimeofday(&start, NULL); // ��s level
#endif 

			m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);
			char cConfidFieldName[128] = { 0 };
			char conf_id_value[128] = { 0 };
			char cConfReportidFieldName[128] = { 0 };
			char conf_report_id_value[128] = { 0 };
			char cDeviceidFieldName[128] = { 0 };
			char device_id_value[128] = { 0 };
			char cDeviceDetailidFieldName[128] = { 0 };
			char device_detail_id_value[128] = { 0 }; 
			char cRealuseStarttimeFieldName[128] = { 0 };
			char realuse_starttime_value[128] = { 0 };
			char cRealuseStoptimeFieldName[128] = { 0 };
			//char realuse_stoptime_value[128] = { 0 };

			char devconfdetail_confid_deviceid_key[128] = { 0 };

			sprintf(cConfidFieldName, "%s", "conf_id");
			sprintf(cDeviceidFieldName, "%s", "device_id");
			sprintf(cConfReportidFieldName, "%s", "conf_report_id");
			sprintf(cDeviceDetailidFieldName, "%s", "device_detail_id");
			sprintf(cRealuseStarttimeFieldName, "%s", "realuse_starttime");
			

			sprintf(cConfidFieldName, "%s", "conf_id");

#ifdef WIN32
			sprintf(conf_id_value, "%I64d", confid);
			sprintf(devconfdetail_confid_deviceid_key, "devconfdtl_%I64d_%d", confid, deviceid);
#elif defined LINUX
			sprintf(conf_id_value, "%lld", confid);
			sprintf(devconfdetail_confid_deviceid_key, "devconfdtl_%lld_%d", confid, deviceid);
#endif

			//////////////////////////////////////////////////////////////////////////

			REDISKEY strdevconfdetailconfiddevidkey = devconfdetail_confid_deviceid_key;
			REDISFILEDS vGetFileds;
			vGetFileds.clear();
			RedisReplyArray vRedisReplyArray;
			vRedisReplyArray.clear();

			// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
			REDISKEY strConfReportidFieldName = cConfReportidFieldName;
			REDISKEY strDeviceDetailidFieldName = cDeviceDetailidFieldName;
			REDISKEY strRealuseStarttimeFieldName = cRealuseStarttimeFieldName;
			vGetFileds.push_back(strConfReportidFieldName);
			vGetFileds.push_back(strDeviceDetailidFieldName);
			vGetFileds.push_back(strRealuseStarttimeFieldName);

			bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMGet(strdevconfdetailconfiddevidkey, vGetFileds, vRedisReplyArray);

			//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMGet==devconfdetail_confid_deviceid_key--->> bhashHMGet_ok:%d, vRedisReplyArray.size:%d\n", bhashHMGet_ok, vRedisReplyArray.size());

			char devconfdetail_confid_devid_rptid_key[128] = { 0 };

			if (bhashHMGet_ok
				&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
			{
				sprintf(conf_report_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(device_detail_id_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(realuse_starttime_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��

				//char devconfdetail_confid_devid_rptid_key[128] = { 0 };
				sprintf(devconfdetail_confid_devid_rptid_key, "devconfdtl_%s_%d_%s", conf_id_value, deviceid, conf_report_id_value);

				sprintf(device_id_value, "%d", deviceid);
				sprintf(cRealuseStoptimeFieldName, "%s", "realuse_stoptime");

				REDISKEY strdevconfdetailconfiddevidrptidkey = devconfdetail_confid_devid_rptid_key;
				REDISVDATA vDevconfdtlConfidDevidRptidData;
				vDevconfdtlConfidDevidRptidData.clear();

				REDISKEY strConfidFieldName = cConfidFieldName;
				REDISVALUE strconfidvalue = conf_id_value;
				vDevconfdtlConfidDevidRptidData.push_back(strConfidFieldName);
				vDevconfdtlConfidDevidRptidData.push_back(strconfidvalue);

				REDISKEY strConfReportidFieldName = cConfReportidFieldName;
				REDISVALUE strconfreportidvalue = conf_report_id_value;
				vDevconfdtlConfidDevidRptidData.push_back(strConfReportidFieldName);
				vDevconfdtlConfidDevidRptidData.push_back(strconfreportidvalue);

				REDISKEY strDeviceidFieldName = cDeviceidFieldName;
				REDISVALUE strdeviceidvalue = device_id_value;
				vDevconfdtlConfidDevidRptidData.push_back(strDeviceidFieldName);
				vDevconfdtlConfidDevidRptidData.push_back(strdeviceidvalue);

				REDISKEY strDeviceDetailidFieldName = cDeviceDetailidFieldName;
				REDISVALUE strdevicedetailidvalue = device_detail_id_value;
				vDevconfdtlConfidDevidRptidData.push_back(strDeviceDetailidFieldName);
				vDevconfdtlConfidDevidRptidData.push_back(strdevicedetailidvalue);

				REDISKEY strRealuseStarttimeFieldName = cRealuseStarttimeFieldName;
				REDISVALUE strrealusestarttimevalue = realuse_starttime_value;
				vDevconfdtlConfidDevidRptidData.push_back(strRealuseStarttimeFieldName);
				vDevconfdtlConfidDevidRptidData.push_back(strrealusestarttimevalue);

				REDISKEY strRealuseStoptimeFieldName = cRealuseStoptimeFieldName;
				REDISVALUE strrealusestoptimevalue = ptime;
				vDevconfdtlConfidDevidRptidData.push_back(strRealuseStoptimeFieldName);
				vDevconfdtlConfidDevidRptidData.push_back(strrealusestoptimevalue);

				bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strdevconfdetailconfiddevidrptidkey, vDevconfdtlConfidDevidRptidData);

				//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet==devconfdetail_confid_devid_rptid_key---->>>> %d\n", bhashHMSet_ok);
				bool bListLPushOK = false;
				bListLPushOK =  m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", devconfdetail_confid_devid_rptid_key);
				sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateDeviceConfDetail listLPush ** (%s) **bListLPushOK=%d\n", devconfdetail_confid_devid_rptid_key, bListLPushOK);

				m_pRedisConnList[e_RC_TT_UpMsqThread]->deletevalue(devconfdetail_confid_deviceid_key);

				if (pWriteDBThread)
				{
					typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
					CParam* pParam;
					pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pWriteDBThread);

					typedef void (CDevMgr::* ACTION)(void*);
					pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
						*this, &CDevMgr::Handle_WriteUserConfInfoToDB, (void*)pParam));
				}
			}

#ifdef LINUX
			gettimeofday(&end, NULL); // ��s level
			time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

			sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateDeviceConfDetail into db(4) ** (%s) **time_use** is: %lf us \n", devconfdetail_confid_devid_rptid_key, time_use);
#endif
		}
	} while (0);



	delete pParam;
	pParam = NULL;
}

void CDevMgr::InsertConfRecord(unsigned long long confid, unsigned long long confreportid, unsigned long long confrecordid, char* time, time_t lltime, char* confname)
{
	if (!m_pUpMsqThread)
		return;

	if (time == NULL)
		return;

	typedef CBufferT<unsigned long long, unsigned long long, unsigned long long, CAsyncThread*, time_t, CAsyncThread*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), confname, strlen(confname), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, confid, confreportid, confrecordid, m_pUpMsqThread, lltime, m_pWriteDBThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_InsertConfRecord, (void*)pParam));
	return;
}

void CDevMgr::Handle_InsertConfRecord(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, unsigned long long, unsigned long long, CAsyncThread*, time_t, CAsyncThread*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	char* ptime = (char*)pParam->m_pData1;

	std::string strconfname;
	strconfname.clear();
	if (pParam->m_nLen2 > 0)
	{
		strconfname.assign(pParam->m_pData2);
	}

	unsigned long long confid = pParam->m_Arg1;
	unsigned long long confreportid = pParam->m_Arg2;
	unsigned long long confrecordid = pParam->m_Arg3;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg4);
	time_t add_time = (time_t)(pParam->m_Arg5);
	CAsyncThread* pWriteDBThread = (CAsyncThread*)(pParam->m_Arg6);

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_InsertConfRecord pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CConfRecord* pConfRecord = NULL;
	std::map<CAsyncThread*, CConfRecord*>::iterator iter_mapAsyncThreadConfRecord = m_mapAsyncThreadConfRecord.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfRecord != m_mapAsyncThreadConfRecord.end())
	{
		pConfRecord = iter_mapAsyncThreadConfRecord->second;
	}

	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}

	if (pConfRecord == NULL
		|| pConfReport == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_InsertConfRecord pConfRecord or pConfReport is NULL.\n");

		return;
	}

	{
#ifdef LINUX
		float time_use = 0;
		struct timeval start;
		struct timeval end;
		gettimeofday(&start, NULL); // ��s level
#endif 
		
		char cConfidFieldName[128] = { 0 };
		char conf_id_value[128] = { 0 };
		char cConfReportidFieldName[128] = { 0 };
		char conf_report_id_value[128] = { 0 };
		char cConfRecordidFieldName[128] = { 0 };
		char conf_record_id_value[128] = { 0 };
		char cCRSRecStarttimeFieldName[128] = { 0 };
		char crsrec_starttime_value[128] = { 0 };
		
		sprintf(cConfidFieldName, "%s", "conf_id");
		sprintf(cConfReportidFieldName, "%s", "conf_report_id");
		sprintf(cConfRecordidFieldName, "%s", "conf_record_id");
		sprintf(cCRSRecStarttimeFieldName, "%s", "starttime");

		char cCRSRecEndtimeFieldName[128] = { 0 };
		char crsrec_endtime_value[128] = { 0 };
		sprintf(cCRSRecEndtimeFieldName, "%s", "endtime");
		sprintf(crsrec_endtime_value, "%s", "19700101000000");

		char cConfnameFieldName[256] = { 0 };
		char conf_name_value[256] = { 0 };
		sprintf(cConfnameFieldName, "%s", "confname");
		sprintf(conf_name_value, "%s", strconfname.c_str());

		char crsconfrecordidset_confid_key[128] = { 0 };
		char Insertcrsrecinfo_confid_confrecid_key[128] = { 0 };
		//char Insertcrsrecinfo_confid_confrptid_confrecid_key[128] = { 0 };
#ifdef WIN32
		sprintf(conf_id_value, "%I64d", confid);
		sprintf(crsconfrecordidset_confid_key, "crsconfrecordidset_%I64d", confid);
		sprintf(Insertcrsrecinfo_confid_confrecid_key, "inscrsrecordid_%I64d_%I64d", confid, confrecordid);
		//sprintf(Insertcrsrecinfo_confid_confrptid_confrecid_key, "inscrsrecordid_%I64d_%I64d_%I64d", confid, confreportid, confrecordid);
#elif defined LINUX
		sprintf(conf_id_value, "%lld", confid);
		sprintf(crsconfrecordidset_confid_key, "crsconfrecordidset_%lld", confid);
		sprintf(Insertcrsrecinfo_confid_confrecid_key, "inscrsrecordid_%lld_%lld", confid, confrecordid);
		//sprintf(Insertcrsrecinfo_confid_confrptid_confrecid_key, "inscrsrecordid_%lld_%lld_%lld", confid, confreportid, confrecordid);
#endif

		m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);

		if (confreportid == 0)// 
		{
			bool bFindConfReportID = false;

			char confstarttimerptid_confid_key[128] = { 0 };
			sprintf(confstarttimerptid_confid_key, "confstimerptidset_%s", conf_id_value);
			RedisReplyArray vRedisReplyArray;
			vRedisReplyArray.clear();

			bool bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll(confstarttimerptid_confid_key, vRedisReplyArray);

			unsigned int uiConfstrptinfosize = vRedisReplyArray.size();

			sr_printf(SR_LOGLEVEL_DEBUG, " m_pRedisConnList[Handle_InsertConfRecord]->hashHGetAll==%s-->> %d, size = %d\n", confstarttimerptid_confid_key, bhashHGetAll_ok, uiConfstrptinfosize);

			if (bhashHGetAll_ok
				&& (uiConfstrptinfosize > 0)
				&& (uiConfstrptinfosize % 2 == 0))
			{
				for (unsigned int ii = 0; ii < uiConfstrptinfosize;)
				{
					sr_printf(SR_LOGLEVEL_DEBUG, "==Handle_InsertConfRecord==In redis db(4) key:%s ==>>> find starttime=%s, rptid=%s\n", confstarttimerptid_confid_key, vRedisReplyArray[ii].str.c_str(), vRedisReplyArray[ii + 1].str.c_str());
					ii += 2;
				}

				// �ҵ�����ٿ������confreportid

#ifdef WIN32
				sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%I64d", &confreportid);
#elif defined LINUX
				sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%lld", &confreportid);
#endif

				bFindConfReportID = true;
			}

			if (bFindConfReportID == false) // ���ڴ���δ�ҵ��������ݿ�
			{
				pConfReport->SetConfID(confid);
				if (pConfReport->SelectDB())// 
				{
					confreportid = pConfReport->GetConfReportID();

#ifdef WIN32
					sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_InsertConfRecord == find confreportid from ConfReport table=== confid=%I64d, confreportid=%I64d\n", confid, confreportid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_InsertConfRecord == find confreportid from ConfReport table=== confid=%lld, confreportid=%lld\n", confid, confreportid);
#endif
				}
				else
				{
#ifdef WIN32
					sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_InsertConfRecord == Not find confreportid from ConfReport table=== confid=%I64d\n", confid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_InsertConfRecord == Not find confreportid from ConfReport table=== confid=%lld\n", confid);
#endif
				}

			}

			if (confreportid == 0)
			{
#ifdef WIN32
				sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_InsertConfRecord == Not find confreportid of Netmp === confid=%I64d\n", confid);
#elif defined LINUX
				sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_InsertConfRecord == Not find confreportid of Netmp === confid=%lld\n", confid);
#endif

				delete pParam;
				pParam = NULL;

				return;
			}
		}

		sprintf(crsrec_starttime_value, "%s", ptime);

#ifdef WIN32
		sprintf(conf_report_id_value, "%I64d", confreportid);
		sprintf(conf_record_id_value, "%I64d", confrecordid);
#elif defined LINUX
		sprintf(conf_report_id_value, "%lld", confreportid);
		sprintf(conf_record_id_value, "%lld", confrecordid);
#endif


		m_pRedisConnList[e_RC_TT_UpMsqThread]->sethashvalue(crsconfrecordidset_confid_key, conf_record_id_value, ptime);// ��ֹ�����������ݻ�дʱ�û����ֱ������ٿ�

		REDISKEY strInsertCRSRecinfoConfidConfrecidkey = Insertcrsrecinfo_confid_confrecid_key;
		//REDISKEY strInsertCRSRecinfoConfidConfrptidConfrecidkey = Insertcrsrecinfo_confid_confrptid_confrecid_key;
		REDISVDATA vConfRecordData;
		vConfRecordData.clear();

		REDISKEY strConfidFieldName = cConfidFieldName;
		REDISVALUE strconfidvalue = conf_id_value;
		vConfRecordData.push_back(strConfidFieldName);
		vConfRecordData.push_back(strconfidvalue);

		REDISKEY strConfRecordidName = cConfRecordidFieldName;
		REDISVALUE strconfrecordidvalue = conf_record_id_value;
		vConfRecordData.push_back(strConfRecordidName);
		vConfRecordData.push_back(strconfrecordidvalue);

		REDISKEY strConfReportidFieldName = cConfReportidFieldName;
		REDISVALUE strconfreportidvalue = conf_report_id_value;
		vConfRecordData.push_back(strConfReportidFieldName);
		vConfRecordData.push_back(strconfreportidvalue);
		
		REDISKEY strCRSRecStarttimeFieldName = cCRSRecStarttimeFieldName;
		REDISVALUE strcrsrecstarttimevalue = crsrec_starttime_value;
		vConfRecordData.push_back(strCRSRecStarttimeFieldName);
		vConfRecordData.push_back(strcrsrecstarttimevalue);

		REDISKEY strCRSRecEndtimeFieldName = cCRSRecEndtimeFieldName;
		REDISVALUE strcrsrecendtimevalue = crsrec_endtime_value;
		vConfRecordData.push_back(strCRSRecEndtimeFieldName);
		vConfRecordData.push_back(strcrsrecendtimevalue);

		REDISKEY strConfnameFieldName = cConfnameFieldName;
		REDISVALUE strconfnamevalue = conf_name_value;
		vConfRecordData.push_back(strConfnameFieldName);
		vConfRecordData.push_back(strconfnamevalue);

		bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strInsertCRSRecinfoConfidConfrecidkey, vConfRecordData);
		bool bListLPushOK = false;
		bListLPushOK = m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", Insertcrsrecinfo_confid_confrecid_key);
		sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_InsertConfRecord listLPush ** (%s) **bListLPushOK=%d\n", Insertcrsrecinfo_confid_confrecid_key, bListLPushOK);

		//bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strInsertCRSRecinfoConfidConfrptidConfrecidkey, vConfRecordData);
		//bool bListLPushOK = false;
		//bListLPushOK = m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", Insertcrsrecinfo_confid_confrptid_confrecid_key);
		//sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_InsertConfRecord listLPush ** (%s) **bListLPushOK=%d\n", Insertcrsrecinfo_confid_confrptid_confrecid_key, bListLPushOK);

		if (pWriteDBThread)
		{
			typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
			CParam* pParam;
			pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pWriteDBThread);

			typedef void (CDevMgr::* ACTION)(void*);
			pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
				*this, &CDevMgr::Handle_WriteUserConfInfoToDB, (void*)pParam));
		}

#ifdef LINUX
		gettimeofday(&end, NULL); // ��s level
		time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

		sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_InsertConfRecord into db(4) ** (%s) **time_use** is: %lf us \n", crsconfrecordidset_confid_key, time_use);
#endif 

	}

	delete pParam;
	pParam = NULL;
}

void CDevMgr::InsertRecordFile(unsigned long long confid, unsigned long long confreportid, unsigned long long confrecordid, unsigned long long recordfileid, char* filestorsvrip, char* filerootpath, char* filepath, char* sdefilepath, time_t lltime, unsigned long long filesize)
{
	if (!m_pUpMsqThread)
		return;

	if (filestorsvrip == NULL
		|| filepath == NULL)
		return;

	typedef CBufferT<unsigned long long, unsigned long long, unsigned long long, unsigned long long, CAsyncThread*, time_t, unsigned long long, CAsyncThread*> CParam;
	CParam* pParam;
	pParam = new CParam(filestorsvrip, strlen(filestorsvrip), filerootpath, strlen(filerootpath), filepath, strlen(filepath), sdefilepath, strlen(sdefilepath), NULL, 0, NULL, 0, NULL, 0, NULL, 0, confid, confreportid, confrecordid, recordfileid, m_pUpMsqThread, lltime, filesize, m_pWriteDBThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_InsertRecordFile, (void*)pParam));
	return;
}

void CDevMgr::Handle_InsertRecordFile(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, unsigned long long, unsigned long long, unsigned long long, CAsyncThread*, time_t, unsigned long long, CAsyncThread*> CParam;
	CParam* pParam = (CParam*)pArg;

	char filestorsvrip[128];
	memset(filestorsvrip, 0, sizeof(filestorsvrip));
	char filerootpath[300];
	memset(filerootpath, 0, sizeof(filerootpath));
	char filepath[300];
	memset(filepath, 0, sizeof(filepath));
	char sdefilepath[300];
	memset(sdefilepath, 0, sizeof(sdefilepath));

	sr_printf(SR_LOGLEVEL_NORMAL, "==waring-->> Handle_InsertRecordFile filestorsvrip len=%d, filerootpath len=%d, filestorpath len=%d, sdefilepath len=%d.\n", pParam->m_nLen1, pParam->m_nLen2, pParam->m_nLen3, pParam->m_nLen4);

	//if (pParam->m_nLen1 > 0)
	//{
	//	memcpy(filestorsvrip, pParam->m_pData1, pParam->m_nLen1);
	//}

	//if (pParam->m_nLen2 > 0)
	//{
	//	memcpy(filepath, pParam->m_pData2, pParam->m_nLen2);
	//}

	//if (pParam->m_nLen3 > 0)
	//{
	//	memcpy(sdefilepath, pParam->m_pData3, pParam->m_nLen3);
	//}

	//char* filestorsvrip = (char*)pParam->m_pData1;
	//char* filepath = (char*)pParam->m_pData2;
	//char* sdefilepath = (char*)pParam->m_pData3; // �п���Ϊ��

	unsigned long long confid = pParam->m_Arg1;
	unsigned long long confreportid = pParam->m_Arg2; // �п���Ϊ0,Ϊ0ʱ��Ҫ��redis�洢�������л�ȡ,�ٻ�ȡ������mysql��ȡ
	unsigned long long confrecordid = pParam->m_Arg3; // �п���Ϊ0,Ϊ0ʱ��Ҫ��redis�洢�������л�ȡ
	unsigned long long recordfileid = pParam->m_Arg4;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg5);
	time_t add_time = (time_t)(pParam->m_Arg6);
	unsigned long long filesize = pParam->m_Arg7;
	CAsyncThread* pWriteDBThread = (CAsyncThread*)(pParam->m_Arg8);

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_InsertRecordFile pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CRecordFile* pRecordFile = NULL;
	std::map<CAsyncThread*, CRecordFile*>::iterator iter_mapAsyncThreadRecordFile = m_mapAsyncThreadRecordFile.find(pUpMsgThread);
	if (iter_mapAsyncThreadRecordFile != m_mapAsyncThreadRecordFile.end())
	{
		pRecordFile = iter_mapAsyncThreadRecordFile->second;
	}
	CConfRecord* pConfRecord = NULL;
	std::map<CAsyncThread*, CConfRecord*>::iterator iter_mapAsyncThreadConfRecord = m_mapAsyncThreadConfRecord.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfRecord != m_mapAsyncThreadConfRecord.end())
	{
		pConfRecord = iter_mapAsyncThreadConfRecord->second;
	}
	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}

	if (pRecordFile == NULL
		|| pConfRecord == NULL
		|| pConfReport == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_InsertRecordFile pRecordFile is NULL.\n");

		return;
	}

	{
#ifdef LINUX
		float time_use = 0;
		struct timeval start;
		struct timeval end;
		gettimeofday(&start, NULL); // ��s level
#endif 

		char cConfidFieldName[128] = { 0 };
		char conf_id_value[128] = { 0 };
		char cRecordFileidFieldName[128] = { 0 };
		char record_file_id_value[128] = { 0 };
		char cConfRecordidFieldName[128] = { 0 };
		char conf_record_id_value[128] = { 0 };
		char cCRSRecFilepathFieldName[128] = { 0 };
		char crsrec_filepath_value[300] = { 0 };
		char cCRSRecSdepathFieldName[128] = { 0 };
		char crsrec_sdepath_value[300] = { 0 };
		char cCRSRecFileStorSvrIPFieldName[128] = { 0 };
		char crsrec_filestorsvrip_value[128] = { 0 };
		char cCRSRecFilerootpathFieldName[128] = { 0 };
		char crsrec_filerootpath_value[300] = { 0 };
		char cRecordFileSizeFieldName[128] = { 0 };
		char record_file_size_value[128] = { 0 };

		sprintf(cConfidFieldName, "%s", "conf_id");
		sprintf(cRecordFileidFieldName, "%s", "record_file_id");
		sprintf(cConfRecordidFieldName, "%s", "conf_record_id");
		sprintf(cCRSRecFilepathFieldName, "%s", "filepath");
		sprintf(cCRSRecSdepathFieldName, "%s", "sdepath");
		sprintf(cCRSRecFileStorSvrIPFieldName, "%s", "serverip");
		sprintf(cCRSRecFilerootpathFieldName, "%s", "relativepath");
		sprintf(cRecordFileSizeFieldName, "%s", "filesize");

		//sprintf(crsrec_filepath_value, "%s", filepath);
		//sprintf(crsrec_sdepath_value, "%s", sdefilepath);
		//sprintf(crsrec_filestorsvrip_value, "%s", filestorsvrip);

		if (pParam->m_nLen1 > 0)
		{
			memcpy(crsrec_filestorsvrip_value, pParam->m_pData1, pParam->m_nLen1);
		}

		if (pParam->m_nLen2 > 0)
		{
			memcpy(crsrec_filerootpath_value, pParam->m_pData2, pParam->m_nLen2);
		}

		if (pParam->m_nLen3 > 0)
		{
			memcpy(crsrec_filepath_value, pParam->m_pData3, pParam->m_nLen3);
		}

		if (pParam->m_nLen4 > 0)
		{
			memcpy(crsrec_sdepath_value, pParam->m_pData4, pParam->m_nLen4);
		}

#ifdef WIN32
		sprintf(conf_id_value, "%I64d", confid);
		sprintf(record_file_size_value, "%I64d", filesize);
#elif defined LINUX
		sprintf(conf_id_value, "%lld", confid);
		sprintf(record_file_size_value, "%lld", filesize);
#endif

		m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);

		if (confreportid == 0)// 
		{
			bool bFindConfReportID = false;

			char confstarttimerptid_confid_key[128] = { 0 };
			sprintf(confstarttimerptid_confid_key, "confstimerptidset_%s", conf_id_value);
			RedisReplyArray vRedisReplyArray;
			vRedisReplyArray.clear();

			bool bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll(confstarttimerptid_confid_key, vRedisReplyArray);

			unsigned int uiConfstrptinfosize = vRedisReplyArray.size();

			sr_printf(SR_LOGLEVEL_DEBUG, " m_pRedisConnList[Handle_InsertRecordFile]->hashHGetAll==%s-->> %d, size = %d\n", confstarttimerptid_confid_key, bhashHGetAll_ok, uiConfstrptinfosize);

			if (bhashHGetAll_ok
				&& (uiConfstrptinfosize > 0)
				&& (uiConfstrptinfosize % 2 == 0))
			{
				for (unsigned int ii = 0; ii < uiConfstrptinfosize;)
				{
					sr_printf(SR_LOGLEVEL_DEBUG, "==Handle_InsertRecordFile==In redis db(4) key:%s ==>>> find starttime=%s, rptid=%s\n", confstarttimerptid_confid_key, vRedisReplyArray[ii].str.c_str(), vRedisReplyArray[ii + 1].str.c_str());
					ii += 2;
				}

				// �ҵ�����ٿ������confreportid

#ifdef WIN32
				sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%I64d", &confreportid);
#elif defined LINUX
				sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%lld", &confreportid);
#endif

				bFindConfReportID = true;
			}

			if (bFindConfReportID == false) // ���ڴ���δ�ҵ��������ݿ�
			{
				pConfReport->SetConfID(confid);
				if (pConfReport->SelectDB())// 
				{
					confreportid = pConfReport->GetConfReportID();

#ifdef WIN32
					sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_InsertRecordFile == find confreportid from ConfReport table=== confid=%I64d, confreportid=%I64d\n", confid, confreportid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_InsertRecordFile == find confreportid from ConfReport table=== confid=%lld, confreportid=%lld\n", confid, confreportid);
#endif
				}
				else
				{
#ifdef WIN32
					sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_InsertRecordFile == Not find confreportid from ConfReport table=== confid=%I64d\n", confid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_InsertRecordFile == Not find confreportid from ConfReport table=== confid=%lld\n", confid);
#endif
				}

			}

			if (confreportid == 0)
			{
#ifdef WIN32
				sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_InsertConfRecord == Not find confreportid === confid=%I64d\n", confid);
#elif defined LINUX
				sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_InsertConfRecord == Not find confreportid === confid=%lld\n", confid);
#endif

				delete pParam;
				pParam = NULL;

				return;
			}
		}
		
		if (confrecordid == 0)// 
		{
			bool bFindConfRecordID = false;

			char crsconfrecordidset_confid_key[128] = { 0 };
			sprintf(crsconfrecordidset_confid_key, "crsconfrecordidset_%s", conf_id_value);
			RedisReplyArray vRedisReplyArray;
			vRedisReplyArray.clear();

			bool bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll(crsconfrecordidset_confid_key, vRedisReplyArray);

			unsigned int uiConfRecordidsize = vRedisReplyArray.size();

			sr_printf(SR_LOGLEVEL_DEBUG, " m_pRedisConnList[Handle_InsertRecordFile]->hashHGetAll==%s-->> %d, size = %d\n", crsconfrecordidset_confid_key, bhashHGetAll_ok, uiConfRecordidsize);

			if (bhashHGetAll_ok
				&& (uiConfRecordidsize > 0)
				&& (uiConfRecordidsize % 2 == 0))
			{
				for (unsigned int ii = 0; ii < uiConfRecordidsize;)
				{
					sr_printf(SR_LOGLEVEL_DEBUG, "==Handle_InsertRecordFile==In redis db(4) key:%s ==>>> find starttime=%s, rptid=%s\n", crsconfrecordidset_confid_key, vRedisReplyArray[ii].str.c_str(), vRedisReplyArray[ii + 1].str.c_str());
					ii += 2;
				}

				// �ҵ�����ٿ������confreportid

#ifdef WIN32
				sscanf(vRedisReplyArray[uiConfRecordidsize - 1].str.c_str(), "%I64d", &confrecordid);
#elif defined LINUX
				sscanf(vRedisReplyArray[uiConfRecordidsize - 1].str.c_str(), "%lld", &confrecordid);
#endif

				bFindConfRecordID = true;
			}

			if (bFindConfRecordID == false) // ���ڴ���δ�ҵ��������ݿ�
			{
				pConfRecord->SetConfReportID(confreportid);
				if (pConfRecord->SelectConfRecordidFromDB())// 
				{
					confrecordid = pConfRecord->GetConfRecordID();

#ifdef WIN32
					sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_InsertRecordFile == find confrecordid from pConfRecord table=== confid=%I64d, confreportid=%I64d, confrecordid=%I64d\n", confid, confreportid, confrecordid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_InsertRecordFile == find confrecordid from pConfRecord table=== confid=%lld, confreportid=%lld, confrecordid=%lld\n", confid, confreportid, confrecordid);
#endif
				}
				else
				{
#ifdef WIN32
					sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_InsertRecordFile == Not find confrecordid from pConfRecord table=== confid=%I64d, confreportid=%I64d\n", confid, confreportid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_InsertRecordFile == Not find confrecordid from pConfRecord table=== confid=%lld, confreportid=%lld\n", confid, confreportid);
#endif
				}

			}

			if (confrecordid == 0)
			{
#ifdef WIN32
				sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_InsertRecordFile == Not find confrecordid === confid=%I64d\n", confid);
#elif defined LINUX
				sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_InsertRecordFile == Not find confrecordid === confid=%lld\n", confid);
#endif

				delete pParam;
				pParam = NULL;

				return;
			}
		}


		//char Insertcrsrecfile_confid_recfileid_key[128] = { 0 };
		char Insertcrsrecfile_confid_confrecid_recfileid_key[128] = { 0 };
#ifdef WIN32
		sprintf(conf_record_id_value, "%I64d", confrecordid);
		sprintf(record_file_id_value, "%I64d", recordfileid);
		//sprintf(Insertcrsrecfile_confid_recfileid_key, "inscrsrecfile_%I64d_%I64d", confid, recordfileid);
		sprintf(Insertcrsrecfile_confid_confrecid_recfileid_key, "inscrsrecfile_%I64d_%I64d_%I64d", confid, confrecordid, recordfileid);
#elif defined LINUX
		sprintf(conf_record_id_value, "%lld", confrecordid);
		sprintf(record_file_id_value, "%lld", recordfileid);
		//sprintf(Insertcrsrecfile_confid_recfileid_key, "inscrsrecfile_%lld_%lld", confid, recordfileid);
		sprintf(Insertcrsrecfile_confid_confrecid_recfileid_key, "inscrsrecfile_%lld_%lld_%lld", confid, confrecordid, recordfileid);
#endif

		//REDISKEY strInsertCRSRecfileConfidRecfileidkey = Insertcrsrecfile_confid_recfileid_key;
		REDISKEY strInsertCRSRecfileConfidConfrecidRecfileidkey = Insertcrsrecfile_confid_confrecid_recfileid_key;
		REDISVDATA vRecordFileData;
		vRecordFileData.clear();

		REDISKEY strConfidFieldName = cConfidFieldName;
		REDISVALUE strconfidvalue = conf_id_value;
		vRecordFileData.push_back(strConfidFieldName);
		vRecordFileData.push_back(strconfidvalue);

		REDISKEY strRecordFileidName = cRecordFileidFieldName;
		REDISVALUE strrecordfileidvalue = record_file_id_value;
		vRecordFileData.push_back(strRecordFileidName);
		vRecordFileData.push_back(strrecordfileidvalue);

		REDISKEY strConfRecordidName = cConfRecordidFieldName;
		REDISVALUE strconfrecordidvalue = conf_record_id_value;
		vRecordFileData.push_back(strConfRecordidName);
		vRecordFileData.push_back(strconfrecordidvalue);
		
		REDISKEY strCRSRecFilepathFieldName = cCRSRecFilepathFieldName;
		REDISVALUE strcrsrecfilepathvalue = crsrec_filepath_value;
		vRecordFileData.push_back(strCRSRecFilepathFieldName);
		vRecordFileData.push_back(strcrsrecfilepathvalue);

		REDISKEY strCRSRecSdepathFieldName = cCRSRecSdepathFieldName;
		REDISVALUE strcrsrecsdepathvalue = crsrec_sdepath_value;
		vRecordFileData.push_back(strCRSRecSdepathFieldName);
		vRecordFileData.push_back(strcrsrecsdepathvalue);

		REDISKEY strCRSRecFileStorSvrIPFieldName = cCRSRecFileStorSvrIPFieldName;
		REDISVALUE strcrsrecfilestorsvripvalue = crsrec_filestorsvrip_value;
		vRecordFileData.push_back(strCRSRecFileStorSvrIPFieldName);
		vRecordFileData.push_back(strcrsrecfilestorsvripvalue);

		REDISKEY strCRSRecFilerootpathFieldName = cCRSRecFilerootpathFieldName;
		REDISVALUE strcrsrecfilerootpathvalue = crsrec_filerootpath_value;
		vRecordFileData.push_back(strCRSRecFilerootpathFieldName);
		vRecordFileData.push_back(strcrsrecfilerootpathvalue);

		REDISKEY strRecordFileSizeFieldName = cRecordFileSizeFieldName;
		REDISVALUE strrecord_file_size_value = record_file_size_value;
		vRecordFileData.push_back(strRecordFileSizeFieldName);
		vRecordFileData.push_back(strrecord_file_size_value);

		//bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strInsertCRSRecfileConfidRecfileidkey, vRecordFileData);

		bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strInsertCRSRecfileConfidConfrecidRecfileidkey, vRecordFileData);

		bool bListLPushOK = false;
		bListLPushOK = m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", Insertcrsrecfile_confid_confrecid_recfileid_key);
		sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_InsertRecordFile listLPush ** (%s) **bListLPushOK=%d\n", Insertcrsrecfile_confid_confrecid_recfileid_key, bListLPushOK);
		
		if (pWriteDBThread)
		{
			typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
			CParam* pParam;
			pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pWriteDBThread);

			typedef void (CDevMgr::* ACTION)(void*);
			pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
				*this, &CDevMgr::Handle_WriteUserConfInfoToDB, (void*)pParam));
		}

#ifdef LINUX
		gettimeofday(&end, NULL); // ��s level
		time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

		sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_InsertRecordFile into db(4) ** (%s) **time_use** is: %lf us \n", Insertcrsrecfile_confid_confrecid_recfileid_key, time_use);
#endif 

	}
}

/*
void CDevMgr::UpdateRecordFile(unsigned long long confid, unsigned long long confrecordid, char* time, time_t lltime)
{
	if (!m_pUpMsqThread)
		return;

	if (time == NULL)
		return;

	typedef CBufferT<unsigned long long, unsigned long long, CAsyncThread*, time_t, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, confid, confid, m_pUpMsqThread, lltime);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_UpdateRecordFile, (void*)pParam));
	return;
}

void CDevMgr::Handle_UpdateRecordFile(void* pArg)
{
}
*/

void CDevMgr::UpdateConfRecord(unsigned long long confid, unsigned long long confreportid, char* time, time_t lltime, char* confname)
{
	if (!m_pUpMsqThread)
		return;

	if (time == NULL)
		return;

	typedef CBufferT<unsigned long long, CAsyncThread*, time_t, unsigned long long, CAsyncThread*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), confname, strlen(confname), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, confid, m_pUpMsqThread, lltime, confreportid, m_pWriteDBThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_UpdateConfRecord, (void*)pParam));
	return;
}

void CDevMgr::Handle_UpdateConfRecord(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, CAsyncThread*, time_t, unsigned long long, CAsyncThread*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	char* ptime = (char*)pParam->m_pData1;

	std::string strconfname;
	strconfname.clear();
	if (pParam->m_nLen2 > 0)
	{
		strconfname.assign(pParam->m_pData2);
	}

	unsigned long long confid = pParam->m_Arg1;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg2);
	time_t add_time = (time_t)(pParam->m_Arg3);

	unsigned long long confreportid = 0;
	confreportid = pParam->m_Arg4;

	CAsyncThread* pWriteDBThread = (CAsyncThread*)(pParam->m_Arg5);

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateConfRecord pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CConfRecord* pConfRecord = NULL;
	std::map<CAsyncThread*, CConfRecord*>::iterator iter_mapAsyncThreadConfRecord = m_mapAsyncThreadConfRecord.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfRecord != m_mapAsyncThreadConfRecord.end())
	{
		pConfRecord = iter_mapAsyncThreadConfRecord->second;
	}
	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}	
	if (pConfRecord == NULL
		|| pConfReport == NULL)
	{
		delete pParam;
		pParam = NULL;
		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateConfRecord pConfRecord is NULL.\n");
		return;
	}

	{
#ifdef LINUX
		float time_use = 0;
		struct timeval start;
		struct timeval end;
		gettimeofday(&start, NULL); // ��s level
#endif 
		m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);
		char cConfidFieldName[128] = { 0 };
		char conf_id_value[128] = { 0 };
		char cConfRecordidFieldName[128] = { 0 };
		char conf_recordid_value[128] = { 0 };
		char cConfReportidFieldName[128] = { 0 };
		char conf_report_id_value[128] = { 0 };
		char cRecStarttimeFieldName[128] = { 0 };
		char rec_starttime_value[128] = { 0 };
		char cRecEndtimeFieldName[128] = { 0 };

		sprintf(cConfidFieldName, "%s", "conf_id");
		sprintf(cConfRecordidFieldName, "%s", "conf_record_id");
		sprintf(cConfReportidFieldName, "%s", "conf_report_id");
		sprintf(cRecStarttimeFieldName, "%s", "starttime");
		sprintf(cRecEndtimeFieldName, "%s", "endtime");

		char cConfnameFieldName[256] = { 0 };
		char conf_name_value[256] = { 0 };
		sprintf(cConfnameFieldName, "%s", "confname");
		sprintf(conf_name_value, "%s", strconfname.c_str());

#ifdef WIN32
		sprintf(conf_id_value, "%I64d", confid);
#elif defined LINUX
		sprintf(conf_id_value, "%lld", confid);
#endif
		//unsigned long long confreportid = 0;

		if (confreportid == 0)// 
		{
			bool bFindConfReportID = false;

			char confstarttimerptid_confid_key[128] = { 0 };
			sprintf(confstarttimerptid_confid_key, "confstimerptidset_%s", conf_id_value);
			RedisReplyArray vRedisReplyArray;
			vRedisReplyArray.clear();

			bool bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll(confstarttimerptid_confid_key, vRedisReplyArray);

			unsigned int uiConfstrptinfosize = vRedisReplyArray.size();

			sr_printf(SR_LOGLEVEL_DEBUG, " m_pRedisConnList[Handle_UpdateConfRecord]->hashHGetAll==%s-->> %d, size = %d\n", confstarttimerptid_confid_key, bhashHGetAll_ok, uiConfstrptinfosize);

			if (bhashHGetAll_ok
				&& (uiConfstrptinfosize > 0)
				&& (uiConfstrptinfosize % 2 == 0))
			{
				for (unsigned int ii = 0; ii < uiConfstrptinfosize;)
				{
					sr_printf(SR_LOGLEVEL_DEBUG, "==Handle_UpdateConfRecord==In redis db(4) key:%s ==>>> find starttime=%s, rptid=%s\n", confstarttimerptid_confid_key, vRedisReplyArray[ii].str.c_str(), vRedisReplyArray[ii + 1].str.c_str());
					ii += 2;
				}

				// �ҵ�����ٿ������confreportid

#ifdef WIN32
				sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%I64d", &confreportid);
#elif defined LINUX
				sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%lld", &confreportid);
#endif

				bFindConfReportID = true;
			}

			if (bFindConfReportID == false) // ���ڴ���δ�ҵ��������ݿ�
			{
				pConfReport->SetConfID(confid);
				if (pConfReport->SelectDB())// 
				{
					confreportid = pConfReport->GetConfReportID();

#ifdef WIN32
					sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_UpdateConfRecord == find confreportid from ConfReport table=== confid=%I64d, confreportid=%I64d\n", confid, confreportid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_UpdateConfRecord == find confreportid from ConfReport table=== confid=%lld, confreportid=%lld\n", confid, confreportid);
#endif
				}
				else
				{
#ifdef WIN32
					sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_UpdateConfRecord == Not find confreportid from ConfReport table=== confid=%I64d\n", confid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_UpdateConfRecord == Not find confreportid from ConfReport table=== confid=%lld\n", confid);
#endif
				}

			}

			if (confreportid == 0)
			{
#ifdef WIN32
				sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_UpdateConfRecord == Not find confreportid === confid=%I64d\n", confid);
#elif defined LINUX
				sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_UpdateConfRecord == Not find confreportid === confid=%lld\n", confid);
#endif

				delete pParam;
				pParam = NULL;

				return;
			}
		}


#ifdef WIN32
		sprintf(conf_report_id_value, "%I64d", confreportid);
#elif defined LINUX
		sprintf(conf_report_id_value, "%lld", confreportid);
#endif

		char up_confrecordid_confid_recordid_key[128] = { 0 };

		char crsconfrecordidset_confid_key[128] = { 0 };
		sprintf(crsconfrecordidset_confid_key, "crsconfrecordidset_%s", conf_id_value);
		RedisReplyArray vRedisReplyRecordidArray;
		vRedisReplyRecordidArray.clear();

		bool bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll(crsconfrecordidset_confid_key, vRedisReplyRecordidArray);

		unsigned int uiConfrecidinfosize = vRedisReplyRecordidArray.size();

		sr_printf(SR_LOGLEVEL_DEBUG, " m_pRedisConnList[Handle_UpdateConfRecord]->hashHGetAll==%s-->> %d, size = %d\n", crsconfrecordidset_confid_key, bhashHGetAll_ok, uiConfrecidinfosize);

		if (bhashHGetAll_ok
			&& (uiConfrecidinfosize > 0)
			&& (uiConfrecidinfosize % 2 == 0))
		{
			bool bFindConfRecordID = false;

			for (unsigned int ii = 0; ii < uiConfrecidinfosize;)
			{
				sr_printf(SR_LOGLEVEL_DEBUG, "==Handle_UpdateConfRecord==In redis db(4) key:%s ==>>> find starttime=%s, recordid=%s\n", crsconfrecordidset_confid_key, vRedisReplyRecordidArray[ii].str.c_str(), vRedisReplyRecordidArray[ii + 1].str.c_str());
				ii += 2;
			}

			sprintf(rec_starttime_value, "%s", vRedisReplyRecordidArray[uiConfrecidinfosize - 2].str.c_str());
			sprintf(conf_recordid_value, "%s", vRedisReplyRecordidArray[uiConfrecidinfosize - 1].str.c_str());

			// ֹͣ¼�ƣ�����¼�ƽ���ʱ��
			sprintf(up_confrecordid_confid_recordid_key, "upconfrecordid_%s_%s", conf_id_value, conf_recordid_value);
			REDISKEY strupconfrecordidkey = up_confrecordid_confid_recordid_key;
			REDISVDATA vConfrecorddata;
			vConfrecorddata.clear();

			REDISKEY strConfidFieldName = cConfidFieldName;
			REDISVALUE strconfidvalue = conf_id_value;
			vConfrecorddata.push_back(strConfidFieldName);
			vConfrecorddata.push_back(strconfidvalue);

			REDISKEY strConfRecordidName = cConfRecordidFieldName;
			REDISVALUE strconfrecordidvalue = conf_recordid_value;
			vConfrecorddata.push_back(strConfRecordidName);
			vConfrecorddata.push_back(strconfrecordidvalue);

			REDISKEY strConfReportidFieldName = cConfReportidFieldName;
			REDISVALUE strconfreportidvalue = conf_report_id_value;
			vConfrecorddata.push_back(strConfReportidFieldName);
			vConfrecorddata.push_back(strconfreportidvalue);

			REDISKEY strRecStarttimeFieldName = cRecStarttimeFieldName;
			REDISVALUE strrecstarttimevalue = rec_starttime_value;
			vConfrecorddata.push_back(strRecStarttimeFieldName);
			vConfrecorddata.push_back(strrecstarttimevalue);

			REDISKEY strRecEndtimeFieldName = cRecEndtimeFieldName;
			REDISVALUE strrecendtimevalue = ptime;
			vConfrecorddata.push_back(strRecEndtimeFieldName);
			vConfrecorddata.push_back(strrecendtimevalue);

			REDISKEY strConfnameFieldName = cConfnameFieldName;
			REDISVALUE strconfnamevalue = conf_name_value;
			vConfrecorddata.push_back(strConfnameFieldName);
			vConfrecorddata.push_back(strconfnamevalue);

			bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strupconfrecordidkey, vConfrecorddata);

			bool bListLPushOK = false;
			bListLPushOK = m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", up_confrecordid_confid_recordid_key);
			sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateConfRecord listLPush ** (%s) **bListLPushOK=%d\n", up_confrecordid_confid_recordid_key, bListLPushOK);

			if (pWriteDBThread)
			{
				typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
				CParam* pParam;
				pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pWriteDBThread);

				typedef void (CDevMgr::* ACTION)(void*);
				pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
					*this, &CDevMgr::Handle_WriteUserConfInfoToDB, (void*)pParam));
			}

		}

#ifdef LINUX
		gettimeofday(&end, NULL); // ��s level
		time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

		sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateConfRecord into db(4) ** (%s) **time_use** is: %lf us \n", up_confrecordid_confid_recordid_key, time_use);
#endif 

	}

	delete pParam;
	pParam = NULL;
}


void CDevMgr::InsertConfLiveInfo(unsigned long long liveinfoid, const SRMsgs::IndCRSStartLive* ind, char* time, time_t lltime)
{
	if (!m_pUpMsqThread)
		return;

	// LiveSetting��Ϣ������mc��ind��Ϣ������ֱ�Ӷ�ȡredis������(���devmgr��redis�Ͽ����߻��/������֪ͨmc�������õ�����redisʧ��)
	const SRMsgs::IndCRSStartLive_LiveSetting& liveset = ind->livesetinfo();
	char strlivechairman[255] = { 0 };
	char strlivesubject[255] = { 0 };
	char strliveabstract[255] = { 0 };
	char strlivepwd[40] = { 0 };
	unsigned int uiispublic = 0;
	unsigned int uiisuserec = 1;
	unsigned long long confreportid = 0;

	//char strlivepushurl[1024] = { 0 };
	char strlivepullurl[1024] = { 0 };
	char strliveplayurl[255] = { 0 };

	sprintf(strlivechairman, "%s", liveset.chairman().c_str());
	sprintf(strlivesubject, "%s", liveset.subject().c_str());
	sprintf(strliveabstract, "%s", liveset.abstract().c_str());
	sprintf(strlivepwd, "%s", liveset.livepwd().c_str());
	uiispublic = liveset.ispublic();
	uiisuserec = liveset.isuserec();

	//sprintf(strlivepushurl, "%s", ind->livepushurl().c_str());
	sprintf(strlivepullurl, "%s", ind->livepullurl().c_str());
	sprintf(strliveplayurl, "%s", ind->liveplayurl().c_str());

	confreportid = ind->confreportid();
	unsigned int uilivesvrtype = 0;
	uilivesvrtype = ind->livesvrtype();
	char strliveaddrs[2048] = { 0 };
	int strliveaddrslen = 0;
	char *sep1 = { "MySRLv1" };
	char *sep2 = { "MySRLv2" };
	//int seplen1 = strlen(sep1);
	//int seplen2 = strlen(sep2);

	for (int i = 0; i < ind->liveaddrs_size(); i++)
	{
		const SRMsgs::IndCRSStartLive_LiveAddr& liveaddr = ind->liveaddrs(i);
		if (liveaddr.type() != 0)
		{
			strliveaddrslen += sprintf(strliveaddrs + strliveaddrslen, "%d%s%s%s", liveaddr.type(), sep2, liveaddr.url().c_str(), sep1);

			//strliveaddrslen += sprintf(strliveaddrs + strliveaddrslen, "%s", sep1);
		}
	}

	typedef CBufferT<unsigned long long, unsigned long long, unsigned int, CAsyncThread*, time_t, unsigned int, unsigned long long, unsigned int> CParam;
	CParam* pParam;
	pParam = new CParam(strlivechairman, strlen(strlivechairman), strlivesubject, strlen(strlivesubject), strliveabstract, strlen(strliveabstract), strlivepwd, strlen(strlivepwd), time, strlen(time), strlivepullurl, strlen(strlivepullurl), strliveplayurl, strlen(strliveplayurl), strliveaddrs, strlen(strliveaddrs)
		, ind->confid(), liveinfoid, uiispublic, m_pUpMsqThread, lltime, uiisuserec, confreportid, uilivesvrtype);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_InsertConfLiveInfo, (void*)pParam));

	return;
}

void CDevMgr::Handle_InsertConfLiveInfo(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, unsigned long long, unsigned int, CAsyncThread*, time_t, unsigned int, unsigned long long, unsigned int> CParam;
	CParam* pParam = (CParam*)pArg;

	//sr_printf(SR_LOGLEVEL_NORMAL, "==waring-->> Handle_InsertConfLiveInfo filestorsvrip len=%d, filerootpath len=%d, filestorpath len=%d, sdefilepath len=%d.\n", pParam->m_nLen1, pParam->m_nLen2, pParam->m_nLen3, pParam->m_nLen4);

	unsigned long long confreportid = 0;

	unsigned long long confid = pParam->m_Arg1;
	unsigned long long liveinfoid = pParam->m_Arg2;
	unsigned int uiispublic = pParam->m_Arg3;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg4);
	time_t add_time = (time_t)(pParam->m_Arg5);
	unsigned int uiisuserec = pParam->m_Arg6;
	confreportid = pParam->m_Arg7;
	unsigned int uilivesvrtype = pParam->m_Arg8;
	
	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_InsertConfLiveInfo pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}

	CConfLiveInfo* pConfLiveInfo = NULL;
	std::map<CAsyncThread*, CConfLiveInfo*>::iterator iter_mapAsyncThreadConfLiveInfo = m_mapAsyncThreadConfLiveInfo.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfLiveInfo != m_mapAsyncThreadConfLiveInfo.end())
	{
		pConfLiveInfo = iter_mapAsyncThreadConfLiveInfo->second;
	}

	if (pConfReport == NULL
		|| pConfLiveInfo == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_InsertLiveInfo pLiveInfo is NULL.\n");

		return;
	}

	{
#ifdef LINUX
		float time_use = 0;
		struct timeval start;
		struct timeval end;
		gettimeofday(&start, NULL); // ��s level
#endif 

		char cLiveinfoidFieldName[128] = { 0 };
		char liveinfo_id_value[128] = { 0 };
		char cConfidFieldName[128] = { 0 };
		char conf_id_value[128] = { 0 };
		char cConfReportidFieldName[128] = { 0 };
		char conf_report_id_value[128] = { 0 };
		
		char cLiveSubjectFieldName[128] = { 0 };
		char live_subject_value[255] = { 0 };

		char cLiveStarttimeFieldName[128] = { 0 };
		char live_starttime_value[128] = { 0 };
		char cLiveEndtimeFieldName[128] = { 0 };
		char live_endtime_value[128] = { 0 };

		char cLiveChairmanFieldName[128] = { 0 };
		char live_chairman_value[255] = { 0 };
		char cLiveAbstractFieldName[128] = { 0 };
		char live_abstract_value[255] = { 0 };
		char cLiveisPublicFieldName[128] = { 0 };
		char live_ispublic_value[128] = { 0 };
		char cLivePWDFieldName[128] = { 0 };
		char live_pwd_value[128] = { 0 };
		char cLiveisUseRecFieldName[128] = { 0 };
		char live_isuserec_value[128] = { 0 };

		char cLiveAddrFieldName[128] = { 0 };
		char live_addr_value[1024] = { 0 };// ��ӦIndCRSStartLive��Ϣ��livepullurl�ֶ�,��livesvrtype����0ʱSRS��������ַ
		char cLiveWatchtimesFieldName[128] = { 0 };
		char live_watch_times_value[128] = { 0 };
		char cLiveLikestimesFieldName[128] = { 0 };
		char live_likes_times_value[128] = { 0 };
		char cLiveWatchaddrFieldName[128] = { 0 };
		char live_watch_addr_value[255] = { 0 };
		char cLiveAndroidtimesFieldName[128] = { 0 };
		char live_android_times_value[128] = { 0 };
		char cLiveIOStimesFieldName[128] = { 0 };
		char live_ios_times_value[128] = { 0 };
		char cLivePCtimesFieldName[128] = { 0 };
		char live_pc_times_value[128] = { 0 };
		char cLiveMobiletimesFieldName[128] = { 0 };
		char live_mobile_times_value[128] = { 0 };

		char cLiveConvertStatusFieldName[128] = { 0 };
		char live_convert_status_value[128] = { 0 };
		char cLiveTaskIdFieldName[128] = { 0 };
		char live_task_id_value[128] = { 0 };

		char cLiveSvrtypeFieldName[128] = { 0 };
		char live_svrtype_value[32] = { 0 };
		char cRTMPUrlFieldName[128] = { 0 };
		char rtmp_url_value[1024] = { 0 };
		char cFLVUrlFieldName[128] = { 0 };
		char flv_url_value[1024] = { 0 };
		char cHLSUrlFieldName[128] = { 0 };
		char hls_url_value[1024] = { 0 };
		
		sprintf(cLiveinfoidFieldName, "%s", "li_id");
		sprintf(cConfidFieldName, "%s", "conf_id");
		sprintf(cConfReportidFieldName, "%s", "conf_report_id");

		sprintf(cLiveSubjectFieldName, "%s", "li_subject");

		sprintf(cLiveStarttimeFieldName, "%s", "starttime");
		sprintf(cLiveEndtimeFieldName, "%s", "endtime");

		sprintf(cLiveChairmanFieldName, "%s", "chairman");
		sprintf(cLiveAbstractFieldName, "%s", "li_abstract");
		sprintf(cLiveisPublicFieldName, "%s", "is_public");
		sprintf(cLivePWDFieldName, "%s", "live_pwd");
		sprintf(cLiveisUseRecFieldName, "%s", "is_userec");

		sprintf(cLiveAddrFieldName, "%s", "live_addr");
		sprintf(cLiveWatchtimesFieldName, "%s", "watch_times");
		sprintf(cLiveLikestimesFieldName, "%s", "likes_times");
		sprintf(cLiveWatchaddrFieldName, "%s", "watch_addr");
		sprintf(cLiveAndroidtimesFieldName, "%s", "android");
		sprintf(cLiveIOStimesFieldName, "%s", "ios_times");
		sprintf(cLivePCtimesFieldName, "%s", "pc_times");
		sprintf(cLiveMobiletimesFieldName, "%s", "mobile_times");

		sprintf(cLiveConvertStatusFieldName, "%s", "convert_status");
		sprintf(cLiveTaskIdFieldName, "%s", "task_id");

		sprintf(cLiveSvrtypeFieldName, "%s", "live_svrtype");
		sprintf(cRTMPUrlFieldName, "%s", "rtmp_url");
		sprintf(cFLVUrlFieldName, "%s", "flv_url");
		sprintf(cHLSUrlFieldName, "%s", "hls_url");
		

		char confliveinfoidset_confid_key[128] = { 0 };
		char liveinfo_confid_liveinfoid_key[128] = { 0 };
#ifdef WIN32
		sprintf(liveinfo_id_value, "%I64d", liveinfoid);
		sprintf(conf_id_value, "%I64d", confid);
		sprintf(confliveinfoidset_confid_key, "confliveinfoidset_%I64d", confid);
		sprintf(liveinfo_confid_liveinfoid_key, "liveinfo_%I64d_%I64d", confid, liveinfoid);
#elif defined LINUX
		sprintf(liveinfo_id_value, "%lld", liveinfoid);
		sprintf(conf_id_value, "%lld", confid);
		sprintf(confliveinfoidset_confid_key, "confliveinfoidset_%lld", confid);
		sprintf(liveinfo_confid_liveinfoid_key, "liveinfo_%lld_%lld", confid, liveinfoid);
#endif

		if (pParam->m_nLen1 > 0)
		{
			memcpy(live_chairman_value, pParam->m_pData1, pParam->m_nLen1);
		}
		if (pParam->m_nLen2 > 0)
		{
			memcpy(live_subject_value, pParam->m_pData2, pParam->m_nLen2);
		}
		if (pParam->m_nLen3 > 0)
		{
			memcpy(live_abstract_value, pParam->m_pData3, pParam->m_nLen3);
		}
		if (pParam->m_nLen4 > 0)
		{
			memcpy(live_pwd_value, pParam->m_pData4, pParam->m_nLen4);
		}
		if (pParam->m_nLen5 > 0)
		{
			memcpy(live_starttime_value, pParam->m_pData5, pParam->m_nLen5);
		}
		if (pParam->m_nLen6 > 0)
		{
			memcpy(live_addr_value, pParam->m_pData6, pParam->m_nLen6);
		}
		if (pParam->m_nLen7 > 0)
		{
			memcpy(live_watch_addr_value, pParam->m_pData7, pParam->m_nLen7);
		}

		char strliveaddrs[2048] = { 0 };
		if (pParam->m_nLen8 > 0)
		{
			memcpy(strliveaddrs, pParam->m_pData8, pParam->m_nLen8);
		}

		sprintf(live_endtime_value, "%s", "19700101000000");
		sprintf(live_ispublic_value, "%u", uiispublic);
		sprintf(live_isuserec_value, "%u", uiisuserec);

		sprintf(live_svrtype_value, "%u", uilivesvrtype);
		int index;
		std::map<std::string, std::string> liveaddrms;
		std::string strtmp;
		liveaddrms.clear();
		char *sep1 = { "MySRLv1" };
		char *sep2 = { "MySRLv2" };
		int seplen1 = strlen(sep1);
		int seplen2 = strlen(sep2);
		
		strtmp = strliveaddrs;
		printf("strliveaddrs->>%s\n", strtmp.c_str());
		//printf("seplen1,seplen2->>%d,%d\n", seplen1, seplen2);

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
				liveaddrms.insert(std::pair<std::string, std::string>(srcsubstr.substr(0, subindex), srcsubstr.substr(subindex + seplen2, srcsubstr.length())));
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
					liveaddrms.insert(std::pair<std::string, std::string>(lastsubstr.substr(0, lastsubindex), lastsubstr.substr(lastsubindex + seplen2, lastsubstr.length())));
				}
			}
		}

		for (std::map<std::string, std::string>::iterator ms_itor = liveaddrms.begin();
			ms_itor != liveaddrms.end(); ms_itor++)
		{
			printf("liveaddrms<key:%s, value:%s>\n", ms_itor->first.c_str(), ms_itor->second.c_str());
			if (atoi(ms_itor->first.c_str()) == 1)
			{
				sprintf(rtmp_url_value, "%s", ms_itor->second.c_str());
			}
			else if (atoi(ms_itor->first.c_str()) == 2)
			{
				sprintf(flv_url_value, "%s", ms_itor->second.c_str());
			}
			else if (atoi(ms_itor->first.c_str()) == 3)
			{
				sprintf(hls_url_value, "%s", ms_itor->second.c_str());
			}
		}

		m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);

		if (confreportid == 0)// 
		{
			bool bFindConfReportID = false;

			char confstarttimerptid_confid_key[128] = { 0 };
			sprintf(confstarttimerptid_confid_key, "confstimerptidset_%s", conf_id_value);
			RedisReplyArray vRedisReplyArray;
			vRedisReplyArray.clear();

			bool bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll(confstarttimerptid_confid_key, vRedisReplyArray);

			unsigned int uiConfstrptinfosize = vRedisReplyArray.size();

			sr_printf(SR_LOGLEVEL_DEBUG, " m_pRedisConnList[Handle_InsertConfLiveInfo]->hashHGetAll==%s-->> %d, size = %d\n", confstarttimerptid_confid_key, bhashHGetAll_ok, uiConfstrptinfosize);

			if (bhashHGetAll_ok
				&& (uiConfstrptinfosize > 0)
				&& (uiConfstrptinfosize % 2 == 0))
			{
				for (unsigned int ii = 0; ii < uiConfstrptinfosize;)
				{
					sr_printf(SR_LOGLEVEL_DEBUG, "==Handle_InsertConfLiveInfo==In redis db(4) key:%s ==>>> find starttime=%s, rptid=%s\n", confstarttimerptid_confid_key, vRedisReplyArray[ii].str.c_str(), vRedisReplyArray[ii + 1].str.c_str());
					ii += 2;
				}

				// �ҵ�����ٿ������confreportid

#ifdef WIN32
				sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%I64d", &confreportid);
#elif defined LINUX
				sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%lld", &confreportid);
#endif

				bFindConfReportID = true;
			}

			if (bFindConfReportID == false) // ���ڴ���δ�ҵ��������ݿ�
			{
				pConfReport->SetConfID(confid);
				if (pConfReport->SelectDB())// 
				{
					confreportid = pConfReport->GetConfReportID();

#ifdef WIN32
					sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_InsertConfLiveInfo == find confreportid from ConfReport table=== confid=%I64d, confreportid=%I64d\n", confid, confreportid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_WARNING, "====== Handle_InsertConfLiveInfo == find confreportid from ConfReport table=== confid=%lld, confreportid=%lld\n", confid, confreportid);
#endif
				}
				else
				{
#ifdef WIN32
					sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_InsertConfLiveInfo == Not find confreportid from ConfReport table=== confid=%I64d\n", confid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_ERROR, "====== Handle_InsertConfLiveInfo == Not find confreportid from ConfReport table=== confid=%lld\n", confid);
#endif
				}

			}

			if (confreportid == 0)
			{
#ifdef WIN32
				sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_InsertConfLiveInfo == Not find confreportid of Netmp === confid=%I64d\n", confid);
#elif defined LINUX
				sr_printf(SR_LOGLEVEL_ERROR, "===return=== Handle_InsertConfLiveInfo == Not find confreportid of Netmp === confid=%lld\n", confid);
#endif

				delete pParam;
				pParam = NULL;

				return;
			}
		}

		// ���confreportid
#ifdef WIN32
		sprintf(conf_report_id_value, "%I64d", confreportid);
		//sprintf(liveinfo_confid_liveinfoid_key, "liveinfo_%I64d_%I64d", confid, liveinfoid);
#elif defined LINUX
		sprintf(conf_report_id_value, "%lld", confreportid);
		//sprintf(liveinfo_confid_liveinfoid_key, "liveinfo_%lld_%lld", confid, liveinfoid);
#endif

		m_pRedisConnList[e_RC_TT_UpMsqThread]->sethashvalue(confliveinfoidset_confid_key, liveinfo_id_value, live_starttime_value);// ��ֹ�����������ݻ�дʱ�û����ֱ������ٿ�

		sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_InsertConfLiveInfo into db(4) table(%s) [key:%s,value:%s] \n", confliveinfoidset_confid_key, live_starttime_value, liveinfo_id_value);

		REDISKEY strliveinfo_confid_liveinfoid_key = liveinfo_confid_liveinfoid_key;
		REDISVDATA vLiveinfoData;
		vLiveinfoData.clear();
		
		REDISKEY strLiveinfoidFieldName = cLiveinfoidFieldName;
		REDISVALUE strliveinfoidvalue = liveinfo_id_value;
		vLiveinfoData.push_back(strLiveinfoidFieldName);
		vLiveinfoData.push_back(strliveinfoidvalue);

		REDISKEY strConfidFieldName = cConfidFieldName;
		REDISVALUE strconfidvalue = conf_id_value;
		vLiveinfoData.push_back(strConfidFieldName);
		vLiveinfoData.push_back(strconfidvalue);

		REDISKEY strConfReportidFieldName = cConfReportidFieldName;
		REDISVALUE strconfreportidvalue = conf_report_id_value;
		vLiveinfoData.push_back(strConfReportidFieldName);
		vLiveinfoData.push_back(strconfreportidvalue);

		REDISKEY strLiveSubjectFieldName = cLiveSubjectFieldName;
		REDISVALUE strlivesubjectvalue = live_subject_value;
		vLiveinfoData.push_back(strLiveSubjectFieldName);
		vLiveinfoData.push_back(strlivesubjectvalue);

		REDISKEY strLiveStarttimeFieldName = cLiveStarttimeFieldName;
		REDISVALUE strlivestarttimevalue = live_starttime_value;
		vLiveinfoData.push_back(strLiveStarttimeFieldName);
		vLiveinfoData.push_back(strlivestarttimevalue);

		REDISKEY strLiveEndtimeFieldName = cLiveEndtimeFieldName;
		REDISVALUE strliveendtimevalue = live_endtime_value;
		vLiveinfoData.push_back(strLiveEndtimeFieldName);
		vLiveinfoData.push_back(strliveendtimevalue);

		REDISKEY strLiveChairmanFieldName = cLiveChairmanFieldName;
		REDISVALUE strlivechairmanvalue = live_chairman_value;
		vLiveinfoData.push_back(strLiveChairmanFieldName);
		vLiveinfoData.push_back(strlivechairmanvalue);

		REDISKEY strLiveAbstractFieldName = cLiveAbstractFieldName;
		REDISVALUE strliveabstractvalue = live_abstract_value;
		vLiveinfoData.push_back(strLiveAbstractFieldName);
		vLiveinfoData.push_back(strliveabstractvalue);

		REDISKEY strLiveisPublicFieldName = cLiveisPublicFieldName;
		REDISVALUE strliveispublicvalue = live_ispublic_value;
		vLiveinfoData.push_back(strLiveisPublicFieldName);
		vLiveinfoData.push_back(strliveispublicvalue);

		REDISKEY strLivePWDFieldName = cLivePWDFieldName;
		REDISVALUE strlivepwdvalue = live_pwd_value;
		vLiveinfoData.push_back(strLivePWDFieldName);
		vLiveinfoData.push_back(strlivepwdvalue);

		REDISKEY strLiveisUseRecFieldName = cLiveisUseRecFieldName;
		REDISVALUE strliveisuserecvalue = live_isuserec_value;
		vLiveinfoData.push_back(strLiveisUseRecFieldName);
		vLiveinfoData.push_back(strliveisuserecvalue);

		REDISKEY strLiveAddrFieldName = cLiveAddrFieldName;
		REDISVALUE strliveaddrvalue = live_addr_value;
		vLiveinfoData.push_back(strLiveAddrFieldName);
		vLiveinfoData.push_back(strliveaddrvalue);

		REDISKEY strLiveWatchtimesFieldName = cLiveWatchtimesFieldName;
		REDISVALUE strlivewatchtimesvalue = live_watch_times_value;
		vLiveinfoData.push_back(strLiveWatchtimesFieldName);
		vLiveinfoData.push_back(strlivewatchtimesvalue);

		REDISKEY strLiveLikestimesFieldName = cLiveLikestimesFieldName;
		REDISVALUE strlivelikestimesvalue = live_likes_times_value;
		vLiveinfoData.push_back(strLiveLikestimesFieldName);
		vLiveinfoData.push_back(strlivelikestimesvalue);
		
		REDISKEY strLiveWatchaddrFieldName = cLiveWatchaddrFieldName;
		REDISVALUE strlivewatchaddrvalue = live_watch_addr_value;
		vLiveinfoData.push_back(strLiveWatchaddrFieldName);
		vLiveinfoData.push_back(strlivewatchaddrvalue);
		
		REDISKEY strLiveAndroidtimesFieldName = cLiveAndroidtimesFieldName;
		REDISVALUE strliveandroidtimesvalue = live_android_times_value;
		vLiveinfoData.push_back(strLiveAndroidtimesFieldName);
		vLiveinfoData.push_back(strliveandroidtimesvalue);

		REDISKEY strLiveIOStimesFieldName = cLiveIOStimesFieldName;
		REDISVALUE strliveiostimesvalue = live_ios_times_value;
		vLiveinfoData.push_back(strLiveIOStimesFieldName);
		vLiveinfoData.push_back(strliveiostimesvalue);

		REDISKEY strLivePCtimesFieldName = cLivePCtimesFieldName;
		REDISVALUE strlivepctimesvalue = live_pc_times_value;
		vLiveinfoData.push_back(strLivePCtimesFieldName);
		vLiveinfoData.push_back(strlivepctimesvalue);

		REDISKEY strLiveMobiletimesFieldName = cLiveMobiletimesFieldName;
		REDISVALUE strlivemobiletimesvalue = live_mobile_times_value;
		vLiveinfoData.push_back(strLiveMobiletimesFieldName);
		vLiveinfoData.push_back(strlivemobiletimesvalue);

		REDISKEY strLiveConvertStatusFieldName = cLiveConvertStatusFieldName;
		REDISVALUE strliveconvertstatusvalue = live_convert_status_value;
		vLiveinfoData.push_back(strLiveConvertStatusFieldName);
		vLiveinfoData.push_back(strliveconvertstatusvalue);

		REDISKEY strLiveTaskIdFieldName = cLiveTaskIdFieldName;
		REDISVALUE strlivetaskidvalue = live_task_id_value;
		vLiveinfoData.push_back(strLiveTaskIdFieldName);
		vLiveinfoData.push_back(strlivetaskidvalue);

		REDISKEY strLiveSvrtypeFieldName = cLiveSvrtypeFieldName;
		REDISVALUE strlivesvrtypevalue = live_svrtype_value;
		vLiveinfoData.push_back(strLiveSvrtypeFieldName);
		vLiveinfoData.push_back(strlivesvrtypevalue);

		REDISKEY strRTMPUrlFieldName = cRTMPUrlFieldName;
		REDISVALUE strrtmpurlvalue = rtmp_url_value;
		vLiveinfoData.push_back(strRTMPUrlFieldName);
		vLiveinfoData.push_back(strrtmpurlvalue);

		REDISKEY strFLVUrlFieldName = cFLVUrlFieldName;
		REDISVALUE strflvurlvalue = flv_url_value;
		vLiveinfoData.push_back(strFLVUrlFieldName);
		vLiveinfoData.push_back(strflvurlvalue);
		
		REDISKEY strHLSUrlFieldName = cHLSUrlFieldName;
		REDISVALUE strhlsurlvalue = hls_url_value;
		vLiveinfoData.push_back(strHLSUrlFieldName);
		vLiveinfoData.push_back(strhlsurlvalue);
		
		bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strliveinfo_confid_liveinfoid_key, vLiveinfoData);


#ifdef LINUX
		gettimeofday(&end, NULL); // ��s level
		time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

		sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_InsertConfLiveInfo into db(4) ** (%s) **time_use** is: %lf us \n", liveinfo_confid_liveinfoid_key, time_use);
#endif 
	}


	delete pParam;
	pParam = NULL;
}

void CDevMgr::UpdateConfLiveInfo(const SRMsgs::IndCRSStopLive* ind, char* time, time_t lltime)
{
	if (!m_pUpMsqThread)
		return;

	char strliveurl[255] = { 0 };

	sprintf(strliveurl, "%s", ind->liveurl().c_str());

	typedef CBufferT<unsigned long long, CAsyncThread*, time_t, CAsyncThread*, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), strliveurl, strlen(strliveurl), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, ind->confid(), m_pUpMsqThread, lltime, m_pWriteDBThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_UpdateConfLiveInfo, (void*)pParam));
	return;
}

void CDevMgr::Handle_UpdateConfLiveInfo(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, CAsyncThread*, time_t, CAsyncThread*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	char* ptime = pParam->m_pData1;

	//sr_printf(SR_LOGLEVEL_NORMAL, "==waring-->> Handle_UpdateLiveInfo filestorsvrip len=%d, filerootpath len=%d, filestorpath len=%d, sdefilepath len=%d.\n", pParam->m_nLen1, pParam->m_nLen2, pParam->m_nLen3, pParam->m_nLen4);

	unsigned long long confreportid = 0;

	unsigned long long confid = pParam->m_Arg1;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg2);
	time_t add_time = (time_t)(pParam->m_Arg3);
	CAsyncThread* pWriteDBThread = (CAsyncThread*)(pParam->m_Arg4);

	//if (pParam->m_nLen1 > 0)
	//{
	//	memcpy(live_chairman_value, pParam->m_pData1, pParam->m_nLen1);
	//}
	//if (pParam->m_nLen2 > 0)
	//{
	//	memcpy(live_subject_value, pParam->m_pData2, pParam->m_nLen2);
	//}

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateConfLiveInfo pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}

	CConfLiveInfo* pConfLiveInfo = NULL;
	std::map<CAsyncThread*, CConfLiveInfo*>::iterator iter_mapAsyncThreadConfLiveInfo = m_mapAsyncThreadConfLiveInfo.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfLiveInfo != m_mapAsyncThreadConfLiveInfo.end())
	{
		pConfLiveInfo = iter_mapAsyncThreadConfLiveInfo->second;
	}

	if (pConfReport == NULL
		|| pConfLiveInfo == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateConfLiveInfo pConfLiveInfo is NULL.\n");

		return;
	}

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif 

	char cLiveinfoidFieldName[128] = { 0 };
	char liveinfo_id_value[128] = { 0 };
	char cConfidFieldName[128] = { 0 };
	char conf_id_value[128] = { 0 };
	char cConfReportidFieldName[128] = { 0 };
	char conf_report_id_value[128] = { 0 };

	char cLiveSubjectFieldName[128] = { 0 };
	char live_subject_value[255] = { 0 };

	char cLiveStarttimeFieldName[128] = { 0 };
	char live_starttime_value[128] = { 0 };
	char cLiveEndtimeFieldName[128] = { 0 };
	char live_endtime_value[128] = { 0 };

	char cLiveChairmanFieldName[128] = { 0 };
	char live_chairman_value[255] = { 0 };
	char cLiveAbstractFieldName[128] = { 0 };
	char live_abstract_value[255] = { 0 };
	char cLiveisPublicFieldName[128] = { 0 };
	char live_ispublic_value[128] = { 0 };
	char cLivePWDFieldName[128] = { 0 };
	char live_pwd_value[128] = { 0 };
	char cLiveisUseRecFieldName[128] = { 0 };
	char live_isuserec_value[128] = { 0 };

	char cLiveAddrFieldName[128] = { 0 };
	char live_addr_value[1024] = { 0 };// ��ӦIndCRSStartLive��Ϣ��livepullurl�ֶ�
	char cLiveWatchtimesFieldName[128] = { 0 };
	char live_watch_times_value[128] = { 0 };
	char cLiveLikestimesFieldName[128] = { 0 };
	char live_likes_times_value[128] = { 0 };
	char cLiveWatchaddrFieldName[128] = { 0 };
	char live_watch_addr_value[255] = { 0 };
	char cLiveAndroidtimesFieldName[128] = { 0 };
	char live_android_times_value[128] = { 0 };
	char cLiveIOStimesFieldName[128] = { 0 };
	char live_ios_times_value[128] = { 0 };
	char cLivePCtimesFieldName[128] = { 0 };
	char live_pc_times_value[128] = { 0 };
	char cLiveMobiletimesFieldName[128] = { 0 };
	char live_mobile_times_value[128] = { 0 };

	char cLiveConvertStatusFieldName[128] = { 0 };
	char live_convert_status_value[128] = { 0 };
	char cLiveTaskIdFieldName[128] = { 0 };
	char live_task_id_value[128] = { 0 };


	sprintf(cLiveinfoidFieldName, "%s", "li_id");
	sprintf(cConfidFieldName, "%s", "conf_id");
	sprintf(cConfReportidFieldName, "%s", "conf_report_id");

	sprintf(cLiveSubjectFieldName, "%s", "li_subject");

	sprintf(cLiveStarttimeFieldName, "%s", "starttime");
	sprintf(cLiveEndtimeFieldName, "%s", "endtime");

	sprintf(cLiveChairmanFieldName, "%s", "chairman");
	sprintf(cLiveAbstractFieldName, "%s", "li_abstract");
	sprintf(cLiveisPublicFieldName, "%s", "is_public");
	sprintf(cLivePWDFieldName, "%s", "live_pwd");
	sprintf(cLiveisUseRecFieldName, "%s", "is_userec");

	sprintf(cLiveAddrFieldName, "%s", "live_addr");
	sprintf(cLiveWatchtimesFieldName, "%s", "watch_times");
	sprintf(cLiveLikestimesFieldName, "%s", "likes_times");
	sprintf(cLiveWatchaddrFieldName, "%s", "watch_addr");
	sprintf(cLiveAndroidtimesFieldName, "%s", "android");
	sprintf(cLiveIOStimesFieldName, "%s", "ios_times");
	sprintf(cLivePCtimesFieldName, "%s", "pc_times");
	sprintf(cLiveMobiletimesFieldName, "%s", "mobile_times");

	sprintf(cLiveConvertStatusFieldName, "%s", "convert_status");
	sprintf(cLiveTaskIdFieldName, "%s", "task_id");


	m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);

	char liveinfo_confid_liveinfoid_key[128] = { 0 };
#ifdef WIN32
	sprintf(conf_id_value, "%I64d", confid);
#elif defined LINUX
	sprintf(conf_id_value, "%lld", confid);
#endif

	char confliveinfoidset_confid_key[128] = { 0 };
	sprintf(confliveinfoidset_confid_key, "confliveinfoidset_%s", conf_id_value);
	RedisReplyArray vRedisReplyArray;
	vRedisReplyArray.clear();

	bool bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll(confliveinfoidset_confid_key, vRedisReplyArray);

	unsigned int uiConfliveinfoidsize = vRedisReplyArray.size();

	sr_printf(SR_LOGLEVEL_DEBUG, " m_pRedisConnList[Handle_UpdateConfLiveInfo]->hashHGetAll==%s-->> %d, size = %d\n", confliveinfoidset_confid_key, bhashHGetAll_ok, uiConfliveinfoidsize);

	if (bhashHGetAll_ok
		&& (uiConfliveinfoidsize > 0)
		&& (uiConfliveinfoidsize % 2 == 0))
	{
		unsigned long long liveinfoid = 0;
		char livestrattime[128] = { 0 };

		for (unsigned int ii = 0; ii < uiConfliveinfoidsize;)
		{
			sr_printf(SR_LOGLEVEL_DEBUG, "==Handle_UpdateConfLiveInfo==In redis db(4) key:%s ==>>> find starttime=%s, liveinfoid=%s\n", confliveinfoidset_confid_key, vRedisReplyArray[ii].str.c_str(), vRedisReplyArray[ii + 1].str.c_str());
			ii += 2;
		}

		//// �ҵ�����ٿ������liveinfoid
		//sscanf(vRedisReplyArray[uiConfliveinfoidsize - 2].str.c_str(), "%s", livestrattime);
		sprintf(livestrattime, "%s", vRedisReplyArray[uiConfliveinfoidsize - 2].str.c_str());
#ifdef WIN32
		sscanf(vRedisReplyArray[uiConfliveinfoidsize - 1].str.c_str(), "%I64d", &liveinfoid);
#elif defined LINUX
		sscanf(vRedisReplyArray[uiConfliveinfoidsize - 1].str.c_str(), "%lld", &liveinfoid);
#endif

		if (liveinfoid != 0)
		{
			char liveinfo_confid_liveinfoid_key[128] = { 0 };
			char insert_liveinfo_confid_liveinfoid_key[128] = { 0 };
#ifdef WIN32
			sprintf(liveinfo_confid_liveinfoid_key, "liveinfo_%I64d_%I64d", confid, liveinfoid);
			sprintf(insert_liveinfo_confid_liveinfoid_key, "insliveinfo_%I64d_%I64d", confid, liveinfoid);
#elif defined LINUX
			sprintf(liveinfo_confid_liveinfoid_key, "liveinfo_%lld_%lld", confid, liveinfoid);
			sprintf(insert_liveinfo_confid_liveinfoid_key, "insliveinfo_%lld_%lld", confid, liveinfoid);
#endif
			
			REDISKEY strliveinfoconfidliveinfoidkey = liveinfo_confid_liveinfoid_key;
			REDISFILEDS vGetFileds;
			vGetFileds.clear();
			RedisReplyArray vRedisReplyArray_liveinfo;
			vRedisReplyArray_liveinfo.clear();
			// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
			REDISKEY strLiveinfoidFieldName = cLiveinfoidFieldName;
			REDISKEY strConfidFieldName = cConfidFieldName;
			REDISKEY strConfReportidFieldName = cConfReportidFieldName;
			REDISKEY strLiveSubjectFieldName = cLiveSubjectFieldName;
			REDISKEY strLiveStarttimeFieldName = cLiveStarttimeFieldName;
			REDISKEY strLiveEndtimeFieldName = cLiveEndtimeFieldName;
			REDISKEY strLiveChairmanFieldName = cLiveChairmanFieldName;
			REDISKEY strLiveAbstractFieldName = cLiveAbstractFieldName;
			REDISKEY strLiveisPublicFieldName = cLiveisPublicFieldName;
			REDISKEY strLivePWDFieldName = cLivePWDFieldName;
			REDISKEY strLiveisUseRecFieldName = cLiveisUseRecFieldName;
			REDISKEY strLiveAddrFieldName = cLiveAddrFieldName;
			REDISKEY strLiveWatchtimesFieldName = cLiveWatchtimesFieldName;
			REDISKEY strLiveLikestimesFieldName = cLiveLikestimesFieldName;
			REDISKEY strLiveWatchaddrFieldName = cLiveWatchaddrFieldName;
			REDISKEY strLiveAndroidtimesFieldName = cLiveAndroidtimesFieldName;
			REDISKEY strLiveIOStimesFieldName = cLiveIOStimesFieldName;
			REDISKEY strLivePCtimesFieldName = cLivePCtimesFieldName;
			REDISKEY strLiveMobiletimesFieldName = cLiveMobiletimesFieldName;
			REDISKEY strLiveConvertStatusFieldName = cLiveConvertStatusFieldName;
			REDISKEY strLiveTaskIdFieldName = cLiveTaskIdFieldName;
			vGetFileds.push_back(strLiveinfoidFieldName);
			vGetFileds.push_back(strConfidFieldName);
			vGetFileds.push_back(strConfReportidFieldName);
			vGetFileds.push_back(strLiveSubjectFieldName);
			vGetFileds.push_back(strLiveStarttimeFieldName);
			vGetFileds.push_back(strLiveEndtimeFieldName);
			vGetFileds.push_back(strLiveChairmanFieldName);
			vGetFileds.push_back(strLiveAbstractFieldName);
			vGetFileds.push_back(strLiveisPublicFieldName);
			vGetFileds.push_back(strLivePWDFieldName);
			vGetFileds.push_back(strLiveisUseRecFieldName);
			vGetFileds.push_back(strLiveAddrFieldName);
			vGetFileds.push_back(strLiveWatchtimesFieldName);
			vGetFileds.push_back(strLiveLikestimesFieldName);
			vGetFileds.push_back(strLiveWatchaddrFieldName);
			vGetFileds.push_back(strLiveAndroidtimesFieldName);
			vGetFileds.push_back(strLiveIOStimesFieldName);
			vGetFileds.push_back(strLivePCtimesFieldName);
			vGetFileds.push_back(strLiveMobiletimesFieldName);
			vGetFileds.push_back(strLiveConvertStatusFieldName);
			vGetFileds.push_back(strLiveTaskIdFieldName);

			bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMGet(strliveinfoconfidliveinfoidkey, vGetFileds, vRedisReplyArray_liveinfo);

			if (bhashHMGet_ok
				&& vRedisReplyArray_liveinfo.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
			{
				REDISKEY strinsertliveinfoconfidliveinfoidkey = insert_liveinfo_confid_liveinfoid_key;
				REDISVDATA vLiveinfoData;
				vLiveinfoData.clear();

				sprintf(liveinfo_id_value, "%s", vRedisReplyArray_liveinfo[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(conf_id_value, "%s", vRedisReplyArray_liveinfo[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(conf_report_id_value, "%s", vRedisReplyArray_liveinfo[2].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_subject_value, "%s", vRedisReplyArray_liveinfo[3].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_starttime_value, "%s", vRedisReplyArray_liveinfo[4].str.c_str()); // ��Ҫ��push_back����˳��һ��
				//sprintf(live_endtime_value, "%s", vRedisReplyArray_liveinfo[5].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_chairman_value, "%s", vRedisReplyArray_liveinfo[6].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_abstract_value, "%s", vRedisReplyArray_liveinfo[7].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_ispublic_value, "%s", vRedisReplyArray_liveinfo[8].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_pwd_value, "%s", vRedisReplyArray_liveinfo[9].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_isuserec_value, "%s", vRedisReplyArray_liveinfo[10].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_addr_value, "%s", vRedisReplyArray_liveinfo[11].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_watch_times_value, "%s", vRedisReplyArray_liveinfo[12].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_likes_times_value, "%s", vRedisReplyArray_liveinfo[13].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_watch_addr_value, "%s", vRedisReplyArray_liveinfo[14].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_android_times_value, "%s", vRedisReplyArray_liveinfo[15].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_ios_times_value, "%s", vRedisReplyArray_liveinfo[16].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_pc_times_value, "%s", vRedisReplyArray_liveinfo[17].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_mobile_times_value, "%s", vRedisReplyArray_liveinfo[18].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_convert_status_value, "%s", vRedisReplyArray_liveinfo[19].str.c_str()); // ��Ҫ��push_back����˳��һ��
				sprintf(live_task_id_value, "%s", vRedisReplyArray_liveinfo[20].str.c_str()); // ��Ҫ��push_back����˳��һ��

				//REDISKEY strLiveinfoidFieldName = cLiveinfoidFieldName;
				REDISVALUE strliveinfoidvalue = liveinfo_id_value;
				vLiveinfoData.push_back(strLiveinfoidFieldName);
				vLiveinfoData.push_back(strliveinfoidvalue);

				//REDISKEY strConfidFieldName = cConfidFieldName;
				REDISVALUE strconfidvalue = conf_id_value;
				vLiveinfoData.push_back(strConfidFieldName);
				vLiveinfoData.push_back(strconfidvalue);

				//REDISKEY strConfReportidFieldName = cConfReportidFieldName;
				REDISVALUE strconfreportidvalue = conf_report_id_value;
				vLiveinfoData.push_back(strConfReportidFieldName);
				vLiveinfoData.push_back(strconfreportidvalue);

				//REDISKEY strLiveSubjectFieldName = cLiveSubjectFieldName;
				REDISVALUE strlivesubjectvalue = live_subject_value;
				vLiveinfoData.push_back(strLiveSubjectFieldName);
				vLiveinfoData.push_back(strlivesubjectvalue);

				//REDISKEY strLiveStarttimeFieldName = cLiveStarttimeFieldName;
				REDISVALUE strlivestarttimevalue = live_starttime_value;
				vLiveinfoData.push_back(strLiveStarttimeFieldName);
				vLiveinfoData.push_back(strlivestarttimevalue);

				//REDISKEY strLiveEndtimeFieldName = cLiveEndtimeFieldName;
				REDISVALUE strliveendtimevalue = ptime;
				vLiveinfoData.push_back(strLiveEndtimeFieldName);
				vLiveinfoData.push_back(strliveendtimevalue);

				//REDISKEY strLiveChairmanFieldName = cLiveChairmanFieldName;
				REDISVALUE strlivechairmanvalue = live_chairman_value;
				vLiveinfoData.push_back(strLiveChairmanFieldName);
				vLiveinfoData.push_back(strlivechairmanvalue);

				//REDISKEY strLiveAbstractFieldName = cLiveAbstractFieldName;
				REDISVALUE strliveabstractvalue = live_abstract_value;
				vLiveinfoData.push_back(strLiveAbstractFieldName);
				vLiveinfoData.push_back(strliveabstractvalue);

				//REDISKEY strLiveisPublicFieldName = cLiveisPublicFieldName;
				REDISVALUE strliveispublicvalue = live_ispublic_value;
				vLiveinfoData.push_back(strLiveisPublicFieldName);
				vLiveinfoData.push_back(strliveispublicvalue);

				//REDISKEY strLivePWDFieldName = cLivePWDFieldName;
				REDISVALUE strlivepwdvalue = live_pwd_value;
				vLiveinfoData.push_back(strLivePWDFieldName);
				vLiveinfoData.push_back(strlivepwdvalue);


				//REDISKEY strLiveisUseRecFieldName = cLiveisUseRecFieldName;
				REDISVALUE strliveisuserecvalue = live_isuserec_value;
				vLiveinfoData.push_back(strLiveisUseRecFieldName);
				vLiveinfoData.push_back(strliveisuserecvalue);

				//REDISKEY strLiveAddrFieldName = cLiveAddrFieldName;
				REDISVALUE strliveaddrvalue = live_addr_value;
				vLiveinfoData.push_back(strLiveAddrFieldName);
				vLiveinfoData.push_back(strliveaddrvalue);

				//REDISKEY strLiveWatchtimesFieldName = cLiveWatchtimesFieldName;
				REDISVALUE strlivewatchtimesvalue = live_watch_times_value;
				vLiveinfoData.push_back(strLiveWatchtimesFieldName);
				vLiveinfoData.push_back(strlivewatchtimesvalue);

				//REDISKEY strLiveLikestimesFieldName = cLiveLikestimesFieldName;
				REDISVALUE strlivelikestimesvalue = live_likes_times_value;
				vLiveinfoData.push_back(strLiveLikestimesFieldName);
				vLiveinfoData.push_back(strlivelikestimesvalue);

				//REDISKEY strLiveWatchaddrFieldName = cLiveWatchaddrFieldName;
				REDISVALUE strlivewatchaddrvalue = live_watch_addr_value;
				vLiveinfoData.push_back(strLiveWatchaddrFieldName);
				vLiveinfoData.push_back(strlivewatchaddrvalue);

				//REDISKEY strLiveAndroidtimesFieldName = cLiveAndroidtimesFieldName;
				REDISVALUE strliveandroidtimesvalue = live_android_times_value;
				vLiveinfoData.push_back(strLiveAndroidtimesFieldName);
				vLiveinfoData.push_back(strliveandroidtimesvalue);

				//REDISKEY strLiveIOStimesFieldName = cLiveIOStimesFieldName;
				REDISVALUE strliveiostimesvalue = live_ios_times_value;
				vLiveinfoData.push_back(strLiveIOStimesFieldName);
				vLiveinfoData.push_back(strliveiostimesvalue);

				//REDISKEY strLivePCtimesFieldName = cLivePCtimesFieldName;
				REDISVALUE strlivepctimesvalue = live_pc_times_value;
				vLiveinfoData.push_back(strLivePCtimesFieldName);
				vLiveinfoData.push_back(strlivepctimesvalue);

				//REDISKEY strLiveMobiletimesFieldName = cLiveMobiletimesFieldName;
				REDISVALUE strlivemobiletimesvalue = live_mobile_times_value;
				vLiveinfoData.push_back(strLiveMobiletimesFieldName);
				vLiveinfoData.push_back(strlivemobiletimesvalue);

				//REDISKEY strLiveConvertStatusFieldName = cLiveConvertStatusFieldName;
				REDISVALUE strliveconvertstatusvalue = live_convert_status_value;
				vLiveinfoData.push_back(strLiveConvertStatusFieldName);
				vLiveinfoData.push_back(strliveconvertstatusvalue);

				//REDISKEY strLiveTaskIdFieldName = cLiveTaskIdFieldName;
				REDISVALUE strlivetaskidvalue = live_task_id_value;
				vLiveinfoData.push_back(strLiveTaskIdFieldName);
				vLiveinfoData.push_back(strlivetaskidvalue);

				bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strinsertliveinfoconfidliveinfoidkey, vLiveinfoData);

				bool bListLPushOK = false;
				bListLPushOK = m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", insert_liveinfo_confid_liveinfoid_key);
				sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateConfLiveInfo listLPush ** (%s) **bListLPushOK=%d\n", insert_liveinfo_confid_liveinfoid_key, bListLPushOK);

				// 
				m_pRedisConnList[e_RC_TT_UpMsqThread]->deletevalue(liveinfo_confid_liveinfoid_key);
				m_pRedisConnList[e_RC_TT_UpMsqThread]->deletehashvalue(confliveinfoidset_confid_key, livestrattime);

				if (pWriteDBThread)
				{
					typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
					CParam* pParam;
					pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pWriteDBThread);

					typedef void (CDevMgr::* ACTION)(void*);
					pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
						*this, &CDevMgr::Handle_WriteUserConfInfoToDB, (void*)pParam));
				}
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "==Handle_UpdateConfLiveInfo-->> m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMGet %s: is error.\n", strliveinfoconfidliveinfoidkey.c_str());
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "==Handle_UpdateConfLiveInfo-->> liveinfoid is 0\n");
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "==Handle_UpdateConfLiveInfo-->> m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll %s: is error.\n", confliveinfoidset_confid_key);
	}


	//pConfLiveInfo->SetLiveInfoID();
	//pConfLiveInfo->SetConfID();
	//pConfLiveInfo->SetConfReportID();
	//pConfLiveInfo->SetLiveSubject();
	//pConfLiveInfo->SetStartTime();
	//pConfLiveInfo->SetEndTime();
	//pConfLiveInfo->SetLiveChairman();
	//pConfLiveInfo->SetLiveAbstract();
	//pConfLiveInfo->SetIsPublic();
	//pConfLiveInfo->SetLivePWD();
	//pConfLiveInfo->SetLiveAddr();
	//pConfLiveInfo->SetWatchtimes();
	//pConfLiveInfo->SetLiketimes();
	//pConfLiveInfo->SetWatchaddr();
	//pConfLiveInfo->InsertConfLiveInfoToDB();

#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateConfLiveInfo into db(4) ** (%s) **time_use** is: %lf us \n", liveinfo_confid_liveinfoid_key, time_use);
#endif 

	delete pParam;
	pParam = NULL;
}

void CDevMgr::ModifyConfCallListInfo(const SRMsgs::IndModifyConfCallList* ind, char* time, time_t lltime)
{
	if (!m_pUpMsqThread)
		return;

	unsigned int uioptype = 0;
	unsigned long long ullrollcallid = 0;
	uioptype = ind->optype();
	ullrollcallid = ind->rollcalllist().rcid();
	char strrollcallname[255] = { 0 };
	sprintf(strrollcallname, "%s", ind->rollcalllist().rcname().c_str());

	std::string strcalllist;
	strcalllist.clear();
	for (int orderno = 0; orderno < ind->rollcalllist().parts_size(); orderno++)
	{
		//ind->rollcalllist().parts(orderno).psuid();
		//ind->rollcalllist().parts(orderno).nickname();
		char partinfo[255] = { 0 };
		sprintf(partinfo, "%d@%d", ind->rollcalllist().parts(orderno).psuid(), orderno+1);
		strcalllist.append(partinfo);
		strcalllist.append("#");
	}

	// �Ƚ�����������redis

	// ������confid+rollcallid+time
	// key����������--value����Ա�б��ַ���
	char confrollcallinfo_confid_rollcallid_time_key[256] = { 0 };
#ifdef WIN32
	sprintf(confrollcallinfo_confid_rollcallid_time_key, "confrollcallinfo_%I64d_%I64d_%I64d", ind->confid(), ullrollcallid, lltime);
#elif defined LINUX
	sprintf(confrollcallinfo_confid_rollcallid_time_key, "confrollcallinfo_%lld_%lld_%lld", ind->confid(), ullrollcallid, lltime);
#endif

	m_pRedisConnList[e_RC_TT_MainThread]->selectdb(4);
	m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(confrollcallinfo_confid_rollcallid_time_key, strcalllist.c_str(), strrollcallname);

	typedef CBufferT<unsigned long long, unsigned int, unsigned long long, CAsyncThread*, time_t, CAsyncThread*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), strrollcallname, strlen(strrollcallname), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, ind->confid(), uioptype, ullrollcallid, m_pUpMsqThread, lltime, m_pWriteDBThread);

	typedef void (CDevMgr::* ACTION)(void*);
	m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_ModifyConfCallListInfo, (void*)pParam));
	return;
}

void CDevMgr::Handle_ModifyConfCallListInfo(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, unsigned int, unsigned long long, CAsyncThread*, time_t, CAsyncThread*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;
	
	////sr_printf(SR_LOGLEVEL_NORMAL, "==waring-->> Handle_UpdateLiveInfo filestorsvrip len=%d, filerootpath len=%d, filestorpath len=%d, sdefilepath len=%d.\n", pParam->m_nLen1, pParam->m_nLen2, pParam->m_nLen3, pParam->m_nLen4);
	
	unsigned long long confid = pParam->m_Arg1;
	unsigned int optype = pParam->m_Arg2;
	unsigned long long rollcallid = pParam->m_Arg3;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg4);
	time_t modify_time = (time_t)(pParam->m_Arg5);
	CAsyncThread* pWriteDBThread = (CAsyncThread*)(pParam->m_Arg6);

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_ModifyConfCallListInfo pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	char cConfidFieldName[128] = { 0 };
	char conf_id_value[128] = { 0 };

	char cRollcallidFieldName[128] = { 0 };
	char rollcall_id_value[128] = { 0 };

	char cRollcallNameFieldName[128] = { 0 };
	char rollcall_name_value[256] = { 0 };

	char cOPtypeFieldName[128] = { 0 };
	char op_type_value[128] = { 0 };

	char cPartInfoFieldName[128] = { 0 };
	//char part_info_value[128] = { 0 };
	std::string strPartinfoValue;
	strPartinfoValue.clear();


	sprintf(cConfidFieldName, "%s", "confid");
	sprintf(cRollcallidFieldName, "%s", "rcid");
	sprintf(cRollcallNameFieldName, "%s", "rcname");
	sprintf(cOPtypeFieldName, "%s", "optype");
	sprintf(cPartInfoFieldName, "%s", "partinfo");

	REDISKEY strConfidFieldName = cConfidFieldName;
	REDISKEY strRollcallidFieldName = cRollcallidFieldName;
	REDISKEY strRollcallNameFieldName = cRollcallNameFieldName;
	REDISKEY strOPtypeFieldName = cOPtypeFieldName;
	REDISKEY strPartInfoFieldName = cPartInfoFieldName;
	

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif 

	char confrollcallinfo_confid_rollcallid_time_key[256] = { 0 };
	char modifyrollcallinfo_confid_rollcallid_time_key[256] = { 0 };
#ifdef WIN32
	sprintf(conf_id_value, "%I64d", confid);
	sprintf(rollcall_id_value, "%I64d", rollcallid);
	sprintf(confrollcallinfo_confid_rollcallid_time_key, "confrollcallinfo_%I64d_%I64d_%I64d", confid, rollcallid, modify_time);
	sprintf(modifyrollcallinfo_confid_rollcallid_time_key, "modifyrollcallinfo_%I64d_%I64d_%I64d", confid, rollcallid, modify_time);
#elif defined LINUX
	sprintf(conf_id_value, "%lld", confid);
	sprintf(rollcall_id_value, "%lld", rollcallid);
	sprintf(confrollcallinfo_confid_rollcallid_time_key, "confrollcallinfo_%lld_%lld_%lld", confid, rollcallid, modify_time);
	sprintf(modifyrollcallinfo_confid_rollcallid_time_key, "modifyrollcallinfo_%lld_%lld_%lld", confid, rollcallid, modify_time);
#endif


	//char* ptime = pParam->m_pData1;	
	if (pParam->m_nLen2 > 0)
	{
		memcpy(rollcall_name_value, pParam->m_pData2, pParam->m_nLen2);
	}
	sprintf(op_type_value, "%d", optype);
		
	m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);
	char *ptrPartInfo = NULL;
	ptrPartInfo = m_pRedisConnList[e_RC_TT_UpMsqThread]->gethashvalue(confrollcallinfo_confid_rollcallid_time_key, rollcall_name_value); // ��ȡ��key��Ӧ��value

	//printf("=== %s ====[%d] m_pRedis->gethashvalue =====-----------ptrPartInfo----------->>> %p\n", __FILE__, __LINE__, ptr);

	if (ptrPartInfo != NULL)
	{
		strPartinfoValue.assign(ptrPartInfo);
	}

	REDISKEY strmodifyrollcallinfoconfidrollcallidtimekey = modifyrollcallinfo_confid_rollcallid_time_key;
	REDISVDATA vRollCallInfoData;
	vRollCallInfoData.clear();
		
	//REDISKEY strConfidFieldName = cConfidFieldName;
	REDISVALUE strconfidvalue = conf_id_value;
	vRollCallInfoData.push_back(strConfidFieldName);
	vRollCallInfoData.push_back(strconfidvalue);

	//REDISKEY strRollcallidFieldName = cRollcallidFieldName;
	REDISVALUE strrollcallidvalue = rollcall_id_value;
	vRollCallInfoData.push_back(strRollcallidFieldName);
	vRollCallInfoData.push_back(strrollcallidvalue);

	//REDISKEY strRollcallNameFieldName = cRollcallNameFieldName;
	REDISVALUE strrollcallnamevalue = rollcall_name_value;
	vRollCallInfoData.push_back(strRollcallNameFieldName);
	vRollCallInfoData.push_back(strrollcallnamevalue);

	//REDISKEY strOPtypeFieldName = cOPtypeFieldName;
	REDISVALUE stroptypevalue = op_type_value;
	vRollCallInfoData.push_back(strOPtypeFieldName);
	vRollCallInfoData.push_back(stroptypevalue);
	
	//REDISKEY strPartInfoFieldName = cPartInfoFieldName;
	//REDISVALUE strpartinfovalue = part_info_value;
	vRollCallInfoData.push_back(strPartInfoFieldName);
	vRollCallInfoData.push_back(strPartinfoValue);


	bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strmodifyrollcallinfoconfidrollcallidtimekey, vRollCallInfoData);
	if (bhashHMSet_ok)
	{
		m_pRedisConnList[e_RC_TT_UpMsqThread]->deletehashvalue(confrollcallinfo_confid_rollcallid_time_key, rollcall_name_value);

		bool bListLPushOK = false;
		bListLPushOK = m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", modifyrollcallinfo_confid_rollcallid_time_key);

		if (pWriteDBThread)
		{
			typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
			CParam* pParam;
			pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, pWriteDBThread);

			typedef void (CDevMgr::* ACTION)(void*);
			pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
				*this, &CDevMgr::Handle_WriteUserConfInfoToDB, (void*)pParam));
		}
	}
	
#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_ModifyConfCallListInfo into db(4) ** (%s) **time_use** is: %lf us \n", modifyrollcallinfo_confid_rollcallid_time_key, time_use);
#endif 

	delete pParam;
	pParam = NULL;
}

void CDevMgr::UpdateTerStatisticsInfo(const SRMsgs::IndTerStatisticsInfo* ind, char* time, time_t lltime)
{
	if (!m_pUpMsqThread)
		return;

	// ���ն�Ϊ��λ����
	for (int i = 0; i < ind->tsinfo_size(); i++)
	{
		const SRMsgs::IndTerStatisticsInfo_TerStatisticsInfo& ind_tsi_msg = ind->tsinfo(i);
		if (ind_tsi_msg.suid() != 0)
		{
			char strtertxinfo[2048] = { 0 };
			int nTxinfoLen = 0;
			char strterrxinfo[2048] = { 0 };
			int nRxinfoLen = 0;

			// pars txinfo
			for (SR_int32 j = 0; j < ind_tsi_msg.txinfo_size(); ++j)
			{
				const SRMsgs::IndTerStatisticsInfo_XfeInfo& txinfo = ind_tsi_msg.txinfo(j);
				//SR_bool bhavetxinfo = false;
				for (SR_int32 jj = 0; jj < txinfo.ainfo_size(); ++jj)
				{
					const SRMsgs::IndTerStatisticsInfo_AudioInfo& audioinfo = txinfo.ainfo(jj);
					
					nTxinfoLen += sprintf(strtertxinfo + nTxinfoLen, "atype=%u,atersuid=%u,ateralias=%s,format=%u,bitrate=%u,delay=%u,jitter=%u,loss=%u",
						audioinfo.atype(), audioinfo.atersuid(), audioinfo.ateralias().c_str(), audioinfo.baseinfo().format(), audioinfo.baseinfo().bitrate(), audioinfo.advinfo().delay(), audioinfo.advinfo().jitter(), audioinfo.advinfo().loss());

					nTxinfoLen += sprintf(strtertxinfo + nTxinfoLen, ";");

					//bhavetxinfo = true;
				}
				for (SR_int32 jj = 0; jj < txinfo.vinfo_size(); ++jj)
				{
					const SRMsgs::IndTerStatisticsInfo_VideoInfo& videoinfo = txinfo.vinfo(jj);
					
					nTxinfoLen += sprintf(strtertxinfo + nTxinfoLen, "vtype=%u,vtersuid=%u,vteralias=%s,format=%u,bitrate=%u,delay=%u,jitter=%u,loss=%u",
						videoinfo.vtype(), videoinfo.vtersuid(), videoinfo.vteralias().c_str(), videoinfo.baseinfo().format(), videoinfo.baseinfo().bitrate(), videoinfo.advinfo().delay(), videoinfo.advinfo().jitter(), videoinfo.advinfo().loss());
					
					for (SR_int32 jjj = 0; jjj < videoinfo.frameinfo_size(); ++jjj)
					{
						const SRMsgs::IndTerStatisticsInfo_FrameInfo& frameinfo = videoinfo.frameinfo(jjj);

						nTxinfoLen += sprintf(strtertxinfo + nTxinfoLen, ",fsw=%u,fsh=%u,fps=%u", frameinfo.fsw(), frameinfo.fsh(), frameinfo.fps());
					}

					nTxinfoLen += sprintf(strtertxinfo + nTxinfoLen, ";");
					//bhavetxinfo = true;
				}

			}
			// pars rxinfo
			for (SR_int32 k = 0; k < ind_tsi_msg.rxinfo_size(); ++k)
			{
				const SRMsgs::IndTerStatisticsInfo_XfeInfo& rxinfo = ind_tsi_msg.rxinfo(k);
				//SR_bool bhaverxinfo = false;
				for (SR_int32 kk = 0; kk < rxinfo.ainfo_size(); ++kk)
				{
					const SRMsgs::IndTerStatisticsInfo_AudioInfo& audioinfo = rxinfo.ainfo(kk);
					
					nRxinfoLen += sprintf(strterrxinfo + nRxinfoLen, "atype=%u,atersuid=%u,ateralias=%s,format=%u,bitrate=%u,delay=%u,jitter=%u,loss=%u",
						audioinfo.atype(), audioinfo.atersuid(), audioinfo.ateralias().c_str(), audioinfo.baseinfo().format(), audioinfo.baseinfo().bitrate(), audioinfo.advinfo().delay(), audioinfo.advinfo().jitter(), audioinfo.advinfo().loss());

					nRxinfoLen += sprintf(strterrxinfo + nRxinfoLen, ";");

					//bhaverxinfo = true;
				}
				for (SR_int32 kk = 0; kk < rxinfo.vinfo_size(); ++kk)
				{
					const SRMsgs::IndTerStatisticsInfo_VideoInfo& videoinfo = rxinfo.vinfo(kk);

					nRxinfoLen += sprintf(strterrxinfo + nRxinfoLen, "vtype=%u,vtersuid=%u,vteralias=%s,format=%u,bitrate=%u,delay=%u,jitter=%u,loss=%u",
						videoinfo.vtype(), videoinfo.vtersuid(), videoinfo.vteralias().c_str(), videoinfo.baseinfo().format(), videoinfo.baseinfo().bitrate(), videoinfo.advinfo().delay(), videoinfo.advinfo().jitter(), videoinfo.advinfo().loss());
					
					for (SR_int32 kkk = 0; kkk < videoinfo.frameinfo_size(); ++kkk)
					{
						const SRMsgs::IndTerStatisticsInfo_FrameInfo& frameinfo = videoinfo.frameinfo(kkk);

						nRxinfoLen += sprintf(strterrxinfo + nRxinfoLen, ",fsw=%u,fsh=%u,fps=%u", frameinfo.fsw(), frameinfo.fsh(), frameinfo.fps());
					}
					nRxinfoLen += sprintf(strterrxinfo + nRxinfoLen, ";");
					//bhaverxinfo = true;
				}
			}

			typedef CBufferT<unsigned long long, unsigned int, unsigned int, unsigned int, unsigned int, CAsyncThread*, time_t, void*> CParam;
			CParam* pParam;
			pParam = new CParam(time, strlen(time), strtertxinfo, strlen(strtertxinfo), strterrxinfo, strlen(strterrxinfo), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0
				, ind->confid(), ind_tsi_msg.suid(), ind_tsi_msg.fromtype(), ind_tsi_msg.termtype(), ind_tsi_msg.txbweresult(), m_pUpMsqThread, lltime);

			typedef void (CDevMgr::* ACTION)(void*);
			m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
				*this, &CDevMgr::Handle_UpdateTerStatisticsInfo, (void*)pParam));
		}
	}

	return;
}

void CDevMgr::Handle_UpdateTerStatisticsInfo(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<unsigned long long, unsigned int, unsigned int, unsigned int, unsigned int, CAsyncThread*, time_t, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	unsigned long long confid = pParam->m_Arg1;
	unsigned int tersuid = pParam->m_Arg2;
	unsigned int fromtype = pParam->m_Arg3;
	unsigned int termtype = pParam->m_Arg4;
	unsigned int txbweresult = pParam->m_Arg5;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg6);
	time_t Up_time = (time_t)(pParam->m_Arg7);

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_UpdateTerStatisticsInfo pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

//#ifdef LINUX
//	float time_use = 0;
//	struct timeval start;
//	struct timeval end;
//	gettimeofday(&start, NULL); // ��s level
//#endif 

	// ��txinfo��rxinfo���µ�redis��
	char tercalldetail_confid_tersuid_key[256] = { 0 };
#ifdef WIN32
	sprintf(tercalldetail_confid_tersuid_key, "tercalldetail_%I64d_%u", confid, tersuid);
#elif defined LINUX
	sprintf(tercalldetail_confid_tersuid_key, "tercalldetail_%lld_%u", confid, tersuid);
#endif

	char cFromTypeFieldName[64] = { 0 };
	char from_type_value[64] = { 0 };
	char cTermTypeFieldName[64] = { 0 };
	char term_type_value[64] = { 0 };
	char cTXInfoFieldName[64] = { 0 };
	char tx_info_value[2048] = { 0 };
	char cRXInfoFieldName[64] = { 0 };
	char rx_info_value[2048] = { 0 };
	char cTXBWERsltFieldName[64] = { 0 };
	char tx_bwerslt_value[64] = { 0 };

	sprintf(cFromTypeFieldName, "%s", "fromtype");
	sprintf(cTermTypeFieldName, "%s", "termtype");
	sprintf(cTXInfoFieldName, "%s", "txinfo");
	sprintf(cRXInfoFieldName, "%s", "rxinfo"); 
	sprintf(cTXBWERsltFieldName, "%s", "txbweresult");
	
	sprintf(from_type_value, "%d", fromtype);
	sprintf(term_type_value, "%d", termtype);
	if (pParam->m_nLen2 > 0)
	{
		memcpy(tx_info_value, pParam->m_pData2, pParam->m_nLen2);
	}
	if (pParam->m_nLen3 > 0)
	{
		memcpy(rx_info_value, pParam->m_pData3, pParam->m_nLen3);
	}
	sprintf(tx_bwerslt_value, "%d", txbweresult);

	REDISKEY strtercalldetailconfidtersuidkey = tercalldetail_confid_tersuid_key;
	REDISVDATA vTerCallDetailInfoData;
	vTerCallDetailInfoData.clear();

	REDISKEY strFromTypeFieldName = cFromTypeFieldName;
	REDISKEY strTermTypeFieldName = cTermTypeFieldName;
	REDISKEY strTXInfoFieldName = cTXInfoFieldName;
	REDISKEY strRXInfoFieldName = cRXInfoFieldName;
	REDISKEY strTXBWERsltFieldName = cTXBWERsltFieldName;

	REDISVALUE strfromtypevalue = from_type_value;
	vTerCallDetailInfoData.push_back(strFromTypeFieldName);
	vTerCallDetailInfoData.push_back(strfromtypevalue);

	REDISVALUE strtermtypevalue = term_type_value;
	vTerCallDetailInfoData.push_back(strTermTypeFieldName);
	vTerCallDetailInfoData.push_back(strtermtypevalue);

	REDISVALUE strtxinfovalue = tx_info_value;
	vTerCallDetailInfoData.push_back(strTXInfoFieldName);
	vTerCallDetailInfoData.push_back(strtxinfovalue);

	REDISVALUE strrxinfovalue = rx_info_value;
	vTerCallDetailInfoData.push_back(strRXInfoFieldName);
	vTerCallDetailInfoData.push_back(strrxinfovalue);
	
	REDISVALUE strtxbwersltvalue = tx_bwerslt_value;
	vTerCallDetailInfoData.push_back(strTXBWERsltFieldName);
	vTerCallDetailInfoData.push_back(strtxbwersltvalue);

	m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(7);
	bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(strtercalldetailconfidtersuidkey, vTerCallDetailInfoData);

//#ifdef LINUX
//	gettimeofday(&end, NULL); // ��s level
//	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
//
//	sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_UpdateTerStatisticsInfo into db(7) ** (%s) **time_use** is: %lf us \n", tercalldetail_confid_tersuid_key, time_use);
//#endif 

	delete pParam;
	pParam = NULL;
}

void CDevMgr::AddUserConfDetail(int suid, unsigned long long confid, unsigned long long confreportid, unsigned long long userrptdetailid, char* alias, char* time, time_t lltime, int fromtype, int termtype)
{
	if (!m_pUpMsqThread)
		return;

	if (alias == NULL || time == NULL)
		return;

	typedef CBufferT<int, unsigned long long, unsigned long long, unsigned long long, CAsyncThread*, time_t, int, int> CParam;
	CParam* pParam;
	pParam = new CParam(alias, strlen(alias), time, strlen(time), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, suid, confid, confreportid, userrptdetailid, m_pUpMsqThread, lltime, fromtype, termtype);

	typedef void (CDevMgr::* ACTION)(void*);
    m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_AddUserConfDetail, (void*)pParam));
	return;
}

void CDevMgr::Handle_AddUserConfDetail(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<int, unsigned long long, unsigned long long, unsigned long long, CAsyncThread*, time_t, int, int> CParam;
	CParam* pParam = (CParam*)pArg;

	char* alias = (char*)pParam->m_pData1;
	char* ptime = (char*)pParam->m_pData2;

	int suid = pParam->m_Arg1;
	unsigned long long confid = pParam->m_Arg2;
	unsigned long long confreportid = pParam->m_Arg3;
	unsigned long long userrptdetailid = pParam->m_Arg4;
	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg5);
	time_t hand_add_time = (time_t)(pParam->m_Arg6);
	int fromtype = pParam->m_Arg7;
	int termtype = pParam->m_Arg8;

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL) // test delet by csh at 2016.10.21
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_AddUserConfDetail pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CUserConfDetail* pUserConfDetail = NULL;
	std::map<CAsyncThread*, CUserConfDetail*>::iterator iter_mapAsyncThreadUserConfDetial = m_mapAsyncThreadUserConfDetial.find(pUpMsgThread);
	if (iter_mapAsyncThreadUserConfDetial != m_mapAsyncThreadUserConfDetial.end())
	{
		pUserConfDetail = iter_mapAsyncThreadUserConfDetial->second;
	}


	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pUpMsgThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}
	
	if (pConfReport == NULL
		|| pUserConfDetail == NULL)
	{
		delete pParam;
		pParam = NULL;
		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_AddUserConfDetail pConfReport and pUserConfDetail is NULL.\n");
		return;
	}


	do
	{
		{
#ifdef LINUX
			float time_use = 0;
			struct timeval start;
			struct timeval end;
			gettimeofday(&start, NULL); // ��s level
#endif 

			bool bFindConfReportID = false;
			unsigned long long ullConfRptid = 0;

			char cConfidFieldName[128] = { 0 };
			char conf_id_value[128] = { 0 };
			char cConfReportidFieldName[128] = { 0 };
			char conf_report_id_value[128] = { 0 };
			char cUseRptDetailidFieldName[128] = { 0 };
			char use_rpt_detail_id_value[128] = { 0 };
			char cUseridextFieldName[128] = { 0 };
			char user_id_ext_value[128] = { 0 };
			char cUserAliasFieldName[128] = { 0 };
			char user_alias_value[128] = { 0 };
			char cRealJointimeFieldName[128] = { 0 };
			char real_jointime_value[128] = { 0 };
			char cFromtypeFieldName[32] = { 0 };
			char fromtype_value[32] = { 0 };
			char cTermtypeFieldName[32] = { 0 };
			char termtype_value[32] = { 0 };

			char userconfdetail_confid_suid_key[128] = { 0 };
			char confterlist_confid_key[128] = { 0 };

#ifdef WIN32
			sprintf(conf_id_value, "%I64d", confid);
			sprintf(use_rpt_detail_id_value, "%I64d", userrptdetailid);
#elif defined LINUX
			sprintf(conf_id_value, "%lld", confid);
			sprintf(use_rpt_detail_id_value, "%lld", userrptdetailid);
#endif
			sprintf(userconfdetail_confid_suid_key, "userconfdtl_%s_%d", conf_id_value, suid);
			sprintf(confterlist_confid_key, "confterlist_%s", conf_id_value);

			sprintf(cConfidFieldName, "%s", "conf_id");
			sprintf(cConfReportidFieldName, "%s", "conf_report_id");
			sprintf(cUseRptDetailidFieldName, "%s", "use_rpt_detaild_id");
			sprintf(cUseridextFieldName, "%s", "user_id_ext");
			sprintf(cUserAliasFieldName, "%s", "user_alias");
			sprintf(cRealJointimeFieldName, "%s", "real_jointime");
			sprintf(cFromtypeFieldName, "%s", "fromtype");
			sprintf(cTermtypeFieldName, "%s", "termtype");

			sprintf(user_id_ext_value, "%d", suid);
			sprintf(user_alias_value, "%s", alias);
			sprintf(real_jointime_value, "%s", ptime);
			sprintf(fromtype_value, "%d", fromtype);
			sprintf(termtype_value, "%d", termtype);

			m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);

			if (confreportid == 0)
			{
				char confstarttimerptid_confid_key[128] = { 0 };
				sprintf(confstarttimerptid_confid_key, "confstimerptidset_%s", conf_id_value);
				RedisReplyArray vRedisReplyArray;
				vRedisReplyArray.clear();

				bool bhashHGetAll_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHGetAll(confstarttimerptid_confid_key, vRedisReplyArray);

				unsigned int uiConfstrptinfosize = vRedisReplyArray.size();

				sr_printf(SR_LOGLEVEL_DEBUG, " m_pRedisConnList[Handle_AddUserConfDetail]->hashHGetAll==%s-->> %d, size = %d\n", confstarttimerptid_confid_key, bhashHGetAll_ok, uiConfstrptinfosize);

				if (bhashHGetAll_ok
					&& (uiConfstrptinfosize > 0)
					&& (uiConfstrptinfosize % 2 == 0))
				{
					for (unsigned int ii = 0; ii < uiConfstrptinfosize;)
					{
						sr_printf(SR_LOGLEVEL_DEBUG, "==Handle_AddUserConfDetail==In redis db(4) key:%s ==>>> find starttime=%s, rptid=%s\n", confstarttimerptid_confid_key, vRedisReplyArray[ii].str.c_str(), vRedisReplyArray[ii + 1].str.c_str());
						ii += 2;
					}

					// �ҵ�����ٿ������confreportid

#ifdef WIN32
					sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%I64d", &confreportid);
#elif defined LINUX
					sscanf(vRedisReplyArray[uiConfstrptinfosize - 1].str.c_str(), "%lld", &confreportid);
#endif

					bFindConfReportID = true;
				}

				if (bFindConfReportID == false) // ��redis��δ�ҵ�����mysql���ݿ�
				{
					pConfReport->SetConfID(confid);
					if (pConfReport->SelectDB())// 
					{
						confreportid = pConfReport->GetConfReportID();

#ifdef WIN32
						sr_printf(SR_LOGLEVEL_WARNING, "==ok== Handle_AddUserConfDetail == find confreportid from ConfReport table=== confid=%I64d, confreportid=%I64d\n", confid, confreportid);
#elif defined LINUX
						sr_printf(SR_LOGLEVEL_WARNING, "==ok== Handle_AddUserConfDetail == find confreportid from ConfReport table=== confid=%lld, confreportid=%lld\n", confid, confreportid);
#endif
					}
					else
					{
#ifdef WIN32
						sr_printf(SR_LOGLEVEL_ERROR, "==error== Handle_AddUserConfDetail == Not find confreportid from ConfReport table=== confid=%I64d\n", confid);
#elif defined LINUX
						sr_printf(SR_LOGLEVEL_ERROR, "==error== Handle_AddUserConfDetail == Not find confreportid from ConfReport table=== confid=%lld\n", confid);
#endif
					}

				}

				if (confreportid == 0)
				{

#ifdef WIN32
					sr_printf(SR_LOGLEVEL_ERROR, "==error== Handle_AddUserConfDetail == Not find confreportid in redisdb14(%s) and ConfReport table=== confid=%I64d\n", confstarttimerptid_confid_key, confid);
#elif defined LINUX
					sr_printf(SR_LOGLEVEL_ERROR, "==error== Handle_AddUserConfDetail == Not find confreportid in redisdb14(%s) and ConfReport table=== confid=%lld\n", confstarttimerptid_confid_key, confid);
#endif

					delete pParam;
					pParam = NULL;
					return;
				}
			}


#ifdef WIN32
			sprintf(conf_report_id_value, "%I64d", confreportid);
#elif defined LINUX
			sprintf(conf_report_id_value, "%lld", confreportid);
#endif


			REDISKEY struserconfdetailconfidsuidkey = userconfdetail_confid_suid_key;
			REDISVDATA vUserConfdtlConfidSuidData;
			vUserConfdtlConfidSuidData.clear();

			REDISKEY strConfidFieldName = cConfidFieldName;
			REDISVALUE strconfidvalue = conf_id_value;
			vUserConfdtlConfidSuidData.push_back(strConfidFieldName);
			vUserConfdtlConfidSuidData.push_back(strconfidvalue);

			REDISKEY strConfReportidFieldName = cConfReportidFieldName;
			REDISVALUE strconfreportidvalue = conf_report_id_value;
			vUserConfdtlConfidSuidData.push_back(strConfReportidFieldName);
			vUserConfdtlConfidSuidData.push_back(strconfreportidvalue);

			REDISKEY strUserRptDtlidFieldName = cUseRptDetailidFieldName;
			REDISVALUE struserrptdtlidvalue = use_rpt_detail_id_value;
			vUserConfdtlConfidSuidData.push_back(strUserRptDtlidFieldName);
			vUserConfdtlConfidSuidData.push_back(struserrptdtlidvalue);

			REDISKEY strUseridextFieldName = cUseridextFieldName;
			REDISVALUE struseridextvalue = user_id_ext_value;
			vUserConfdtlConfidSuidData.push_back(strUseridextFieldName);
			vUserConfdtlConfidSuidData.push_back(struseridextvalue);

			REDISKEY strUserAliasFieldName = cUserAliasFieldName;
			REDISVALUE struseraliasvalue = user_alias_value;
			vUserConfdtlConfidSuidData.push_back(strUserAliasFieldName);
			vUserConfdtlConfidSuidData.push_back(struseraliasvalue);

			REDISKEY strRealJointimeFieldName = cRealJointimeFieldName;
			REDISVALUE strrealjointimevalue = real_jointime_value;
			vUserConfdtlConfidSuidData.push_back(strRealJointimeFieldName);
			vUserConfdtlConfidSuidData.push_back(strrealjointimevalue);

			REDISKEY strFromtypeFieldName = cFromtypeFieldName;
			REDISVALUE strfromtypevalue = fromtype_value;
			vUserConfdtlConfidSuidData.push_back(strFromtypeFieldName);
			vUserConfdtlConfidSuidData.push_back(strfromtypevalue);

			REDISKEY strTermtypeFieldName = cTermtypeFieldName;
			REDISVALUE strtermtypevalue = termtype_value;
			vUserConfdtlConfidSuidData.push_back(strTermtypeFieldName);
			vUserConfdtlConfidSuidData.push_back(strtermtypevalue);

			//m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush(confterlist_confid_key, user_id_ext_value);
			m_pRedisConnList[e_RC_TT_UpMsqThread]->setvalue(confterlist_confid_key, use_rpt_detail_id_value, user_id_ext_value);

			bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(struserconfdetailconfidsuidkey, vUserConfdtlConfidSuidData);

			//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet==userconfdetail_confid_suid_key---->>>> %d\n", bhashHMSet_ok);

#ifdef LINUX
			gettimeofday(&end, NULL); // ��s level
			time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

			sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_AddUserConfDetail into db(4) ** (%s) **time_use** is: %lf us \n", userconfdetail_confid_suid_key, time_use);
#endif 
		}
	} while (0);



	delete pParam;
	pParam = NULL;
}

void CDevMgr::Handle_WriteUserConfInfoToDB(void* pArg)
{
	typedef CBufferT<CAsyncThread*, void*, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	CAsyncThread* pWriteDBThread = (CAsyncThread*)(pParam->m_Arg1);

	if (pWriteDBThread == NULL)
	{
		delete pParam;
		pParam = NULL;
		printf("== Handle_WriteUserConfInfoToDB == pWriteDBThread == NULL = return !!!!\n");
		return;
	}

	CConference* pConference = NULL;
	std::map<CAsyncThread*, CConference*>::iterator iter_mapAsyncThreadConference = m_mapAsyncThreadConference.find(pWriteDBThread);
	if (iter_mapAsyncThreadConference != m_mapAsyncThreadConference.end())
	{
		pConference = iter_mapAsyncThreadConference->second;
	}

	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pWriteDBThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}

	CDeviceConfDetail* pDeviceConfDetail = NULL;
	std::map<CAsyncThread*, CDeviceConfDetail*>::iterator iter_mapAsyncThreadDeviceConfDetail = m_mapAsyncThreadDeviceConfDetail.find(pWriteDBThread);
	if (iter_mapAsyncThreadDeviceConfDetail != m_mapAsyncThreadDeviceConfDetail.end())
	{
		pDeviceConfDetail = iter_mapAsyncThreadDeviceConfDetail->second;
	}

	CUserConfDetail* pUserConfDetail = NULL;
	std::map<CAsyncThread*, CUserConfDetail*>::iterator iter_mapAsyncThreadUserConfDetial = m_mapAsyncThreadUserConfDetial.find(pWriteDBThread);
	if (iter_mapAsyncThreadUserConfDetial != m_mapAsyncThreadUserConfDetial.end())
	{
		pUserConfDetail = iter_mapAsyncThreadUserConfDetial->second;
	}

	CConfRecord* pConfRecord = NULL;
	std::map<CAsyncThread*, CConfRecord*>::iterator iter_mapAsyncThreadConfRecord = m_mapAsyncThreadConfRecord.find(pWriteDBThread);
	if (iter_mapAsyncThreadConfRecord != m_mapAsyncThreadConfRecord.end())
	{
		pConfRecord = iter_mapAsyncThreadConfRecord->second;
	}

	CRecordFile* pRecordFile = NULL;
	std::map<CAsyncThread*, CRecordFile*>::iterator iter_mapAsyncThreadRecordFile = m_mapAsyncThreadRecordFile.find(pWriteDBThread);
	if (iter_mapAsyncThreadRecordFile != m_mapAsyncThreadRecordFile.end())
	{
		pRecordFile = iter_mapAsyncThreadRecordFile->second;
	}

	CConfLiveInfo* pConfLiveInfo = NULL;
	std::map<CAsyncThread*, CConfLiveInfo*>::iterator iter_mapAsyncThreadConfLiveInfo = m_mapAsyncThreadConfLiveInfo.find(pWriteDBThread);
	if (iter_mapAsyncThreadConfLiveInfo != m_mapAsyncThreadConfLiveInfo.end())
	{
		pConfLiveInfo = iter_mapAsyncThreadConfLiveInfo->second;
	}

	CConfRollCallInfo* pConfRollCallInfo = NULL;
	std::map<CAsyncThread*, CConfRollCallInfo*>::iterator iter_mapAsyncThreadConfRollCallInfo = m_mapAsyncThreadConfRollCallInfo.find(pWriteDBThread);
	if (iter_mapAsyncThreadConfRollCallInfo != m_mapAsyncThreadConfRollCallInfo.end())
	{
		pConfRollCallInfo = iter_mapAsyncThreadConfRollCallInfo->second;
	}

	CRollCallList* pRollCallList = NULL;
	std::map<CAsyncThread*, CRollCallList*>::iterator iter_mapAsyncThreadRollCallList = m_mapAsyncThreadRollCallList.find(pWriteDBThread);
	if (iter_mapAsyncThreadRollCallList != m_mapAsyncThreadRollCallList.end())
	{
		pRollCallList = iter_mapAsyncThreadRollCallList->second;
	}

	delete pParam;
	pParam = NULL;

	unsigned int uiOPUserConfnum = WriteUserConfInfoToDB(pConference, pConfReport, pDeviceConfDetail, pUserConfDetail, pConfRecord, pRecordFile, pConfLiveInfo, pConfRollCallInfo, pRollCallList);

}

unsigned int CDevMgr::WriteUserConfInfoToDB(CConference* pConference, CConfReport* pConfReport, CDeviceConfDetail* pDeviceConfDetail, CUserConfDetail* pUserConfDetail, CConfRecord* pConfRecord, CRecordFile* pRecordFile, CConfLiveInfo* pConfLiveInfo
	, CConfRollCallInfo* pConfRollCallInfo, CRollCallList* pRollCallList)
{
	unsigned int uiOPConfRptnum = 0;
	unsigned int uiOPDevConfDetialnum = 0;
	unsigned int uiOPUserConfnum = 0;
	
	if (pConference == NULL
		|| pConfReport == NULL
		|| pDeviceConfDetail == NULL
		|| pUserConfDetail == NULL
		|| pConfRecord == NULL
		|| pRecordFile == NULL
		|| pConfLiveInfo == NULL
		|| pConfRollCallInfo == NULL
		|| pRollCallList == NULL
		|| m_pRedisConnList[e_RC_TT_WriteDBThread] == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "== WriteUserConfInfoToDB == pUserConfDetail == NULL = return !!!!\n");
		return 0;
	}



	//sr_printf(SR_LOGLEVEL_DEBUG, " IN== WriteUserConfInfoToDB == Current m_sConfReport.size() = %d, m_sDeviceConfDetail.size() = %d, m_sUserConfInfo.size() = %d\n", m_sConfReport.size(), m_sDeviceConfDetail.size(), m_sUserConfInfo.size());

#ifdef LINUX
	float time_use = 0;
	struct timeval start;
	struct timeval end;
	gettimeofday(&start, NULL); // ��s level
#endif 

	m_pRedisConnList[e_RC_TT_WriteDBThread]->selectdb(4);// hash
	//m_pRedis->selectdb(15);// list

	bool bBreak = false;
	unsigned int uiOpRedisNum = 0;
	long long llListlen = 0;

	llListlen = m_pRedisConnList[e_RC_TT_WriteDBThread]->listLLen("op_key_list");

#ifdef WIN32
	sr_printf(SR_LOGLEVEL_NORMAL, " ####### WriteUserConfInfoToDB ==--->>llListlen = %I64d\n", llListlen);
#elif defined LINUX
	sr_printf(SR_LOGLEVEL_NORMAL, " ####### WriteUserConfInfoToDB ==--->>llListlen = %lld\n", llListlen);
#endif

	while (llListlen)
	{
		char* ptrOPkey = NULL;
		ptrOPkey = m_pRedisConnList[e_RC_TT_WriteDBThread]->listRPop("op_key_list");

#ifdef WIN32
		sr_printf(SR_LOGLEVEL_NORMAL, " ==>>> WriteUserConfInfoToDB ==llListlen = %I64d, listRPop(op_key_list) = %s\n", llListlen, ptrOPkey);
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_NORMAL, " ==>>> WriteUserConfInfoToDB ==llListlen = %lld, listRPop(op_key_list) = %s\n", llListlen, ptrOPkey);
#endif

		if (ptrOPkey != NULL)
		{
			if (strncmp("upconfrealstarttime", ptrOPkey, 19) == 0
				|| strncmp("upconfrealendtime", ptrOPkey, 17) == 0)
			{
				char cConfidFieldName[128] = { 0 };
				char conf_id_value[128] = { 0 };
				char cRealtimeFieldName[128] = { 0 };
				char real_time_value[128] = { 0 };
				char cPermanentEnableFieldName[32] = { 0 };
				char permanent_enable_value[32] = { 0 };

				sprintf(cConfidFieldName, "%s", "conf_id");

				if (strncmp("upconfrealstarttime", ptrOPkey, 19) == 0)
				{
					sprintf(cRealtimeFieldName, "%s", "real_starttime");
				} 
				else if (strncmp("upconfrealendtime", ptrOPkey, 17) == 0)
				{
					sprintf(cRealtimeFieldName, "%s", "real_endtime");
				}

				sprintf(cPermanentEnableFieldName, "%s", "permanent_enable");

				//////////////////////////////////////////////////////////////////////////
				REDISKEY strconfrealtimekey = ptrOPkey;
				REDISFILEDS vGetFileds;
				vGetFileds.clear();
				RedisReplyArray vRedisReplyArray;
				vRedisReplyArray.clear();

				// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
				REDISKEY strConfidFieldName = cConfidFieldName;
				REDISKEY strRealtimeFieldName = cRealtimeFieldName;
				REDISKEY strPermanentEnableFieldName = cPermanentEnableFieldName;
				vGetFileds.push_back(strConfidFieldName);
				vGetFileds.push_back(strRealtimeFieldName);
				vGetFileds.push_back(strPermanentEnableFieldName);

				bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet(strconfrealtimekey, vGetFileds, vRedisReplyArray);

				//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet==confreport_confid_reportid_key--->> bhashHMGet_ok:%d, vRedisReplyArray.size:%d\n", bhashHMGet_ok, vRedisReplyArray.size());

				if (bhashHMGet_ok
					&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
				{
					sprintf(conf_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(real_time_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(permanent_enable_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��

					unsigned long long ullConfid = 0;
					unsigned long long ullConfreportid = 0;
					int iPermanentenable = 0;

#ifdef WIN32
					sscanf(conf_id_value, "%I64d", &ullConfid);
#elif defined LINUX
					sscanf(conf_id_value, "%lld", &ullConfid);
#endif

					sscanf(permanent_enable_value, "%d", &iPermanentenable);

					pConference->SetConfID(ullConfid);
					//pConference->SetPermanentEnable(iPermanentenable);

					if (strncmp("upconfrealstarttime", ptrOPkey, 19) == 0)
					{
						pConference->SetConfRealStartTime(real_time_value);
						pConference->SetConfStatus(2);// 0��δ��ʼ��1���ѽ�����2��������
						bool bUpdateOK = false;
						bUpdateOK = pConference->UpdateDB(0); // update conference set conf_realstarttime
						if (bUpdateOK)// �����ɾ�����ܻᵼ��redis��������
						{
							m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);
						}
					}
					else if (strncmp("upconfrealendtime", ptrOPkey, 17) == 0)
					{
						pConference->SetConfRealEndTime(real_time_value);
						if (iPermanentenable == 0)// һ���Ի���
						{
							pConference->SetConfStatus(1);// 0��δ��ʼ��1���ѽ�����2��������
						}
						else // �����Ի���
						{
							pConference->SetConfStatus(0);// 0��δ��ʼ��1���ѽ�����2��������
						}
						bool bUpdateOK = false;
						bUpdateOK = pConference->UpdateDB(1); // update conference set conf_realendtime
						if (bUpdateOK)// �����ɾ�����ܻᵼ��redis��������
						{
							m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);
						}
					}
				}

			}
			else if (strncmp("insconfrptinfo", ptrOPkey, 14) == 0
				|| strncmp("upconfreport", ptrOPkey, 12) == 0)// �ӳ�д��mysql�����н���ʱ��ʱһ��д��
			{
				char cConfidFieldName[128] = { 0 };
				char conf_id_value[128] = { 0 };
				char cConfReportidFieldName[128] = { 0 };
				char conf_report_id_value[128] = { 0 };
				char cRealStarttimeFieldName[128] = { 0 };
				char real_starttime_value[128] = { 0 };
				char cRealEndtimeFieldName[128] = { 0 };
				char real_endtime_value[128] = { 0 };

				sprintf(cConfidFieldName, "%s", "conf_id");
				sprintf(cConfReportidFieldName, "%s", "conf_report_id");
				sprintf(cRealStarttimeFieldName, "%s", "real_starttime");
				sprintf(cRealEndtimeFieldName, "%s", "real_endtime");

				//////////////////////////////////////////////////////////////////////////
				//REDISKEY strconfreportidkey = confreport_confid_reportid_key;
				REDISKEY strconfreportidkey = ptrOPkey;
				REDISFILEDS vGetFileds;
				vGetFileds.clear();
				RedisReplyArray vRedisReplyArray;
				vRedisReplyArray.clear();

				// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
				REDISKEY strConfidFieldName = cConfidFieldName;
				REDISKEY strConfReportidFieldName = cConfReportidFieldName;
				REDISKEY strRealStarttimeFieldName = cRealStarttimeFieldName;
				REDISKEY strRealEndtimeFieldName = cRealEndtimeFieldName;
				vGetFileds.push_back(strConfidFieldName);
				vGetFileds.push_back(strConfReportidFieldName);
				vGetFileds.push_back(strRealStarttimeFieldName);
				vGetFileds.push_back(strRealEndtimeFieldName);

				bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet(strconfreportidkey, vGetFileds, vRedisReplyArray);

				//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet==confreport_confid_reportid_key--->> bhashHMGet_ok:%d, vRedisReplyArray.size:%d\n", bhashHMGet_ok, vRedisReplyArray.size());

				if (bhashHMGet_ok
					&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
				{
					sprintf(conf_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(conf_report_id_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(real_starttime_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(real_endtime_value, "%s", vRedisReplyArray[3].str.c_str()); // ��Ҫ��push_back����˳��һ��

					unsigned long long ullConfid = 0;
					unsigned long long ullConfreportid = 0;

#ifdef WIN32
					sscanf(conf_id_value, "%I64d", &ullConfid);
					sscanf(conf_report_id_value, "%I64d", &ullConfreportid);
#elif defined LINUX
					sscanf(conf_id_value, "%lld", &ullConfid);
					sscanf(conf_report_id_value, "%lld", &ullConfreportid);
#endif

					pConfReport->SetConfID(ullConfid);
					pConfReport->SetConfReportID(ullConfreportid);
					pConfReport->SetStartTime(real_starttime_value);
					pConfReport->SetStopTime(real_endtime_value);

					if (strncmp("insconfrptinfo", ptrOPkey, 14) == 0)
					{
						bool bInsertOK = false;
						bInsertOK = pConfReport->InsertConfReportRecordToDB();
						if (bInsertOK)// �����ɾ�����ܻᵼ��redis��������
						{
							m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);
						}
					}
					else if (strncmp("upconfreport", ptrOPkey, 12) == 0)
					{
						bool bUpdateOK = false;
						char confstarttimerptid_confid_key[128] = { 0 };
						sprintf(confstarttimerptid_confid_key, "confstimerptidset_%s", conf_id_value);
						bUpdateOK = pConfReport->UpdateConfRealEndtimeForDB();
						if (bUpdateOK)// �����ɾ�����ܻᵼ��redis��������
						{
							m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);

							m_pRedisConnList[e_RC_TT_WriteDBThread]->deletehashvalue(confstarttimerptid_confid_key, real_starttime_value);
						}
					}
				}

			}
			else if (strncmp("inscrsrecordid", ptrOPkey, 14) == 0
				|| strncmp("upconfrecordid", ptrOPkey, 14) == 0)// �ӳ�д��mysql�����н���ʱ��ʱһ��д��
			{
				char cConfidFieldName[128] = { 0 };
				char conf_id_value[128] = { 0 };
				char cConfRecordidFieldName[128] = { 0 };
				char conf_record_id_value[128] = { 0 };
				char cConfReportidFieldName[128] = { 0 };
				char conf_report_id_value[128] = { 0 };
				char cCRSRecStarttimeFieldName[128] = { 0 };
				char crsrec_starttime_value[128] = { 0 };
				char cCRSRecEndtimeFieldName[128] = { 0 };
				char crsrec_endtime_value[128] = { 0 };
				char cConfnameFieldName[256] = { 0 };
				char conf_name_value[256] = { 0 };

				sprintf(cConfidFieldName, "%s", "conf_id");
				sprintf(cConfRecordidFieldName, "%s", "conf_record_id");
				sprintf(cConfReportidFieldName, "%s", "conf_report_id");
				sprintf(cCRSRecStarttimeFieldName, "%s", "starttime");
				sprintf(cCRSRecEndtimeFieldName, "%s", "endtime");
				sprintf(cConfnameFieldName, "%s", "confname");

				//////////////////////////////////////////////////////////////////////////
				//REDISKEY strconfreportidkey = confreport_confid_reportid_key;
				REDISKEY strconfrecordidkey = ptrOPkey;
				REDISFILEDS vGetFileds;
				vGetFileds.clear();
				RedisReplyArray vRedisReplyArray;
				vRedisReplyArray.clear();

				// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
				REDISKEY strConfidFieldName = cConfidFieldName;
				REDISKEY strConfRecordidFieldName = cConfRecordidFieldName;
				REDISKEY strConfReportidFieldName = cConfReportidFieldName;
				REDISKEY strCRSRecStarttimeFieldName = cCRSRecStarttimeFieldName;
				REDISKEY strCRSRecEndtimeFieldName = cCRSRecEndtimeFieldName;
				REDISKEY strConfnameFieldName = cConfnameFieldName;
				vGetFileds.push_back(strConfidFieldName);
				vGetFileds.push_back(strConfRecordidFieldName);
				vGetFileds.push_back(strConfReportidFieldName);
				vGetFileds.push_back(strCRSRecStarttimeFieldName);
				vGetFileds.push_back(strCRSRecEndtimeFieldName);
				vGetFileds.push_back(strConfnameFieldName);

				bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet(strconfrecordidkey, vGetFileds, vRedisReplyArray);

				//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet==confreport_confid_reportid_key--->> bhashHMGet_ok:%d, vRedisReplyArray.size:%d\n", bhashHMGet_ok, vRedisReplyArray.size());

				if (bhashHMGet_ok
					&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
				{
					sprintf(conf_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(conf_record_id_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(conf_report_id_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(crsrec_starttime_value, "%s", vRedisReplyArray[3].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(crsrec_endtime_value, "%s", vRedisReplyArray[4].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(conf_name_value, "%s", vRedisReplyArray[5].str.c_str()); // ��Ҫ��push_back����˳��һ��

					unsigned long long ullConfid = 0;
					unsigned long long ullConfRecordid = 0;
					unsigned long long ullConfreportid = 0;

#ifdef WIN32
					sscanf(conf_id_value, "%I64d", &ullConfid);
					sscanf(conf_record_id_value, "%I64d", &ullConfRecordid);
					sscanf(conf_report_id_value, "%I64d", &ullConfreportid);
#elif defined LINUX
					sscanf(conf_id_value, "%lld", &ullConfid);
					sscanf(conf_record_id_value, "%lld", &ullConfRecordid);
					sscanf(conf_report_id_value, "%lld", &ullConfreportid);
#endif

					pConfRecord->SetConfRecordID(ullConfRecordid);
					pConfRecord->SetConfReportID(ullConfreportid);
					pConfRecord->SetStartTime(crsrec_starttime_value);
					pConfRecord->SetStopTime(crsrec_endtime_value);
					pConfRecord->SetConfName(conf_name_value);

					if (strncmp("inscrsrecordid", ptrOPkey, 14) == 0)
					{
						bool bInsertOK = false;
						bInsertOK = pConfRecord->InsertConfCRSRecordToDB();
						if (bInsertOK)// �����ɾ�����ܻᵼ��redis��������
						{
							m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);

							//bool bUpdateOK = false;
							pConfReport->SetConfID(ullConfid);
							pConfReport->SetConfReportID(ullConfreportid);
							pConfReport->SetHasRecord(1);
							pConfReport->UpdateConfHasRecordForDB();
							//bUpdateOK = pConfReport->UpdateConfRealEndtimeForDB();
							//if (bUpdateOK)// �����ɾ�����ܻᵼ��redis��������
							//{
							//}
						}
					}
					else if (strncmp("upconfrecordid", ptrOPkey, 14) == 0)
					{
						bool bUpdateOK = false;
						char crsconfrecordidset_confid_key[128] = { 0 };
						sprintf(crsconfrecordidset_confid_key, "crsconfrecordidset_%s", conf_id_value);
						bUpdateOK = pConfRecord->UpdateConfCRSRealEndtimeForDB();
						if (bUpdateOK)// �����ɾ�����ܻᵼ��redis��������
						{
							m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);
							m_pRedisConnList[e_RC_TT_WriteDBThread]->deletehashvalue(crsconfrecordidset_confid_key, crsrec_starttime_value);
						}
					}
				}

			}
			else if (strncmp("devconfdtl", ptrOPkey, 10) == 0)
			{
				char cConfidFieldName[128] = { 0 };
				char conf_id_value[128] = { 0 };
				char cConfReportidFieldName[128] = { 0 };
				char conf_report_id_value[128] = { 0 };
				char cDeviceidFieldName[128] = { 0 };
				char device_id_value[128] = { 0 };
				char cDeviceDetailidFieldName[128] = { 0 };
				char device_detail_id_value[128] = { 0 };
				char cRealuseStarttimeFieldName[128] = { 0 };
				char realuse_starttime_value[128] = { 0 };
				char cRealuseStoptimeFieldName[128] = { 0 };
				char realuse_stoptime_value[128] = { 0 };


				sprintf(cConfidFieldName, "%s", "conf_id");
				sprintf(cConfReportidFieldName, "%s", "conf_report_id");
				sprintf(cDeviceidFieldName, "%s", "device_id");
				sprintf(cDeviceDetailidFieldName, "%s", "device_detail_id");
				sprintf(cRealuseStarttimeFieldName, "%s", "realuse_starttime");
				sprintf(cRealuseStoptimeFieldName, "%s", "realuse_stoptime");

				//////////////////////////////////////////////////////////////////////////
				//REDISKEY strdevconfdetailconfiddevidrptidkey = devconfdetail_confid_devid_rptid_key;
				REDISKEY strdevconfdetailconfiddevidrptidkey = ptrOPkey;
				REDISFILEDS vGetFileds;
				vGetFileds.clear();
				RedisReplyArray vRedisReplyArray;
				vRedisReplyArray.clear();

				// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
				REDISKEY strConfidFieldName = cConfidFieldName;
				REDISKEY strConfReportidFieldName = cConfReportidFieldName;
				REDISKEY strDeviceidFieldName = cDeviceidFieldName;
				REDISKEY strDeviceDetailidFieldName = cDeviceDetailidFieldName;
				REDISKEY strRealuseStarttimeFieldName = cRealuseStarttimeFieldName;
				REDISKEY strRealuseStoptimeFieldName = cRealuseStoptimeFieldName;
				vGetFileds.push_back(strConfidFieldName);
				vGetFileds.push_back(strConfReportidFieldName);
				vGetFileds.push_back(strDeviceidFieldName);
				vGetFileds.push_back(strDeviceDetailidFieldName);
				vGetFileds.push_back(strRealuseStarttimeFieldName);
				vGetFileds.push_back(strRealuseStoptimeFieldName);

				bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet(strdevconfdetailconfiddevidrptidkey, vGetFileds, vRedisReplyArray);

				//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet==devconfdetail_confid_devid_rptid_key--->> bhashHMGet_ok:%d, vRedisReplyArray.size:%d\n", bhashHMGet_ok, vRedisReplyArray.size());

				if (bhashHMGet_ok
					&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
				{
					sprintf(conf_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(conf_report_id_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(device_id_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(device_detail_id_value, "%s", vRedisReplyArray[3].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(realuse_starttime_value, "%s", vRedisReplyArray[4].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(realuse_stoptime_value, "%s", vRedisReplyArray[5].str.c_str()); // ��Ҫ��push_back����˳��һ��

					unsigned long long ullConfid = 0;
					unsigned long long ullConfreportid = 0;
					int iDeviceid = 0;
					unsigned long long ullDeviceDtlid = 0;

#ifdef WIN32
					sscanf(conf_id_value, "%I64d", &ullConfid);
					sscanf(conf_report_id_value, "%I64d", &ullConfreportid);
					sscanf(device_detail_id_value, "%I64d", &ullDeviceDtlid);
#elif defined LINUX
					sscanf(conf_id_value, "%lld", &ullConfid);
					sscanf(conf_report_id_value, "%lld", &ullConfreportid);
					sscanf(device_detail_id_value, "%lld", &ullDeviceDtlid);
#endif

					sscanf(device_id_value, "%d", &iDeviceid);

					pDeviceConfDetail->SetConfID(ullConfid);
					pDeviceConfDetail->SetConfReportID(ullConfreportid);
					pDeviceConfDetail->SetDeviceID(iDeviceid);
					pDeviceConfDetail->SetDeviceDetailID(ullDeviceDtlid);
					pDeviceConfDetail->SetRealuseStarttime(realuse_starttime_value);
					pDeviceConfDetail->SetRealuseStoptime(realuse_stoptime_value);

					bool bInsertOK = false;
					bInsertOK = pDeviceConfDetail->InsertDeviceConfDetailToDB();

					if (bInsertOK)// �����ɾ�����ܻᵼ��redis��������
					{
						m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);
					}
				}
			}
			else if (strncmp("userconfdtl", ptrOPkey, 11) == 0)
			{
				char cConfidFieldName[128] = { 0 };
				char conf_id_value[128] = { 0 };
				char cConfReportidFieldName[128] = { 0 };
				char conf_report_id_value[128] = { 0 };
				char cUseRptDetailidFieldName[128] = { 0 };
				char use_rpt_detail_id_value[128] = { 0 };
				char cUseridextFieldName[128] = { 0 };
				char user_id_ext_value[128] = { 0 };
				char cUserAliasFieldName[128] = { 0 };
				char user_alias_value[128] = { 0 };
				char cRealJointimeFieldName[128] = { 0 };
				char real_jointime_value[128] = { 0 };
				char cRealLeavetimeFieldName[128] = { 0 };
				char real_leavetime_value[128] = { 0 };
				char cFromtypeFieldName[32] = { 0 };
				char fromtype_value[32] = { 0 };
				char cTermtypeFieldName[32] = { 0 };
				char termtype_value[32] = { 0 };

				sprintf(cConfidFieldName, "%s", "conf_id");
				sprintf(cConfReportidFieldName, "%s", "conf_report_id");
				sprintf(cUseRptDetailidFieldName, "%s", "use_rpt_detaild_id");
				sprintf(cUseridextFieldName, "%s", "user_id_ext");
				sprintf(cUserAliasFieldName, "%s", "user_alias");
				sprintf(cRealJointimeFieldName, "%s", "real_jointime");
				sprintf(cRealLeavetimeFieldName, "%s", "real_leavetime");
				sprintf(cFromtypeFieldName, "%s", "fromtype");
				sprintf(cTermtypeFieldName, "%s", "termtype");

				//////////////////////////////////////////////////////////////////////////
				//REDISKEY struserconfdetailconfidsuidrptidkey = userconfdetail_confid_suid_rptid_key;
				REDISKEY struserconfdetailconfidsuidrptidkey = ptrOPkey;
				REDISFILEDS vGetFileds;
				vGetFileds.clear();
				RedisReplyArray vRedisReplyArray;
				vRedisReplyArray.clear();

				// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
				REDISKEY strConfidFieldName = cConfidFieldName;
				REDISKEY strConfReportidFieldName = cConfReportidFieldName;
				REDISKEY strUseRptDetailidFieldName = cUseRptDetailidFieldName;
				REDISKEY strUseridextFieldName = cUseridextFieldName;
				REDISKEY strUserAliasFieldName = cUserAliasFieldName;
				REDISKEY strRealJointimeFieldName = cRealJointimeFieldName;
				REDISKEY strRealLeavetimeFieldName = cRealLeavetimeFieldName;
				REDISKEY strFromtypeFieldName = cFromtypeFieldName;
				REDISKEY strTermtypeFieldName = cTermtypeFieldName;

				vGetFileds.push_back(strConfidFieldName);
				vGetFileds.push_back(strConfReportidFieldName);
				vGetFileds.push_back(strUseRptDetailidFieldName);
				vGetFileds.push_back(strUseridextFieldName);
				vGetFileds.push_back(strUserAliasFieldName);
				vGetFileds.push_back(strRealJointimeFieldName);
				vGetFileds.push_back(strRealLeavetimeFieldName);
				vGetFileds.push_back(strFromtypeFieldName);
				vGetFileds.push_back(strTermtypeFieldName);

				bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet(struserconfdetailconfidsuidrptidkey, vGetFileds, vRedisReplyArray);

				//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet==userconfdetail_confid_suid_rptid_key--->> bhashHMGet_ok:%d, vRedisReplyArray.size:%d\n", bhashHMGet_ok, vRedisReplyArray.size());

				if (bhashHMGet_ok
					&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
				{
					sprintf(conf_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(conf_report_id_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(use_rpt_detail_id_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(user_id_ext_value, "%s", vRedisReplyArray[3].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(user_alias_value, "%s", vRedisReplyArray[4].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(real_jointime_value, "%s", vRedisReplyArray[5].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(real_leavetime_value, "%s", vRedisReplyArray[6].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(fromtype_value, "%s", vRedisReplyArray[7].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(termtype_value, "%s", vRedisReplyArray[8].str.c_str()); // ��Ҫ��push_back����˳��һ��

					unsigned long long ullConfid = 0;
					unsigned long long ullConfreportid = 0;
					unsigned long long ullUserRptDetailid = 0;
					int iUserid = 0;
					int iFromtype = 0;
					int iTermtype = 0;

#ifdef WIN32
					sscanf(conf_id_value, "%I64d", &ullConfid);
					sscanf(conf_report_id_value, "%I64d", &ullConfreportid);
					sscanf(use_rpt_detail_id_value, "%I64d", &ullUserRptDetailid);
#elif defined LINUX
					sscanf(conf_id_value, "%lld", &ullConfid);
					sscanf(conf_report_id_value, "%lld", &ullConfreportid);
					sscanf(use_rpt_detail_id_value, "%lld", &ullUserRptDetailid);
#endif

					sscanf(user_id_ext_value, "%d", &iUserid);
					sscanf(fromtype_value, "%d", &iFromtype);
					sscanf(termtype_value, "%d", &iTermtype);

					pUserConfDetail->SetConfID(ullConfid);
					pUserConfDetail->SetConfReportID(ullConfreportid);
					pUserConfDetail->SetUserRptDetailID(ullUserRptDetailid);
					pUserConfDetail->SetUserID(iUserid);
					pUserConfDetail->SetUserAlias(user_alias_value);
					pUserConfDetail->SetRealjointime(real_jointime_value);
					pUserConfDetail->SetRealleavetime(real_leavetime_value);
					pUserConfDetail->SetFromtype(iFromtype);
					pUserConfDetail->SetTermtype(iTermtype);

					bool bInsertOK = false;
					bInsertOK = pUserConfDetail->InsertUserConfDetailToDB();

					if (bInsertOK)// �����ɾ�����ܻᵼ��redis��������
					{
						m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);
					}
				}
			}
			else if (strncmp("inscrsrecfile", ptrOPkey, 13) == 0)
			{
				char cConfidFieldName[128] = { 0 };
				char conf_id_value[128] = { 0 };
				char cRecordFileidFieldName[128] = { 0 };
				char record_file_id_value[128] = { 0 };
				char cConfRecordidFieldName[128] = { 0 };
				char conf_record_id_value[128] = { 0 };
				char cCRSRecFilepathFieldName[128] = { 0 };
				char crsrec_filepath_value[300] = { 0 };
				char cCRSRecSdepathFieldName[128] = { 0 };
				char crsrec_sdepath_value[300] = { 0 };
				char cCRSRecFileStorSvrIPFieldName[128] = { 0 };
				char crsrec_filestorsvrip_value[128] = { 0 };
				char cCRSRecFilerootpathFieldName[128] = { 0 };
				char crsrec_filerootpath_value[300] = { 0 };
				char cRecordFileSizeFieldName[128] = { 0 };
				char record_file_size_value[128] = { 0 };

				sprintf(cConfidFieldName, "%s", "conf_id");
				sprintf(cRecordFileidFieldName, "%s", "record_file_id");
				sprintf(cConfRecordidFieldName, "%s", "conf_record_id");
				sprintf(cCRSRecFilepathFieldName, "%s", "filepath");
				sprintf(cCRSRecSdepathFieldName, "%s", "sdepath");
				sprintf(cCRSRecFileStorSvrIPFieldName, "%s", "serverip");
				sprintf(cCRSRecFilerootpathFieldName, "%s", "relativepath");
				sprintf(cRecordFileSizeFieldName, "%s", "filesize");

				//////////////////////////////////////////////////////////////////////////
				//REDISKEY strInsertCRSRecfileConfidConfrecidRecfileidkey = Insertcrsrecfile_confid_confrecid_recfileid_key;
				REDISKEY strInsertCRSRecfileConfidConfrecidRecfileidkey = ptrOPkey;
				REDISFILEDS vGetFileds;
				vGetFileds.clear();
				RedisReplyArray vRedisReplyArray;
				vRedisReplyArray.clear();

				// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
				REDISKEY strConfidFieldName = cConfidFieldName;
				REDISKEY strRecordFileidFieldName = cRecordFileidFieldName;
				REDISKEY strConfRecordidFieldName = cConfRecordidFieldName;
				REDISKEY strCRSRecFilepathFieldName = cCRSRecFilepathFieldName;
				REDISKEY strCRSRecSdepathFieldName = cCRSRecSdepathFieldName;
				REDISKEY strCRSRecFileStorSvrIPFieldName = cCRSRecFileStorSvrIPFieldName;
				REDISKEY strCRSRecFilerootpathFieldName = cCRSRecFilerootpathFieldName;
				REDISKEY strRecordFileSizeFieldName = cRecordFileSizeFieldName;
				vGetFileds.push_back(strConfidFieldName);
				vGetFileds.push_back(strRecordFileidFieldName);
				vGetFileds.push_back(strConfRecordidFieldName);
				vGetFileds.push_back(strCRSRecFilepathFieldName);
				vGetFileds.push_back(strCRSRecSdepathFieldName);
				vGetFileds.push_back(strCRSRecFileStorSvrIPFieldName);
				vGetFileds.push_back(strCRSRecFilerootpathFieldName);
				vGetFileds.push_back(strRecordFileSizeFieldName);

				bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet(strInsertCRSRecfileConfidConfrecidRecfileidkey, vGetFileds, vRedisReplyArray);

				//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet==userconfdetail_confid_suid_rptid_key--->> bhashHMGet_ok:%d, vRedisReplyArray.size:%d\n", bhashHMGet_ok, vRedisReplyArray.size());

				if (bhashHMGet_ok
					&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
				{
					sprintf(conf_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(record_file_id_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(conf_record_id_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��

					//sprintf(crsrec_filepath_value, "%s", vRedisReplyArray[3].str.c_str()); // ��Ҫ��push_back����˳��һ��
					//sprintf(crsrec_sdepath_value, "%s", vRedisReplyArray[4].str.c_str()); // ��Ҫ��push_back����˳��һ��
					//sprintf(crsrec_filestorsvrip_value, "%s", vRedisReplyArray[5].str.c_str()); // ��Ҫ��push_back����˳��һ��

					// url·�����ܸ�ʽ��,
					memcpy(crsrec_filepath_value, vRedisReplyArray[3].str.c_str(), vRedisReplyArray[3].str.length()); // ��Ҫ��push_back����˳��һ��
					memcpy(crsrec_sdepath_value, vRedisReplyArray[4].str.c_str(), vRedisReplyArray[4].str.length()); // ��Ҫ��push_back����˳��һ��
					memcpy(crsrec_filestorsvrip_value, vRedisReplyArray[5].str.c_str(), vRedisReplyArray[5].str.length()); // ��Ҫ��push_back����˳��һ��
					memcpy(crsrec_filerootpath_value, vRedisReplyArray[6].str.c_str(), vRedisReplyArray[6].str.length()); // ��Ҫ��push_back����˳��һ��
					memcpy(record_file_size_value, vRedisReplyArray[7].str.c_str(), vRedisReplyArray[7].str.length()); // ��Ҫ��push_back����˳��һ��

					unsigned long long ullConfid = 0;
					unsigned long long ullRecordFileid = 0;
					unsigned long long ullConfRecordid = 0;
					int iUserid = 0;
					unsigned long long ullRecordFileSize = 0;

#ifdef WIN32
					sscanf(conf_id_value, "%I64d", &ullConfid);
					sscanf(record_file_id_value, "%I64d", &ullRecordFileid);
					sscanf(conf_record_id_value, "%I64d", &ullConfRecordid);
					sscanf(record_file_size_value, "%I64d", &ullRecordFileSize);
#elif defined LINUX
					sscanf(conf_id_value, "%lld", &ullConfid);
					sscanf(record_file_id_value, "%lld", &ullRecordFileid);
					sscanf(conf_record_id_value, "%lld", &ullConfRecordid);
					sscanf(record_file_size_value, "%lld", &ullRecordFileSize);
#endif
					
					//pRecordFile->SetConfID(ullConfid);
					pRecordFile->SetRecordFileID(ullRecordFileid);
					pRecordFile->SetConfRecordID(ullConfRecordid);
					pRecordFile->SetFilePath(crsrec_filepath_value, vRedisReplyArray[3].str.length());
					pRecordFile->SetSdeFilePath(crsrec_sdepath_value, vRedisReplyArray[4].str.length());
					pRecordFile->SetFileStorSvrIP(crsrec_filestorsvrip_value, vRedisReplyArray[5].str.length());
					pRecordFile->SetFileRootPath(crsrec_filerootpath_value, vRedisReplyArray[6].str.length());
					pRecordFile->SetRecordFileSize(ullRecordFileSize);

					bool bInsertOK = false;
					bInsertOK = pRecordFile->InsertRecordFileInfoToDB();

					if (bInsertOK)// �����ɾ�����ܻᵼ��redis��������
					{
						m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);
					}
				}
			}
			else if (strncmp("insliveinfo", ptrOPkey, 11) == 0)
			{
				char cLiveinfoidFieldName[128] = { 0 };
				char liveinfo_id_value[128] = { 0 };
				char cConfidFieldName[128] = { 0 };
				char conf_id_value[128] = { 0 };
				char cConfReportidFieldName[128] = { 0 };
				char conf_report_id_value[128] = { 0 };

				char cLiveSubjectFieldName[128] = { 0 };
				char live_subject_value[255] = { 0 };

				char cLiveStarttimeFieldName[128] = { 0 };
				char live_starttime_value[128] = { 0 };
				char cLiveEndtimeFieldName[128] = { 0 };
				char live_endtime_value[128] = { 0 };

				char cLiveChairmanFieldName[128] = { 0 };
				char live_chairman_value[255] = { 0 };
				char cLiveAbstractFieldName[128] = { 0 };
				char live_abstract_value[255] = { 0 };
				char cLiveisPublicFieldName[128] = { 0 };
				char live_ispublic_value[128] = { 0 };
				char cLivePWDFieldName[128] = { 0 };
				char live_pwd_value[128] = { 0 };
				char cLiveisUseRecFieldName[128] = { 0 };
				char live_isuserec_value[128] = { 0 };

				char cLiveAddrFieldName[128] = { 0 };
				char live_addr_value[1024] = { 0 };// ��ӦIndCRSStartLive��Ϣ��livepullurl�ֶ�
				char cLiveWatchtimesFieldName[128] = { 0 };
				char live_watch_times_value[128] = { 0 };
				char cLiveLikestimesFieldName[128] = { 0 };
				char live_likes_times_value[128] = { 0 };
				char cLiveWatchaddrFieldName[128] = { 0 };
				char live_watch_addr_value[255] = { 0 };
				char cLiveAndroidtimesFieldName[128] = { 0 };
				char live_android_times_value[128] = { 0 };
				char cLiveIOStimesFieldName[128] = { 0 };
				char live_ios_times_value[128] = { 0 };
				char cLivePCtimesFieldName[128] = { 0 };
				char live_pc_times_value[128] = { 0 };
				char cLiveMobiletimesFieldName[128] = { 0 };
				char live_mobile_times_value[128] = { 0 };

				char cLiveConvertStatusFieldName[128] = { 0 };
				char live_convert_status_value[128] = { 0 };
				char cLiveTaskIdFieldName[128] = { 0 };
				char live_task_id_value[128] = { 0 };


				sprintf(cLiveinfoidFieldName, "%s", "li_id");
				sprintf(cConfidFieldName, "%s", "conf_id");
				sprintf(cConfReportidFieldName, "%s", "conf_report_id");

				sprintf(cLiveSubjectFieldName, "%s", "li_subject");

				sprintf(cLiveStarttimeFieldName, "%s", "starttime");
				sprintf(cLiveEndtimeFieldName, "%s", "endtime");

				sprintf(cLiveChairmanFieldName, "%s", "chairman");
				sprintf(cLiveAbstractFieldName, "%s", "li_abstract");
				sprintf(cLiveisPublicFieldName, "%s", "is_public");
				sprintf(cLivePWDFieldName, "%s", "live_pwd");
				sprintf(cLiveisUseRecFieldName, "%s", "is_userec");

				sprintf(cLiveAddrFieldName, "%s", "live_addr");
				sprintf(cLiveWatchtimesFieldName, "%s", "watch_times");
				sprintf(cLiveLikestimesFieldName, "%s", "likes_times");
				sprintf(cLiveWatchaddrFieldName, "%s", "watch_addr");
				sprintf(cLiveAndroidtimesFieldName, "%s", "android");
				sprintf(cLiveIOStimesFieldName, "%s", "ios_times");
				sprintf(cLivePCtimesFieldName, "%s", "pc_times");
				sprintf(cLiveMobiletimesFieldName, "%s", "mobile_times");

				sprintf(cLiveConvertStatusFieldName, "%s", "convert_status");
				sprintf(cLiveTaskIdFieldName, "%s", "task_id");

				REDISKEY strinsertliveinfoconfidliveinfoidkey = ptrOPkey;
				REDISFILEDS vGetFileds;
				vGetFileds.clear();
				RedisReplyArray vRedisReplyArray;
				vRedisReplyArray.clear();
				// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
				REDISKEY strLiveinfoidFieldName = cLiveinfoidFieldName;
				REDISKEY strConfidFieldName = cConfidFieldName;
				REDISKEY strConfReportidFieldName = cConfReportidFieldName;
				REDISKEY strLiveSubjectFieldName = cLiveSubjectFieldName;
				REDISKEY strLiveStarttimeFieldName = cLiveStarttimeFieldName;
				REDISKEY strLiveEndtimeFieldName = cLiveEndtimeFieldName;
				REDISKEY strLiveChairmanFieldName = cLiveChairmanFieldName;
				REDISKEY strLiveAbstractFieldName = cLiveAbstractFieldName;
				REDISKEY strLiveisPublicFieldName = cLiveisPublicFieldName;
				REDISKEY strLivePWDFieldName = cLivePWDFieldName;
				REDISKEY strLiveisUseRecFieldName = cLiveisUseRecFieldName;
				REDISKEY strLiveAddrFieldName = cLiveAddrFieldName;
				REDISKEY strLiveWatchtimesFieldName = cLiveWatchtimesFieldName;
				REDISKEY strLiveLikestimesFieldName = cLiveLikestimesFieldName;
				REDISKEY strLiveWatchaddrFieldName = cLiveWatchaddrFieldName;
				REDISKEY strLiveAndroidtimesFieldName = cLiveAndroidtimesFieldName;
				REDISKEY strLiveIOStimesFieldName = cLiveIOStimesFieldName;
				REDISKEY strLivePCtimesFieldName = cLivePCtimesFieldName;
				REDISKEY strLiveMobiletimesFieldName = cLiveMobiletimesFieldName;
				REDISKEY strLiveConvertStatusFieldName = cLiveConvertStatusFieldName;
				REDISKEY strLiveTaskIdFieldName = cLiveTaskIdFieldName;
				vGetFileds.push_back(strLiveinfoidFieldName);
				vGetFileds.push_back(strConfidFieldName);
				vGetFileds.push_back(strConfReportidFieldName);
				vGetFileds.push_back(strLiveSubjectFieldName);
				vGetFileds.push_back(strLiveStarttimeFieldName);
				vGetFileds.push_back(strLiveEndtimeFieldName);
				vGetFileds.push_back(strLiveChairmanFieldName);
				vGetFileds.push_back(strLiveAbstractFieldName);
				vGetFileds.push_back(strLiveisPublicFieldName);
				vGetFileds.push_back(strLivePWDFieldName);
				vGetFileds.push_back(strLiveisUseRecFieldName);
				vGetFileds.push_back(strLiveAddrFieldName);
				vGetFileds.push_back(strLiveWatchtimesFieldName);
				vGetFileds.push_back(strLiveLikestimesFieldName);
				vGetFileds.push_back(strLiveWatchaddrFieldName);
				vGetFileds.push_back(strLiveAndroidtimesFieldName);
				vGetFileds.push_back(strLiveIOStimesFieldName);
				vGetFileds.push_back(strLivePCtimesFieldName);
				vGetFileds.push_back(strLiveMobiletimesFieldName);
				vGetFileds.push_back(strLiveConvertStatusFieldName);
				vGetFileds.push_back(strLiveTaskIdFieldName);

				bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet(strinsertliveinfoconfidliveinfoidkey, vGetFileds, vRedisReplyArray);

				if (bhashHMGet_ok
					&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
				{
					sprintf(liveinfo_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(conf_id_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(conf_report_id_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_subject_value, "%s", vRedisReplyArray[3].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_starttime_value, "%s", vRedisReplyArray[4].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_endtime_value, "%s", vRedisReplyArray[5].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_chairman_value, "%s", vRedisReplyArray[6].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_abstract_value, "%s", vRedisReplyArray[7].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_ispublic_value, "%s", vRedisReplyArray[8].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_pwd_value, "%s", vRedisReplyArray[9].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_isuserec_value, "%s", vRedisReplyArray[10].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_addr_value, "%s", vRedisReplyArray[11].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_watch_times_value, "%s", vRedisReplyArray[12].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_likes_times_value, "%s", vRedisReplyArray[13].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_watch_addr_value, "%s", vRedisReplyArray[14].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_android_times_value, "%s", vRedisReplyArray[15].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_ios_times_value, "%s", vRedisReplyArray[16].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_pc_times_value, "%s", vRedisReplyArray[17].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_mobile_times_value, "%s", vRedisReplyArray[18].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_convert_status_value, "%s", vRedisReplyArray[19].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(live_task_id_value, "%s", vRedisReplyArray[20].str.c_str()); // ��Ҫ��push_back����˳��һ��

					unsigned long long ullLiveinfoid = 0;
					unsigned long long ullConfid = 0;
					unsigned long long ullConfRecordid = 0;
					int ispublic = 0;
					int isuserec = 1;
					int iWatchtimes = 0;
					int iLiketimes = 0;
					int iAndroidtimes = 0;
					int iIOStimes = 0;
					int iPCtimes = 0;
					int iMobiletimes = 0;
					int iConvertstatus = 0;

#ifdef WIN32
					sscanf(liveinfo_id_value, "%I64d", &ullLiveinfoid);
					sscanf(conf_id_value, "%I64d", &ullConfid);
					sscanf(conf_report_id_value, "%I64d", &ullConfRecordid);
#elif defined LINUX
					sscanf(liveinfo_id_value, "%lld", &ullLiveinfoid);
					sscanf(conf_id_value, "%lld", &ullConfid);
					sscanf(conf_report_id_value, "%lld", &ullConfRecordid);
#endif

					sscanf(live_ispublic_value, "%d", &ispublic);
					sscanf(live_isuserec_value, "%d", &isuserec);
					sscanf(live_watch_times_value, "%d", &iWatchtimes);
					sscanf(live_likes_times_value, "%d", &iLiketimes);
					sscanf(live_android_times_value, "%d", &iAndroidtimes);
					sscanf(live_ios_times_value, "%d", &iIOStimes);
					sscanf(live_pc_times_value, "%d", &iPCtimes);
					sscanf(live_mobile_times_value, "%d", &iMobiletimes);
					sscanf(live_convert_status_value, "%d", &iConvertstatus);

					pConfLiveInfo->SetLiveInfoID(ullLiveinfoid);
					pConfLiveInfo->SetConfID(ullConfid);
					pConfLiveInfo->SetConfReportID(ullConfRecordid);
					pConfLiveInfo->SetLiveSubject(vRedisReplyArray[3].str);
					pConfLiveInfo->SetStartTime(vRedisReplyArray[4].str);
					pConfLiveInfo->SetEndTime(vRedisReplyArray[5].str);
					pConfLiveInfo->SetLiveChairman(vRedisReplyArray[6].str);
					pConfLiveInfo->SetLiveAbstract(vRedisReplyArray[7].str);
					pConfLiveInfo->SetIsPublic(ispublic);
					pConfLiveInfo->SetLivePWD(vRedisReplyArray[9].str);
					pConfLiveInfo->SetIsUseRec(isuserec);
					pConfLiveInfo->SetLiveAddr(vRedisReplyArray[11].str);
					pConfLiveInfo->SetWatchtimes(iWatchtimes);
					pConfLiveInfo->SetLiketimes(iLiketimes);
					pConfLiveInfo->SetWatchaddr(vRedisReplyArray[14].str);
					pConfLiveInfo->SetAndroidtimes(iAndroidtimes);
					pConfLiveInfo->SetIOStimes(iIOStimes);
					pConfLiveInfo->SetPCtimes(iPCtimes);
					pConfLiveInfo->SetMobiletimes(iMobiletimes);
					pConfLiveInfo->SetConvertstatus(iConvertstatus);
					pConfLiveInfo->SetTaskid(vRedisReplyArray[20].str);

					bool bInsertOK = false;
					bInsertOK = pConfLiveInfo->InsertConfLiveInfoToDB();

					if (bInsertOK)// �����ɾ�����ܻᵼ��redis��������
					{
						m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);
					}
					else
					{
						sr_printf(SR_LOGLEVEL_ERROR, " --error-- WriteUserConfInfoToDB ==pConfLiveInfo->InsertConfLiveInfoToDB(): error\n");
					}
				}
				else
				{
					sr_printf(SR_LOGLEVEL_ERROR, " --error-- m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet ==%s\n", strinsertliveinfoconfidliveinfoidkey.c_str());
				}
			}
			else if (strncmp("modifyrollcallinfo", ptrOPkey, 18) == 0)
			{
				char cConfidFieldName[128] = { 0 };
				char conf_id_value[128] = { 0 };
				char cRollcallidFieldName[128] = { 0 };
				char rollcall_id_value[128] = { 0 };
				char cRollcallNameFieldName[128] = { 0 };
				char rollcall_name_value[256] = { 0 };
				char cOPtypeFieldName[128] = { 0 };
				char op_type_value[128] = { 0 };
				char cPartInfoFieldName[128] = { 0 };
				//char part_info_value[128] = { 0 };
				std::string strPartinfoValue;
				strPartinfoValue.clear();

				sprintf(cConfidFieldName, "%s", "confid");
				sprintf(cRollcallidFieldName, "%s", "rcid");
				sprintf(cRollcallNameFieldName, "%s", "rcname");
				sprintf(cOPtypeFieldName, "%s", "optype");
				sprintf(cPartInfoFieldName, "%s", "partinfo");

				REDISKEY strmodifyrollcallinfoconfidrollcallidtime_key = ptrOPkey;
				REDISFILEDS vGetFileds;
				vGetFileds.clear();
				RedisReplyArray vRedisReplyArray;
				vRedisReplyArray.clear();
				// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
				REDISKEY strConfidFieldName = cConfidFieldName;
				REDISKEY strRollcallidFieldName = cRollcallidFieldName;
				REDISKEY strRollcallNameFieldName = cRollcallNameFieldName;
				REDISKEY strOPtypeFieldName = cOPtypeFieldName;
				REDISKEY strPartInfoFieldName = cPartInfoFieldName;
				vGetFileds.push_back(strConfidFieldName);
				vGetFileds.push_back(strRollcallidFieldName);
				vGetFileds.push_back(strRollcallNameFieldName);
				vGetFileds.push_back(strOPtypeFieldName);
				vGetFileds.push_back(strPartInfoFieldName);

				bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet(strmodifyrollcallinfoconfidrollcallidtime_key, vGetFileds, vRedisReplyArray);

				if (bhashHMGet_ok
					&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
				{
					sprintf(conf_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(rollcall_id_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(rollcall_name_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��
					sprintf(op_type_value, "%s", vRedisReplyArray[3].str.c_str()); // ��Ҫ��push_back����˳��һ��
					strPartinfoValue.assign(vRedisReplyArray[4].str.c_str());

					unsigned long long ullConfid = 0;
					unsigned long long ullRollcallid = 0;
					unsigned int uiOptype = 0;
					
#ifdef WIN32
					sscanf(conf_id_value, "%I64d", &ullConfid);
					sscanf(rollcall_id_value, "%I64d", &ullRollcallid);
#elif defined LINUX
					sscanf(conf_id_value, "%lld", &ullConfid);
					sscanf(rollcall_id_value, "%lld", &ullRollcallid);
#endif

					sscanf(op_type_value, "%d", &uiOptype);

					// ����ʲô����������ɾ����������
					pRollCallList->SetCallInfoID(ullRollcallid);
					pRollCallList->DeleteCallListForDB();

					pConfRollCallInfo->SetCallInfoID(ullRollcallid);
					pConfRollCallInfo->SetConfID(ullConfid);
					pConfRollCallInfo->DeleteConfCallInfoForDB();

					// �������޸���Ҫ����������Ա�б�
					if (uiOptype == 1
						|| uiOptype == 3)
					{
						// ����call_list��
						//pRollCallList->SetCallInfoID(ullRollcallid);
						
						char *sep1 = { "#" };
						char *sep2 = { "@" };
						std::string str;
						std::string struserid;
						std::string strorderno;
						int index;
						char *token = strtok((char *)strPartinfoValue.c_str(), sep1);
						while (token != NULL)
						{
							str = token;
							//printf("%s\n", str.c_str());
							index = str.find(sep2);
							if (index > 0)
							{
								//ms.insert(pair_string(str.substr(0, index), str.substr(index + 1, str.length())));
								struserid = str.substr(0, index);
								strorderno = str.substr(index + 1, str.length());

								unsigned int uiUserid = 0;
								unsigned int uiOrderNo = 0;
								sscanf(struserid.c_str(), "%d", &uiUserid);
								sscanf(strorderno.c_str(), "%d", &uiOrderNo);

								pRollCallList->SetUserID(uiUserid);
								pRollCallList->SetOrderNO(uiOrderNo);
								pRollCallList->InsertCallListToDB();
							}
							token = strtok(NULL, sep1);
						}

						// ����call_info��
						//pConfRollCallInfo->SetCallInfoID(ullRollcallid);
						//pConfRollCallInfo->SetConfID(ullConfid);
						pConfRollCallInfo->SetCallInfoName(vRedisReplyArray[2].str);
						pConfRollCallInfo->InsertConfCallInfoToDB();
					}

					m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);
				}
				else
				{
					sr_printf(SR_LOGLEVEL_ERROR, " --error-- m_pRedisConnList[e_RC_TT_WriteDBThread]->hashHMGet ==%s\n", strmodifyrollcallinfoconfidrollcallidtime_key.c_str());
				}
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, " --error-- WriteUserConfInfoToDB ==op_key_list: key= %s\n", ptrOPkey);

				//m_pRedisConnList[e_RC_TT_WriteDBThread]->deletevalue(ptrOPkey);
				//sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_DelUserConfDetail listLPush ** (%s) **bListLPushOK=%d\n", userconfdetail_confid_suid_rptid_key, bListLPushOK);
			}

			delete ptrOPkey; // ��ɾ���ᵼ���ڴ�й©
		}

		uiOpRedisNum++;
		if (uiOpRedisNum % m_op_db_num == 0)
		{
			break;
		}

		llListlen--;

#ifdef WIN32
		sr_printf(SR_LOGLEVEL_DEBUG, " <<<<== WriteUserConfInfoToDB ==uiOpRedisNum = %d, llListlen = %I64d\n", uiOpRedisNum, llListlen);
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_DEBUG, " <<<<== WriteUserConfInfoToDB ==uiOpRedisNum = %d, llListlen = %lld\n", uiOpRedisNum, llListlen);
#endif
	}

#ifdef LINUX
	gettimeofday(&end, NULL); // ��s level
	time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

	sr_printf(SR_LOGLEVEL_NORMAL, "*** WriteUserConfInfoToDB uiOpRedisNum (%d) *****time_use** is: %lf us \n", uiOpRedisNum, time_use);

#endif 

	return uiOpRedisNum;
}

//void CDevMgr::DelUserConfDetail(int suid, unsigned long long confid, char* time)
void CDevMgr::DelUserConfDetail(int suid, unsigned long long confid, char* time, time_t lltime)
{
	if (!m_pUpMsqThread)
		return;

	if (time == NULL)
		return;

	typedef CBufferT<int, unsigned long long, CAsyncThread*, time_t, void*, void*, void*, void*> CParam;
	CParam* pParam;
	pParam = new CParam(time, strlen(time), NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, suid, confid, m_pUpMsqThread, lltime);

	typedef void (CDevMgr::* ACTION)(void*);
    m_pUpMsqThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_DelUserConfDetail, (void*)pParam));
	return;
}

void CDevMgr::Handle_DelUserConfDetail(void* pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}

	typedef CBufferT<int, unsigned long long, CAsyncThread*, time_t, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;

	char* ptime = (char*)pParam->m_pData1;
	int suid = pParam->m_Arg1;
	unsigned long long confid = pParam->m_Arg2;

	CAsyncThread* pUpMsgThread = (CAsyncThread*)(pParam->m_Arg3);

	time_t hand_del_time = (time_t)(pParam->m_Arg4);

	if (pUpMsgThread == NULL
		|| m_pRedisConnList[e_RC_TT_UpMsqThread] == NULL) // test delet by csh at 2016.10.21
	{
		delete pParam;
		pParam = NULL;
		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_DelUserConfDetail pUpMsgThread and m_pRedisConnList is NULL.\n");

		return;
	}

	CUserConfDetail* pUserConfDetail = NULL;
	std::map<CAsyncThread*, CUserConfDetail*>::iterator iter_mapAsyncThreadUserConfDetial = m_mapAsyncThreadUserConfDetial.find(pUpMsgThread);
	if (iter_mapAsyncThreadUserConfDetial != m_mapAsyncThreadUserConfDetial.end())
	{
		pUserConfDetail = iter_mapAsyncThreadUserConfDetial->second;
	}

	if (pUserConfDetail == NULL)
	{
		delete pParam;
		pParam = NULL;

		sr_printf(SR_LOGLEVEL_ERROR, "==error-->> Handle_DelUserConfDetail pUserConfDetail is NULL.\n");

		return;
	}

	do
	{
#ifdef LINUX
		float time_use = 0;
		struct timeval start;
		struct timeval end;
		gettimeofday(&start, NULL); // ��s level
#endif	

		char strOP_Result[10] = { 0 };

		char cConfidFieldName[128] = { 0 };
		char conf_id_value[128] = { 0 };
		char cConfReportidFieldName[128] = { 0 };
		char conf_report_id_value[128] = { 0 };
		char cUseRptDetailidFieldName[128] = { 0 };
		char use_rpt_detail_id_value[128] = { 0 };
		char cUseridextFieldName[128] = { 0 };
		char user_id_ext_value[128] = { 0 };
		char cUserAliasFieldName[128] = { 0 };
		char user_alias_value[128] = { 0 };
		char cRealJointimeFieldName[128] = { 0 };
		char real_jointime_value[128] = { 0 };
		char cRealLeavetimeFieldName[128] = { 0 };
		char real_leavetime_value[128] = { 0 };
		char cFromtypeFieldName[32] = { 0 };
		char fromtype_value[32] = { 0 };
		char cTermtypeFieldName[32] = { 0 };
		char termtype_value[32] = { 0 };

		char userconfdetail_confid_suid_key[128] = { 0 };
		char confterlist_confid_key[128] = { 0 };

#ifdef WIN32
		sprintf(conf_id_value, "%I64d", confid);
#elif defined LINUX
		sprintf(conf_id_value, "%lld", confid);
#endif
		sprintf(userconfdetail_confid_suid_key, "userconfdtl_%s_%d", conf_id_value, suid);
		sprintf(confterlist_confid_key, "confterlist_%s", conf_id_value);


		sprintf(cConfidFieldName, "%s", "conf_id");
		sprintf(cConfReportidFieldName, "%s", "conf_report_id");
		sprintf(cUseRptDetailidFieldName, "%s", "use_rpt_detaild_id");
		sprintf(cUseridextFieldName, "%s", "user_id_ext");
		sprintf(cUserAliasFieldName, "%s", "user_alias");
		sprintf(cRealJointimeFieldName, "%s", "real_jointime");
		sprintf(cRealLeavetimeFieldName, "%s", "real_leavetime");
		sprintf(cFromtypeFieldName, "%s", "fromtype");
		sprintf(cTermtypeFieldName, "%s", "termtype");

		m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(4);
		//////////////////////////////////////////////////////////////////////////
		REDISKEY struserconfdetailconfidsuidkey = userconfdetail_confid_suid_key;
		REDISFILEDS vGetFileds;
		vGetFileds.clear();
		RedisReplyArray vRedisReplyArray;
		vRedisReplyArray.clear();

		// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
		REDISKEY strConfReportidFieldName = cConfReportidFieldName;
		REDISKEY strUseRptDetailidFieldName = cUseRptDetailidFieldName;
		REDISKEY strUserAliasFieldName = cUserAliasFieldName;
		REDISKEY strRealJointimeFieldName = cRealJointimeFieldName;
		REDISKEY strFromtypeFieldName = cFromtypeFieldName;
		REDISKEY strTermtypeFieldName = cTermtypeFieldName;

		vGetFileds.push_back(strConfReportidFieldName);
		vGetFileds.push_back(strUseRptDetailidFieldName);
		vGetFileds.push_back(strUserAliasFieldName);
		vGetFileds.push_back(strRealJointimeFieldName);
		vGetFileds.push_back(strFromtypeFieldName);
		vGetFileds.push_back(strTermtypeFieldName);

		bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMGet(struserconfdetailconfidsuidkey, vGetFileds, vRedisReplyArray);

		//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMGet==userconfdetail_confid_suid_key--->> bhashHMGet_ok:%d, vRedisReplyArray.size:%d\n", bhashHMGet_ok, vRedisReplyArray.size());

		char userconfdetail_confid_suid_rptid_key[128] = { 0 };

		if (bhashHMGet_ok
			&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
		{
			sprintf(conf_report_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
			sprintf(use_rpt_detail_id_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
			sprintf(user_alias_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��
			sprintf(real_jointime_value, "%s", vRedisReplyArray[3].str.c_str()); // ��Ҫ��push_back����˳��һ��
			sprintf(fromtype_value, "%s", vRedisReplyArray[4].str.c_str()); // ��Ҫ��push_back����˳��һ��
			sprintf(termtype_value, "%s", vRedisReplyArray[5].str.c_str()); // ��Ҫ��push_back����˳��һ��

			//char userconfdetail_confid_suid_rptid_key[128] = { 0 };
			sprintf(userconfdetail_confid_suid_rptid_key, "userconfdtl_%s_%d_%s", conf_id_value, suid, conf_report_id_value);

			sprintf(user_id_ext_value, "%d", suid);


			REDISKEY struserconfdtlconfidsuidrptidkey = userconfdetail_confid_suid_rptid_key;
			REDISVDATA vUserConfdtlConfidSuidRptidData;
			vUserConfdtlConfidSuidRptidData.clear();

			REDISKEY strConfidFieldName = cConfidFieldName;
			REDISVALUE strconfidvalue = conf_id_value;
			vUserConfdtlConfidSuidRptidData.push_back(strConfidFieldName);
			vUserConfdtlConfidSuidRptidData.push_back(strconfidvalue);

			REDISKEY strConfReportidFieldName = cConfReportidFieldName;
			REDISVALUE strconfreportidvalue = conf_report_id_value;
			vUserConfdtlConfidSuidRptidData.push_back(strConfReportidFieldName);
			vUserConfdtlConfidSuidRptidData.push_back(strconfreportidvalue);

			REDISKEY strUserRptDtlidFieldName = cUseRptDetailidFieldName;
			REDISVALUE struserrptdtlidvalue = use_rpt_detail_id_value;
			vUserConfdtlConfidSuidRptidData.push_back(strUserRptDtlidFieldName);
			vUserConfdtlConfidSuidRptidData.push_back(struserrptdtlidvalue);

			REDISKEY strUseridextFieldName = cUseridextFieldName;
			REDISVALUE struseridextvalue = user_id_ext_value;
			vUserConfdtlConfidSuidRptidData.push_back(strUseridextFieldName);
			vUserConfdtlConfidSuidRptidData.push_back(struseridextvalue);

			REDISKEY strUserAliasFieldName = cUserAliasFieldName;
			REDISVALUE struseraliasvalue = user_alias_value;
			vUserConfdtlConfidSuidRptidData.push_back(strUserAliasFieldName);
			vUserConfdtlConfidSuidRptidData.push_back(struseraliasvalue);

			REDISKEY strRealJointimeFieldName = cRealJointimeFieldName;
			REDISVALUE strrealjointimevalue = real_jointime_value;
			vUserConfdtlConfidSuidRptidData.push_back(strRealJointimeFieldName);
			vUserConfdtlConfidSuidRptidData.push_back(strrealjointimevalue);

			REDISKEY strRealLeavetimeFieldName = cRealLeavetimeFieldName;
			REDISVALUE strrealleavetimevalue = ptime;
			vUserConfdtlConfidSuidRptidData.push_back(strRealLeavetimeFieldName);
			vUserConfdtlConfidSuidRptidData.push_back(strrealleavetimevalue);

			REDISKEY strFromtypeFieldName = cFromtypeFieldName;
			REDISVALUE strfromtypevalue = fromtype_value;
			vUserConfdtlConfidSuidRptidData.push_back(strFromtypeFieldName);
			vUserConfdtlConfidSuidRptidData.push_back(strfromtypevalue);

			REDISKEY strTermtypeFieldName = cTermtypeFieldName;
			REDISVALUE strtermtypevalue = termtype_value;
			vUserConfdtlConfidSuidRptidData.push_back(strTermtypeFieldName);
			vUserConfdtlConfidSuidRptidData.push_back(strtermtypevalue);

			bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet(struserconfdtlconfidsuidrptidkey, vUserConfdtlConfidSuidRptidData);

			//sr_printf(SR_LOGLEVEL_ERROR, " m_pRedisConnList[e_RC_TT_UpMsqThread]->hashHMSet==userconfdetail_confid_suid_rptid_key---->>>> %d\n", bhashHMSet_ok);
			bool bListLPushOK = false;
			bListLPushOK = m_pRedisConnList[e_RC_TT_UpMsqThread]->listLPush("op_key_list", userconfdetail_confid_suid_rptid_key);
			sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_DelUserConfDetail listLPush ** (%s) **bListLPushOK=%d\n", userconfdetail_confid_suid_rptid_key, bListLPushOK);

			m_pRedisConnList[e_RC_TT_UpMsqThread]->deletevalue(userconfdetail_confid_suid_key);

			m_pRedisConnList[e_RC_TT_UpMsqThread]->deletevalue(confterlist_confid_key, user_id_ext_value);

		}

		// ɾ���ն�ͳ����Ϣ
		char tercalldetail_confid_tersuid_key[256] = { 0 };
		sprintf(tercalldetail_confid_tersuid_key, "tercalldetail_%s_%d", conf_id_value, suid);

		m_pRedisConnList[e_RC_TT_UpMsqThread]->selectdb(7);
		m_pRedisConnList[e_RC_TT_UpMsqThread]->deletevalue(tercalldetail_confid_tersuid_key);

#ifdef LINUX
		gettimeofday(&end, NULL); // ��s level
		time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

		sr_printf(SR_LOGLEVEL_DEBUG, "==-->>Handle_DelUserConfDetail into db(4) ** (%s) **time_use** is: %lf us \n", userconfdetail_confid_suid_rptid_key, time_use);
#endif

	} while (0);


	delete pParam;
	pParam = NULL;
}

bool CDevMgr::ParseClientData(DeviceConnect *pClient, CAsyncThread* pThread, const char* pData, const int nLen, unsigned long recv_data_time)
{
	/*
	char *ss = new char[nLen + 1];
	memcpy(ss, pData, nLen + 1);

	char lpTemp[1024] = { 0 };
	for (int i = 0; i < nLen; i++)
	{
		char temp[6] = { 0 };
		//sprintf(temp, "0x%02x ", (unsigned char)ss[i]);
		sprintf(temp, "%02x ", (unsigned char)ss[i]);
		strcat(lpTemp, temp);
	}

	sr_printf(SR_LOGLEVEL_NORMAL, "====ParseClientData--->>>%s\n", lpTemp);

	if (ss != NULL)
	{
		delete ss;
	}

	char traceBuf[1600] = { 0 };
	*/

	if (nLen < 16)
	{
		sr_printf(SR_LOGLEVEL_ERROR, " ParseClientData data len is too small !!!!\n");
		return false;
	}

	CDevice* pDevice = NULL;
	std::map<CAsyncThread*, CDevice*>::iterator iter_mapAsyncThreadDevice = m_mapAsyncThreadDevice.find(pThread);
	if (iter_mapAsyncThreadDevice != m_mapAsyncThreadDevice.end())
	{
		pDevice = iter_mapAsyncThreadDevice->second;
	}

	CDeviceConfig* pDeviceConfig = NULL;
	std::map<CAsyncThread*, CDeviceConfig*>::iterator iter_mapAsyncThreadDeviceConfig = m_mapAsyncThreadDeviceConfig.find(pThread);
	if (iter_mapAsyncThreadDeviceConfig != m_mapAsyncThreadDeviceConfig.end())
	{
		pDeviceConfig = iter_mapAsyncThreadDeviceConfig->second;
	}

	CConference* pConference = NULL;
	std::map<CAsyncThread*, CConference*>::iterator iter_mapAsyncThreadConference = m_mapAsyncThreadConference.find(pThread);
	if (iter_mapAsyncThreadConference != m_mapAsyncThreadConference.end())
	{
		pConference = iter_mapAsyncThreadConference->second;
	}

	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}

	CUserConfDetail* pUserConfDetail = NULL;
	std::map<CAsyncThread*, CUserConfDetail*>::iterator iter_mapAsyncThreadUserConfDetial = m_mapAsyncThreadUserConfDetial.find(pThread);
	if (iter_mapAsyncThreadUserConfDetial != m_mapAsyncThreadUserConfDetial.end())
	{
		pUserConfDetail = iter_mapAsyncThreadUserConfDetial->second;
	}

	CDeviceConfDetail* pDeviceConfDetail = NULL;
	std::map<CAsyncThread*, CDeviceConfDetail*>::iterator iter_mapAsyncThreadDeviceConfDetail = m_mapAsyncThreadDeviceConfDetail.find(pThread);
	if (iter_mapAsyncThreadDeviceConfDetail != m_mapAsyncThreadDeviceConfDetail.end())
	{
		pDeviceConfDetail = iter_mapAsyncThreadDeviceConfDetail->second;
	}

	CConfParticipant* pConfParticipant = NULL;
	std::map<CAsyncThread*, CConfParticipant*>::iterator iter_mapAsyncThreadConfParticipant = m_mapAsyncThreadConfParticipant.find(pThread);
	if (iter_mapAsyncThreadConfParticipant != m_mapAsyncThreadConfParticipant.end())
	{
		pConfParticipant = iter_mapAsyncThreadConfParticipant->second;
	}

	CConfLiveSetting* pConfLiveSetting = NULL;
	std::map<CAsyncThread*, CConfLiveSetting*>::iterator iter_mapAsyncThreadConfLiveSetting = m_mapAsyncThreadConfLiveSetting.find(pThread);
	if (iter_mapAsyncThreadConfLiveSetting != m_mapAsyncThreadConfLiveSetting.end())
	{
		pConfLiveSetting = iter_mapAsyncThreadConfLiveSetting->second;
	}
	
	CConfRollCallInfo* pConfRollCallInfo = NULL;
	std::map<CAsyncThread*, CConfRollCallInfo*>::iterator iter_mapAsyncThreadConfRollCallInfo = m_mapAsyncThreadConfRollCallInfo.find(pThread);
	if (iter_mapAsyncThreadConfRollCallInfo != m_mapAsyncThreadConfRollCallInfo.end())
	{
		pConfRollCallInfo = iter_mapAsyncThreadConfRollCallInfo->second;
	}
	
	CRollCallList* pRollCallList = NULL;
	std::map<CAsyncThread*, CRollCallList*>::iterator iter_mapAsyncThreadRollCallList = m_mapAsyncThreadRollCallList.find(pThread);
	if (iter_mapAsyncThreadRollCallList != m_mapAsyncThreadRollCallList.end())
	{
		pRollCallList = iter_mapAsyncThreadRollCallList->second;
	}

	CConfPollInfo* pConfPollInfo = NULL;
	std::map<CAsyncThread*, CConfPollInfo*>::iterator iter_mapAsyncThreadConfPollInfo = m_mapAsyncThreadConfPollInfo.find(pThread);
	if (iter_mapAsyncThreadConfPollInfo != m_mapAsyncThreadConfPollInfo.end())
	{
		pConfPollInfo = iter_mapAsyncThreadConfPollInfo->second;
	}

	CPollList* pPollList = NULL;
	std::map<CAsyncThread*, CPollList*>::iterator iter_mapAsyncThreadPollList = m_mapAsyncThreadPollList.find(pThread);
	if (iter_mapAsyncThreadPollList != m_mapAsyncThreadPollList.end())
	{
		pPollList = iter_mapAsyncThreadPollList->second;
	}
	CDataDictionary* pDataDictionary = NULL;
	std::map<CAsyncThread*, CDataDictionary*>::iterator iter_mapAsyncThreadDataDictionary = m_mapAsyncThreadDataDictionary.find(pThread);
	if (iter_mapAsyncThreadDataDictionary != m_mapAsyncThreadDataDictionary.end())
	{
		pDataDictionary = iter_mapAsyncThreadDataDictionary->second;
	}
	if (pDevice == NULL
		|| pDeviceConfig == NULL
		|| pConference == NULL
		|| pConfReport == NULL
		|| pUserConfDetail == NULL
		|| pDeviceConfDetail == NULL
		|| pConfParticipant == NULL
		|| pConfLiveSetting == NULL
		|| pConfRollCallInfo == NULL
		|| pRollCallList == NULL
		|| pConfPollInfo == NULL
		|| pPollList == NULL
		|| pDataDictionary == NULL)
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	//�������ݽ����󣬷ֱ���
	SRMC::S_ProtoMsgHeader header;
	SRMC::unpackPMH(pData + 8, &header);
    //SRMC::unpackPMH(pData, &header);

	unsigned long ulTimeNow = getTickCount();
	unsigned long msg_in_thread_time = ulTimeNow - recv_data_time;

	
	//char strtraceBuf[100] = { 0 };
//#ifdef WIN32
//	sprintf(strtraceBuf, "====header.m_msguid------------>>>%04x, %d, msg_in_thread_time = %I64d ��s\n", header.m_msguid, header.m_msguid, msg_in_thread_time);
//#elif defined LINUX
//	sprintf(strtraceBuf, "====header.m_msguid------------>>>%04x, %d, msg_in_thread_time = %lld ��s\n", header.m_msguid, header.m_msguid, msg_in_thread_time);
//#endif
	//CAutoTrace::GetInstance()->AutoTrace(g_Trace, m_fullpath, strtraceBuf);
	//if (3076 == header.m_msguid && m_fpLogFile != NULL)
	//{
	//	fwrite(strtraceBuf, strlen(strtraceBuf), 1, m_fpLogFile);
	//	fflush(m_fpLogFile);
	//}

	sr_printf(SR_LOGLEVEL_NORMAL, "==recv==Client===header.m_msguid-------->>>%04X, %d\n", header.m_msguid, header.m_msguid);

	switch (header.m_msguid)
	{
	case SRMsgId_ReqRegister:
	{
		SRMsgs::ReqRegister req;
		//if (req.ParsePartialFromArray(pData + 8, nLen - 8))
		if (req.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			if (req.has_ip() && req.has_port() && req.has_auth_password() && req.has_svr_type() && req.has_cpunums())
			{
#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif

				//sr_printf(SR_LOGLEVEL_NORMAL, "xxx->devmgr,SRMsgId_ReqRegister --\n%s\n", req.Utf8DebugString().c_str());

				//����ע��Ľ��
				SRMsgs::RspRegister rspsend;

				if (pClient != NULL)
				{
					bool bRegister = ProcessReqRegister(&req, pClient, pDeviceConfig, pDevice, pDataDictionary, &rspsend);

					SerialProtoMsgAndSend(pClient, SRMsgId_RspRegister, &rspsend);

					if (bRegister) // ע��ɹ������������map��
					{
						//��ȡ��ǰʱ�䴮
						time_t timeNow;
						timeNow = time(NULL);

						int iTmpdeviceid = pDevice->GetDeviceID();
						iter_mapDeviceHeartbeatTime iter = m_MapDeviceHeartbeatTime.find(iTmpdeviceid);
						if (iter == m_MapDeviceHeartbeatTime.end())
						{
							sDeviceInfo dev_info_;
							dev_info_.devicetype = req.svr_type();
							dev_info_.time = timeNow;
							//dev_info_.token = CDevice::GetInstance()->GetDeviceToken();
							dev_info_.token = pDevice->GetDeviceToken();
							dev_info_.pClient = pClient;
							m_MapDeviceHeartbeatTime.insert(valType(iTmpdeviceid, dev_info_));
						}
						else
						{
							iter->second.devicetype = req.svr_type();
							iter->second.time = timeNow; //����map�ж�Ӧ������ʱ��ֵ
							//iter->second.token = CDevice::GetInstance()->GetDeviceToken(); // token��devmgrΪÿ���豸�����
							iter->second.token = pDevice->GetDeviceToken(); // token��devmgrΪÿ���豸�����
							iter->second.pClient = pClient;
						}

						// ע��ɹ��������Է�����������Ϣ
						SRMsgs::IndDevtoserHeart Indsend;
						Indsend.set_token(pDevice->GetDeviceToken().c_str());
						SerialProtoMsgAndSend(pClient, SRMsgId_IndDevtoserHeart, &Indsend);

						// ����¼�devmgrע��ɹ�,���������¼�devmgr����relaymc��stunserver���豸��Ϣ
						if (req.svr_type() == DEVICE_SERVER::DEVICE_DEV)
						{
							// 1����ȡ��(��)������
							char local_domain_name[128] = { 0 };
							RedisReplyArray vrra_domain_level;
							vrra_domain_level.clear();
							unsigned int nDomainAndLevel = 0;

							m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
							bool bHGetAllDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_domain_level);

							nDomainAndLevel = vrra_domain_level.size();

							if (bHGetAllDomain_ok
								&& (nDomainAndLevel > 0)
								&& (nDomainAndLevel % 2 == 0))
							{
								for (int idx = 0; idx < nDomainAndLevel;)
								{
									int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
									//vrra_domain_level[idx].str;// ����
									iLevel = atoi(vrra_domain_level[idx + 1].str.c_str());// level
									if (iLevel == 0)
									{
										sprintf(local_domain_name, "%s", vrra_domain_level[idx].str.c_str());
										break;
									}
									idx += 2;
								}
							}

							if (strlen(local_domain_name) > 0)
							{
								// 2���鱾(��)����local_domain_deviceinfo��svr_type==3��svr_type==4��svr_type==11��deviceid
								// ��������豸��Ϣ[deviceid,svrtype]
								RedisReplyArray vrra_deviceid_svrtype;
								vrra_deviceid_svrtype.clear();
								bool bHGetAllok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_deviceinfo", vrra_deviceid_svrtype);

								unsigned int nDeviceidandSvrtypeNum = vrra_deviceid_svrtype.size();

								bool bNeedSend = false;

								if (bHGetAllok
									&& (nDeviceidandSvrtypeNum > 0)
									&& (nDeviceidandSvrtypeNum % 2 == 0))
								{
									SRMsgs::IndUpSvrInfoToDevmgr indsvrinfo;
									indsvrinfo.set_addordel(1);
									indsvrinfo.set_deviceid(pDevice->GetDeviceID());
									indsvrinfo.set_token(pDevice->GetDeviceToken());
									SRMsgs::IndUpSvrInfoToDevmgr_DomainInfo* psvrdomaininfo = 0;
									{
										psvrdomaininfo = indsvrinfo.add_domaininfos();
										psvrdomaininfo->set_domainname(local_domain_name);
										psvrdomaininfo->set_level(0);// ������ǿ��д0

										SRMsgs::IndUpSvrInfoToDevmgr_ServerInfo* pSvrinfo = 0;

										for (int i = 0; i < nDeviceidandSvrtypeNum;)
										{
											char deviceid_value[128] = { 0 };
											char svrtype_value[128] = { 0 };
											SR_uint32 uiDeviceid = 0;
											SR_uint32 uiSvrtype = 0;
											sprintf(deviceid_value, "%s", vrra_deviceid_svrtype[i].str.c_str());
											sprintf(svrtype_value, "%s", vrra_deviceid_svrtype[i + 1].str.c_str());
											sscanf(vrra_deviceid_svrtype[i].str.c_str(), "%u", &uiDeviceid);
											sscanf(vrra_deviceid_svrtype[i + 1].str.c_str(), "%u", &uiSvrtype);

											if (uiSvrtype == DEVICE_SERVER::DEVICE_RELAY_MC
												|| uiSvrtype == DEVICE_SERVER::DEVICE_RELAYSERVER
												|| uiSvrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
											{
												char device_ipport_deviceid[128] = { 0 };
												sprintf(device_ipport_deviceid, "device_ipport_%s", deviceid_value);
												
												char cInnerFieldName[128] = { 0 };
												char cOuterFieldName[128] = { 0 };
												char cVPNFieldName[128] = { 0 };

												char inner_ipprort_value[65] = { 0 };
												char outer_ipprort_value[65] = { 0 };
												char vpn_ipprort_value[65] = { 0 };

												sprintf(cInnerFieldName, "%s", "inner");
												sprintf(cOuterFieldName, "%s", "outer");
												sprintf(cVPNFieldName, "%s", "vpn");

												REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
												REDISFILEDS vGetFileds;
												vGetFileds.clear();
												RedisReplyArray vRedisReplyArray;
												vRedisReplyArray.clear();

												// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
												REDISKEY strInnerFieldName = cInnerFieldName;
												REDISKEY strOuterFieldName = cOuterFieldName;
												REDISKEY strVPNFieldName = cVPNFieldName;
												vGetFileds.push_back(strInnerFieldName);
												vGetFileds.push_back(strOuterFieldName);
												vGetFileds.push_back(strVPNFieldName);

												bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strdeviceipportdeviceidkey, vGetFileds, vRedisReplyArray);
												if (bhashHMGet_ok
													&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
												{
													pSvrinfo = psvrdomaininfo->add_svrinfos();
													pSvrinfo->set_svrtype(uiSvrtype);
													pSvrinfo->set_deviceid(uiDeviceid);

													SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo* pSvripportinfo = new SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo();
													pSvripportinfo->set_inner(vRedisReplyArray[0].str);
													pSvripportinfo->set_outer(vRedisReplyArray[1].str);
													pSvripportinfo->set_vpn(vRedisReplyArray[2].str);
													pSvrinfo->set_allocated_ipport(pSvripportinfo);

													bNeedSend = true;
												}

												//RedisReplyKeyValuePair vRedisReplyPair;
												//vRedisReplyPair.clear();
												//bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strdeviceipportdeviceidkey, vGetFileds, vRedisReplyPair);
												//if (bhashHMGet_ok
												//	&& vRedisReplyPair.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
												//{
												//	pSvrinfo = psvrdomaininfo->add_svrinfos();
												//	pSvrinfo->set_svrtype(uiSvrtype);
												//	pSvrinfo->set_deviceid(uiDeviceid);

												//	SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo* pSvripportinfo = new SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo();
												//	vRedisReplyPair.find(strInnerFieldName);
												//	pSvripportinfo->set_inner(vRedisReplyArray[0].str);
												//	pSvripportinfo->set_outer(vRedisReplyArray[1].str);
												//	pSvripportinfo->set_vpn(vRedisReplyArray[2].str);
												//	pSvrinfo->set_allocated_ipport(pSvripportinfo);

												//	bNeedSend = true;
												//}
											}

											i += 2;
										}
									}

									if (pClient
										&& bNeedSend == true)
									{
										SerialProtoMsgAndSend(pClient, getMsgIdByClassName(IndUpSvrInfoToDevmgr), &indsvrinfo);
									}
								}
							}
						}
					}
					else
					{
						struct in_addr inIP;
#ifdef WIN32
						inIP.S_un.S_addr = req.ip();
#elif defined LINUX
						inIP.s_addr = req.ip();
#endif
						char selfIP_m[16];
						memset(selfIP_m, 0, sizeof(selfIP_m));
						strcpy(selfIP_m, inet_ntoa(inIP));

						sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqRegister (device_config_serial,svr_type,ip, port)=(%s, %d, %s, %d) Register failed!!!!\n", req.auth_password().c_str(), req.svr_type(), selfIP_m, req.port());
					}
				}
				else
				{
					sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqRegister pClient is null error\n");
				}

				{
					struct in_addr inIP;
#ifdef WIN32
					inIP.S_un.S_addr = req.ip();
#elif defined LINUX
					inIP.s_addr = req.ip();
#endif
					char selfIP_m[16];
					memset(selfIP_m, 0, sizeof(selfIP_m));
					strcpy(selfIP_m, inet_ntoa(inIP));
#ifdef LINUX
					gettimeofday(&end, NULL); // ��s level
					time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

					if (req.has_nettype())
					{
						sr_printf(SR_LOGLEVEL_INFO, "****Handle SRMsgId_ReqRegister param(device_config_serial,svr_type,ip,port,nettype)=(%s, %d, %s, %d, %d)*****time_use** is: %lf us \n", req.auth_password().c_str(), req.svr_type(), selfIP_m, req.port(), req.nettype(), time_use);
					} 
					else
					{
						sr_printf(SR_LOGLEVEL_INFO, "****Handle SRMsgId_ReqRegister param(device_config_serial,svr_type,ip,port)=(%s, %d, %s, %d)*****time_use** is: %lf us \n", req.auth_password().c_str(), req.svr_type(), selfIP_m, req.port(), time_use);
					}
#endif 
				}
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqRegister msg param is null error\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqRegister ParsePartialFromArray failed.\n");
		}
		break;
	}
	//case SRMsgId_RspRegister:
	//{
	//	SRMsgs::RspRegister rsp;
	//	if (rsp.ParsePartialFromArray(pData + 16, nLen - 16))
	//	{
	//		if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr != pClient)
	//		{
	//			sr_printf(SR_LOGLEVEL_ERROR, "RspRegister devmgr isnot the current connected\n");
	//			break;
	//		}
	//		//if (rsp.has_ip() && rsp.has_port() && rsp.has_auth_password() && rsp.has_svr_type() && rsp.has_cpunums())
	//		{
	//			processRspRegister(&rsp);
	//		}
	//		//else
	//		//{
	//		//	sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_RspRegister msg param is null error\n");
	//		//}
	//	}
	//	else
	//	{
	//		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_RspRegister ParsePartialFromArray failed.\n");
	//	}
	//	break;
	//}
	case SRMsgId_ReqUnRegister:
	{
		SRMsgs::ReqUnRegister req;
		//if (req.ParsePartialFromArray(pData + 8, nLen - 8))
		if (req.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			//sr_printf(SR_LOGLEVEL_DEBUG, "SRMsgId_ReqUnRegister.\n");
			if (req.has_token())
			{
				sr_printf(SR_LOGLEVEL_DEBUG, "SRMsgId_ReqUnRegister OK\n");

				//�޸ĸ�serverȥע��״̬�����������ظ����ƣ��������ݿ�


			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqUnRegister param is null error!!!\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqUnRegister ParsePartialFromArray failed!!!\n");
		}
		break;
	}
	case SRMsgId_ReqGetDeviceInfo:
	{
		SRMsgs::ReqGetDeviceInfo req;
		//if (req.ParsePartialFromArray(pData + 8, nLen - 8))
		if (req.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			if (req.has_deviceid() && req.has_token() && req.has_svr_type() && req.has_get_svr_type())
			{

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif

				SRMsgs::RspGetDeviceInfo rsp;
				bool bGetOK = GetDeviceInfo(&req, &rsp);

				if (bGetOK)
				{
					rsp.set_token(req.token());

					if (pClient != NULL)
					{
						SerialProtoMsgAndSend(pClient, SRMsgId_RspGetDeviceInfo, &rsp);
					}
				}
				else
				{
					sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetDeviceInfo GetDeviceInfo return false\n");
				}


#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "**Handle SRMsgId_ReqGetDeviceInfo param(deviceid,token,svr_type,get_svr_type)=(%d, %s, %d, %d)*****time_use** is: %lf us \n", req.deviceid(), req.token().c_str(), req.svr_type(), req.get_svr_type(), time_use);
#endif 

			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetDeviceInfo param is null error\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetDeviceInfo ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_ReqGetSysDeviceInfo:
	{
		SRMsgs::ReqGetSysDeviceInfo req;
		if (req.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			if (req.has_deviceid() && req.has_token() && req.has_svr_type() && req.has_get_svr_type())
			{

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif

				SRMsgs::RspGetSysDeviceInfo rsp;
				bool bGetOK = GetSysDeviceInfo(&req, &rsp);

				if (bGetOK)
				{
					rsp.set_deviceid(req.deviceid());
					rsp.set_token(req.token());
					rsp.set_svrtype(req.get_svr_type());

					if (pClient != NULL)
					{
						SerialProtoMsgAndSend(pClient, SRMsgId_RspGetSysDeviceInfo, &rsp);
					}
				}
				else
				{
					sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSysDeviceInfo GetSysDeviceInfo return false\n");
				}

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "**Handle SRMsgId_ReqGetSysDeviceInfo param(deviceid,token,svr_type,get_svr_type)=(%d, %s, %d, %d)*****time_use** is: %lf us \n", req.deviceid(), req.token().c_str(), req.svr_type(), req.get_svr_type(), time_use);
#endif 

			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSysDeviceInfo param is null error\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSysDeviceInfo ParsePartialFromArray failed.\n");
		}
		break;
	}
	/*
	case SRMsgId_ReqUpdateDeviceInfo:
	{
		SRMsgs::ReqUpdateDeviceInfo req;
		//if (req.ParsePartialFromArray(pData + 8, nLen - 8))
		if (req.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			//sr_printf(SR_LOGLEVEL_DEBUG, "SRMsgId_ReqUpdateDeviceInfo.\n");
			if (req.has_deviceid() && req.has_token() && req.has_svr_type())
			{

				sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_ReqUpdateDeviceInfo OK: param(deviceid,token,svr_type,update_devices_size)=(%d, %s, %d, %d)\n", req.deviceid(), req.token().c_str(), req.svr_type(), req.update_devices_size());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif

				SRMsgs::RspUpdateDeviceInfo rsp;
				//ȡ��
				for (int i = 0; i < req.update_devices_size(); i++)
				{
					int deviceid = req.update_devices(i).deviceid();
					int update_svr_type = req.update_devices(i).update_svr_type();

					char deviceid_s[128] = { 0 };
					sprintf(deviceid_s, "%d", deviceid);

					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqUpdateDeviceInfo OK: update_deviceid[%d] = %s, update_svr_type=%d\n", i, deviceid_s, update_svr_type);

					if (update_svr_type == DEVICE_SERVER::DEVICE_NETMP)
					{
						std::string netmp_id = "netmp_";
						if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
						{
							m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
							char * p = m_pRedisConnList[e_RC_TT_MainThread]->getvalue("dev_deviceidandgroupid", deviceid_s);

							//printf("SRMsgId_ReqUpdateDeviceInfo getvalue groupid redis db(3) from Table:dev_deviceidandgroupid (deviceid)=(%s)\n", deviceid_s);
							if (p == NULL)
							{
								SRMsgs::RspUpdateDeviceInfo_DeviceInfo *pdinfo = rsp.add_update_devices();
								pdinfo->set_deviceid(0);
								pdinfo->set_svr_type(update_svr_type);
								pdinfo->set_ip(0);
								pdinfo->set_port(0);
								pdinfo->set_load(0);
								pdinfo->set_load2(0);

								sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqUpdateDeviceInfo getvalue netmp_ groupid redis db(3) from Table:dev_deviceidandgroupid (deviceid)=(%s) return NULL error!!!\n", deviceid_s);
							}
							else
							{
								netmp_id += p;

								delete p; // ��ɾ���ᵼ���ڴ�й©

								unsigned long long load = 0;

								m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);
								int res = m_pRedisConnList[e_RC_TT_MainThread]->getdbdata(netmp_id.c_str(), deviceid_s, load);

#ifdef WIN32
								sr_printf(SR_LOGLEVEL_DEBUG, "SRMsgId_ReqUpdateDeviceInfo getdbdata redis db(1) netmp_groupid(%s), deviceid_s:%s, load num:%I64u, res:%d\n", netmp_id.c_str(), deviceid_s, load, res);
#elif defined LINUX
								sr_printf(SR_LOGLEVEL_DEBUG, "SRMsgId_ReqUpdateDeviceInfo getdbdata redis db(1) netmp_groupid(%s), deviceid_s:%s, load num:%llu, res:%d\n", netmp_id.c_str(), deviceid_s, load, res);
#endif
								
								if (res == 0)
									continue;

								m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);

								char *ptrRet = NULL;
								ptrRet = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue("deviceid_ipport", deviceid_s); // ��ȡ��key��Ӧ��value

								//printf("=== %s ====[%d] m_pRedis->gethashvalue =====-----------ptrRet----------->>> %p\n", __FILE__, __LINE__, ptrRet);

								if (ptrRet != NULL)
								{
									long long ullIPPort = 0;
#ifdef WIN32
									sscanf(ptrRet, "%I64d", &ullIPPort);
#elif defined LINUX
									sscanf(ptrRet, "%lld", &ullIPPort);
#endif

									//printf("=== %s ====[%d] m_pRedis->gethashvalue =====------ptrRet != NULL-----delete ptrRet---->>> %p\n", __FILE__, __LINE__, ptrRet);

									delete ptrRet; // ��ɾ���ᵼ���ڴ�й©

									SRMsgs::RspUpdateDeviceInfo_DeviceInfo *pdinfo = rsp.add_update_devices();
									pdinfo->set_deviceid(deviceid);
									pdinfo->set_svr_type(update_svr_type);

									int ip = (int)((long long)(ullIPPort) >> 32);
									int port = ((int)((long long)(ullIPPort)& 0xffffffff));
									pdinfo->set_ip(ip);
									pdinfo->set_port(port);

									int iTermNum = (int)((unsigned long long)(load) >> 32);
									int iLoad = ((int)((unsigned long long)(load)& 0xffffffff));

									pdinfo->set_load(iLoad);
									pdinfo->set_load2(iTermNum);

#ifdef WIN32
									sr_printf(SR_LOGLEVEL_DEBUG, "==SRMsgId_ReqUpdateDeviceInfo redis db(2)== deviceid_ipport ==key-> %d, ullIPPort = %I64d; redis db(1)==iTermNum = %d, iLoad = %d\n", deviceid, ullIPPort, iTermNum, iLoad);
#elif defined LINUX
									sr_printf(SR_LOGLEVEL_DEBUG, "==SRMsgId_ReqUpdateDeviceInfo redis db(2)== deviceid_ipport ==key-> %d, ullIPPort = %lld; redis db(1)==iTermNum = %d, iLoad = %d\n", deviceid, ullIPPort, iTermNum, iLoad);
#endif
								}
								else
								{
									SRMsgs::RspUpdateDeviceInfo_DeviceInfo *pdinfo = rsp.add_update_devices();
									pdinfo->set_deviceid(0);
									pdinfo->set_svr_type(update_svr_type);
									pdinfo->set_ip(0);
									pdinfo->set_port(0);
									pdinfo->set_load(0);
									pdinfo->set_load2(0);

									sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqUpdateDeviceInfo netmp redis db(2) gethashvalue deviceid_ipport (%s) ptrRet == NULL error !!!\n", deviceid_s);
								}

							}
						} // if (m_pRedis)
						else
						{
							SRMsgs::RspUpdateDeviceInfo_DeviceInfo *pdinfo = rsp.add_update_devices();
							pdinfo->set_deviceid(0);
							pdinfo->set_svr_type(update_svr_type);
							pdinfo->set_ip(0);
							pdinfo->set_port(0);
							pdinfo->set_load(0);
							pdinfo->set_load2(0);

							sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqUpdateDeviceInfo netmp m_pRedis == NULL error !!!\n");
						}
					} // if (update_svr_type == 2)
					else
					{
						SRMsgs::RspUpdateDeviceInfo_DeviceInfo *pdinfo = rsp.add_update_devices();
						pdinfo->set_deviceid(0);
						pdinfo->set_svr_type(update_svr_type);
						pdinfo->set_ip(0);
						pdinfo->set_port(0);
						pdinfo->set_load(0);
						pdinfo->set_load2(0);

						sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqUpdateDeviceInfo update_svr_type != 2 netmp error !!!\n");
					}

				}

#if 1
				rsp.set_token(req.token());

				if (pClient != NULL)
				{
					SerialProtoMsgAndSend(pClient, SRMsgId_RspUpdateDeviceInfo, &rsp);
				}
#endif

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "**Handle SRMsgId_ReqUpdateDeviceInfo param(deviceid,token,svr_type,update_devices_size)=(%d, %s, %d, %d)*****time_use** is: %lf us \n", req.deviceid(), req.token().c_str(), req.svr_type(), req.update_devices_size(), time_use);
#endif

			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqUpdateDeviceInfo param is null error.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqUpdateDeviceInfo ParsePartialFromArray failed.\n");
		}
		break;
	}
	*/
	case SRMsgId_ReqGetSystemCurLoad:
	{
		SRMsgs::ReqGetSystemCurLoad req;
		//if (req.ParsePartialFromArray(pData + 8, nLen - 8))
		if (req.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,%s(0x%x) --\n%s\n",
				req.deviceid(), req.GetTypeName().c_str(), (getMsgIdByClassName(ReqGetSystemCurLoad)), req.Utf8DebugString().c_str());

			if (req.has_deviceid() && req.has_token())
			{

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif

				SRMsgs::RspGetSystemCurLoad rsp;
				bool bGetOK = GetSystemCurLoad(&req, &rsp);

				if (bGetOK)
				{
					rsp.set_token(req.token());

					if (pClient != NULL)
					{
						SerialProtoMsgAndSend(pClient, SRMsgId_RspGetSystemCurLoad, &rsp);
					}
				}
				else
				{
					sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSystemCurLoad GetDeviceInfo return false\n");
				}

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "**Handle SRMsgId_ReqGetSystemCurLoad param(deviceid,token)=(%d, %s)*****time_use** is: %lf us \n", req.deviceid(), req.token().c_str(), time_use);
#endif 

			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSystemCurLoad param is null error\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSystemCurLoad ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndNewTermJoinConf:
	{
		SRMsgs::IndNewTermJoinConf ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndNewTermJoinConf)), ind.Utf8DebugString().c_str());

			if (ind.has_token() && ind.has_confid() && ind.has_alias() && ind.has_netmpid() && (ind.has_suid() || ind.has_ip()))
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_IndNewTermJoinConf OK: param(token=%s,conf_id=%"SR_PRIu64",suid=%d,ip=%s,alias=%s,netmpid=%d)\n", ind.token().c_str(), ind.confid(), ind.suid(), ind.ip().c_str(), ind.alias().c_str(), ind.netmpid());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif

				//�����ն˼��룬�������ݿ�
				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon+1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				bool busemsgcrptid = false;
				if (ind.has_confreportid())
				{
					if (ind.confreportid() != 0)
					{
						busemsgcrptid = true;
					}
				}

				//����user_conf_detail��
				if (busemsgcrptid)
				{
					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndNewTermJoinConf msg has_confreportid is %"SR_PRIu64".\n", ind.confreportid());
					AddUserConfDetail(ind.suid(), ind.confid(), ind.confreportid(), m_pub_userrpt_detail_id, (char*)(ind.alias().c_str()), szTime, timeNow, ind.fromtype(), ind.termtype());
				}
				else
				{
					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndNewTermJoinConf msg not have confreportid param or is null.\n");
					AddUserConfDetail(ind.suid(), ind.confid(), 0, m_pub_userrpt_detail_id, (char*)(ind.alias().c_str()), szTime, timeNow, ind.fromtype(), ind.termtype());
				}
				m_pub_userrpt_detail_id++;

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "****Handle SRMsgId_IndNewTermJoinConf param(suid=%d,conf_id=%lld,alias=%s)*****time_use** is: %lf us \n", ind.suid(), ind.confid(),(char*)(ind.alias().c_str()), time_use);
#endif 

			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndNewTermJoinConf param is null error.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndNewTermJoinConf ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndTermLeaveConf:
	{
		SRMsgs::IndTermLeaveConf ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndTermLeaveConf)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_token() && ind.has_confid() && ind.has_netmpid() && (ind.has_suid() || ind.has_ip()))
			{
				////�����ն��뿪���������ݿ�
				//sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_IndTermLeaveConf OK: param(deviceid=%d,token=%s,conf_id=%"SR_PRIu64",suid=%d,ip=%s,alias=%s,netmpid=%d)\n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.suid(), ind.ip().c_str(), ind.alias().c_str(), ind.netmpid());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif

				//�����ն˼��룬�������ݿ�
				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon+1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				//// ��ʱ��ʹ����Ϣ�ڵ�confreportid
				//bool busemsgcrptid = false;
				//if (ind.has_confreportid())
				//{
				//	if (ind.confreportid() != 0)
				//	{
				//		busemsgcrptid = true;
				//	}
				//}
				////����user_conf_detail��
				//if (busemsgcrptid)
				//{
				//	DelUserConfDetail(ind.suid(), ind.confid(), szTime, timeNow);
				//}
				//else
				//{
				//	DelUserConfDetail(ind.suid(), ind.confid(), szTime, timeNow);
				//}	

				DelUserConfDetail(ind.suid(), ind.confid(), szTime, timeNow);
			
#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "***Handle SRMsgId_IndTermLeaveConf param(suid=%d,conf_id=%lld,alias=%s)*****time_use** is: %lf us \n", ind.suid(), ind.confid(), (char*)(ind.alias().c_str()), time_use);
#endif 

			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndTermLeaveConf param is null error.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndTermLeaveConf ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndMCEndConf:
	{
		SRMsgs::IndMCEndConf ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndMCEndConf)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_confid() && ind.has_token())
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_IndMCEndConf OK: param(deviceid=%d,token=%s,conf_id=%"SR_PRIu64")\n", ind.deviceid(), ind.token().c_str(), ind.confid());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 

				//����������ʾ���������ݿ�
				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon+1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				bool busemsgcrptid = false;
				if (ind.has_confreportid())
				{
					if (ind.confreportid() != 0)
					{
						busemsgcrptid = true;
					}
				}

				UpdateConfRealtime(ind.confid(), 1, szTime, timeNow, ind.permanentenable()); // update conference set conf_realendtime

				// �����confreportidĿǰû��
				if (busemsgcrptid)
				{
					//����confreport��
					UpdateConfReport(ind.confid(), ind.confreportid(), szTime, timeNow);

				}
				else
				{
					//����confreport��
					UpdateConfReport(ind.confid(), 0, szTime, timeNow);
				}
				//ͬʱ����device_conf_detail
				UpdateDeviceConfDetail(ind.confid(), ind.deviceid(), szTime, timeNow);

				//ɾ��redis��db2
				// conference_deviceid,�����ŵ���[confid,deviceid]��ֵ��,��web server���룬ɾ������devmgrɾ��
				char bufconfid[128] = { 0 };
#ifdef WIN32
				sprintf(bufconfid, "%I64d", ind.confid());
#elif defined LINUX
				sprintf(bufconfid, "%lld", ind.confid());
#endif	

				m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);

				int iMcid = -1;
				char *ptrRet = NULL;
				ptrRet = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue("conference_deviceid", bufconfid); // ��ȡ��key��Ӧ��value
				if (ptrRet != NULL)
				{
					iMcid = atoi(ptrRet);
				}

				if (iMcid == ind.deviceid())
				{
					m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue("conference_deviceid", bufconfid);
					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndMCEndConf deletehashvalue redis db(2) Del Record confid =%s In Table:conference_deviceid\n", bufconfid);
				}
				else
				{
					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndMCEndConf msg mcid=%d not equle redis db(2) conference_deviceid [confid =%s, mcid=%d], so do nothing.\n", ind.deviceid(), bufconfid, iMcid);
				}
				
				//ɾ��db6				
				char deviceid_id[256] = { 0 };
				sprintf(deviceid_id, "deviceid_%d", ind.deviceid());
				char deviceid[256] = { 0 };
				sprintf(deviceid, "%d", ind.deviceid());
				char confid[256] = { 0 };
#ifdef WIN32
				sprintf(confid, "%I64d", ind.confid());
#elif defined LINUX
				sprintf(confid, "%lld", ind.confid());
#endif	
				iMcid = -1;
				ptrRet = NULL;
				ptrRet = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue(deviceid_id, confid); // ��ȡ��key��Ӧ��value
				if (ptrRet != NULL)
				{
					iMcid = atoi(ptrRet);
				}

				if (iMcid == ind.deviceid())
				{
					m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue(deviceid_id, confid);
					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndMCEndConf deletehashvalue redis db(2) Del Record confid =%s In Table:deviceid_id:%s\n", confid, deviceid_id);
				}
				else
				{
					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndMCEndConf msg mcid=%d not equle redis db(2) Table:deviceid_id:%s [confid =%s, mcid=%d], so do nothing.\n", ind.deviceid(), deviceid_id, confid, iMcid);
				}

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "**Handle SRMsgId_IndMCEndConf param(deviceid=%d,token=%s,conf_id=%lld)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.confid(), time_use);
#endif 
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndMCEndConf param is null error.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndMCCreateConf ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_ReqConfInfoFromDevMgr:
	{
#ifdef LINUX
		float time_use=0;
		struct timeval start;
		struct timeval end;
		gettimeofday(&start, NULL); // ��s level
#endif 
		SRMsgs::ReqConfInfoFromDevMgr req;
		//if (req.ParsePartialFromArray(pData + 8, nLen - 8))
		if (req.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				req.deviceid(), req.confid(), req.GetTypeName().c_str(), (getMsgIdByClassName(ReqConfInfoFromDevMgr)), req.Utf8DebugString().c_str());

			if (req.has_deviceid() && req.has_token() && req.has_confid())
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_ReqConfInfoFromDevMgr OK: param(deviceid,token,conf_id)=(%d, %s, %"SR_PRIu64")\n", req.deviceid(), req.token().c_str(), req.confid());

				if (req.deviceid() != 0
					&& req.confid() != 0)
				{
					if (pClient != NULL)
					{
						SRMsgs::RspConfInfoToMC rspsend;
						GetConfInfoFromDB(&req, pConference, pConfParticipant, pConfLiveSetting, pConfRollCallInfo, pRollCallList, pConfPollInfo, pPollList, &rspsend);
						SerialProtoMsgAndSend(pClient, SRMsgId_RspConfInfoToMC, &rspsend);
					}
					else
					{
						sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqConfInfoFromDevMgr pClient is null error\n");
					}
				} 
				else
				{
					sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqConfInfoFromDevMgr deviceid or confid is null error\n");
				}
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqConfInfoFromDevMgr param is null error.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqConfInfoFromDevMgr ParsePartialFromArray failed.\n");
		}

#ifdef LINUX
		gettimeofday(&end, NULL); // ��s level
		time_use = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);

		sr_printf(SR_LOGLEVEL_INFO, "***Handle SRMsgId_ReqConfInfoFromDevMgr param(deviceid,token,conf_id)=(%d, %s, %lld)*****time_use** is: %lf us \n", req.deviceid(), req.token().c_str(), req.confid(), time_use);
#endif 

		break;
	}
	case SRMsgId_IndMCCreateConf:
	{
		SRMsgs::IndMCCreateConf ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndMCCreateConf)), ind.Utf8DebugString().c_str());

			if (ind.has_isok() && ind.has_deviceid() && ind.has_token() && ind.has_confid())
			{

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 

				//���������������ݿ�
				if (ind.isok())
				{
					//��ȡ��ǰʱ�䴮
					time_t timeNow;
					struct tm *ptmNow;
					char szTime[30];
					timeNow = time(NULL);
					ptmNow = localtime(&timeNow);
					sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon+1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

					UpdateConfRealtime(ind.confid(), 0, szTime, timeNow); // update conference set conf_realstarttime

					bool busemsgcrptid = false;
					if (ind.has_confreportid())
					{
						if (ind.confreportid() != 0)
						{
							busemsgcrptid = true;
						}
					}

					if (busemsgcrptid)
					{
						// �˴�ʹ�õ���devmgr���ȷ���õ�confreportid
						InsertConfReport(ind.confid(), ind.confreportid(), szTime, timeNow);
						//ͬʱ����device_conf_detail
						InsertDeviceConfDetail(ind.confid(), ind.deviceid(), ind.confreportid(), m_pub_device_detail_id, szTime, timeNow); // ��device_conf_detail��Ϣд�ڴ棬���ڲ���device_conf_detail��
					}
					else
					{
						InsertConfReport(ind.confid(), m_pub_confreport_id, szTime, timeNow); // ��confreport��Ϣд�ڴ棬���ڲ���confreport��
						//ͬʱ����device_conf_detail
						InsertDeviceConfDetail(ind.confid(), ind.deviceid(), m_pub_confreport_id, m_pub_device_detail_id, szTime, timeNow); // ��device_conf_detail��Ϣд�ڴ棬���ڲ���device_conf_detail��

						m_pub_confreport_id++;
					}
					m_pub_device_detail_id++;

					//����redis
					char deviceid_id[256] = { 0 };
					sprintf(deviceid_id, "deviceid_%d", ind.deviceid());
					char deviceid[256] = { 0 };
					sprintf(deviceid, "%d", ind.deviceid());
					char confid[256] = { 0 };
#ifdef WIN32
					sprintf(confid, "%I64d", ind.confid());
#elif defined LINUX
					sprintf(confid, "%lld", ind.confid());
#endif	

					m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);
					m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(deviceid_id, deviceid, confid);

					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndMCCreateConf update redis db(2) Table:deviceid_id:%s (confid,deviceid)=(%s, %s)\n", deviceid_id, confid, deviceid);

				}


#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "***Handle SRMsgId_IndMCCreateConf param(deviceid,token,conf_id,isok)=(%d, %s, %lld, %d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.isok(), time_use);
#endif 
			}
			else if (!ind.has_isok() && ind.has_failreason())
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndMCCreateConf param is fail.\n");
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndMCCreateConf param is null error.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndMCCreateConf ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndsertodevHeart:
	{
		SRMsgs::IndsertodevHeart ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			//sr_printf(SR_LOGLEVEL_DEBUG, "xxx svr->devmgr,deviceid=%u,%s(0x%x) --\n%s\n",
			//	ind.deviceid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndsertodevHeart)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_token() && ind.has_svr_type())
			{

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 

                time_t timeNow;
                timeNow = time(NULL);

				sr_printf(SR_LOGLEVEL_NORMAL, "********SRMsgId_IndsertodevHeart OK (deviceid, timeNow)=(%d, %"SR_PRIu64")\n", ind.deviceid(), timeNow);

				//��������������ݿ�
				UpdatePeerHeartbeatToDB(&ind, timeNow);

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "***Handle SRMsgId_IndsertodevHeart param(deviceid = %d)*****time_use** is: %lf us \n", ind.deviceid(), time_use);
#endif 
				
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndsertodevHeart param is null error.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndsertodevHeart ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndNetMPConfInfoInMC:
	{
		SRMsgs::IndNetMPConfInfoInMC ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndNetMPConfInfoInMC)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_token() && ind.has_confid() && ind.has_netmpid() && ind.has_addordel())
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_IndNetMPConfInfoInMC OK: param(deviceid,token,confid,netmpid,addordel)=(%d, %s, %I64d, %d, %d)\n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.netmpid(), ind.addordel());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif

				//printf("SRMsgId_IndNetMPConfInfoInMC OK: param(deviceid,token,conf_id,netmpid,addordel)=(%d, %s, %lld, %d, %d)\n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.netmpid(), ind.addordel());

				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				//printf("###SRMsgId_IndNetMPConfInfoInMC OK: Updata CDeviceConfDetail --===-->>\n");

				bool busemsgcrptid = false;
				if (ind.has_confreportid())
				{
					if (ind.confreportid() != 0)
					{
						busemsgcrptid = true;
					}
				}

				//����device_conf_detail
				pDeviceConfDetail->SetConfID(ind.confid());
				pDeviceConfDetail->SetDeviceID(ind.netmpid());
				//if (pDeviceConfDetail->SelectDB())
				{
					if (ind.addordel() == 2)
					{
						UpdateDeviceConfDetail(ind.confid(), ind.netmpid(), szTime, timeNow);

                        //ɾ��db6				
                        char deviceid_id[256] = { 0 };
                        sprintf(deviceid_id, "deviceid_%d", ind.netmpid());
                        char deviceid[256] = { 0 };
                        sprintf(deviceid, "%d", ind.netmpid());
                        char confid[256] = { 0 };
#ifdef WIN32
                        sprintf(confid, "%I64d", ind.confid());
#elif defined LINUX
                        sprintf(confid, "%lld", ind.confid());
#endif	

						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);
						m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue(deviceid_id, confid);


						sr_printf(SR_LOGLEVEL_DEBUG, " -2- update mysql::device_conf_detail set realuse_stoptime and deletehashvalue redis db(2) del record confid =%s In Table:deviceid_id:%s\n", confid, deviceid_id);
					}
				}
				//else
				{
					if (ind.addordel() == 1)
					{
						if (busemsgcrptid)
						{
							InsertDeviceConfDetail(ind.confid(), ind.netmpid(), ind.confreportid(), m_pub_device_detail_id, szTime, timeNow, true);
						}
						else
						{
							InsertDeviceConfDetail(ind.confid(), ind.netmpid(), 0, m_pub_device_detail_id, szTime, timeNow, true);
						}

						m_pub_device_detail_id++;

                        //����redis
                        char deviceid_id[256] = { 0 };
                        sprintf(deviceid_id, "deviceid_%d", ind.netmpid());
                        char deviceid[256] = { 0 };
                        sprintf(deviceid, "%d", ind.netmpid());
                        char confid[256] = { 0 };
#ifdef WIN32
                        sprintf(confid, "%I64d", ind.confid());
#elif defined LINUX
                        sprintf(confid, "%lld", ind.confid());
#endif	

						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);
						m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(deviceid_id, deviceid, confid);

						sr_printf(SR_LOGLEVEL_DEBUG, " -1- insert into mysql::device_conf_detail and sethashvalue redis db(2) In Table:deviceid_id:%s, add record (confid =%s, deviceid=%s)\n", deviceid_id, confid, deviceid);
					}
				}

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle SRMsgId_IndNetMPConfInfoInMC param(deviceid,token,conf_id,netmpid,addordel)=(%d, %s, %lld, %d, %d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.netmpid(), ind.addordel(), time_use);
#endif 

			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndNetMPConfInfoInMC param is null error.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndNetMPConfInfoInMC ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndNetMPConnStatusInMC:
	{
		/*
		SRMsgs::IndNetMPConnStatusInMC ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			if (ind.has_deviceid() && ind.has_token() && ind.has_netmpid() && ind.has_connstatus() && ind.has_groupid())
			{
				sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_IndNetMPConnStatusInMC OK: param(deviceid,token,netmpid,connstatus,groupid)=(%d, %s, %d, %d, %d)\n", ind.deviceid(), ind.token().c_str(), ind.netmpid(), ind.connstatus(), ind.groupid());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif

				char mcid_s[128] = { 0 };
				sprintf(mcid_s, "%d", ind.deviceid());
				char netmpid_s[128] = { 0 };
				sprintf(netmpid_s, "%d", ind.netmpid());
				char netmpid_beconn_mc_netmpgroupid_s[128] = { 0 };
				sprintf(netmpid_beconn_mc_netmpgroupid_s, "netmp_beconn_mc_%d", ind.groupid());

				char *ptrBeconnMCstr = NULL;
				// �Ȳ���ʷ��¼,��netmpid_beconn_mc_netmpgroupid_s��netmpid_s������,���صĶ���NULL
				m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
				ptrBeconnMCstr = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue(netmpid_beconn_mc_netmpgroupid_s, netmpid_s);

				// ��netmp��ĳmc�Ͽ�����
				if (ind.connstatus() == 0)
				{
					// ����ʷ��¼��ɾ����Ӧmc��¼
					if (ptrBeconnMCstr != NULL)
					{
						char strmcids_s[11 * 200] = { 0 };						

						// ����mcid��ptrBeconnMCstr�е��׵�ַ
						char *tempptr = strstr(ptrBeconnMCstr, mcid_s);
						
						//if (strstr(ptrBeconnMCstr, mcid_s) != NULL)
						if (tempptr != NULL)
						{
							vector<std::string> vMCidStr;
							std::string srcstrmcids;
							srcstrmcids.clear();
							srcstrmcids = ptrBeconnMCstr;

							char *sep = { "#" };

							std::string str;
							str.clear();
							int nend = 0, nbegin = 0, lensep = strlen(sep);
							while (nend != -1)
							{
								nend = srcstrmcids.find(sep, nbegin);  //string��nbegin����sep1,û�ҵ��ͷ���-1,�ҵ��ͷ��ص�һ������  
								if (nend == -1)
									str = srcstrmcids.substr(nbegin, srcstrmcids.length() - nbegin); //û�зָ����ˣ��ͽ�ȡ���һ��  
								else
									str = srcstrmcids.substr(nbegin, nend - nbegin);

								if (str != "")
									vMCidStr.push_back(str);

								nbegin = nend + lensep;  //�����µ���ʼ����λ�� 
							}

							printf("vMCidStr.size=%d\n", vMCidStr.size());

							std::string strtemp;
							strtemp.clear();
							strtemp = mcid_s;
							for (vector<std::string>::iterator iter = vMCidStr.begin(); iter != vMCidStr.end(); iter++)
							{
								printf("vMCidStr-->>%s\n", (*iter).c_str());
								
								if (strtemp != (*iter))
								{									
									strcat(strmcids_s, mcid_s);
								}
								else
								{
									continue;
								}

								strcat(strmcids_s, "#");
							}

							// ��mc����ʷ��¼����,ɾ����Ӧmc��¼
							
							//if (strcmp(strmcids_s, "") == 0)
							if (strlen(strmcids_s) == 0)
							{
								m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue(netmpid_beconn_mc_netmpgroupid_s, netmpid_s);
								sr_printf(SR_LOGLEVEL_INFO, " SRMsgId_IndNetMPConnStatusInMC netmpid_s=%s disconn(0) mcid_s=%s and mcidsize is 0 so del redis [netmpid_beconn_mc_netmpgroupid_s=%s, netmpid_s=%s].\n", netmpid_s, mcid_s, netmpid_beconn_mc_netmpgroupid_s, netmpid_s);
							} 
							else
							{
								m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(netmpid_beconn_mc_netmpgroupid_s, strmcids_s, netmpid_s);
								sr_printf(SR_LOGLEVEL_INFO, " SRMsgId_IndNetMPConnStatusInMC netmpid_s=%s disconn(0) mcid_s=%s then updata redis [netmpid_beconn_mc_netmpgroupid_s=%s, netmpid_s=%s, strmcids_s=%s].\n", netmpid_s, mcid_s, netmpid_beconn_mc_netmpgroupid_s, netmpid_s, strmcids_s);
							}							
						}
						else
						{
							// ���� ��mc�Ѿ�������ʷ��¼����
							sr_printf(SR_LOGLEVEL_WARNING, " SRMsgId_IndNetMPConnStatusInMC netmpid_s=%s disconn(0) mcid_s=%s but this mc not in redis [netmpid_beconn_mc_netmpgroupid_s=%s, netmpid_s=%s].\n", netmpid_s, mcid_s, netmpid_beconn_mc_netmpgroupid_s, netmpid_s);
						}
					}
				}
				else
				{
					// ��netmp��ĳmc���ӳɹ�

					char strmcids_s[11*200] = { 0 };

					// ������ʷ��¼�Ƿ��и�mc�����û��������mc
					if (ptrBeconnMCstr != NULL)
					{
						// ��mc�Ѿ�����ʷ��¼����
						if (strstr(ptrBeconnMCstr, mcid_s) != NULL)
						{
							// ����
							sr_printf(SR_LOGLEVEL_WARNING, " SRMsgId_IndNetMPConnStatusInMC netmpid_s=%s conn(1) mcid_s=%s but this mc already in redis [netmpid_beconn_mc_netmpgroupid_s=%s, netmpid_s=%s].\n", netmpid_s, mcid_s, netmpid_beconn_mc_netmpgroupid_s, netmpid_s);
						}
						else
						{
							// ��mc������ʷ��¼����,ƴ�Ӳ���
							strcat(strmcids_s, ptrBeconnMCstr);
							strcat(strmcids_s, "#");
							strcat(strmcids_s, mcid_s);
							strcat(strmcids_s, "#");
							m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(netmpid_beconn_mc_netmpgroupid_s, strmcids_s, netmpid_s);

							sr_printf(SR_LOGLEVEL_INFO, " 22 SRMsgId_IndNetMPConnStatusInMC netmpid_s=%s conn(1) mcid_s=%s then strcat new mcid insert redis [netmpid_beconn_mc_netmpgroupid_s=%s, netmpid_s=%s, strmcids_s=%s].\n", netmpid_s, mcid_s, netmpid_beconn_mc_netmpgroupid_s, netmpid_s, strmcids_s);
						}
					}
					else
					{
						// �˴�����ȱ��,�п��ܻὫԭ��¼�ַ�������
						strcat(strmcids_s, mcid_s);
						strcat(strmcids_s, "#");
						m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(netmpid_beconn_mc_netmpgroupid_s, strmcids_s, netmpid_s);

						sr_printf(SR_LOGLEVEL_INFO, " 11 SRMsgId_IndNetMPConnStatusInMC netmpid_s=%s conn(1) mcid_s=%s then insert redis [netmpid_beconn_mc_netmpgroupid_s=%s, netmpid_s=%s, strmcids_s=%s].\n", netmpid_s, mcid_s, netmpid_beconn_mc_netmpgroupid_s, netmpid_s, strmcids_s);
					}
				}

				if (ptrBeconnMCstr != NULL)
				{
					delete ptrBeconnMCstr; // ��ɾ���ᵼ���ڴ�й©
				}

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle SRMsgId_IndNetMPConnStatusInMC param(deviceid,token,netmpid,connstatus,groupid)=(%d, %s, %d, %d, %d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.netmpid(), ind.connstatus(), ind.groupid(), time_use);
#endif
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndNetMPConnStatusInMC ParsePartialFromArray failed.\n");
		}

		*/

		break;
	}
	case SRMsgId_IndCRSStartRec:
	{
		SRMsgs::IndCRSStartRec ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndCRSStartRec)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_token() && ind.has_confid())
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_IndCRSStartRec OK: param(deviceid,token,conf_id, crsid)=(%d, %s, %"SR_PRIu64", %d)\n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.crsid());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 

				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);


				//char filestorsvrip[128];
				//memset(filestorsvrip, 0, sizeof(filestorsvrip));
				//char filestorpath[300];
				//memset(filestorpath, 0, sizeof(filestorpath));
				//char sdefilepath[300];
				//memset(sdefilepath, 0, sizeof(sdefilepath));
				//char relativepath[300];
				//memset(relativepath, 0, sizeof(relativepath));
				////sprintf(filestorsvrip, "%s", ind.filestorsvrip().c_str());
				////sprintf(filestorpath, "%s", ind.filestorpath().c_str());
				////sprintf(sdefilepath, "%s", ind.sdefilepath().c_str());
				//// ���ܸ�ʽ������ʽ�����׽�·��˫б��\\������ת���ַ�
				//memcpy(filestorsvrip, ind.filestorsvrip().c_str(), ind.filestorsvrip().length());
				//sprintf(filestorpath, ind.filestorpath().c_str(), ind.filestorpath().length());
				//sprintf(sdefilepath, ind.sdefilepath().c_str(), ind.sdefilepath().length());
				//sprintf(relativepath, ind.relativepath().c_str(), ind.relativepath().length());

				//sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_IndCRSStartRec param 1: filestorsvrip=%s, filestorpath=%s, sdefilepath=%s, relativepath=%s.\n", ind.filestorsvrip().c_str(), ind.filestorpath().c_str(), ind.sdefilepath().c_str(), ind.relativepath().c_str());

				//sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_IndCRSStartRec param 2: filestorsvrip=%s, filestorpath=%s, sdefilepath=%s, relativepath=%s.\n", filestorsvrip, filestorpath, sdefilepath, relativepath);
				//
				char strconfname[256];
				memset(strconfname, 0, sizeof(strconfname));
				sprintf(strconfname, "%s", ind.confname().c_str());

				m_pub_confrecord_id++;
				//m_pub_recordfile_id++;

				bool busemsgcrptid = false;
				if (ind.has_confreportid())
				{
					if (ind.confreportid() != 0)
					{
						busemsgcrptid = true;
					}
				}

				if (busemsgcrptid)
				{
					InsertConfRecord(ind.confid(), ind.confreportid(), m_pub_confrecord_id, szTime, timeNow, strconfname); // confreportidȡ0��ʾ��Ҫ��redisȡ��Ӧ�����confreportidֵ,���Ҳ����ʹ�mysql��
					//InsertRecordFile(ind.confid(), ind.confreportid(), m_pub_confrecord_id, m_pub_recordfile_id, filestorsvrip, relativepath, filestorpath, sdefilepath, timeNow);
				}
				else
				{
					InsertConfRecord(ind.confid(), 0, m_pub_confrecord_id, szTime, timeNow, strconfname); // confreportidȡ0��ʾ��Ҫ��redisȡ��Ӧ�����confreportidֵ,���Ҳ����ʹ�mysql��
					//InsertRecordFile(ind.confid(), 0, m_pub_confrecord_id, m_pub_recordfile_id, filestorsvrip, relativepath, filestorpath, sdefilepath, timeNow);
				}

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle SRMsgId_IndCRSStartRec param(deviceid,token,conf_id, crsid)==(%d, %s, %lld, %d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.crsid(), time_use);
#endif 
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndCRSStartRec param is fail.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndCRSStartRec ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndCRSStopRec:
	{
		SRMsgs::IndCRSStopRec ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndCRSStopRec)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_token() && ind.has_confid() /*&& ind.has_stopreason()*/)
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "IndCRSStopRec OK: param(deviceid,token,conf_id, crsid)=(%d, %s, %"SR_PRIu64", %d)\n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.crsid());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 

				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				char filestorsvrip[128];
				memset(filestorsvrip, 0, sizeof(filestorsvrip));
				char filestorpath[300];
				memset(filestorpath, 0, sizeof(filestorpath));
				char sdefilepath[300];
				memset(sdefilepath, 0, sizeof(sdefilepath));
				char relativepath[300];
				memset(relativepath, 0, sizeof(relativepath));
				//sprintf(filestorsvrip, "%s", ind.filestorsvrip().c_str());
				//sprintf(filestorpath, "%s", ind.filestorpath().c_str());
				//sprintf(sdefilepath, "%s", ind.sdefilepath().c_str());
				// ���ܸ�ʽ������ʽ�����׽�·��˫б��\\������ת���ַ�
				memcpy(filestorsvrip, ind.filestorsvrip().c_str(), ind.filestorsvrip().length());
				sprintf(filestorpath, ind.filestorpath().c_str(), ind.filestorpath().length());
				sprintf(sdefilepath, ind.sdefilepath().c_str(), ind.sdefilepath().length());
				sprintf(relativepath, ind.relativepath().c_str(), ind.relativepath().length());

				//if (ind.stopreason() == 0) // �쳣(���������ʱ������Ǩ��ʧ��)����¼�ƣ�
				//{
				//	// �û������д洢�ļ�recordfile����ʱ��Ϊ1970-------�Ľ���ʱ��
				//} 
				//else if (ind.stopreason() == 1) // ¼����������¼��
				//{
				//	// ֻ����ָ���ļ�recordfile����ʱ��Ϊ1970-------�Ľ���ʱ��
				//}

				m_pub_recordfile_id++;

				char strconfname[256];
				memset(strconfname, 0, sizeof(strconfname));
				sprintf(strconfname, "%s", ind.confname().c_str());

				bool busemsgcrptid = false;
				if (ind.has_confreportid())
				{
					if (ind.confreportid() != 0)
					{
						busemsgcrptid = true;
					}
				}

				bool bneedinsert = false;
				if (ind.filestorsvrip().length() > 0
					&& ind.filestorpath().length() > 0
					//&& ind.sdefilepath().length() > 0
					//&& ind.relativepath().length() > 0
					&& ind.stopreason() == 1)
				{
					bneedinsert = true;
				}

				if (busemsgcrptid)
				{
					if (bneedinsert)
					{
						InsertRecordFile(ind.confid(), ind.confreportid(), 0, m_pub_recordfile_id, filestorsvrip, relativepath, filestorpath, sdefilepath, timeNow, ind.filesize());
					}

					UpdateConfRecord(ind.confid(), ind.confreportid(), szTime, timeNow, strconfname);
				}
				else
				{
					if (bneedinsert)
					{
						InsertRecordFile(ind.confid(), 0, 0, m_pub_recordfile_id, filestorsvrip, relativepath, filestorpath, sdefilepath, timeNow, ind.filesize());
					}

					UpdateConfRecord(ind.confid(), 0, szTime, timeNow, strconfname);
				}

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle IndCRSStopRec param(deviceid,token,conf_id, crsid)==(%d, %s, %lld, %d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.crsid(), time_use);
#endif 
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndCRSStopRec param is fail.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndCRSStopRec ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndCRSFileStoragePath:
	{
		SRMsgs::IndCRSFileStoragePath ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndCRSFileStoragePath)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_token() && ind.has_confid() && ind.has_recordstate())
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "IndCRSFileStoragePath OK: param(deviceid,token,conf_id, crsid,recordstate)=(%d, %s, %"SR_PRIu64", %d,%d)\n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.crsid(), ind.recordstate());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 

				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				char filestorsvrip[128];
				memset(filestorsvrip, 0, sizeof(filestorsvrip));
				char filestorpath[300];
				memset(filestorpath, 0, sizeof(filestorpath));
				char sdefilepath[300];
				memset(sdefilepath, 0, sizeof(sdefilepath));
				char relativepath[300];
				memset(relativepath, 0, sizeof(relativepath));
				//sprintf(filestorsvrip, "%s", ind.filestorsvrip().c_str());
				//sprintf(filestorpath, "%s", ind.filestorpath().c_str());
				//sprintf(sdefilepath, "%s", ind.sdefilepath().c_str());
				// ���ܸ�ʽ������ʽ�����׽�·��˫б��\\������ת���ַ�
				memcpy(filestorsvrip, ind.filestorsvrip().c_str(), ind.filestorsvrip().length());
				sprintf(filestorpath, ind.filestorpath().c_str(), ind.filestorpath().length());
				sprintf(sdefilepath, ind.sdefilepath().c_str(), ind.sdefilepath().length());
				sprintf(relativepath, ind.relativepath().c_str(), ind.relativepath().length());

				//if (ind.recordstate() == 1) // ��ʼ
				//{
				//	m_pub_recordfile_id++;
				//	InsertRecordFile(ind.confid(), 0, 0, m_pub_recordfile_id, filestorsvrip, relativepath, filestorpath, sdefilepath, timeNow);// ͬһ������ٿ�ʼ/ֹͣ¼�ƣ�Recordid��Ҫ��redis�л�ȡ���µ�,���Դ˴�����Ϊ0
				//} 
				//else if (ind.recordstate() == 2) // ����
				//{
				//	// ����ָ���ļ�recordfile����ʱ��Ϊ1970-------�Ľ���ʱ��
				//}

				m_pub_recordfile_id++;

				bool busemsgcrptid = false;
				if (ind.has_confreportid())
				{
					if (ind.confreportid() != 0)
					{
						busemsgcrptid = true;
					}
				}

				if (busemsgcrptid)
				{
					InsertRecordFile(ind.confid(), ind.confreportid(), 0, m_pub_recordfile_id, filestorsvrip, relativepath, filestorpath, sdefilepath, timeNow, ind.filesize());// ͬһ������ٿ�ʼ/ֹͣ¼�ƣ�Recordid��Ҫ��redis�л�ȡ���µ�,���Դ˴�����Ϊ0
				}
				else
				{
					InsertRecordFile(ind.confid(), 0, 0, m_pub_recordfile_id, filestorsvrip, relativepath, filestorpath, sdefilepath, timeNow, ind.filesize());// ͬһ������ٿ�ʼ/ֹͣ¼�ƣ�Recordid��Ҫ��redis�л�ȡ���µ�,���Դ˴�����Ϊ0
				}

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle IndCRSFileStoragePath param(deviceid,token,conf_id, crsid,recordstate)=(%d, %s, %lld, %d,%d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.crsid(), ind.recordstate(), time_use);
#endif 
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndCRSFileStoragePath param is fail.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndCRSFileStoragePath ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndCRSStartLive:
	{
		SRMsgs::IndCRSStartLive ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndCRSStartLive)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_token() && ind.has_confid() && ind.has_livepullurl() && ind.has_liveplayurl())
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "IndCRSStartLive OK: param(deviceid,token,conf_id, crsid)=(%d, %s, %"SR_PRIu64", %d)\n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.crsid());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 

				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				m_pub_liveinfo_id++;
				InsertConfLiveInfo(m_pub_liveinfo_id, &ind, szTime, timeNow);

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle IndCRSStartLive param(deviceid,token,conf_id, crsid)=(%d, %s, %lld, %d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.crsid(), time_use);
#endif 
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndCRSStartLive param is fail.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndCRSStartLive ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndCRSStopLive:
	{
		SRMsgs::IndCRSStopLive ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndCRSStopLive)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_token() && ind.has_confid() && ind.has_liveurl())
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "IndCRSStopLive OK: param(deviceid,token,conf_id, crsid)=(%d, %s, %"SR_PRIu64", %d)\n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.crsid());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 

				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				UpdateConfLiveInfo(&ind, szTime, timeNow);

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle IndCRSStopLive param(deviceid,token,conf_id, crsid)=(%d, %s, %lld, %d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.crsid(), time_use);
#endif 
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndCRSStopLive param is fail.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndCRSStopLive ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndModifyConfCallList:
	{
		SRMsgs::IndModifyConfCallList ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndModifyConfCallList)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_token() && ind.has_confid() && ind.has_optype() && ind.has_rollcalllist())
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "IndModifyConfCallList OK: param(deviceid,token,conf_id, optype)=(%d, %s, %"SR_PRIu64", %d)\n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.optype());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 

				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				ModifyConfCallListInfo(&ind, szTime, timeNow);

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle IndModifyConfCallList param(deviceid,token,conf_id, optype)=(%d, %s, %lld, %d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.confid(), ind.optype(), time_use);
#endif 
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndModifyConfCallList param is fail.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndModifyConfCallList ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndTerStatisticsInfo:
	{
		SRMsgs::IndTerStatisticsInfo ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			sr_printf(SR_LOGLEVEL_NORMAL, "mc->devmgr,deviceid=%u,confid=%"SR_PRIu64",%s(0x%x) --\n%s\n",
				ind.deviceid(), ind.confid(), ind.GetTypeName().c_str(), (getMsgIdByClassName(IndTerStatisticsInfo)), ind.Utf8DebugString().c_str());

			if (ind.has_deviceid() && ind.has_token() && ind.has_confid())
			{
				//sr_printf(SR_LOGLEVEL_NORMAL, "IndTerStatisticsInfo OK: param(deviceid,token,conf_id)=(%d, %s, %"SR_PRIu64")\n", ind.deviceid(), ind.token().c_str(), ind.confid());

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 

				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				UpdateTerStatisticsInfo(&ind, szTime, timeNow);

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle IndTerStatisticsInfo param(deviceid,token,conf_id)=(%d, %s, %lld)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.confid(), time_use);
#endif 
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndTerStatisticsInfo param is fail.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndTerStatisticsInfo ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndSubSvrInfoToDevmgr:
	{
		SRMsgs::IndSubSvrInfoToDevmgr ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			if (ind.has_deviceid() && ind.has_token() && ind.has_addordel())
			{
				sr_printf(SR_LOGLEVEL_NORMAL, "IndSubSvrInfoToDevmgr OK: param(deviceid,token,addordel)=(%d, %s, %d)\n", ind.deviceid(), ind.token().c_str(), ind.addordel());
#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 
				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				SyncSubSvrInfo(&ind, timeNow);

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle IndSubSvrInfoToDevmgr param(deviceid,token,addordel)=(%d, %s, %d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.addordel(), time_use);
#endif 
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndSubSvrInfoToDevmgr param is fail.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndSubSvrInfoToDevmgr ParsePartialFromArray failed.\n");
		}
		break;
	}
	case SRMsgId_IndSubSvrHeartTodev:
	{
		SRMsgs::IndSubSvrHeartTodev ind;
		//if (ind.ParsePartialFromArray(pData + 8, nLen - 8))
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			if (ind.has_deviceid() && ind.has_token())
			{
				sr_printf(SR_LOGLEVEL_NORMAL, "IndSubSvrHeartTodev OK: param(deviceid,token)=(%d, %s)\n", ind.deviceid(), ind.token().c_str());
#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif 
				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				UpdateSubSvrHeartbeatToDB(&ind, timeNow);

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "Handle IndSubSvrHeartTodev param(deviceid,token)=(%d, %s)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), time_use);
#endif 
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndSubSvrHeartTodev param is fail.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndSubSvrHeartTodev ParsePartialFromArray failed.\n");
		}
		break;
	}
	}//switch (header.m_msguid)
	return true;
}

void CDevMgr::OnRegisterSelf()
{
	if (!m_pMainThread)
		return;

	typedef void (CDevMgr::* ACTION)(void*);
	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
		*this, &CDevMgr::Handle_RegisterSelf));
	return;
}

void CDevMgr::Handle_RegisterSelf(void *pArg)
{
	if (CParser::GetInstance()->GetMyHTTPListenPort() == 0)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "%s HTTPListenPort=%d error, so register self again.\n", __FUNCTION__, CParser::GetInstance()->GetMyHTTPListenPort());

		createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_register_self_timer, 0ull);
		return;
	}

	SRMsgs::ReqRegister selfregister;
	struct in_addr addr;
	if (0 == inet_pton(AF_INET, CParser::GetInstance()->GetMyListenIpAddr().c_str(), &addr))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "%s inet_pton MyListenIpAddr=%s error, so register self again.\n", __FUNCTION__, CParser::GetInstance()->GetMyListenIpAddr().c_str());

		createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_register_self_timer, 0ull);
		return;
	}
	selfregister.set_auth_password(CParser::GetInstance()->GetSelfRegSerialNO());
	selfregister.set_svr_type(DEVICE_SERVER::DEVICE_DEV);// 
	selfregister.set_ip(addr.s_addr);
	selfregister.set_port(CParser::GetInstance()->GetMyListenPort());
	selfregister.set_cpunums(4);
	selfregister.set_version(m_softwareversion);

	m_bRegSelfok = RegisterSelf(&selfregister, m_pMainDeviceConfig, m_pMainDevice);

	if (!m_bRegSelfok)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "%s RegisterSelf error, so register self again.\n", __FUNCTION__);

		createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_register_self_timer, 0ull);
		return;
	}
	else
	{
		m_iSelfDeviceid = m_pMainDevice->GetDeviceID();
		if (m_iSelfDeviceid != 0)
		{
			sr_printf(SR_LOGLEVEL_INFO, "%s RegisterSelf ok and m_iSelfDeviceid is:%d\n", __FUNCTION__, m_iSelfDeviceid);

			time_t timeNow;
			timeNow = time(NULL);
			UpdateSelfHeartbeatToDB(m_iSelfDeviceid, timeNow);
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "%s RegisterSelf ok but m_iSelfDeviceid is 0, so register self again.\n", __FUNCTION__);

			m_bRegSelfok = false;
			createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_register_self_timer, 0ull);
			return;
		}
	}
}

bool CDevMgr::RegisterSelf(const SRMsgs::ReqRegister* req, CDeviceConfig* pDeviceConfig, CDevice* pDevice)
{
	struct in_addr inIP;
#ifdef WIN32
	inIP.S_un.S_addr = req->ip();
#elif defined LINUX
	inIP.s_addr = req->ip();
#endif

	char reg_msg_IP_s[16];
	memset(reg_msg_IP_s, 0, sizeof(reg_msg_IP_s));
	strcpy(reg_msg_IP_s, inet_ntoa(inIP));

	// �������ip
	char selfIP_m[16];
	memset(selfIP_m, 0, sizeof(selfIP_m));

	strcpy(selfIP_m, reg_msg_IP_s);// ǿ��ʹ�������ļ���������ַ

	struct in_addr ipaddr_in;

	if (0 == inet_pton(AF_INET, selfIP_m, &ipaddr_in))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "%s:%d inet_pton error\n", __FUNCTION__, __LINE__);
		return false;
	}

	std::string strdevdomainname;
	strdevdomainname.clear();

	sr_printf(SR_LOGLEVEL_NORMAL, "RegisterSelf OK: param(device_config_serial,svr_type,ip,port)=(%s, %d, %s, %d)\n", req->auth_password().c_str(), req->svr_type(), reg_msg_IP_s, req->port());

	bool bRegister = false;
	if (m_pRedisConnList[e_RC_TT_MainThread]->isconnectok() == false)
	{
		return false;
	}

	pDeviceConfig->SetDeviceConfigSerial(req->auth_password());
	pDeviceConfig->SetDeviceConfigType(req->svr_type());
	//printf("==RegisterSelf==select * from device_config--DeviceConfigSerial-->%s, ==device_config_type-->%d\n", req.auth_password().c_str(), req.svr_type());
	if (!pDeviceConfig->SelectDB())
	{
		sr_printf(SR_LOGLEVEL_ERROR, "RegisterSelf CDeviceConfig::SelectDB() serial= %s and type = %d is not exists.\n", req->auth_password().c_str(), req->svr_type());

		return false;
	}
	else
	{
		sr_printf(SR_LOGLEVEL_NORMAL, "check self ip:%s port:%d exist in device_config table.\n", selfIP_m, req->port());

		pDevice->SetDeviceConfigSerial(req->auth_password());

		//�޸ĸ�serverע��״̬�����������ظ����ƣ�����tokenֵ���������ݿ�
		pDevice->SetDeviceIP(selfIP_m);
		pDevice->SetDevicePort(req->port());

		if (pDevice->SelectDB1())
		{
			//���ڣ��򷵻�ע�����
			sr_printf(SR_LOGLEVEL_INFO, "RegisterSelf pDevice->SelectDB1() select * from device device_config_serial: %s exist, DeviceIP=%s, DevicePort=%d\n", req->auth_password().c_str(), selfIP_m, req->port());

			//���±���ֵ
			if (req->has_max_bandwidth())
			{
				pDevice->SetMaxBandwidth(req->max_bandwidth());
			}
			pDevice->SetStatus(1);
			if (req->has_systemlicence())
			{
				pDevice->SetSystemLicence(req->systemlicence());
			}
			if (req->has_version())
			{
				pDevice->SetVersion(req->version());
			}
			if (!pDevice->UpdateDB1())
			{
				sr_printf(SR_LOGLEVEL_ERROR, "RegisterSelf pDevice->UpdateDB1(%s)  error!!!\n", req->auth_password().c_str());

				return false;
			}
			else
			{
				sr_printf(SR_LOGLEVEL_INFO, "RegisterSelf pDevice->UpdateDB1(%s) and set max_bandwidth, status OK!!!!!\n", req->auth_password().c_str());

				int mediagroup_id = pDeviceConfig->GetMediaGroupID();
				//����redis
				if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
				{
					char id[256] = { 0 };
					sprintf(id, "%d", pDevice->GetDeviceID());

					m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
					char mgid[256] = { 0 };
					sprintf(mgid, "%d", mediagroup_id);
					m_pRedisConnList[e_RC_TT_MainThread]->setvalue("dev_deviceidandgroupid", mgid, id);

					sr_printf(SR_LOGLEVEL_INFO, "RegisterSelf update redis db(3) Table:dev_deviceidandgroupid (deviceid,mediagroup_id)=(%s, %s)\n", id, mgid);

					char device_ipport_deviceid[128] = { 0 };
					sprintf(device_ipport_deviceid, "device_ipport_%s", id);

					char cInnerFieldName[128] = { 0 };
					char cOuterFieldName[128] = { 0 };
					char cVPNFieldName[128] = { 0 };
					char cHTTPFieldName[128] = { 0 };

					char inner_ipprort_value[65] = { 0 };
					char outer_ipprort_value[65] = { 0 };
					char vpn_ipprort_value[65] = { 0 };
					char http_ipprort_value[65] = { 0 };

					sprintf(cInnerFieldName, "%s", "inner");
					sprintf(cOuterFieldName, "%s", "outer");
					sprintf(cVPNFieldName, "%s", "vpn");
					sprintf(cHTTPFieldName, "%s", "http");

					//�������ip port
					unsigned long long ullinner = ((unsigned long long)(ipaddr_in.s_addr) << 32) + req->port();
#ifdef WIN32
					sprintf(inner_ipprort_value, "%I64d", ullinner);
#elif defined LINUX
					sprintf(inner_ipprort_value, "%lld", ullinner);
#endif	

					sprintf(outer_ipprort_value, "%s", "");
					sprintf(vpn_ipprort_value, "%s", "");
					
					//�������ip port
					unsigned long long ullhttp = ((unsigned long long)(ipaddr_in.s_addr) << 32) + CParser::GetInstance()->GetMyHTTPListenPort();
#ifdef WIN32
					sprintf(http_ipprort_value, "%I64d", ullhttp);
#elif defined LINUX
					sprintf(http_ipprort_value, "%lld", ullhttp);
#endif	
					
					REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
					REDISVDATA vIPPortInfodata;
					vIPPortInfodata.clear();

					REDISKEY strInnerFieldName = cInnerFieldName;
					REDISVALUE strinneripprortvalue = inner_ipprort_value;
					vIPPortInfodata.push_back(strInnerFieldName);
					vIPPortInfodata.push_back(strinneripprortvalue);

					REDISKEY strOuterFieldName = cOuterFieldName;
					REDISVALUE strouteripprortvalue = outer_ipprort_value;
					vIPPortInfodata.push_back(strOuterFieldName);
					vIPPortInfodata.push_back(strouteripprortvalue);

					REDISKEY strVPNFieldName = cVPNFieldName;
					REDISVALUE strvpnipprortvalue = vpn_ipprort_value;
					vIPPortInfodata.push_back(strVPNFieldName);
					vIPPortInfodata.push_back(strvpnipprortvalue);
					
					REDISKEY strHTTPFieldName = cHTTPFieldName;
					REDISVALUE strhttpipprortvalue = http_ipprort_value;
					vIPPortInfodata.push_back(strHTTPFieldName);
					vIPPortInfodata.push_back(strhttpipprortvalue);

					bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMSet(strdeviceipportdeviceidkey, vIPPortInfodata);

					sr_printf(SR_LOGLEVEL_INFO, "RegisterSelf update redis db(3) Table:%s ipport info.\n", device_ipport_deviceid);
				}
				
				bRegister = true;

				sr_printf(SR_LOGLEVEL_INFO, "RegisterSelf pDevice->UpdateDB1() %s success\n", req->auth_password().c_str());
			}
		}
		else
		{
			//�����ڣ����������ݿ�

			sr_printf(SR_LOGLEVEL_INFO, "RegisterSelf pDevice->SelectDB1() select * from device device_config_serial: %s is not exist, DeviceIP=%s, DevicePort=%d\n", req->auth_password().c_str(), selfIP_m, req->port());

			// ���ϵͳԤ�����deviceid������,����ϵͳ�������
			if (pDevice->GetDeviceID() == 0)
			{
				int iDeviceid = pDevice->GenerateDeviceID();

				// ���ϵͳ�ٷ���ʧ��,ֱ�Ӿܾ�ע��
				if (iDeviceid == 0)
				{
					sr_printf(SR_LOGLEVEL_ERROR, "RegisterSelf select * from device serial= %s is not exist and system generate deviceid error.\n", req->auth_password().c_str());

					return false;
				}
				else
				{
					if (pDevice->GetDeviceID() == 0)
					{
						sr_printf(SR_LOGLEVEL_ERROR, "RegisterSelf select * from device serial=%s is not exist and get deviceid error.\n", req->auth_password().c_str());

						return false;
					}
					else
					{
						pDevice->SetDeviceID(iDeviceid);
					}
				}
			}

			pDevice->SetDeviceConfigSerial(req->auth_password());
			pDevice->SetDeviceType(req->svr_type());

			pDevice->SetDeviceIP(selfIP_m);

			pDevice->SetDevicePort(req->port());
			if (req->has_max_bandwidth())
			{
				pDevice->SetMaxBandwidth(req->max_bandwidth());
			}
			if (req->has_systemlicence())
			{
				pDevice->SetSystemLicence(req->systemlicence());
			}
			if (req->has_version())
			{
				pDevice->SetVersion(req->version());
			}

			//�������ip port
			unsigned long long ullinner = ((unsigned long long)(ipaddr_in.s_addr) << 32) + req->port();

			pDevice->SetDeviceIPandPort(ullinner);
			pDevice->SetStatus(1);
			if (!pDevice->InsertDB(pDeviceConfig))
			{
				sr_printf(SR_LOGLEVEL_ERROR, "RegisterSelf pDevice->InsertDB() %s error!!!\n", req->auth_password().c_str());

				return false;
			}
			else
			{
				sr_printf(SR_LOGLEVEL_INFO, "RegisterSelf pDevice->InsertDB() insert into device OK!!!!!\n");

				int mediagroup_id = pDeviceConfig->GetMediaGroupID();

				//����redis
				if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
				{
					char id[256] = { 0 };
					sprintf(id, "%d", pDevice->GetDeviceID());

					m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
					char buf[256] = { 0 };
					sprintf(buf, "%d", mediagroup_id);
					m_pRedisConnList[e_RC_TT_MainThread]->setvalue("dev_deviceidandgroupid", buf, id);

					sr_printf(SR_LOGLEVEL_INFO, "RegisterSelf update redis db(3) Table:dev_deviceidandgroupid (deviceid,mediagroup_id)=(%s, %s)\n", id, buf);

					char device_ipport_deviceid[128] = { 0 };
					sprintf(device_ipport_deviceid, "device_ipport_%s", id);

					char cInnerFieldName[128] = { 0 };
					char cOuterFieldName[128] = { 0 };
					char cVPNFieldName[128] = { 0 };
					char cHTTPFieldName[128] = { 0 };

					char inner_ipprort_value[65] = { 0 };
					char outer_ipprort_value[65] = { 0 };
					char vpn_ipprort_value[65] = { 0 };
					char http_ipprort_value[65] = { 0 };

					sprintf(cInnerFieldName, "%s", "inner");
					sprintf(cOuterFieldName, "%s", "outer");
					sprintf(cVPNFieldName, "%s", "vpn");
					sprintf(cHTTPFieldName, "%s", "http");

					//�������ip port
					unsigned long long ullinner = ((unsigned long long)(ipaddr_in.s_addr) << 32) + req->port();
#ifdef WIN32
					sprintf(inner_ipprort_value, "%I64d", ullinner);
#elif defined LINUX
					sprintf(inner_ipprort_value, "%lld", ullinner);
#endif	

					sprintf(outer_ipprort_value, "%s", "");
					sprintf(vpn_ipprort_value, "%s", "");

					//�������ip port
					unsigned long long ullhttp = ((unsigned long long)(ipaddr_in.s_addr) << 32) + CParser::GetInstance()->GetMyHTTPListenPort();
#ifdef WIN32
					sprintf(http_ipprort_value, "%I64d", ullhttp);
#elif defined LINUX
					sprintf(http_ipprort_value, "%lld", ullhttp);
#endif	

					REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
					REDISVDATA vIPPortInfodata;
					vIPPortInfodata.clear();

					REDISKEY strInnerFieldName = cInnerFieldName;
					REDISVALUE strinneripprortvalue = inner_ipprort_value;
					vIPPortInfodata.push_back(strInnerFieldName);
					vIPPortInfodata.push_back(strinneripprortvalue);

					REDISKEY strOuterFieldName = cOuterFieldName;
					REDISVALUE strouteripprortvalue = outer_ipprort_value;
					vIPPortInfodata.push_back(strOuterFieldName);
					vIPPortInfodata.push_back(strouteripprortvalue);

					REDISKEY strVPNFieldName = cVPNFieldName;
					REDISVALUE strvpnipprortvalue = vpn_ipprort_value;
					vIPPortInfodata.push_back(strVPNFieldName);
					vIPPortInfodata.push_back(strvpnipprortvalue);

					REDISKEY strHTTPFieldName = cHTTPFieldName;
					REDISVALUE strhttpipprortvalue = http_ipprort_value;
					vIPPortInfodata.push_back(strHTTPFieldName);
					vIPPortInfodata.push_back(strhttpipprortvalue);

					bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMSet(strdeviceipportdeviceidkey, vIPPortInfodata);

					sr_printf(SR_LOGLEVEL_INFO, "RegisterSelf update redis db(3) Table:%s ipport info.\n", device_ipport_deviceid);
				}
				
				bRegister = true;

				sr_printf(SR_LOGLEVEL_INFO, "RegisterSelf pDevice->InsertDB() %s success.\n", req->auth_password().c_str());
			}
		}
	}

	return bRegister;
}

void CDevMgr::UpdateSelfHeartbeatToDB(int selfdeviceid, time_t timeNow)
{
	if (m_pMainThread)
	{
		typedef CBufferT<int, time_t, void*, void*, void*, void*, void*, void*> CParam;
		CParam* pParam;
		pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, selfdeviceid, timeNow);

		typedef void (CDevMgr::* ACTION)(void*);
		m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*this, &CDevMgr::Handle_UpdateSelfHeartbeatToDB, (void*)pParam));
	}
	return;
}

void CDevMgr::Handle_UpdateSelfHeartbeatToDB(void *pArg)
{
	if (pArg == NULL)
	{
		assert(0);
		return;
	}
	typedef CBufferT<int, time_t, void*, void*, void*, void*, void*, void*> CParam;
	CParam* pParam = (CParam*)pArg;
	int selfdeviceid = (int)(pParam->m_Arg1);
	time_t timeNow = (time_t)(pParam->m_Arg2);

	if (selfdeviceid <= 0
		/*|| m_HttpSvrListenISok == false*/)
	{
		//sr_printf(SR_LOGLEVEL_INFO, "Handle_UpdateSelfHeartbeatToDB selfdeviceid=%d or HttpSvrListenISok=%d is error.\n", selfdeviceid, m_HttpSvrListenISok);
		sr_printf(SR_LOGLEVEL_INFO, "Handle_UpdateSelfHeartbeatToDB selfdeviceid=%d is error.\n", selfdeviceid);
		return;
	}

	char load2_s[128] = { 0 };
	char deviceid_s[128] = { 0 };
	sprintf(deviceid_s, "%d", selfdeviceid);

	std::string svrname_groupid;
	svrname_groupid.clear();

	svrname_groupid = "devmgr_";
	sr_printf(SR_LOGLEVEL_INFO, "Handle_UpdateSelfHeartbeatToDB deviceid_s=%s\n", deviceid_s);

	if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
	{
		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
		char * p = m_pRedisConnList[e_RC_TT_MainThread]->getvalue("dev_deviceidandgroupid", deviceid_s);

		//printf("Handle_UpdateSelfHeartbeatToDB getvalue groupid redis db(3) from Table:dev_deviceidandgroupid (deviceid)=(%s)\n", deviceid_s);

		if (p != NULL)
		{
			svrname_groupid += p;

			m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0); // db0ר�Ŵ洢���豸�ĸ�����Ϣ, ZSET

			char ternum_cpu_load[256] = { 0 };
			sprintf(ternum_cpu_load, "%d", 0);
			m_pRedisConnList[e_RC_TT_MainThread]->setvalue(svrname_groupid.c_str(), ternum_cpu_load, deviceid_s);

			sr_printf(SR_LOGLEVEL_DEBUG, "Handle_UpdateSelfHeartbeatToDB getvalue groupid from redis db(3) Table:dev_deviceidandgroupid (deviceid)=(%s) then setvalue to redis db(0)  Table:%s :(deviceid, load1+load2)=(%s, %s)\n", deviceid_s, svrname_groupid.c_str(), deviceid_s, ternum_cpu_load);

			delete p; // ��ɾ���ᵼ���ڴ�й©
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "Handle_UpdateSelfHeartbeatToDB getvalue groupid from redis db(3) Table:dev_deviceidandgroupid (deviceid)=(%s) return NULL error!!!\n", deviceid_s);

			return;
		}

		char time_s[256] = { 0 };
#ifdef WIN32
		sprintf(time_s, "%I64d", timeNow);
#elif defined LINUX
		sprintf(time_s, "%lld", timeNow);
#endif	

		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1); // db1ר�Ŵ洢���豸����ʱ��,HASH
		m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(svrname_groupid.c_str(), time_s, deviceid_s);

		sr_printf(SR_LOGLEVEL_DEBUG, "Handle_UpdateSelfHeartbeatToDB sethashvalue to redis db(1)  Table:%s :(deviceid, time_s)=(%s, %s)\n", svrname_groupid.c_str(), deviceid_s, time_s);

		struct in_addr ipaddr_self;
		if (0 == inet_pton(AF_INET, CParser::GetInstance()->GetMyListenIpAddr().c_str(), &ipaddr_self))
		{
			sr_printf(SR_LOGLEVEL_ERROR, "%s:%d inet_pton error\n", __FUNCTION__, __LINE__);
			return;
		}
		else
		{
			char device_ipport_deviceid[128] = { 0 };
			char cHTTPFieldName[128] = { 0 };
			char http_ipprort_value[65] = { 0 };

			sprintf(device_ipport_deviceid, "device_ipport_%s", deviceid_s);
			sprintf(cHTTPFieldName, "%s", "http");

			if (m_HttpSvrListenISok)
			{
				//�������ip port
				unsigned long long ullhttp = ((unsigned long long)(ipaddr_self.s_addr) << 32) + CParser::GetInstance()->GetMyHTTPListenPort();
#ifdef WIN32
				sprintf(http_ipprort_value, "%I64d", ullhttp);
#elif defined LINUX
				sprintf(http_ipprort_value, "%lld", ullhttp);
#endif	
			}
			else
			{
			}

			m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
			m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(device_ipport_deviceid, http_ipprort_value, cHTTPFieldName);

			sr_printf(SR_LOGLEVEL_DEBUG, "Handle_UpdateSelfHeartbeatToDB sethashvalue to redis db(3)  Table:%s :(httpfield, http_ipprort_value)=(%s, %s)\n", device_ipport_deviceid, cHTTPFieldName, http_ipprort_value);
		}
	}
}

bool CDevMgr::ProcessReqRegister(const SRMsgs::ReqRegister* req, DeviceConnect *pClient, CDeviceConfig* pDeviceConfig, CDevice* pDevice, CDataDictionary* pDataDictionary, SRMsgs::RspRegister* rspsend)
{

	struct in_addr inIP;
#ifdef WIN32
	inIP.S_un.S_addr = req->ip();
#elif defined LINUX
	inIP.s_addr = req->ip();
#endif
	char dev_con_IP_s[16];
	memset(dev_con_IP_s, 0, sizeof(dev_con_IP_s));
	strcpy(dev_con_IP_s, pClient->m_remote_ip_.c_str());

	char reg_msg_IP_s[16];
	memset(reg_msg_IP_s, 0, sizeof(reg_msg_IP_s));
	strcpy(reg_msg_IP_s, inet_ntoa(inIP));

	// �������ip
	char selfIP_m[16];
	memset(selfIP_m, 0, sizeof(selfIP_m));
	struct in_addr ipaddr_in;
	struct in_addr ipaddr_out;
	struct in_addr ipaddr_vpn;

	std::string strdevdomainname;
	strdevdomainname.clear();

	sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_ReqRegister OK: param(device_config_serial,svr_type,ip,port)=(%s, %d, %s, %d)\n", req->auth_password().c_str(), req->svr_type(), reg_msg_IP_s, req->port());

	//sr_printf(SR_LOGLEVEL_NORMAL, "ProcessReqRegister dev_con_IP_s=%s, reg_msg_IP_s=%s, reg_msg_port=%d\n", dev_con_IP_s, reg_msg_IP_s, req->port());

	if (req->has_nettype())
	{
		sr_printf(SR_LOGLEVEL_NORMAL, "ProcessReqRegister dev_con_IP_s=%s, reg_msg_IP_s=%s, reg_msg_port=%d, nettype=%d\n", dev_con_IP_s, reg_msg_IP_s, req->port(), req->nettype());
		// msg ip is inner
		if (req->nettype() == 0)
		{
			strcpy(selfIP_m, reg_msg_IP_s);
		} 
		else if (req->nettype() == 1)// msg ip is outer
		{
			strcpy(selfIP_m, dev_con_IP_s);
			
			if (0 == inet_pton(AF_INET, reg_msg_IP_s, &ipaddr_out))
			{
				sr_printf(SR_LOGLEVEL_ERROR, "%s:%d inet_pton error\n", __FUNCTION__, __LINE__);

				rspsend->set_isok(false);
				rspsend->set_deviceid(0);
				rspsend->set_token("");
				rspsend->set_failurereason("ipaddr_out is error");
				rspsend->set_errorcode(0x03000A);
				return false;
			}
		}
		else if (req->nettype() == 2)// msg ip is vpn
		{
			strcpy(selfIP_m, dev_con_IP_s);

			if (0 == inet_pton(AF_INET, reg_msg_IP_s, &ipaddr_vpn))
			{
				sr_printf(SR_LOGLEVEL_ERROR, "%s:%d inet_pton error\n", __FUNCTION__, __LINE__);

				rspsend->set_isok(false);
				rspsend->set_deviceid(0);
				rspsend->set_token("");
				rspsend->set_failurereason("ipaddr_vpn is error");
				rspsend->set_errorcode(0x03000B);
				return false;
			}
		}
		else
		{
			//������Ӧֵ
			rspsend->set_isok(false);
			rspsend->set_deviceid(0);
			rspsend->set_token("");
			rspsend->set_failurereason("nettype is error");
			rspsend->set_errorcode(0x03000C);

			return false;
		}

		if (0 == inet_pton(AF_INET, selfIP_m, &ipaddr_in))
		{
			sr_printf(SR_LOGLEVEL_ERROR, "%s:%d inet_pton error\n", __FUNCTION__, __LINE__);

			rspsend->set_isok(false);
			rspsend->set_deviceid(0);
			rspsend->set_token("");
			rspsend->set_failurereason("ipaddr_in is error");
			rspsend->set_errorcode(0x030009);
			return false;
		}
		//mcregister.set_ip(ipaddr.s_addr);
	} 
	else
	{
		sr_printf(SR_LOGLEVEL_NORMAL, "ProcessReqRegister dev_con_IP_s=%s, reg_msg_IP_s=%s, reg_msg_port=%d\n", dev_con_IP_s, reg_msg_IP_s, req->port());

		//������Ӧֵ
		rspsend->set_isok(false);
		rspsend->set_deviceid(0);
		rspsend->set_token("");
		rspsend->set_failurereason("nettype is null");
		rspsend->set_errorcode(0x030008);

		return false;
	}

	bool bRegister = false;

	if (m_pRedisConnList[e_RC_TT_MainThread]->isconnectok() == false)
	{
		//������Ӧֵ
		rspsend->set_isok(false);
		rspsend->set_deviceid(0);
		rspsend->set_token("");
		rspsend->set_failurereason("redis disconnect");
		rspsend->set_errorcode(0x030000);

		return false;
	}

	// ����������¼��豸������ע��,��Ҫ�����¼��豸������Я���������Ƿ��ڵ�ǰ�ڴ�������,
	// ��������ڽ�һ����ϵͳhosts�ļ����Ƿ���ڸ�����,���������ֱ�Ӿܾ�ע��,������ڶԱ���Ӧ��ip��ַ�Ƿ����
	if (req->svr_type() == DEVICE_SERVER::DEVICE_DEV)
	{
		// 1���Ȳ�������Ƿ���redis,����ֱ�Ӿܾ�
		bool bfindinredis = false;
		RedisReplyArray vrra_domain_level;
		vrra_domain_level.clear();
		unsigned int nDomainAndLevel = 0;
		std::list<std::string> subdomainnamelist;
		subdomainnamelist.clear();
		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
		bool bHGetAllDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_domain_level);

		nDomainAndLevel = vrra_domain_level.size();

		if (bHGetAllDomain_ok
			&& (nDomainAndLevel > 0)
			&& (nDomainAndLevel % 2 == 0))
		{
			for (int idx = 0; idx < nDomainAndLevel;)
			{
				int iLevel = -99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
				//vrra_domain_level[idx].str;// ����
				iLevel = atoi(vrra_domain_level[idx + 1].str.c_str());// level
				if (iLevel > 0
					&& vrra_domain_level[idx].str.length() > 0)
				{
					if (0 == vrra_domain_level[idx].str.compare(req->domainname().c_str()))
					{
						bfindinredis = true;
					}

					subdomainnamelist.push_back(vrra_domain_level[idx].str);
				}
				idx += 2;
			}
		}

		if (!bfindinredis)
		{
			//������Ӧֵ
			rspsend->set_isok(false);
			rspsend->set_deviceid(0);
			rspsend->set_token("");
			rspsend->set_failurereason("domainname not in redis");
			rspsend->set_errorcode(0x03000D);

			return false;
		}
		// 2����redis��,�ٲ��ڴ�ֵ(ϵͳhosts�ļ�����Ч�����ı���)
		std::map<std::string, std::string>::iterator domaininfo_config_itor = m_mapSystemDomainInfo.find(req->domainname());
		if (domaininfo_config_itor == m_mapSystemDomainInfo.end())
		{
			// 2.1 ��redis��,�������ڴ�,�ø�����ȥhosts�����ļ�����,�������ļ����ݺ��ڴ�ֵ
			LoadSystemDomainInfo();
			std::map<std::string, std::string>::iterator domaininfo_check_itor = m_mapSystemDomainInfo.find(req->domainname());
			if (domaininfo_check_itor == m_mapSystemDomainInfo.end())
			{
				//������Ӧֵ
				rspsend->set_isok(false);
				rspsend->set_deviceid(0);
				rspsend->set_token("");
				rspsend->set_failurereason("domainname not in hosts file");
				rspsend->set_errorcode(0x03000E);

				return false;
			}
			else
			{
				// �Ա����¸��µ��ڴ�ֵ(ϵͳhosts�ļ�����Ч�����ı���)��ip��ַ��һ��,
				// ��Ҫ���¸�������Ӧ��ip
				if (0 != domaininfo_check_itor->second.compare(dev_con_IP_s))
				{
					int ires = UpDomainIPToHostsFile(dev_con_IP_s, (char*)req->domainname().c_str());
					if (ires == -1)// δ���ҵ�������
					{
						//������Ӧֵ
						rspsend->set_isok(false);
						rspsend->set_deviceid(0);
						rspsend->set_token("");
						rspsend->set_failurereason("not find domainname in hosts file");
						rspsend->set_errorcode(0x03000F);

						return false;
					}
					else if (ires == 0)// ���ҵ�����������ipû�仯
					{
					}
					else if (ires == 1)// ���ҵ���������ip���³ɹ�
					{
						//LoadSystemDomainInfo();
						int isreloadok = ReloadNginx((char*)req->domainname().c_str(), reg_msg_IP_s, dev_con_IP_s, req->port());
						if (isreloadok == -1)
						{
							sr_printf(SR_LOGLEVEL_NORMAL, "11111 ProcessReqRegister dev_con_IP_s=%s have change reload nginx failed.\n", dev_con_IP_s);
							//������Ӧֵ
							rspsend->set_isok(false);
							rspsend->set_deviceid(0);
							rspsend->set_token("");
							rspsend->set_failurereason("domainname ip have change reload nginx failed");
							rspsend->set_errorcode(0x030011);

							return false;
						}
					}
				}
			}
		}
		else
		{
			// 2.2 ��redis��,Ҳ�ڵ�ǰ�ڴ�,��һ���Ա�ip��ַ,�����һ����Ҫ����hosts�ļ����ڴ�
			if (0 != domaininfo_config_itor->second.compare(dev_con_IP_s))
			{
				int ires = UpDomainIPToHostsFile(dev_con_IP_s, (char*)req->domainname().c_str());
				if (ires == -1)// δ���ҵ�������
				{
					//������Ӧֵ
					rspsend->set_isok(false);
					rspsend->set_deviceid(0);
					rspsend->set_token("");
					rspsend->set_failurereason("update ip to hosts file failed");
					rspsend->set_errorcode(0x030010);

					return false;
				}
				else if (ires == 0)// ���ҵ�����������ipû�仯
				{
				}
				else if (ires == 1)// ���ҵ���������ip���³ɹ�
				{
					LoadSystemDomainInfo();
					int isreloadok = ReloadNginx((char*)req->domainname().c_str(), reg_msg_IP_s, dev_con_IP_s, req->port());
					if (isreloadok == -1)
					{
						sr_printf(SR_LOGLEVEL_NORMAL, "22222 ProcessReqRegister dev_con_IP_s=%s have change reload nginx failed.\n", dev_con_IP_s);
						//������Ӧֵ
						rspsend->set_isok(false);
						rspsend->set_deviceid(0);
						rspsend->set_token("");
						rspsend->set_failurereason("domainname ip have change reload nginx failed");
						rspsend->set_errorcode(0x030011);

						return false;
					}
				}
			}
		}
	}

	pDeviceConfig->SetDeviceConfigSerial(req->auth_password());
	pDeviceConfig->SetDeviceConfigType(req->svr_type());
	//printf("==SRMsgId_ReqRegister==select * from device_config--DeviceConfigSerial-->%s, ==device_config_type-->%d\n", req.auth_password().c_str(), req.svr_type());
	if (!pDeviceConfig->SelectDB())
	{
		//������Ӧֵ
		rspsend->set_isok(false);
		rspsend->set_deviceid(0);
		rspsend->set_token("");
		rspsend->set_failurereason("serial error");
		rspsend->set_errorcode(0x030001);

		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqRegister CDeviceConfig::SelectDB() serial= %s and type = %d is not exists.\n", req->auth_password().c_str(), req->svr_type());
	}
	else
	{
		sr_printf(SR_LOGLEVEL_NORMAL, "check this inner ip:%s port:%d exist in device_config table.\n", selfIP_m, req->port());

		pDevice->SetDeviceConfigSerial(req->auth_password());

		//�޸ĸ�serverע��״̬�����������ظ����ƣ�����tokenֵ���������ݿ�
		pDevice->SetDeviceIP(selfIP_m);
		pDevice->SetDevicePort(req->port());

		if (pDevice->SelectDB1())
		{
			//���ڣ��򷵻�ע�����
			sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister pDevice->SelectDB1() select * from device device_config_serial: %s exist, DeviceIP=%s, DevicePort=%d\n", req->auth_password().c_str(), selfIP_m, req->port());

			//���±���ֵ
			if (req->has_max_bandwidth())
			{
				pDevice->SetMaxBandwidth(req->max_bandwidth());
			}
			pDevice->SetStatus(1);
			if (req->has_systemlicence())
			{
				pDevice->SetSystemLicence(req->systemlicence());
			}
			if (req->has_version())
			{
				pDevice->SetVersion(req->version());
			}
			if (!pDevice->UpdateDB1())
			{
				//������Ӧֵ
				rspsend->set_isok(false);
				rspsend->set_deviceid(0);
				rspsend->set_token("");
				rspsend->set_failurereason("update device db param error");
				rspsend->set_errorcode(0x030002);

				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqRegister pDevice->UpdateDB1(%s)  error!!!\n", req->auth_password().c_str());
			}
			else
			{
				sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister pDevice->UpdateDB1(%s) and set max_bandwidth, status OK!!!!!\n", req->auth_password().c_str());

				int mediagroup_id = pDeviceConfig->GetMediaGroupID();
				//����redis
				if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
				{
					char id[256] = { 0 };
					sprintf(id, "%d", pDevice->GetDeviceID());

					m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
					char mgid[256] = { 0 };
					sprintf(mgid, "%d", mediagroup_id);
					m_pRedisConnList[e_RC_TT_MainThread]->setvalue("dev_deviceidandgroupid", mgid, id);

					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister update redis db(3) Table:dev_deviceidandgroupid (deviceid,mediagroup_id)=(%s, %s)\n", id, mgid);
					
					char device_ipport_deviceid[128] = { 0 };
					sprintf(device_ipport_deviceid, "device_ipport_%s", id);

					char cInnerFieldName[128] = { 0 };
					char cOuterFieldName[128] = { 0 };
					char cVPNFieldName[128] = { 0 };
					char cMapInternetIPsFieldName[128] = { 0 };

					char inner_ipprort_value[65] = { 0 };
					char outer_ipprort_value[65] = { 0 };
					char vpn_ipprort_value[65] = { 0 };
					char map_internetips_value[512] = { 0 };// ��ʽ�����ʮ����ip1,���ʮ����ip2,....���ʮ����ipn

					sprintf(cInnerFieldName, "%s", "inner");
					sprintf(cOuterFieldName, "%s", "outer");
					sprintf(cVPNFieldName, "%s", "vpn");
					sprintf(cMapInternetIPsFieldName, "%s", "mapinternetips");
					
					//�������ip port
					unsigned long long ullinner = ((unsigned long long)(ipaddr_in.s_addr) << 32) + req->port();
#ifdef WIN32
					sprintf(inner_ipprort_value, "%I64d", ullinner);
#elif defined LINUX
					sprintf(inner_ipprort_value, "%lld", ullinner);
#endif	

					// msg ip is inner
					if (req->nettype() == 0)
					{
						sprintf(outer_ipprort_value, "%s", "");
						sprintf(vpn_ipprort_value, "%s", "");
					}
					else if (req->nettype() == 1)// msg ip is outer
					{
						//�������ip port
						unsigned long long ullouter = ((unsigned long long)(ipaddr_out.s_addr) << 32) + req->port();
#ifdef WIN32
						sprintf(outer_ipprort_value, "%I64d", ullouter);
#elif defined LINUX
						sprintf(outer_ipprort_value, "%lld", ullouter);
#endif	

						sprintf(vpn_ipprort_value, "%s", "");
					}
					else if (req->nettype() == 2)// msg ip is vpn
					{
						sprintf(outer_ipprort_value, "%s", "");

						//���vpn ip port
						unsigned long long ullvpn = ((unsigned long long)(ipaddr_vpn.s_addr) << 32) + req->port();
#ifdef WIN32
						sprintf(vpn_ipprort_value, "%I64d", ullvpn);
#elif defined LINUX
						sprintf(vpn_ipprort_value, "%lld", ullvpn);
#endif	
					}

					REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
					REDISVDATA vIPPortInfodata;
					vIPPortInfodata.clear();

					REDISKEY strInnerFieldName = cInnerFieldName;
					REDISVALUE strinneripprortvalue = inner_ipprort_value;
					vIPPortInfodata.push_back(strInnerFieldName);
					vIPPortInfodata.push_back(strinneripprortvalue);

					REDISKEY strOuterFieldName = cOuterFieldName;
					REDISVALUE strouteripprortvalue = outer_ipprort_value;
					vIPPortInfodata.push_back(strOuterFieldName);
					vIPPortInfodata.push_back(strouteripprortvalue);
					
					REDISKEY strVPNFieldName = cVPNFieldName;
					REDISVALUE strvpnipprortvalue = vpn_ipprort_value;
					vIPPortInfodata.push_back(strVPNFieldName);
					vIPPortInfodata.push_back(strvpnipprortvalue);

					// netmp��relaymcע�������ӳ������ip
					if (req->mapinternetips_size() > 0
						&& (req->svr_type() == DEVICE_SERVER::DEVICE_NETMP
						|| req->svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC))
					{
						int nMapinternetipsLen = 0;
						for (int i = 0; i < req->mapinternetips_size(); i++)
						{
							const SRMsgs::ReqRegister_MapIPInfo& internetip = req->mapinternetips(i);
							if (internetip.mapip().length() > 0)
							{
								nMapinternetipsLen += sprintf(map_internetips_value + nMapinternetipsLen, "%s,", internetip.mapip().c_str());
							}
						}
						// ��ֹֻ�ж��ŷָ���
						if (nMapinternetipsLen > 1)
						{
							REDISKEY strMapInternetIPsFieldName = cMapInternetIPsFieldName;
							REDISVALUE strmapinternetipsvalue = map_internetips_value;
							vIPPortInfodata.push_back(strMapInternetIPsFieldName);
							vIPPortInfodata.push_back(strmapinternetipsvalue);
						}
					}

					bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMSet(strdeviceipportdeviceidkey, vIPPortInfodata);

					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister update redis db(3) Table:%s ipport info.\n", device_ipport_deviceid);

					// ���������µ�relaymc��relayserver��Ϣ�浽local_domain_deviceinfo����,ͬʱ֪ͨ�ϼ�devmgr
					if (req->svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC
						|| req->svr_type() == DEVICE_SERVER::DEVICE_RELAYSERVER
						|| req->svr_type() == DEVICE_SERVER::DEVICE_STUNSERVER)
					{
						char svrtype_s[16] = { 0 };
						sprintf(svrtype_s, "%d", req->svr_type());
						m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue("local_domain_deviceinfo", svrtype_s, id);
						
						sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister update redis db(3) Table:local_domain_deviceinfo svrtype info[%s, %s].\n", id, svrtype_s);


						// ��ȡ��������
						RedisReplyArray vrra_domain_level;
						vrra_domain_level.clear();
						unsigned int nDomainAndLevel = 0;

						bool bHGetAllDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_domain_level);

						nDomainAndLevel = vrra_domain_level.size();

						if (bHGetAllDomain_ok
							&& (nDomainAndLevel > 0)
							&& (nDomainAndLevel % 2 == 0))
						{
							for (int idx = 0; idx < nDomainAndLevel;)
							{
								int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
								//vrra_domain_level[idx].str;// ����
								iLevel = atoi(vrra_domain_level[idx + 1].str.c_str());// level
								if (iLevel == 0)
								{
									strdevdomainname.assign(vrra_domain_level[idx].str.c_str());

									//// ���ϼ�devmgrͬ��
									//if (req->svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC
									//	|| req->svr_type() == DEVICE_SERVER::DEVICE_RELAYSERVER)
									{
										if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr
											&& strdevdomainname.length() > 0)
										{
											SRMsgs::IndSubSvrInfoToDevmgr indsvrinfo;
											indsvrinfo.set_addordel(1);
											indsvrinfo.set_deviceid(m_uiMyDeviceid);// �¼�devmgr��id
											indsvrinfo.set_token(m_szMyToken);// �¼�devmgr��token

											SRMsgs::IndSubSvrInfoToDevmgr_DomainInfo* psvrdomaininfo = 0;
											{
												psvrdomaininfo = indsvrinfo.add_domaininfos();
												psvrdomaininfo->set_domainname(strdevdomainname);
												psvrdomaininfo->set_level(iLevel);
												SRMsgs::IndSubSvrInfoToDevmgr_ServerInfo* pSvrinfo = 0;
												{
													pSvrinfo = psvrdomaininfo->add_svrinfos();
													pSvrinfo->set_svrtype(req->svr_type());
													pSvrinfo->set_deviceid(pDevice->GetDeviceID());

													SRMsgs::IndSubSvrInfoToDevmgr_IPPORTInfo* pSvripportinfo = new SRMsgs::IndSubSvrInfoToDevmgr_IPPORTInfo();
													pSvripportinfo->set_inner(inner_ipprort_value);
													pSvripportinfo->set_outer(outer_ipprort_value);
													pSvripportinfo->set_vpn(vpn_ipprort_value);
													pSvrinfo->set_allocated_ipport(pSvripportinfo);
												}
											}

											SerialAndSendDevmgr((AcitiveConnect*)(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr), getMsgIdByClassName(IndSubSvrInfoToDevmgr), &indsvrinfo);
										}
									}

									//// ���¼�devmgrͬ����relaymc��stunserver����Ϣ
									//if (req->svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC
									//	|| req->svr_type() == DEVICE_SERVER::DEVICE_STUNSERVER)
									{
										// ��(��)��������Ϊ��
										if (strdevdomainname.length() > 0)
										{
											SRMsgs::IndUpSvrInfoToDevmgr indsvrinfo;
											indsvrinfo.set_addordel(1);
											//indsvrinfo.set_deviceid(pDevice->GetDeviceID());// �¼�devmgr��id
											//indsvrinfo.set_token(pDevice->GetDeviceToken());// �¼�devmgr��token
											SRMsgs::IndUpSvrInfoToDevmgr_DomainInfo* psvrdomaininfo = 0;
											{
												psvrdomaininfo = indsvrinfo.add_domaininfos();
												psvrdomaininfo->set_domainname(strdevdomainname);
												psvrdomaininfo->set_level(iLevel);// ������ǿ��д0

												SRMsgs::IndUpSvrInfoToDevmgr_ServerInfo* pSvrinfo = 0;
												{
													pSvrinfo = psvrdomaininfo->add_svrinfos();
													pSvrinfo->set_svrtype(req->svr_type());
													pSvrinfo->set_deviceid(pDevice->GetDeviceID());

													SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo* pSvripportinfo = new SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo();
													pSvripportinfo->set_inner(inner_ipprort_value);
													pSvripportinfo->set_outer(outer_ipprort_value);
													pSvripportinfo->set_vpn(vpn_ipprort_value);
													pSvrinfo->set_allocated_ipport(pSvripportinfo);
												}
											}


											// 2������¼�devmgrָ��
											for (iter_mapDeviceHeartbeatTime dev_itor = m_MapDeviceHeartbeatTime.begin();
												dev_itor != m_MapDeviceHeartbeatTime.end(); dev_itor++)
											{
												if (dev_itor->second.devicetype == DEVICE_SERVER::DEVICE_DEV)
												{
													if (dev_itor->second.pClient)
													{
														indsvrinfo.set_deviceid(dev_itor->first);// �¼�devmgr��id
														indsvrinfo.set_token(dev_itor->second.token);// �¼�devmgr��token

														SerialProtoMsgAndSend(dev_itor->second.pClient, getMsgIdByClassName(IndUpSvrInfoToDevmgr), &indsvrinfo);
													}
												}
											}
										}
									}

									break;
								}
								idx += 2;
							}
						}
					}
				}

				//������Ӧֵ
				rspsend->set_isok(true);
				rspsend->set_deviceid(pDevice->GetDeviceID());
				rspsend->set_token(pDevice->GetDeviceToken().c_str());
				rspsend->set_failurereason("");
				rspsend->set_groupid(mediagroup_id);
				if (req->svr_type() == DEVICE_SERVER::DEVICE_MC
					|| req->svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC)
				{
					rspsend->set_domainname(strdevdomainname);
				}
				if (req->svr_type() == DEVICE_SERVER::DEVICE_GW)
				{
					SRMsgs::RspRegister_CapInfo* paddcapinfo = 0;

					//������Ƶ������
					std::list<std::string> audioProtocols;
					audioProtocols.clear();
					pDataDictionary->SelectAudioProtocol(audioProtocols);
					if (audioProtocols.size() > 0)
					{
						paddcapinfo = rspsend->add_capinfos();
						paddcapinfo->set_captype(1);

						SRMsgs::RspRegister_Capset* paddcapset = 0;

						for (std::list<std::string>::iterator ap_itor = audioProtocols.begin();
							ap_itor != audioProtocols.end(); ap_itor++)
						{
							paddcapset = paddcapinfo->add_capsets();
							paddcapset->set_fmt((*ap_itor).c_str());
						}
					}

					//������Ƶ������
					std::list<std::string> videoProtocols;
					videoProtocols.clear();
					pDataDictionary->SelectVideoProtocol(videoProtocols);
					if (videoProtocols.size() > 0)
					{
						paddcapinfo = rspsend->add_capinfos();
						paddcapinfo->set_captype(2);

						SRMsgs::RspRegister_Capset* paddcapset = 0;

						for (std::list<std::string>::iterator vp_itor = videoProtocols.begin();
							vp_itor != videoProtocols.end(); vp_itor++)
						{
							paddcapset = paddcapinfo->add_capsets();
							paddcapset->set_fmt((*vp_itor).c_str());
						}
					}
				}
				if (pClient != NULL)
				{
					bRegister = true;
					pClient->SetDeviceID(pDevice->GetDeviceID());

					// mcע��ʱЯ�����������л�����Ϣ�����������Ϣ��Ϊ������Ҫ��������ͬ��
					// ���ǵ�һ��ע�ᣬ���ܻ��л�����Ϣ
					if (req->svr_type() == 1
						&& req->conflist_size() > 0)
					{
						time_t timeNow;
						timeNow = time(NULL);
						SyncConfInfo(req, pDevice, timeNow);
					}
				}

				sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister pDevice->UpdateDB1() %s success\n", req->auth_password().c_str());

			}
		}
		else
		{
			//�����ڣ����������ݿ�
			
			sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister pDevice->SelectDB1() select * from device device_config_serial: %s is not exist, DeviceIP=%s, DevicePort=%d\n", req->auth_password().c_str(), selfIP_m, req->port());
			
			// ���ϵͳԤ�����deviceid������,����ϵͳ�������
			if (pDevice->GetDeviceID() == 0)
			{
				int iDeviceid = pDevice->GenerateDeviceID();

				// ���ϵͳ�ٷ���ʧ��,ֱ�Ӿܾ�ע��
				if (iDeviceid == 0)
				{
					//������Ӧֵ
					rspsend->set_isok(false);
					rspsend->set_deviceid(0);
					rspsend->set_token("");
					rspsend->set_failurereason("generate deviceid error");
					rspsend->set_errorcode(0x030003);

					sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqRegister select * from device serial= %s is not exist and system generate deviceid error.\n", req->auth_password().c_str());

					return false;
				}
				else
				{
					if (pDevice->GetDeviceID() == 0)
					{
						//������Ӧֵ
						rspsend->set_isok(false);
						rspsend->set_deviceid(0);
						rspsend->set_token("");
						rspsend->set_failurereason("get deviceid error");
						rspsend->set_errorcode(0x030004);

						sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqRegister select * from device serial=%s is not exist and get deviceid error.\n", req->auth_password().c_str());

						return false;
					}
					else
					{
						pDevice->SetDeviceID(iDeviceid);
					}
				}
			}

			pDevice->SetDeviceConfigSerial(req->auth_password());
			pDevice->SetDeviceType(req->svr_type());

			pDevice->SetDeviceIP(selfIP_m);

			pDevice->SetDevicePort(req->port());
			if (req->has_max_bandwidth())
			{
				pDevice->SetMaxBandwidth(req->max_bandwidth());
			}
			if (req->has_systemlicence())
			{
				pDevice->SetSystemLicence(req->systemlicence());
			}
			if (req->has_version())
			{
				pDevice->SetVersion(req->version());
			}
			
			//�������ip port
			unsigned long long ullinner = ((unsigned long long)(ipaddr_in.s_addr) << 32) + req->port();

			pDevice->SetDeviceIPandPort(ullinner);
			pDevice->SetStatus(1);
			if (!pDevice->InsertDB(pDeviceConfig))
			{
				//������Ӧֵ
				rspsend->set_isok(false);
				rspsend->set_deviceid(0);
				rspsend->set_token("");
				rspsend->set_failurereason("insert into device param error");
				rspsend->set_errorcode(0x030005);

				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqRegister pDevice->InsertDB() %s error!!!\n", req->auth_password().c_str());
			}
			else
			{
				sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister pDevice->InsertDB() insert into device OK!!!!!\n");

				int mediagroup_id = pDeviceConfig->GetMediaGroupID();

				//����redis
				if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
				{
					char id[256] = { 0 };
					sprintf(id, "%d", pDevice->GetDeviceID());

					m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
					char buf[256] = { 0 };
					sprintf(buf, "%d", mediagroup_id);
					m_pRedisConnList[e_RC_TT_MainThread]->setvalue("dev_deviceidandgroupid", buf, id);

					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister update redis db(3) Table:dev_deviceidandgroupid (deviceid,mediagroup_id)=(%s, %s)\n", id, buf);
					
					char device_ipport_deviceid[128] = { 0 };
					sprintf(device_ipport_deviceid, "device_ipport_%s", id);
					
					char cInnerFieldName[128] = { 0 };
					char cOuterFieldName[128] = { 0 };
					char cVPNFieldName[128] = { 0 };
					char cMapInternetIPsFieldName[128] = { 0 };

					char inner_ipprort_value[65] = { 0 };
					char outer_ipprort_value[65] = { 0 };
					char vpn_ipprort_value[65] = { 0 };
					char map_internetips_value[512] = { 0 };// ��ʽ�����ʮ����ip1,���ʮ����ip2,....���ʮ����ipn

					sprintf(cInnerFieldName, "%s", "inner");
					sprintf(cOuterFieldName, "%s", "outer");
					sprintf(cVPNFieldName, "%s", "vpn");
					sprintf(cMapInternetIPsFieldName, "%s", "mapinternetips");

					//�������ip port
					unsigned long long ullinner = ((unsigned long long)(ipaddr_in.s_addr) << 32) + req->port();
#ifdef WIN32
					sprintf(inner_ipprort_value, "%I64d", ullinner);
#elif defined LINUX
					sprintf(inner_ipprort_value, "%lld", ullinner);
#endif	

					// msg ip is inner
					if (req->nettype() == 0)
					{
						sprintf(outer_ipprort_value, "%s", "");
						sprintf(vpn_ipprort_value, "%s", "");
					}
					else if (req->nettype() == 1)// msg ip is outer
					{
						//�������ip port
						unsigned long long ullouter = ((unsigned long long)(ipaddr_out.s_addr) << 32) + req->port();
#ifdef WIN32
						sprintf(outer_ipprort_value, "%I64d", ullouter);
#elif defined LINUX
						sprintf(outer_ipprort_value, "%lld", ullouter);
#endif	

						sprintf(vpn_ipprort_value, "%s", "");
					}
					else if (req->nettype() == 2)// msg ip is vpn
					{
						sprintf(outer_ipprort_value, "%s", "");

						//���vpn ip port
						unsigned long long ullvpn = ((unsigned long long)(ipaddr_vpn.s_addr) << 32) + req->port();
#ifdef WIN32
						sprintf(vpn_ipprort_value, "%I64d", ullvpn);
#elif defined LINUX
						sprintf(vpn_ipprort_value, "%lld", ullvpn);
#endif	
					}

					REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
					REDISVDATA vIPPortInfodata;
					vIPPortInfodata.clear();

					REDISKEY strInnerFieldName = cInnerFieldName;
					REDISVALUE strinneripprortvalue = inner_ipprort_value;
					vIPPortInfodata.push_back(strInnerFieldName);
					vIPPortInfodata.push_back(strinneripprortvalue);

					REDISKEY strOuterFieldName = cOuterFieldName;
					REDISVALUE strouteripprortvalue = outer_ipprort_value;
					vIPPortInfodata.push_back(strOuterFieldName);
					vIPPortInfodata.push_back(strouteripprortvalue);

					REDISKEY strVPNFieldName = cVPNFieldName;
					REDISVALUE strvpnipprortvalue = vpn_ipprort_value;
					vIPPortInfodata.push_back(strVPNFieldName);
					vIPPortInfodata.push_back(strvpnipprortvalue);

					// netmp��relaymcע�������ӳ������ip
					if (req->mapinternetips_size() > 0
						&& (req->svr_type() == DEVICE_SERVER::DEVICE_NETMP
						|| req->svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC))
					{
						int nMapinternetipsLen = 0;
						for (int i = 0; i < req->mapinternetips_size(); i++)
						{
							const SRMsgs::ReqRegister_MapIPInfo& internetip = req->mapinternetips(i);
							if (internetip.mapip().length() > 0)
							{
								nMapinternetipsLen += sprintf(map_internetips_value + nMapinternetipsLen, "%s,", internetip.mapip().c_str());
							}
						}
						// ��ֹֻ�ж��ŷָ���
						if (nMapinternetipsLen > 1)
						{
							REDISKEY strMapInternetIPsFieldName = cMapInternetIPsFieldName;
							REDISVALUE strmapinternetipsvalue = map_internetips_value;
							vIPPortInfodata.push_back(strMapInternetIPsFieldName);
							vIPPortInfodata.push_back(strmapinternetipsvalue);
						}
					}

					bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMSet(strdeviceipportdeviceidkey, vIPPortInfodata);
					
					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister update redis db(3) Table:%s ipport info.\n", device_ipport_deviceid);

					// ���������µ�relaymc��relayserver��Ϣ�浽local_domain_deviceinfo����,ͬʱ֪ͨ�ϼ�devmgr
					if (req->svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC
						|| req->svr_type() == DEVICE_SERVER::DEVICE_RELAYSERVER
						|| req->svr_type() == DEVICE_SERVER::DEVICE_STUNSERVER)
					{
						char svrtype_s[16] = { 0 };
						sprintf(svrtype_s, "%d", req->svr_type());
						m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue("local_domain_deviceinfo", svrtype_s, id);

						sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister update redis db(3) Table:local_domain_deviceinfo svrtype info[%s, %s].\n", id, svrtype_s);

						// ��ȡ��������
						RedisReplyArray vrra_domain_level;
						vrra_domain_level.clear();
						unsigned int nDomainAndLevel = 0;

						bool bHGetAllDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_domain_level);

						nDomainAndLevel = vrra_domain_level.size();

						if (bHGetAllDomain_ok
							&& (nDomainAndLevel > 0)
							&& (nDomainAndLevel % 2 == 0))
						{
							for (int idx = 0; idx < nDomainAndLevel;)
							{
								int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
								//vrra_domain_level[idx].str;// ����
								iLevel = atoi(vrra_domain_level[idx + 1].str.c_str());// level
								if (iLevel == 0)
								{
									strdevdomainname.assign(vrra_domain_level[idx].str.c_str());

									//// ���ϼ�devmgrͬ��
									//if (req->svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC
									//	|| req->svr_type() == DEVICE_SERVER::DEVICE_RELAYSERVER)
									{
										if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr
											&& strdevdomainname.length() > 0)
										{
											SRMsgs::IndSubSvrInfoToDevmgr indsvrinfo;
											indsvrinfo.set_addordel(1);
											indsvrinfo.set_deviceid(m_uiMyDeviceid);
											indsvrinfo.set_token(m_szMyToken);

											SRMsgs::IndSubSvrInfoToDevmgr_DomainInfo* psvrdomaininfo = 0;
											{
												psvrdomaininfo = indsvrinfo.add_domaininfos();
												psvrdomaininfo->set_domainname(strdevdomainname);
												psvrdomaininfo->set_level(iLevel);
												SRMsgs::IndSubSvrInfoToDevmgr_ServerInfo* pSvrinfo = 0;
												{
													pSvrinfo = psvrdomaininfo->add_svrinfos();
													pSvrinfo->set_svrtype(req->svr_type());
													pSvrinfo->set_deviceid(pDevice->GetDeviceID());

													SRMsgs::IndSubSvrInfoToDevmgr_IPPORTInfo* pSvripportinfo = new SRMsgs::IndSubSvrInfoToDevmgr_IPPORTInfo();
													pSvripportinfo->set_inner(inner_ipprort_value);
													pSvripportinfo->set_outer(outer_ipprort_value);
													pSvripportinfo->set_vpn(vpn_ipprort_value);
													pSvrinfo->set_allocated_ipport(pSvripportinfo);
												}
											}

											SerialAndSendDevmgr((AcitiveConnect*)(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr), getMsgIdByClassName(IndSubSvrInfoToDevmgr), &indsvrinfo);
										}
									}
									
									//// ���¼�devmgrͬ��
									//if (req->svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC
									//	|| req->svr_type() == DEVICE_SERVER::DEVICE_STUNSERVER)
									{
										// ��(��)��������Ϊ��
										if (strdevdomainname.length() > 0)
										{
											SRMsgs::IndUpSvrInfoToDevmgr indsvrinfo;
											indsvrinfo.set_addordel(1);
											//indsvrinfo.set_deviceid(pDevice->GetDeviceID());// �¼�devmgr��id
											//indsvrinfo.set_token(pDevice->GetDeviceToken());// �¼�devmgr��token
											SRMsgs::IndUpSvrInfoToDevmgr_DomainInfo* psvrdomaininfo = 0;
											{
												psvrdomaininfo = indsvrinfo.add_domaininfos();
												psvrdomaininfo->set_domainname(strdevdomainname);
												psvrdomaininfo->set_level(iLevel);// ������ǿ��д0

												SRMsgs::IndUpSvrInfoToDevmgr_ServerInfo* pSvrinfo = 0;
												{
													pSvrinfo = psvrdomaininfo->add_svrinfos();
													pSvrinfo->set_svrtype(req->svr_type());
													pSvrinfo->set_deviceid(pDevice->GetDeviceID());

													SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo* pSvripportinfo = new SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo();
													pSvripportinfo->set_inner(inner_ipprort_value);
													pSvripportinfo->set_outer(outer_ipprort_value);
													pSvripportinfo->set_vpn(vpn_ipprort_value);
													pSvrinfo->set_allocated_ipport(pSvripportinfo);
												}
											}

											// 2������¼�devmgrָ��
											for (iter_mapDeviceHeartbeatTime dev_itor = m_MapDeviceHeartbeatTime.begin();
												dev_itor != m_MapDeviceHeartbeatTime.end(); dev_itor++)
											{
												if (dev_itor->second.devicetype == DEVICE_SERVER::DEVICE_DEV)
												{
													if (dev_itor->second.pClient)
													{
														indsvrinfo.set_deviceid(dev_itor->first);// �¼�devmgr��id
														indsvrinfo.set_token(dev_itor->second.token);// �¼�devmgr��token

														SerialProtoMsgAndSend(dev_itor->second.pClient, getMsgIdByClassName(IndUpSvrInfoToDevmgr), &indsvrinfo);
													}
												}
											}
										}
									}
									
									break;
								}
								idx += 2;
							}
						}
					}
				}

				//������Ӧֵ
				rspsend->set_isok(true);
				rspsend->set_deviceid(pDevice->GetDeviceID());
				rspsend->set_token(pDevice->GetDeviceToken().c_str());
				rspsend->set_failurereason("");
				rspsend->set_groupid(mediagroup_id);
				if (req->svr_type() == DEVICE_SERVER::DEVICE_MC
					|| req->svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC)
				{
					rspsend->set_domainname(strdevdomainname);
				}
				if (req->svr_type() == DEVICE_SERVER::DEVICE_GW)
				{
					SRMsgs::RspRegister_CapInfo* paddcapinfo = 0;

					//������Ƶ������
					std::list<std::string> audioProtocols;
					audioProtocols.clear();
					pDataDictionary->SelectAudioProtocol(audioProtocols);
					if (audioProtocols.size() > 0)
					{
						paddcapinfo = rspsend->add_capinfos();
						paddcapinfo->set_captype(1);

						SRMsgs::RspRegister_Capset* paddcapset = 0;

						for (std::list<std::string>::iterator ap_itor = audioProtocols.begin();
							ap_itor != audioProtocols.end(); ap_itor++)
						{
							paddcapset = paddcapinfo->add_capsets();
							paddcapset->set_fmt((*ap_itor).c_str());
						}
					}

					//������Ƶ������
					std::list<std::string> videoProtocols;
					videoProtocols.clear();
					pDataDictionary->SelectVideoProtocol(videoProtocols);
					if (videoProtocols.size() > 0)
					{
						paddcapinfo = rspsend->add_capinfos();
						paddcapinfo->set_captype(2);

						SRMsgs::RspRegister_Capset* paddcapset = 0;

						for (std::list<std::string>::iterator vp_itor = videoProtocols.begin();
							vp_itor != videoProtocols.end(); vp_itor++)
						{
							paddcapset = paddcapinfo->add_capsets();
							paddcapset->set_fmt((*vp_itor).c_str());
						}
					}
				}
				if (pClient != NULL)
				{
					bRegister = true;
					pClient->SetDeviceID(pDevice->GetDeviceID());

					//// mcע��ʱЯ�����������л�����Ϣ�����������Ϣ��Ϊ������Ҫ��������ͬ��
					//// ��һ��ע�ᣬ��Ӧ�û��л�����Ϣ������Ҫͬ������
					//if (req->svr_type() == 1
					//	&& req->conflist_size() > 0)
					//{
					//	time_t timeNow;
					//	timeNow = time(NULL);
					//	SyncConfInfo(req, pDevice, timeNow);
					//}
				}

				sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqRegister pDevice->InsertDB() %s success.\n", req->auth_password().c_str());
			}
		}
	}

	return bRegister;
}

bool CDevMgr::ParseServerData(AcitiveConnect *pSvr, CAsyncThread* pThread, const char* pData, const int nLen, unsigned long recv_data_time)
{
	if (nLen < 16)
	{
		sr_printf(SR_LOGLEVEL_ERROR, " ParseServerData data len is too small !!!!\n");
		return false;
	}
	
	CDevice* pDevice = NULL;
	std::map<CAsyncThread*, CDevice*>::iterator iter_mapAsyncThreadDevice = m_mapAsyncThreadDevice.find(pThread);
	if (iter_mapAsyncThreadDevice != m_mapAsyncThreadDevice.end())
	{
		pDevice = iter_mapAsyncThreadDevice->second;
	}

	CDeviceConfig* pDeviceConfig = NULL;
	std::map<CAsyncThread*, CDeviceConfig*>::iterator iter_mapAsyncThreadDeviceConfig = m_mapAsyncThreadDeviceConfig.find(pThread);
	if (iter_mapAsyncThreadDeviceConfig != m_mapAsyncThreadDeviceConfig.end())
	{
		pDeviceConfig = iter_mapAsyncThreadDeviceConfig->second;
	}

	CConference* pConference = NULL;
	std::map<CAsyncThread*, CConference*>::iterator iter_mapAsyncThreadConference = m_mapAsyncThreadConference.find(pThread);
	if (iter_mapAsyncThreadConference != m_mapAsyncThreadConference.end())
	{
		pConference = iter_mapAsyncThreadConference->second;
	}

	CConfReport* pConfReport = NULL;
	std::map<CAsyncThread*, CConfReport*>::iterator iter_mapAsyncThreadConfReport = m_mapAsyncThreadConfReport.find(pThread);
	if (iter_mapAsyncThreadConfReport != m_mapAsyncThreadConfReport.end())
	{
		pConfReport = iter_mapAsyncThreadConfReport->second;
	}

	CUserConfDetail* pUserConfDetail = NULL;
	std::map<CAsyncThread*, CUserConfDetail*>::iterator iter_mapAsyncThreadUserConfDetial = m_mapAsyncThreadUserConfDetial.find(pThread);
	if (iter_mapAsyncThreadUserConfDetial != m_mapAsyncThreadUserConfDetial.end())
	{
		pUserConfDetail = iter_mapAsyncThreadUserConfDetial->second;
	}

	CDeviceConfDetail* pDeviceConfDetail = NULL;
	std::map<CAsyncThread*, CDeviceConfDetail*>::iterator iter_mapAsyncThreadDeviceConfDetail = m_mapAsyncThreadDeviceConfDetail.find(pThread);
	if (iter_mapAsyncThreadDeviceConfDetail != m_mapAsyncThreadDeviceConfDetail.end())
	{
		pDeviceConfDetail = iter_mapAsyncThreadDeviceConfDetail->second;
	}

	CConfParticipant* pConfParticipant = NULL;
	std::map<CAsyncThread*, CConfParticipant*>::iterator iter_mapAsyncThreadConfParticipant = m_mapAsyncThreadConfParticipant.find(pThread);
	if (iter_mapAsyncThreadConfParticipant != m_mapAsyncThreadConfParticipant.end())
	{
		pConfParticipant = iter_mapAsyncThreadConfParticipant->second;
	}

	CConfLiveSetting* pConfLiveSetting = NULL;
	std::map<CAsyncThread*, CConfLiveSetting*>::iterator iter_mapAsyncThreadConfLiveSetting = m_mapAsyncThreadConfLiveSetting.find(pThread);
	if (iter_mapAsyncThreadConfLiveSetting != m_mapAsyncThreadConfLiveSetting.end())
	{
		pConfLiveSetting = iter_mapAsyncThreadConfLiveSetting->second;
	}

	if (pDevice == NULL
		|| pDeviceConfig == NULL
		|| pConference == NULL
		|| pConfReport == NULL
		|| pUserConfDetail == NULL
		|| pDeviceConfDetail == NULL
		|| pConfParticipant == NULL
		|| pConfLiveSetting == NULL)
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	//�������ݽ����󣬷ֱ���
	SRMC::S_ProtoMsgHeader header;
	SRMC::unpackPMH(pData + 8, &header);
	//SRMC::unpackPMH(pData, &header);

	unsigned long ulTimeNow = getTickCount();
	unsigned long msg_in_thread_time = ulTimeNow - recv_data_time;

	sr_printf(SR_LOGLEVEL_NORMAL, "==recv==Server===header.m_msguid-------->>>%04X, %d\n", header.m_msguid, header.m_msguid);
	switch (header.m_msguid)
	{
	case SRMsgId_IndDevtoserHeart:
	{
		SRMsgs::IndDevtoserHeart indheart;
		SR_bool isparseok = indheart.ParsePartialFromArray(pData + 16/*sizeof(SRMC::S_ProtoMsgHeader)*/, nLen - 16);
		if (false == isparseok)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "protobuf_msguid=0x%x,SRMsgId_IndDevtoserHeart ParsePartialFromArray error\n", header.m_msguid);
			return false;
		}
		if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr != pSvr)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "IndDevtoserHeart devmgr sockptr(%p) is not the current devmgr connected sockptr(%p)\n", pSvr, m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr);
			return false;
		}

		sr_printf(SR_LOGLEVEL_DEBUG, "devmgr(server)->me(client), IndDevtoserHeart -- \n%s\n", indheart.Utf8DebugString().c_str());

		m_devConnMgr->m_devmgrinfomanager->m_current_dev_stat = DEVMGR_HEART_FLAGS;
		break;
	}
	case SRMsgId_RspRegister:
	{
		SRMsgs::RspRegister rsp;
		if (rsp.ParsePartialFromArray(pData + 16, nLen - 16))
		{

			if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr != pSvr)
			{
				sr_printf(SR_LOGLEVEL_ERROR, "RspRegister devmgr isnot the current connected devmgr\n");
				return false;
			}

			sr_printf(SR_LOGLEVEL_DEBUG, "devmgr(server)->me(client), RspRegister -- \n%s\n", rsp.Utf8DebugString().c_str());

			//if (rsp.has_ip() && rsp.has_port() && rsp.has_auth_password() && rsp.has_svr_type() && rsp.has_cpunums())
			//{
				processRspRegister(&rsp);
			//}
			//else
			//{
			//	sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_RspRegister msg param is null error\n");
			//}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_RspRegister ParsePartialFromArray failed.\n");
		}

		break;
	}
	case SRMsgId_IndUpSvrInfoToDevmgr:
	{
		SRMsgs::IndUpSvrInfoToDevmgr ind;
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{

			if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr != pSvr)
			{
				sr_printf(SR_LOGLEVEL_ERROR, "IndUpSvrInfoToDevmgr devmgr isnot the current connected devmgr\n");
				return false;
			}


			sr_printf(SR_LOGLEVEL_DEBUG, "devmgr(server)->me(client), IndUpSvrInfoToDevmgr -- \n%s\n", ind.Utf8DebugString().c_str());

//#ifdef LINUX
//			float time_use = 0;
//			struct timeval start;
//			struct timeval end;
//			gettimeofday(&start, NULL); // ��s level
//#endif 
			//��ȡ��ǰʱ�䴮
			time_t timeNow;
			struct tm *ptmNow;
			char szTime[30];
			timeNow = time(NULL);
			ptmNow = localtime(&timeNow);
			sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

			SyncUpSvrInfo(&ind, timeNow);

//#ifdef LINUX
//				gettimeofday(&end, NULL); // ��s level
//			time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
//
//			sr_printf(SR_LOGLEVEL_INFO, "Handle IndUpSvrInfoToDevmgr param(deviceid,token,addordel)=(%d, %s, %d)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), ind.addordel(), time_use);
//#endif 
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndUpSvrInfoToDevmgr ParsePartialFromArray failed.\n");
		}

		break;
	}
	case SRMsgId_IndUpSvrHeartTodev:
	{
		SRMsgs::IndUpSvrHeartTodev ind;
		if (ind.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr != pSvr)
			{
				sr_printf(SR_LOGLEVEL_ERROR, "IndUpSvrHeartTodev devmgr isnot the current connected devmgr\n");
				return false;
			}


			sr_printf(SR_LOGLEVEL_DEBUG, "devmgr(server)->me(client), IndUpSvrHeartTodev -- \n%s\n", ind.Utf8DebugString().c_str());

//#ifdef LINUX
//			float time_use = 0;
//			struct timeval start;
//			struct timeval end;
//			gettimeofday(&start, NULL); // ��s level
//#endif 
			//��ȡ��ǰʱ�䴮
			time_t timeNow;
			struct tm *ptmNow;
			char szTime[30];
			timeNow = time(NULL);
			ptmNow = localtime(&timeNow);
			sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

			UpdateUpSvrHeartbeatToDB(&ind, timeNow);

//#ifdef LINUX
//			gettimeofday(&end, NULL); // ��s level
//			time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
//
//			sr_printf(SR_LOGLEVEL_INFO, "Handle IndUpSvrHeartTodev param(deviceid,token)=(%d, %s)*****time_use** is: %lf us \n", ind.deviceid(), ind.token().c_str(), time_use);
//#endif 

		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndUpSvrHeartTodev ParsePartialFromArray failed.\n");
		}

		break;
	}
	case SRMsgId_RspGetSysDeviceInfo:
	{
		SRMsgs::RspGetSysDeviceInfo rsp;
		if (rsp.ParsePartialFromArray(pData + 16, nLen - 16))
		{
			if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr != pSvr)
			{
				sr_printf(SR_LOGLEVEL_ERROR, "RspGetSysDeviceInfo devmgr isnot the current connected devmgr\n");
				return false;
			}

			sr_printf(SR_LOGLEVEL_DEBUG, "devmgr(server)->me(client), RspGetSysDeviceInfo -- \n%s\n", rsp.Utf8DebugString().c_str());

			if (rsp.has_deviceid() && rsp.has_token() && rsp.has_svrtype())
			{

#ifdef LINUX
				float time_use = 0;
				struct timeval start;
				struct timeval end;
				gettimeofday(&start, NULL); // ��s level
#endif

				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);
				SyncSysSvrInfo(&rsp, timeNow);

#ifdef LINUX
				gettimeofday(&end, NULL); // ��s level
				time_use = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

				sr_printf(SR_LOGLEVEL_INFO, "**Handle SRMsgId_RspGetSysDeviceInfo param(deviceid,token,svrtype)=(%d, %s, %d)*****time_use** is: %lf us \n", rsp.deviceid(), rsp.token().c_str(), rsp.svrtype(), time_use);
#endif 

			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_RspGetSysDeviceInfo param is null error\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_RspGetSysDeviceInfo ParsePartialFromArray failed.\n");
		}
		break;
	}
	}//switch (header.m_msguid)
	return true;
}

SR_void CDevMgr::processRspRegister(const SRMsgs::RspRegister* s)
{
	if ((!s->has_isok()) || (!s->has_deviceid()) || (!s->has_token()))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "RspRegister param is null,then after(%ds) to re-Register.\n", CONNECT_DEVMGR_TIME);
		//sr_printf(SR_LOGLEVEL_ERROR, "RspRegister param is null,then after(%ds) to re-Register.\n", MCCfgInfo::instance()->get_reconnect_devmgr_time());

		createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_re_register_timer, 0ull);
		return;
	}
	if (false == s->isok())
	{
		sr_printf(SR_LOGLEVEL_INFO, "me Register DevMgr Fail,then after(%ds) to re-Register.\n", CONNECT_DEVMGR_TIME);
		//m_isRegisterOK = false;

		createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_re_register_timer, 0ull);
		return;
	}

	m_uiMyDeviceid = s->deviceid();
	m_szMyToken = s->token();
	m_uiMyGroupid = s->groupid();

	sr_printf(SR_LOGLEVEL_INFO, "my Register DevMgr OK\n");

	if (!m_heart2devmgr_periodtimer)
		m_heart2devmgr_periodtimer = createTimer(SEND_HEARTBEAT_TO_DEVMGR_TIME, SRMC::e_periodicity_timer, e_hearttodevmgr_timer, 0ull);

	m_devConnMgr->m_devmgrinfomanager->m_current_dev_stat = DEVMGR_HEART_FLAGS;
	if (!m_check_svr_heartbeat_timer)
	{
		sr_printf(SR_LOGLEVEL_NORMAL, "next startup check devmgr_heart_timer\n");

		m_check_svr_heartbeat_timer = createTimer(CHECK_SVR_HEARTBEAT_TIME, SRMC::e_periodicity_timer, e_check_svr_heartbeat_timer, 0llu);
	}

	if (!m_req_up_relaymcinfo_periodtimer)
	{
		sr_printf(SR_LOGLEVEL_NORMAL, "next startup req_up_relaymcinfo_timer\n");

		m_req_up_relaymcinfo_periodtimer = createTimer(CHECK_SVR_HEARTBEAT_TIME, SRMC::e_periodicity_timer, e_req_up_relaymcinfo_timer, 0llu);
	}
	if (!m_req_up_relayserverinfo_periodtimer)
	{
		sr_printf(SR_LOGLEVEL_NORMAL, "next startup req_up_relayserverinfo_timer\n");

		m_req_up_relayserverinfo_periodtimer = createTimer(CHECK_SVR_HEARTBEAT_TIME, SRMC::e_periodicity_timer, e_req_up_relayserverinfo_timer, 0llu);
	}
	if (!m_req_up_stunserverinfo_periodtimer)
	{
		sr_printf(SR_LOGLEVEL_NORMAL, "next startup req_up_stunserverinfo_timer\n");

		m_req_up_stunserverinfo_periodtimer = createTimer(CHECK_SVR_HEARTBEAT_TIME, SRMC::e_periodicity_timer, e_req_up_stunserverinfo_timer, 0llu);
	}

	// ע��ɹ��������ϼ�devmgr��������
	SendHeartBeatToDevmgr();

	// �������ͱ����������Ѿ�ע��ķ�����ϢIndSubSvrInfoToDevmgr
	// 1����ȡ��������
	char local_domain_name[128] = { 0 };
	RedisReplyArray vrra_domain_level;
	vrra_domain_level.clear();
	unsigned int nDomainAndLevel = 0;

	m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
	bool bHGetAllDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_domain_level);

	nDomainAndLevel = vrra_domain_level.size();

	if (bHGetAllDomain_ok
		&& (nDomainAndLevel > 0)
		&& (nDomainAndLevel % 2 == 0))
	{
		for (int idx = 0; idx < nDomainAndLevel;)
		{
			int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
			//vrra_domain_level[idx].str;// ����
			iLevel = atoi(vrra_domain_level[idx + 1].str.c_str());// level
			if (iLevel == 0)
			{
				sprintf(local_domain_name, "%s", vrra_domain_level[idx].str.c_str());
				break;
			}
			idx += 2;
		}
	}

	if (strlen(local_domain_name) > 0)
	{
		// 2���鱾����local_domain_deviceinfo��svr_type==3��svr_type==4��svr_type==11��deviceid
		// ��������豸��Ϣ[deviceid,svrtype]
		RedisReplyArray vrra_deviceid_svrtype;
		vrra_deviceid_svrtype.clear();
		bool bHGetAllok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_deviceinfo", vrra_deviceid_svrtype);

		unsigned int nDeviceidandSvrtypeNum = vrra_deviceid_svrtype.size();

		bool bNeedSend = false;

		if (bHGetAllok
			&& (nDeviceidandSvrtypeNum > 0)
			&& (nDeviceidandSvrtypeNum % 2 == 0))
		{
			SRMsgs::IndSubSvrInfoToDevmgr indsvrinfo;
			indsvrinfo.set_addordel(1);
			indsvrinfo.set_deviceid(m_uiMyDeviceid);
			indsvrinfo.set_token(m_szMyToken);
			SRMsgs::IndSubSvrInfoToDevmgr_DomainInfo* psvrdomaininfo = 0;
			{
				psvrdomaininfo = indsvrinfo.add_domaininfos();
				psvrdomaininfo->set_domainname(local_domain_name);
				psvrdomaininfo->set_level(0);// ������ǿ��д0

				SRMsgs::IndSubSvrInfoToDevmgr_ServerInfo* pSvrinfo = 0;

				for (int i = 0; i < nDeviceidandSvrtypeNum;)
				{
					char deviceid_value[128] = { 0 };
					char svrtype_value[128] = { 0 };
					SR_uint32 uiDeviceid = 0;
					SR_uint32 uiSvrtype = 0;
					sprintf(deviceid_value, "%s", vrra_deviceid_svrtype[i].str.c_str());
					sprintf(svrtype_value, "%s", vrra_deviceid_svrtype[i + 1].str.c_str());
					sscanf(vrra_deviceid_svrtype[i].str.c_str(), "%u", &uiDeviceid);
					sscanf(vrra_deviceid_svrtype[i + 1].str.c_str(), "%u", &uiSvrtype);

					if (uiSvrtype == DEVICE_SERVER::DEVICE_RELAY_MC
						|| uiSvrtype == DEVICE_SERVER::DEVICE_RELAYSERVER
						|| uiSvrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
					{
						char device_ipport_deviceid[128] = { 0 };
						sprintf(device_ipport_deviceid, "device_ipport_%s", deviceid_value);

						char cInnerFieldName[128] = { 0 };
						char cOuterFieldName[128] = { 0 };
						char cVPNFieldName[128] = { 0 };

						char inner_ipprort_value[65] = { 0 };
						char outer_ipprort_value[65] = { 0 };
						char vpn_ipprort_value[65] = { 0 };

						sprintf(cInnerFieldName, "%s", "inner");
						sprintf(cOuterFieldName, "%s", "outer");
						sprintf(cVPNFieldName, "%s", "vpn");

						REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
						REDISFILEDS vGetFileds;
						vGetFileds.clear();
						RedisReplyArray vRedisReplyArray;
						vRedisReplyArray.clear();

						// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
						REDISKEY strInnerFieldName = cInnerFieldName;
						REDISKEY strOuterFieldName = cOuterFieldName;
						REDISKEY strVPNFieldName = cVPNFieldName;
						vGetFileds.push_back(strInnerFieldName);
						vGetFileds.push_back(strOuterFieldName);
						vGetFileds.push_back(strVPNFieldName);

						bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strdeviceipportdeviceidkey, vGetFileds, vRedisReplyArray);
						if (bhashHMGet_ok
							&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
						{
							pSvrinfo = psvrdomaininfo->add_svrinfos();
							pSvrinfo->set_svrtype(uiSvrtype);
							pSvrinfo->set_deviceid(uiDeviceid);

							SRMsgs::IndSubSvrInfoToDevmgr_IPPORTInfo* pSvripportinfo = new SRMsgs::IndSubSvrInfoToDevmgr_IPPORTInfo();
							pSvripportinfo->set_inner(vRedisReplyArray[0].str);
							pSvripportinfo->set_outer(vRedisReplyArray[1].str);
							pSvripportinfo->set_vpn(vRedisReplyArray[2].str);
							pSvrinfo->set_allocated_ipport(pSvripportinfo);

							bNeedSend = true;
						}
					}

					i += 2;
				}
			}

			if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr
				&& bNeedSend == true)
			{

				SerialAndSendDevmgr((AcitiveConnect*)(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr), getMsgIdByClassName(IndSubSvrInfoToDevmgr), &indsvrinfo);
			}
		}
	}
}

void CDevMgr::SyncUpSvrInfo(const SRMsgs::IndUpSvrInfoToDevmgr* ind, time_t timeNow)
{
	// Ŀǰֻͬ��relaymc��relayserver��stunserver����Ϣ
	if (ind->domaininfos_size() > 0)
	{
		for (int i = 0; i < ind->domaininfos_size(); i++)
		{
			const SRMsgs::IndUpSvrInfoToDevmgr_DomainInfo& domaininfo = ind->domaininfos(i);

			if (domaininfo.domainname().length() > 0
				&& domaininfo.level() != 99999) // ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
			{
				char domainname_s[128] = { 0 };
				sprintf(domainname_s, "%s", domaininfo.domainname().c_str());
				char sysdomaindevinfo_s[128] = { 0 };
				sprintf(sysdomaindevinfo_s, "sys_domain_deviceinfo_%s", domaininfo.domainname().c_str());
				int level = domaininfo.level();
				
				//char subdomain_level_s[64] = { 0 };
				//sprintf(subdomain_level_s, "%d", domaininfo.level() - 1);

				//m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
				//// add
				//if (ind->addordel() == 1)
				//{
				//	m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(sysdomaindevinfo_s, svrtype_s, deviceid_s);
				//}
				//else if (ind->addordel() == 2)// delete
				//{
				//	m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue(sysdomaindevinfo_s, deviceid_s);
				//}

				for (int j = 0; j < domaininfo.svrinfos_size(); j++)
				{
					const SRMsgs::IndUpSvrInfoToDevmgr_ServerInfo& svrinfo = domaininfo.svrinfos(j);
					unsigned int uisvrtype = svrinfo.svrtype();
					unsigned int uideviceid = svrinfo.deviceid();
					if (uideviceid == 0
						|| (uisvrtype != DEVICE_SERVER::DEVICE_RELAY_MC
						&& uisvrtype != DEVICE_SERVER::DEVICE_RELAYSERVER
						&& uisvrtype != DEVICE_SERVER::DEVICE_STUNSERVER))
					{
						continue;
					}
					else
					{
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);

						char svrtype_s[33] = { 0 };
						char deviceid_s[33] = { 0 };
						sprintf(svrtype_s, "%u", uisvrtype);
						sprintf(deviceid_s, "%u", uideviceid);

						char sys_svrtype_dmomain_s[256] = { 0 };
						char svrtype_ipport_dmomain_deviceid_s[256] = { 0 };
						if (uisvrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
						{
							sprintf(sys_svrtype_dmomain_s, "sys_relaymc_%s", domainname_s);
							sprintf(svrtype_ipport_dmomain_deviceid_s, "relaymc_ipport_%s_%u", domainname_s, uideviceid);
						}
						else if (uisvrtype == DEVICE_SERVER::DEVICE_RELAYSERVER)
						{
							sprintf(sys_svrtype_dmomain_s, "sys_relayserver_%s", domainname_s);
							sprintf(svrtype_ipport_dmomain_deviceid_s, "relayserver_ipport_%s_%u", domainname_s, uideviceid);
						}
						else if (uisvrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
						{
							sprintf(sys_svrtype_dmomain_s, "sys_stunserver_%s", domainname_s);
							sprintf(svrtype_ipport_dmomain_deviceid_s, "stunserver_ipport_%s_%u", domainname_s, uideviceid);
						}

						const SRMsgs::IndUpSvrInfoToDevmgr_IPPORTInfo& svripportinfo = svrinfo.ipport();

						// add
						if (ind->addordel() == 1)
						{
							m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(sysdomaindevinfo_s, svrtype_s, deviceid_s);

							char cInnerFieldName[128] = { 0 };
							char cOuterFieldName[128] = { 0 };
							char cVPNFieldName[128] = { 0 };

							char inner_ipprort_value[65] = { 0 };
							char outer_ipprort_value[65] = { 0 };
							char vpn_ipprort_value[65] = { 0 };

							sprintf(cInnerFieldName, "%s", "inner");
							sprintf(cOuterFieldName, "%s", "outer");
							sprintf(cVPNFieldName, "%s", "vpn");

							sprintf(inner_ipprort_value, "%s", svripportinfo.inner().c_str());
							sprintf(outer_ipprort_value, "%s", svripportinfo.outer().c_str());
							sprintf(vpn_ipprort_value, "%s", svripportinfo.vpn().c_str());
							

							REDISKEY strsvrtypeipportdmomaindeviceidkey = svrtype_ipport_dmomain_deviceid_s;
							REDISVDATA vIPPortInfodata;
							vIPPortInfodata.clear();

							REDISKEY strInnerFieldName = cInnerFieldName;
							REDISVALUE strinneripprortvalue = inner_ipprort_value;
							vIPPortInfodata.push_back(strInnerFieldName);
							vIPPortInfodata.push_back(strinneripprortvalue);

							REDISKEY strOuterFieldName = cOuterFieldName;
							REDISVALUE strouteripprortvalue = outer_ipprort_value;
							vIPPortInfodata.push_back(strOuterFieldName);
							vIPPortInfodata.push_back(strouteripprortvalue);

							REDISKEY strVPNFieldName = cVPNFieldName;
							REDISVALUE strvpnipprortvalue = vpn_ipprort_value;
							vIPPortInfodata.push_back(strVPNFieldName);
							vIPPortInfodata.push_back(strvpnipprortvalue);

							bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMSet(strsvrtypeipportdmomaindeviceidkey, vIPPortInfodata);
						}
						else if (ind->addordel() == 2)// delete
						{
							m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue(sysdomaindevinfo_s, deviceid_s);

							m_pRedisConnList[e_RC_TT_MainThread]->deletevalue(svrtype_ipport_dmomain_deviceid_s);

							// ����
							m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);
							m_pRedisConnList[e_RC_TT_MainThread]->deletevalue(sys_svrtype_dmomain_s, deviceid_s);
							// ����
							m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);
							m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue(sys_svrtype_dmomain_s, deviceid_s);
						}
					}
				}
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SyncUpSvrInfo IndUpSvrInfoToDevmgr msg domaininfo error!!!\n");
			}
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SyncUpSvrInfo IndUpSvrInfoToDevmgr msg domaininfos_size=%d error!!!\n", ind->domaininfos_size());
	}
}

void CDevMgr::UpdateUpSvrHeartbeatToDB(const SRMsgs::IndUpSvrHeartTodev* ind, time_t timeNow)
{
	// Ŀǰֻͬ��relaymc��relayserver��stunserver�ĸ�����Ϣ
	if (ind->domaininfos_size() > 0)
	{
		for (int i = 0; i < ind->domaininfos_size(); i++)
		{
			const SRMsgs::IndUpSvrHeartTodev_DomainInfo& domaininfo = ind->domaininfos(i);

			if (domaininfo.domainname().length() > 0
				&& domaininfo.level() != 99999) // ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
			{
				char domainname_s[128] = { 0 };
				sprintf(domainname_s, "%s", domaininfo.domainname().c_str());
				char sysdomaindevinfo_s[128] = { 0 };
				sprintf(sysdomaindevinfo_s, "sys_domain_deviceinfo_%s", domaininfo.domainname().c_str());
				//int level = domaininfo.level();

				for (int j = 0; j < domaininfo.svrinfos_size(); j++)
				{
					const SRMsgs::IndUpSvrHeartTodev_ServerInfo& svrinfo = domaininfo.svrinfos(j);
					unsigned int uisvrtype = svrinfo.svrtype();
					unsigned int uideviceid = svrinfo.deviceid();
					if (uideviceid == 0
						|| (uisvrtype != DEVICE_SERVER::DEVICE_RELAY_MC
						&& uisvrtype != DEVICE_SERVER::DEVICE_RELAYSERVER
						&& uisvrtype != DEVICE_SERVER::DEVICE_STUNSERVER))
					{
						continue;
					}
					else
					{

						const SRMsgs::IndUpSvrHeartTodev_LoadInfo& svrloadinfo = svrinfo.loadinfos();

						char svrtype_s[33] = { 0 };
						char deviceid_s[33] = { 0 };
						sprintf(svrtype_s, "%u", uisvrtype);
						sprintf(deviceid_s, "%u", uideviceid);

						// ����
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);

						char sys_svrtype_dmomain_s[256] = { 0 };
						if (uisvrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
						{
							sprintf(sys_svrtype_dmomain_s, "sys_relaymc_%s", domainname_s);

							char ternum_s[33] = { 0 };
							sprintf(ternum_s, "%d", svrloadinfo.ternum());
							m_pRedisConnList[e_RC_TT_MainThread]->setvalue(sys_svrtype_dmomain_s, ternum_s, deviceid_s);

							sr_printf(SR_LOGLEVEL_DEBUG, "UpdateUpSvrHeartbeatToDB IndUpSvrHeartTodev setvalue to redis db(0) Table:%s :(deviceid, ternum)=(%s, %s)\n", sys_svrtype_dmomain_s, deviceid_s, ternum_s);
						}
						else if (uisvrtype == DEVICE_SERVER::DEVICE_RELAYSERVER
							|| uisvrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
						{
							if (uisvrtype == DEVICE_SERVER::DEVICE_RELAYSERVER)
							{
								sprintf(sys_svrtype_dmomain_s, "sys_relayserver_%s", domainname_s);
							}
							else if (uisvrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
							{
								sprintf(sys_svrtype_dmomain_s, "sys_stunserver_%s", domainname_s);
							}

							//uidevicetype == 4���relayserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���

							//uidevicetype == 11���stunserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���
							unsigned long long ulltl = ((unsigned long long)(svrloadinfo.tx()) << 32) + svrloadinfo.ternum();
							char svr_load_s[128] = { 0 };
#ifdef WIN32
							sprintf(svr_load_s, "%I64d", ulltl);
#elif defined LINUX
							sprintf(svr_load_s, "%lld", ulltl);
#endif	
							m_pRedisConnList[e_RC_TT_MainThread]->setvalue(sys_svrtype_dmomain_s, svr_load_s, deviceid_s);

							sr_printf(SR_LOGLEVEL_DEBUG, "UpdateUpSvrHeartbeatToDB IndUpSvrHeartTodev setvalue to redis db(0) Table:%s :(deviceid, tx+ternum)=(%s, %s)\n", sys_svrtype_dmomain_s, deviceid_s, svr_load_s);
						}

						// ����
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);
						char time_s[256] = { 0 };
#ifdef WIN32
						sprintf(time_s, "%I64d", timeNow);
#elif defined LINUX
						sprintf(time_s, "%lld", timeNow);
#endif	
						m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(sys_svrtype_dmomain_s, time_s, deviceid_s);

						sr_printf(SR_LOGLEVEL_DEBUG, "UpdateUpSvrHeartbeatToDB IndUpSvrHeartTodev sethashvalue to redis db(1)  Table:%s :(deviceid, time_s)=(%s, %s)\n", sys_svrtype_dmomain_s, deviceid_s, time_s);
					}// loadinfos
				}// svrinfos
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "UpdateUpSvrHeartbeatToDB IndUpSvrHeartTodev msg domaininfo error!!!\n");
			}
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "UpdateUpSvrHeartbeatToDB IndUpSvrHeartTodev msg domaininfos_size=%d error!!!\n", ind->domaininfos_size());
	}
}

void CDevMgr::SyncSysSvrInfo(const SRMsgs::RspGetSysDeviceInfo* rsp, time_t timeNow)
{
	// Ŀǰֻͬ��relaymc��relayserver��stunserver����Ϣ
	unsigned int rsp_svrtype = 0;
	rsp_svrtype = rsp->svrtype();

	if (!(rsp_svrtype == DEVICE_SERVER::DEVICE_RELAY_MC
		|| rsp_svrtype == DEVICE_SERVER::DEVICE_RELAYSERVER
		|| rsp_svrtype == DEVICE_SERVER::DEVICE_STUNSERVER))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SyncSysSvrInfo RspGetSysDeviceInfo msg svrtype=%d error!!!\n", rsp_svrtype);
		return;
	}
	
	// Ŀǰֻͬ���ϼ����relaymc��relayserver��stunserver��Ϣ
	if (rsp->domaininfos_size() > 0)
	{
		char svrtype_s[33] = { 0 };
		sprintf(svrtype_s, "%u", rsp_svrtype);

		for (int i = 0; i < rsp->domaininfos_size(); i++)
		{
			const SRMsgs::RspGetSysDeviceInfo_DomainInfo& domaininfo = rsp->domaininfos(i);

			if (domaininfo.domainname().length() > 0
				&& domaininfo.level() != 99999) // ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
			{
				char domainname_s[128] = { 0 };
				sprintf(domainname_s, "%s", domaininfo.domainname().c_str());
				char sysdomaindevinfo_s[128] = { 0 };
				sprintf(sysdomaindevinfo_s, "sys_domain_deviceinfo_%s", domaininfo.domainname().c_str());
				char sys_svrtype_dmomain_s[256] = { 0 };
				if (rsp_svrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
				{
					sprintf(sys_svrtype_dmomain_s, "sys_relaymc_%s", domainname_s);
				}
				else if (rsp_svrtype == DEVICE_SERVER::DEVICE_RELAYSERVER)
				{
					sprintf(sys_svrtype_dmomain_s, "sys_relayserver_%s", domainname_s);
				}
				else if (rsp_svrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
				{
					sprintf(sys_svrtype_dmomain_s, "sys_stunserver_%s", domainname_s);
				}
				
				for (int j = 0; j < domaininfo.devices_size(); j++)
				{
					const SRMsgs::RspGetSysDeviceInfo_DeviceInfo& devinfo = domaininfo.devices(j);
					unsigned int uideviceid = devinfo.deviceid();
					if (uideviceid == 0)
					{
						continue;
					}
					else
					{
						char deviceid_s[33] = { 0 };
						sprintf(deviceid_s, "%u", uideviceid);

						// д���豸id�ͷ�������
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
						m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(sysdomaindevinfo_s, svrtype_s, deviceid_s);

						// д��ip��port��ַ��Ϣ
						char svrtype_ipport_dmomain_deviceid_s[256] = { 0 };
						if (rsp_svrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
						{
							sprintf(svrtype_ipport_dmomain_deviceid_s, "relaymc_ipport_%s_%u", domainname_s, uideviceid);
						}
						else if (rsp_svrtype == DEVICE_SERVER::DEVICE_RELAYSERVER)
						{
							sprintf(svrtype_ipport_dmomain_deviceid_s, "relayserver_ipport_%s_%u", domainname_s, uideviceid);
						}
						else if (rsp_svrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
						{
							sprintf(svrtype_ipport_dmomain_deviceid_s, "stunserver_ipport_%s_%u", domainname_s, uideviceid);
						}
						
						const SRMsgs::RspGetSysDeviceInfo_IPPORTInfo& svripportinfo = devinfo.ipport();

						char cInnerFieldName[128] = { 0 };
						char cOuterFieldName[128] = { 0 };
						char cVPNFieldName[128] = { 0 };

						char inner_ipprort_value[65] = { 0 };
						char outer_ipprort_value[65] = { 0 };
						char vpn_ipprort_value[65] = { 0 };

						sprintf(cInnerFieldName, "%s", "inner");
						sprintf(cOuterFieldName, "%s", "outer");
						sprintf(cVPNFieldName, "%s", "vpn");

						sprintf(inner_ipprort_value, "%s", svripportinfo.inner().c_str());
						sprintf(outer_ipprort_value, "%s", svripportinfo.outer().c_str());
						sprintf(vpn_ipprort_value, "%s", svripportinfo.vpn().c_str());

						REDISKEY strsvrtypeipportdmomaindeviceidkey = svrtype_ipport_dmomain_deviceid_s;
						REDISVDATA vIPPortInfodata;
						vIPPortInfodata.clear();

						REDISKEY strInnerFieldName = cInnerFieldName;
						REDISVALUE strinneripprortvalue = inner_ipprort_value;
						vIPPortInfodata.push_back(strInnerFieldName);
						vIPPortInfodata.push_back(strinneripprortvalue);

						REDISKEY strOuterFieldName = cOuterFieldName;
						REDISVALUE strouteripprortvalue = outer_ipprort_value;
						vIPPortInfodata.push_back(strOuterFieldName);
						vIPPortInfodata.push_back(strouteripprortvalue);

						REDISKEY strVPNFieldName = cVPNFieldName;
						REDISVALUE strvpnipprortvalue = vpn_ipprort_value;
						vIPPortInfodata.push_back(strVPNFieldName);
						vIPPortInfodata.push_back(strvpnipprortvalue);

						bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMSet(strsvrtypeipportdmomaindeviceidkey, vIPPortInfodata);

						// д�������͸�����Ϣ
						const SRMsgs::RspGetSysDeviceInfo_LoadInfo& svrloadinfo = devinfo.loadinfos();
						// ����
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);

						if (rsp_svrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
						{
							char ternum_s[33] = { 0 };
							sprintf(ternum_s, "%d", svrloadinfo.ternum());
							m_pRedisConnList[e_RC_TT_MainThread]->setvalue(sys_svrtype_dmomain_s, ternum_s, deviceid_s);

							sr_printf(SR_LOGLEVEL_DEBUG, "SyncSysSvrInfo RspGetSysDeviceInfo setvalue to redis db(0) Table:%s :(deviceid, ternum)=(%s, %s)\n", sys_svrtype_dmomain_s, deviceid_s, ternum_s);
						}
						else if (rsp_svrtype == DEVICE_SERVER::DEVICE_RELAYSERVER
							|| rsp_svrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
						{
							//uidevicetype == 4���relayserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���
							//uidevicetype == 11���stunserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���
							unsigned long long ulltl = ((unsigned long long)(svrloadinfo.tx()) << 32) + svrloadinfo.ternum();
							char svr_load_s[128] = { 0 };
#ifdef WIN32
							sprintf(svr_load_s, "%I64d", ulltl);
#elif defined LINUX
							sprintf(svr_load_s, "%lld", ulltl);
#endif	
							m_pRedisConnList[e_RC_TT_MainThread]->setvalue(sys_svrtype_dmomain_s, svr_load_s, deviceid_s);

							sr_printf(SR_LOGLEVEL_DEBUG, "SyncSysSvrInfo RspGetSysDeviceInfo setvalue to redis db(0) Table:%s :(deviceid, tx+ternum)=(%s, %s)\n", sys_svrtype_dmomain_s, deviceid_s, svr_load_s);
						}

						// ����
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);
						char time_s[256] = { 0 };
#ifdef WIN32
						sprintf(time_s, "%I64d", timeNow);
#elif defined LINUX
						sprintf(time_s, "%lld", timeNow);
#endif	
						m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(sys_svrtype_dmomain_s, time_s, deviceid_s);

						sr_printf(SR_LOGLEVEL_DEBUG, "SyncSysSvrInfo RspGetSysDeviceInfo sethashvalue to redis db(1)  Table:%s :(deviceid, time_s)=(%s, %s)\n", sys_svrtype_dmomain_s, deviceid_s, time_s);
					}
				}
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SyncSysSvrInfo RspGetSysDeviceInfo msg domaininfo error!!!\n");
			}
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SyncSysSvrInfo RspGetSysDeviceInfo msg rsp_svrtype=%d, domaininfos_size=%d error!!!\n", rsp_svrtype, rsp->domaininfos_size());
	}
}

void CDevMgr::GetSubGroupidInfo(SR_uint32 uiParentGroupid, std::set<SR_uint32> &groupids, SR_uint32 &uiNodeNum)
{

	groupids.insert(uiParentGroupid);

	uiNodeNum++;
	if (uiNodeNum == MAX_SUB_NODE_NUM)
	{
		sr_printf(SR_LOGLEVEL_WARNING, "GetSubGroupidInfo uiNodeNum reached MAX_SUB_NODE_NUM(%d), so return.\n", MAX_SUB_NODE_NUM);
		return;
	}

	char submediagroup_groupid[32] = { 0 };
	sprintf(submediagroup_groupid, "submediagroup_%u", uiParentGroupid);

	// ���������uiParentGroupid�������ӽڵ�groupid
	RedisReplyArray vrra_db3_submediagroup_info;
	vrra_db3_submediagroup_info.clear();
	bool bhashHGetAllGroupid_ok = false;
	unsigned int nGroupidAndParentgroupidNum = 0;

	m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3); // ȡ�������������ӽڵ��<groupid, parentgroupid>��Ϣ
	bhashHGetAllGroupid_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(submediagroup_groupid, vrra_db3_submediagroup_info);
	nGroupidAndParentgroupidNum = vrra_db3_submediagroup_info.size();

	if (bhashHGetAllGroupid_ok
		&& (nGroupidAndParentgroupidNum > 0)
		&& (nGroupidAndParentgroupidNum % 2 == 0))
	{
		for (int i = 0; i < nGroupidAndParentgroupidNum;)
		{
			char groupid_value[32] = { 0 };
			char parentgroupid_value[32] = { 0 };
			sprintf(groupid_value, "%s", vrra_db3_submediagroup_info[i].str.c_str());
			sprintf(parentgroupid_value, "%s", vrra_db3_submediagroup_info[i + 1].str.c_str());

			int iTempGroupid = 0;
			iTempGroupid = atoi(groupid_value);//

			if (iTempGroupid != 0)
			{
				GetSubGroupidInfo(iTempGroupid, groupids, uiNodeNum);
			}

			i += 2;
		}
	}
	else
	{
		return;
	}
}

bool CDevMgr::GetDeviceInfo(const SRMsgs::ReqGetDeviceInfo* req, SRMsgs::RspGetDeviceInfo* rsp)
{
	int devnums = 2;
	if (req->has_devnums())
		devnums = req->devnums();

	char req_deviceid_s[128] = { 0 };
	sprintf(req_deviceid_s, "%d", req->deviceid());
	unsigned int req_svr_type = 0;
	req_svr_type = req->svr_type();

	sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_ReqGetDeviceInfo OK: param(deviceid,token,svr_type,get_svr_type, req.devnums)=(%d, %s, %d, %d, %d)\n", req->deviceid(), req->token().c_str(), req_svr_type, req->get_svr_type(), devnums);

	if (req->deviceid() == 0)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetDeviceInfo req->deviceid == 0 error !!!\n");
		return false;
	}

	CNetMpLoad** load = NULL;
	typedef CNetMpLoad* DevLoadPtr;

	std::string req_get_svr_domainname;
	req_get_svr_domainname.clear();
	unsigned int req_get_net_type = 0;// Ĭ��������ַ
	//req_get_net_type = req->nettype(); // Ŀǰ����δ���Ӹ��ֶ�,relaymcǿ���ó�nettype=2����ȡvpn��ַ
	
	std::string getsvrname;
	getsvrname.clear();
	unsigned int req_get_svr_type = 0;
	req_get_svr_type = req->get_svr_type();

	if (req_get_svr_type == DEVICE_SERVER::DEVICE_NETMP) // mc -> netmp��relayserver -> netmp
	{
		getsvrname = "netmp_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo get_svr_type is netmp_ !!!\n");
	}
	else if (req_get_svr_type == DEVICE_SERVER::DEVICE_MC) // relaymc -> mc��suiruigw -> mc
	{
		getsvrname = "mc_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo get_svr_type is mc_ !!!\n");
	}
	else if (req_get_svr_type == DEVICE_SERVER::DEVICE_MP) // mc -> mp
	{
		getsvrname = "mp_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo get_svr_type is mp_ !!!\n");
	}
	else if (req_get_svr_type == DEVICE_SERVER::DEVICE_GW) // mc -> gw
	{
		getsvrname = "gw_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo get_svr_type is gw_ !!!\n");
	}
	else if (req_get_svr_type == DEVICE_SERVER::DEVICE_CRS) // mc -> crs
	{
		getsvrname = "crs_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo get_svr_type is crs_ !!!\n");
	}
	else if (req_get_svr_type == DEVICE_SERVER::DEVICE_SRS) // mc -> srs
	{
		getsvrname = "srs_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo get_svr_type is srs_ !!!\n");
	}
	else if (req_get_svr_type == DEVICE_SERVER::DEVICE_RELAYSERVER) // mc -> relayserver
	{
		getsvrname = "relayserver_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo get_svr_type is relayserver_ !!!\n");
	}
	else if (req_get_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC) // relaymc -> relaymc
	{
		if (req_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC) // relaymc -> relaymc
		{
			getsvrname = "relaymc_";
			req_get_svr_domainname.assign(req->domainname());
			//req_get_net_type = req->nettype(); // Ŀǰ����δ���Ӹ��ֶ�,relaymcǿ���ó�nettype=2����ȡvpn��ַ
			req_get_net_type = 2;
			sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo get_svr_type is relaymc_ !!!\n");
		} 
		else if (req_svr_type == DEVICE_SERVER::DEVICE_MC) // mc -> relaymc
		{
			getsvrname = "relaymc_";
			//req_get_net_type = 2;// mc��ȡ�Ǳ���,����Ҫ��ȡvpn��Ϣ
			sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo get_svr_type is relaymc_ !!!\n");
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetDeviceInfo req_svr_type and get_svr_type is not relaymc!!!\n");
			return false;
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetDeviceInfo get_svr_type error type !!!\n");
		return false;
	}

	if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
	{
		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
		char * p = m_pRedisConnList[e_RC_TT_MainThread]->getvalue("dev_deviceidandgroupid", req_deviceid_s);
		if (p == NULL)
		{
			//SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
			//pdinfo->set_deviceid(0);
			//pdinfo->set_svr_type(req_get_svr_type);
			//pdinfo->set_get_svr_type(req_get_svr_type);
			//pdinfo->set_ip(0);
			//pdinfo->set_port(0);
			//pdinfo->set_load(0);
			//pdinfo->set_load2(0);

			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetDeviceInfo getvalue groupid from redis db(3) Table:dev_deviceidandgroupid (deviceid)=(%s) return NULL error!!!\n", req_deviceid_s);
			return false;
		}
		else
		{
			// ������groupid
			SR_uint32 uiReqGroupid = 0;
			sscanf(p, "%u", &uiReqGroupid);

			std::set<SR_uint32> sGroupid;
			sGroupid.clear();
			sGroupid.insert(uiReqGroupid);

			//// relaymc -> mc
			//// mc->netmp��relayserver->netmp
			//// mc -> crs
			//// mc -> srs
			//if (/*(req_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC
			//	&& req_get_svr_type == DEVICE_SERVER::DEVICE_MC)
			//	//|| req_get_svr_type == DEVICE_SERVER::DEVICE_MC
			//	||*/ req_get_svr_type == DEVICE_SERVER::DEVICE_NETMP
			//	|| req_get_svr_type == DEVICE_SERVER::DEVICE_CRS
			//	|| req_get_svr_type == DEVICE_SERVER::DEVICE_SRS)
			//{
			//	SR_uint32 uiNodeNum = 0;
			//	GetSubGroupidInfo(uiReqGroupid, sGroupid, uiNodeNum);
			//}

			if ((req_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC
				&& req_get_svr_type == DEVICE_SERVER::DEVICE_MC)
				//|| req_get_svr_type == DEVICE_SERVER::DEVICE_MC
				||req_get_svr_type == DEVICE_SERVER::DEVICE_NETMP)
			{
				char local_mediagroup_mesh[32] = { 0 };
				sprintf(local_mediagroup_mesh, "local_mediagroup_mesh");

				RedisReplyArray vrra_db3_localmediagroup_info;
				vrra_db3_localmediagroup_info.clear();
				bool bhashHGetGroupidMesh_ok = false;
				unsigned int nGroupidMeshNum = 0;

				//m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
				bhashHGetGroupidMesh_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(local_mediagroup_mesh, vrra_db3_localmediagroup_info);
				nGroupidMeshNum = vrra_db3_localmediagroup_info.size();

				if (bhashHGetGroupidMesh_ok
					&& (nGroupidMeshNum > 0)
					&& (nGroupidMeshNum % 2 == 0))
				{
					//sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo hashHGetAll redis db(3) Table:%s (nGroupidMeshNum)=(%u) OK!!!\n", local_mediagroup_mesh, nGroupidMeshNum);

					for (int i = 0; i < nGroupidMeshNum;)
					{
						SR_uint32 srcgroupid = 0;
						std::string strsrc;
						strsrc.clear();
						SR_uint32 dstgroupid = 0;
						std::string strdst;
						strdst.clear();
						SR_int32 iweight = -1; // Ĭ�ϲ�ͨ

						//sr_printf(SR_LOGLEVEL_INFO, "==>>>redis db(3) Table:%s key[%d]=%s\n", local_mediagroup_mesh, i, vrra_db3_localmediagroup_info[i].str.c_str());

						size_t fi_idx = vrra_db3_localmediagroup_info[i].str.find(":");
						if (fi_idx != std::string::npos)
						{
							strsrc = vrra_db3_localmediagroup_info[i].str.substr(0, fi_idx);
							strdst = vrra_db3_localmediagroup_info[i].str.substr(fi_idx + 1, vrra_db3_localmediagroup_info[i].str.length());

							sscanf(strsrc.c_str(), "%u", &srcgroupid);// �ǵ�ȡ��ַ��,�������б���
							sscanf(strdst.c_str(), "%u", &dstgroupid);
							sscanf(vrra_db3_localmediagroup_info[i + 1].str.c_str(), "%d", &iweight);


							// ֻ��ȡ��uiReqGroupid�ܹ���ͨ��groupid
							if (srcgroupid != 0 && dstgroupid != 0 && iweight >= 0
								&& (uiReqGroupid == srcgroupid || uiReqGroupid == dstgroupid))
							{
								if (uiReqGroupid == srcgroupid)
								{
									sGroupid.insert(dstgroupid);
								} 
								else if (uiReqGroupid == dstgroupid)
								{
									sGroupid.insert(srcgroupid);
								}
							}
						}

						i += 2;
					}
				}
			}

			delete p; // ��ɾ���ᵼ���ڴ�й©

			bool bHaveDeviceinfo = false;

			for (std::set<SR_uint32>::iterator groupid_itor = sGroupid.begin();
				groupid_itor != sGroupid.end(); groupid_itor++)
			{
				SR_uint32 uiGroupid = 0;
				uiGroupid = (*groupid_itor);
				if (uiGroupid != 0)
				{
					std::string svrname_groupid;
					svrname_groupid.clear();
					svrname_groupid.assign(getsvrname);

					char cGroupid[32] = { 0 };
					sprintf(cGroupid, "%u", uiGroupid);

					svrname_groupid += cGroupid;

					sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo getvalue svrname_groupid=%s from redis db(3) Table:dev_deviceidandgroupid (deviceid)=(%s) OK!!!\n", svrname_groupid.c_str(), req_deviceid_s);

					// relaymc -> mc��suiruigw -> mc
					// mc -> srs
					if (req_get_svr_type == DEVICE_SERVER::DEVICE_MC
						|| req_get_svr_type == DEVICE_SERVER::DEVICE_SRS) 
					{
						// ��ǰ���mc��Ϣ[mcid,heartbeat]
						RedisReplyArray vrra_db1_deviceid_beat;
						vrra_db1_deviceid_beat.clear();
						bool bHGetAlldevidandbeat_ok = false;
						unsigned int nDevidandBeatNum = 0;
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);// ��ȡĳgroupid����������������mc/srs
						bHGetAlldevidandbeat_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(svrname_groupid.c_str(), vrra_db1_deviceid_beat);

						nDevidandBeatNum = vrra_db1_deviceid_beat.size();

						std::set<int> vOnlineDeviceidset;
						vOnlineDeviceidset.clear();

						if (bHGetAlldevidandbeat_ok
							&& (nDevidandBeatNum > 0)
							&& (nDevidandBeatNum % 2 == 0))
						{
							sr_printf(SR_LOGLEVEL_DEBUG, " ok req_get_svr_type -> mc/srs SRMsgId_ReqGetDeviceInfo gethashdbdata redis db(1) Table:%s (bHGetAlldevidandbeat_ok, nDevidandBeatNum)=(%d, %d) \n", svrname_groupid.c_str(), bHGetAlldevidandbeat_ok, nDevidandBeatNum);

							for (int j = 0; j < nDevidandBeatNum;)
							{
								char devid_value[128] = { 0 };
								char heartbeat_value[128] = { 0 };
								sprintf(devid_value, "%s", vrra_db1_deviceid_beat[j].str.c_str());
								sprintf(heartbeat_value, "%s", vrra_db1_deviceid_beat[j + 1].str.c_str());

								//char deviceid_netmpid[256] = { 0 };
								int iDevid = 0;
								iDevid = atoi(devid_value);//
								//sprintf(deviceid_netmpid, "deviceid_%d", iNetmpid);

								time_t dbtime = 0;
#ifdef WIN32
								sscanf(heartbeat_value, "%I64d", &dbtime);
#elif defined LINUX
								sscanf(heartbeat_value, "%lld", &dbtime);
#endif

								time_t timeNow;
								timeNow = time(NULL);

								if (timeNow > dbtime + TOLERANCE_TIME) // ��ǰ��ѯʱ�� > ���ݿ�����ʱ�� + ʧЧ������ʱ��
								{
									sr_printf(SR_LOGLEVEL_DEBUG, "***** WARRING ***svrname_groupid(%s)*** devid_value: %s gethashtime > inster db time + TOLERANCE_TIME *****\n", svrname_groupid.c_str(), devid_value);
								}
								else // ������netmp
								{
									vOnlineDeviceidset.insert(iDevid);
								}

								j += 2;
							}
							
							//bool bHaveDeviceinfo = false;

							for (std::set<int>::iterator devid_itor = vOnlineDeviceidset.begin();
								devid_itor != vOnlineDeviceidset.end(); devid_itor++)
							{
								int tempDeviceid = (*devid_itor);
								if (tempDeviceid == 0)
								{
									continue;
								}

								char cOnlineDeviceidbuf[128] = { 0 };
								sprintf(cOnlineDeviceidbuf, "%d", tempDeviceid);

								//// Ŀǰû�а��ո��ش�С�������򣬰����е������豸�������Է�
								//SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
								//pdinfo->set_deviceid(tempDeviceid);
								//pdinfo->set_svr_type(req->get_svr_type());
								//pdinfo->set_get_svr_type(req->get_svr_type());
								//pdinfo->set_groupid(uiGroupid);

								m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
								// ��ȡmc��srs������ip��port
								if (req_get_svr_type == DEVICE_SERVER::DEVICE_MC
									|| req_get_svr_type == DEVICE_SERVER::DEVICE_SRS)
								{
									char device_ipport_deviceid[128] = { 0 };
									sprintf(device_ipport_deviceid, "device_ipport_%s", cOnlineDeviceidbuf);

									char cInnerFieldName[128] = { 0 };
									char cOuterFieldName[128] = { 0 };
									char cVPNFieldName[128] = { 0 };

									//char inner_ipprort_value[65] = { 0 };
									//char outer_ipprort_value[65] = { 0 };
									//char vpn_ipprort_value[65] = { 0 };

									sprintf(cInnerFieldName, "%s", "inner");
									sprintf(cOuterFieldName, "%s", "outer");
									sprintf(cVPNFieldName, "%s", "vpn");

									REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
									REDISFILEDS vGetFileds;
									vGetFileds.clear();
									RedisReplyArray vRedisReplyArray;
									vRedisReplyArray.clear();

									// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
									REDISKEY strInnerFieldName = cInnerFieldName;
									REDISKEY strOuterFieldName = cOuterFieldName;
									REDISKEY strVPNFieldName = cVPNFieldName;
									vGetFileds.push_back(strInnerFieldName);
									vGetFileds.push_back(strOuterFieldName);
									vGetFileds.push_back(strVPNFieldName);
									
									bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strdeviceipportdeviceidkey, vGetFileds, vRedisReplyArray);
									if (bhashHMGet_ok
										&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
									{
										unsigned int rspip = 0;
										unsigned int rspport = 0;
										unsigned long long ullrspIPPort = 0;

										// ��ȡ��������ַ
										if (req_get_net_type == 0)
										{
											if (vRedisReplyArray[0].str.length() > 0
												&& vRedisReplyArray[1].str.length() == 0
												&& vRedisReplyArray[2].str.length() == 0)
											{
#ifdef WIN32
												sscanf(vRedisReplyArray[0].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
												sscanf(vRedisReplyArray[0].str.c_str(), "%lld", &ullrspIPPort);
#endif
											}
										}
										else if (req_get_net_type == 1)// ��ȡ��������ַ
										{
											if (vRedisReplyArray[0].str.length() > 0
												&& vRedisReplyArray[1].str.length() > 0
												&& vRedisReplyArray[2].str.length() == 0)
											{
#ifdef WIN32
												sscanf(vRedisReplyArray[1].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
												sscanf(vRedisReplyArray[1].str.c_str(), "%lld", &ullrspIPPort);
#endif
											}
										}
										else if (req_get_net_type == 2) // ��ȡ��vpn��ַ
										{
											if (vRedisReplyArray[0].str.length() > 0
												&& vRedisReplyArray[1].str.length() == 0
												&& vRedisReplyArray[2].str.length() > 0)
											{
#ifdef WIN32
												sscanf(vRedisReplyArray[2].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
												sscanf(vRedisReplyArray[2].str.c_str(), "%lld", &ullrspIPPort);
#endif
											}
										}

										rspip = (unsigned int)((unsigned long long)(ullrspIPPort) >> 32);
										rspport = ((unsigned int)((unsigned long long)(ullrspIPPort)& 0xffffffff));
										
										// ��ȡ�ĵ�ַΪ��Ч��ַ
										if (rspip == 0
											|| rspport == 0)
										{
											continue;
										}
										
										if (req_get_svr_type == DEVICE_SERVER::DEVICE_MC)
										{
											char svrname_load_s[128] = { 0 };
											sprintf(svrname_load_s, "%s_%d", svrname_groupid.c_str(), tempDeviceid);
											m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);

											char cTernumFieldName[128] = { 0 };
											char cCpuFieldName[128] = { 0 };
											char cSRcntFieldName[128] = { 0 };
											char cSTDcntFieldName[128] = { 0 };
											char cRECcntFieldName[128] = { 0 };
											char cConfcntFieldName[128] = { 0 };
											char cLivecntFieldName[128] = { 0 };

											sprintf(cTernumFieldName, "%s", "ternum");
											sprintf(cCpuFieldName, "%s", "cpu");
											sprintf(cSRcntFieldName, "%s", "srcnt");
											sprintf(cSTDcntFieldName, "%s", "stdcnt");
											sprintf(cRECcntFieldName, "%s", "reccnt");
											sprintf(cConfcntFieldName, "%s", "confcnt");
											sprintf(cLivecntFieldName, "%s", "livecnt");

											REDISKEY strsvrnamegroupiddeviceidkey = svrname_load_s;
											REDISFILEDS vGetFileds;
											vGetFileds.clear();
											RedisReplyArray vRedisReplyArray;
											vRedisReplyArray.clear();

											// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
											REDISKEY strTernumFieldName = cTernumFieldName;
											REDISKEY strCpuFieldName = cCpuFieldName;
											REDISKEY strSRcntFieldName = cSRcntFieldName;
											REDISKEY strSTDcntFieldName = cSTDcntFieldName;
											REDISKEY strRECcntFieldName = cRECcntFieldName;
											REDISKEY strConfcntFieldName = cConfcntFieldName;
											REDISKEY strLivecntFieldName = cLivecntFieldName;
											vGetFileds.push_back(strTernumFieldName);
											vGetFileds.push_back(strCpuFieldName);
											vGetFileds.push_back(strSRcntFieldName);
											vGetFileds.push_back(strSTDcntFieldName);
											vGetFileds.push_back(strRECcntFieldName);
											vGetFileds.push_back(strConfcntFieldName);
											vGetFileds.push_back(strLivecntFieldName);

											bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strsvrnamegroupiddeviceidkey, vGetFileds, vRedisReplyArray);
											if (bhashHMGet_ok
												&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
											{
												bHaveDeviceinfo = true;
												int iTernum = 0;
												int iCpu = 0;

												sscanf(vRedisReplyArray[0].str.c_str(), "%d", &iTernum);
												sscanf(vRedisReplyArray[1].str.c_str(), "%d", &iCpu);
												
												// Ŀǰû�а��ո��ش�С�������򣬰����е������豸�������Է�
												SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
												pdinfo->set_deviceid(tempDeviceid);
												pdinfo->set_svr_type(req->get_svr_type());
												pdinfo->set_get_svr_type(req->get_svr_type());
												pdinfo->set_groupid(uiGroupid);
												pdinfo->set_ip(rspip);
												pdinfo->set_port(rspport);

												pdinfo->set_load(iTernum); //set_load��������[netmp:�����д�����mp:���д�����netmp->mp����mc���ն���������gw���ն���������crs������ʹ����]
												pdinfo->set_load2(iCpu);//set_load2��������[netmp:�ն���������mp:cpu����;mc:cpu����;gw:cpu����;crs:cpu����]
											}
											else
											{
												sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(0)== Table:%s == get mc load error onlinedeviceid=%s !!!\n", svrname_load_s, cOnlineDeviceidbuf);
											}
										}
										else if (req_get_svr_type == DEVICE_SERVER::DEVICE_SRS)
										{
											char srs_isroot_groupid_s[128] = { 0 };
											sprintf(srs_isroot_groupid_s, "srs_isroot_%s", cGroupid);
											m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
											char *ptrRetisroot = NULL;
											ptrRetisroot = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue(srs_isroot_groupid_s, cOnlineDeviceidbuf); // ��ȡ��key��Ӧ��value
											if (ptrRetisroot != NULL)
											{
												bHaveDeviceinfo = true;
												unsigned int uiisroot = 0;
												sscanf(ptrRetisroot, "%u", &uiisroot);

												delete ptrRetisroot; // ��ɾ���ᵼ���ڴ�й©


												// Ŀǰû�а��ո��ش�С�������򣬰����е������豸�������Է�
												SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
												pdinfo->set_deviceid(tempDeviceid);
												pdinfo->set_svr_type(req->get_svr_type());
												pdinfo->set_get_svr_type(req->get_svr_type());
												pdinfo->set_groupid(uiGroupid);
												pdinfo->set_ip(rspip);
												pdinfo->set_port(rspport);

												pdinfo->set_load(uiisroot); // srs�Ƿ���ڵ�,0-����,1-��
												pdinfo->set_load2(0);// ����ע
											}
											else
											{
												sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s == get srs is root error onlinedeviceid=%s !!!\n", srs_isroot_groupid_s, cOnlineDeviceidbuf);
											}
										}
									}
									else // �޷���ȡ���豸��ip + port
									{
										sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s == get IPPort error onlinedeviceid=%s !!!\n", device_ipport_deviceid, cOnlineDeviceidbuf);
									}
								} 
								else
								{
									/*
									// netmp������
									char deviceipport_deviceid_key[128] = { 0 };
									sprintf(deviceipport_deviceid_key, "device_ipport_%d", tempDeviceid);
									RedisReplyArray vrra_db2_device_netidandipport;
									vrra_db2_device_netidandipport.clear();
									bool bHGetAllDevicenetidandipport_ok = false;
									unsigned int nDevicenetidandipportNum = 0;

									bHGetAllDevicenetidandipport_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(deviceipport_deviceid_key, vrra_db2_device_netidandipport);
									nDevicenetidandipportNum = vrra_db2_device_netidandipport.size();
									if (bHGetAllDevicenetidandipport_ok
										&& (nDevicenetidandipportNum > 0)
										&& (nDevicenetidandipportNum % 2 == 0))
									{
										m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);
										for (int k = 0; k < nDevicenetidandipportNum;)
										{
											char netid_value[128] = { 0 };
											sprintf(netid_value, "%s", vrra_db2_device_netidandipport[k].str.c_str());
											char svrname_load_s[128] = { 0 };
											sprintf(svrname_load_s, "%s_%d_%s", svrname_groupid.c_str(), tempDeviceid, netid_value);

											char cNetidFieldName[128] = { 0 };
											char cIpPortFieldName[128] = { 0 };
											char cNettypeFieldName[128] = { 0 };
											char cRXFieldName[128] = { 0 }; // ����
											char cTXFieldName[128] = { 0 }; // ����

											char net_id_value[128] = { 0 };
											char ip_port_value[128] = { 0 };
											char net_type_value[128] = { 0 };
											char rx_value[33] = { 0 };
											char tx_value[33] = { 0 };

											sprintf(cNetidFieldName, "%s", "netid");
											sprintf(cIpPortFieldName, "%s", "ipandport");
											sprintf(cNettypeFieldName, "%s", "nettype");
											sprintf(cRXFieldName, "%s", "rx");
											sprintf(cTXFieldName, "%s", "tx");

											//////////////////////////////////////////////////////////////////////////
											//REDISKEY strconfreportidkey = confreport_confid_reportid_key;
											REDISKEY strsvrnamegroupiddeviceidnetidkey = svrname_load_s;
											REDISFILEDS vGetFileds;
											vGetFileds.clear();
											RedisReplyArray vRedisReplyArray;
											vRedisReplyArray.clear();

											// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
											REDISKEY strNetidFieldName = cNetidFieldName;
											REDISKEY strIpPortFieldName = cIpPortFieldName;
											REDISKEY strNettypeFieldName = cNettypeFieldName;
											REDISKEY strRXFieldName = cRXFieldName;
											REDISKEY strTXFieldName = cTXFieldName;
											vGetFileds.push_back(strNetidFieldName);
											vGetFileds.push_back(strIpPortFieldName);
											vGetFileds.push_back(strNettypeFieldName);
											vGetFileds.push_back(strRXFieldName);
											vGetFileds.push_back(strTXFieldName);

											bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strsvrnamegroupiddeviceidnetidkey, vGetFileds, vRedisReplyArray);

											if (bhashHMGet_ok
												&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
											{
												bHaveDeviceinfo = true;

												sprintf(net_id_value, "%s", vRedisReplyArray[0].str.c_str()); // ��Ҫ��push_back����˳��һ��
												sprintf(ip_port_value, "%s", vRedisReplyArray[1].str.c_str()); // ��Ҫ��push_back����˳��һ��
												sprintf(net_type_value, "%s", vRedisReplyArray[2].str.c_str()); // ��Ҫ��push_back����˳��һ��
												sprintf(rx_value, "%s", vRedisReplyArray[3].str.c_str()); // ��Ҫ��push_back����˳��һ��
												sprintf(tx_value, "%s", vRedisReplyArray[4].str.c_str()); // ��Ҫ��push_back����˳��һ��

												int inetid = 0;
												long long ullIPPort = 0;
												int inettype = -1;
												int irx = 0;
												int itx = 0;

												sscanf(vRedisReplyArray[0].str.c_str(), "%d", &inetid);

#ifdef WIN32
												sscanf(vRedisReplyArray[1].str.c_str(), "%I64d", &ullIPPort);
#elif defined LINUX
												sscanf(vRedisReplyArray[1].str.c_str(), "%lld", &ullIPPort);
#endif

												int ip = (int)((long long)(ullIPPort) >> 32);
												int port = ((int)((long long)(ullIPPort)& 0xffffffff));

												sscanf(vRedisReplyArray[2].str.c_str(), "%d", &inettype);
												sscanf(vRedisReplyArray[3].str.c_str(), "%d", &irx);
												sscanf(vRedisReplyArray[4].str.c_str(), "%d", &itx);

												SRMsgs::RspGetDeviceInfo_NetworkInfo *pnetinfo = pdinfo->add_networklist();
												pnetinfo->set_netno(inetid);
												pnetinfo->set_ip(ip);
												pnetinfo->set_port(port);
												pnetinfo->set_nettype(inettype);
												pnetinfo->set_rx(irx);
												pnetinfo->set_tx(itx);
											}

											k += 2;
										}
									}
									*/
								} // ������
							}

							//if (bHaveDeviceinfo == false)// û�м�¼�������м�¼���ǽ�ʬ��¼
							//{
							//	SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
							//	pdinfo->set_deviceid(0);
							//	pdinfo->set_svr_type(req->get_svr_type());
							//	pdinfo->set_get_svr_type(req->get_svr_type());
							//	/*pdinfo->set_ip(0);
							//	pdinfo->set_port(0);
							//	pdinfo->set_load(0);
							//	pdinfo->set_load2(0);*/
							//	pdinfo->set_groupid(0);
							//	sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(1,2,6)==svrname_groupid(%s)== device_ipport ==hashHMGet--bHaveRecord == false error !!!\n", svrname_groupid.c_str());
							//}
						}
						else
						{
							sr_printf(SR_LOGLEVEL_ERROR, " mc -error-> netmp SRMsgId_ReqGetDeviceInfo gethashdbdata redis db(1) Table:%s (bHGetAlldevidandbeat_ok, nDevidandBeatNum)=(%d, %d) \n", svrname_groupid.c_str(), bHGetAlldevidandbeat_ok, nDevidandBeatNum);
							continue;
						}
					}
					else
					{
						// mc->netmp��relayserver->netmp
						// mc->mp��mc->suiruigw��mc->crs��mc->relayserver
						// relaymc->relaymc��mc->relaymc

						// relaymc->relaymc(һ��group�µĶ��domain)
						if (req_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC
							&& req_get_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC)
						{
							// ��ȡָ�����relaymc
							if (req_get_svr_domainname.length() > 0)
							{
								// ָ�����Ƿ��ǺϷ�����
								bool isLegalDomain = false;
								// ָ�����Ƿ��Ǳ�����
								bool isLocaldomain = false;

								// ��ȡ��������
								RedisReplyArray vrra_local_domain_level;
								vrra_local_domain_level.clear();
								unsigned int nLocalDomainAndLevel = 0;

								m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
								bool bHGetAllLocalDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_local_domain_level);

								nLocalDomainAndLevel = vrra_local_domain_level.size();

								if (bHGetAllLocalDomain_ok
									&& (nLocalDomainAndLevel > 0)
									&& (nLocalDomainAndLevel % 2 == 0))
								{
									for (int idx = 0; idx < nLocalDomainAndLevel;)
									{
										int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
										//vrra_local_domain_level[idx].str;// ����
										iLevel = atoi(vrra_local_domain_level[idx + 1].str.c_str());// level
										if (req_get_svr_domainname.compare(vrra_local_domain_level[idx].str) == 0)
										{
											isLegalDomain = true;
											if (iLevel == 0)
											{
												isLocaldomain = true;
											}

											break;
										}

										idx += 2;
									}
								}

								if (isLegalDomain)
								{
									// (ָ��)�Ϸ������Ǳ�������
									if (isLocaldomain)
									{
										// �鱾����local_domain_deviceinfo��svr_type==3��deviceid
										// ��������豸��Ϣ[deviceid,svrtype]
										RedisReplyArray vrra_deviceid_svrtype;
										vrra_deviceid_svrtype.clear();
										bool bHGetAllok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_deviceinfo", vrra_deviceid_svrtype);

										unsigned int nDeviceidandSvrtypeNum = vrra_deviceid_svrtype.size();

										if (bHGetAllok
											&& (nDeviceidandSvrtypeNum > 0)
											&& (nDeviceidandSvrtypeNum % 2 == 0))
										{
											for (int i = 0; i < nDeviceidandSvrtypeNum;)
											{
												char deviceid_value[128] = { 0 };
												char svrtype_value[128] = { 0 };
												SR_uint32 uiDeviceid = 0;
												SR_uint32 uiSvrtype = 0;
												sprintf(deviceid_value, "%s", vrra_deviceid_svrtype[i].str.c_str());
												sprintf(svrtype_value, "%s", vrra_deviceid_svrtype[i + 1].str.c_str());
												sscanf(vrra_deviceid_svrtype[i].str.c_str(), "%u", &uiDeviceid);
												sscanf(vrra_deviceid_svrtype[i + 1].str.c_str(), "%u", &uiSvrtype);

												// ������������relaymc
												if (uiSvrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
												{
													char *ptrhbtime = NULL;
													m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1); // ��ȡ���豸������Ϣ
													ptrhbtime = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue(svrname_groupid.c_str(), deviceid_value);
													if (ptrhbtime != NULL)
													{
														time_t hbdbtime = 0;
#ifdef WIN32
														sscanf(ptrhbtime, "%I64d", &hbdbtime);
														//printf("==SRMsgId_ReqGetDeviceInfo redis db(1) ==key---->> %s, hbdbtime = %I64d\n", deviceid_value, hbdbtime);
#elif defined LINUX
														sscanf(ptrhbtime, "%lld", &hbdbtime);
														//printf("==SRMsgId_ReqGetDeviceInfo redis db(1) ==key---->> %s, hbdbtime = %lld\n", deviceid_value, hbdbtime);
#endif

														delete ptrhbtime;// ��ɾ���ᵼ���ڴ�й©

														time_t timeNow;
														timeNow = time(NULL);

														if (timeNow > hbdbtime + TOLERANCE_TIME) // ��ǰ��ѯʱ�� > ���ݿ�����ʱ�� + ʧЧ������ʱ��
														{
															sr_printf(SR_LOGLEVEL_DEBUG, "***** WARRING ***svrname_groupid(%s)*** key: %d gethashtime > inster db time + TOLERANCE_TIME *****\n", svrname_groupid.c_str(), deviceid_value);
														}
														else // ����������relaymc
														{
															m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);

															char device_ipport_deviceid[128] = { 0 };
															sprintf(device_ipport_deviceid, "device_ipport_%s", deviceid_value);

															char cInnerFieldName[128] = { 0 };
															char cOuterFieldName[128] = { 0 };
															char cVPNFieldName[128] = { 0 };

															//char inner_ipprort_value[65] = { 0 };
															//char outer_ipprort_value[65] = { 0 };
															//char vpn_ipprort_value[65] = { 0 };

															sprintf(cInnerFieldName, "%s", "inner");
															sprintf(cOuterFieldName, "%s", "outer");
															sprintf(cVPNFieldName, "%s", "vpn");

															REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
															REDISFILEDS vGetFileds;
															vGetFileds.clear();
															RedisReplyArray vRedisReplyArray;
															vRedisReplyArray.clear();

															// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
															REDISKEY strInnerFieldName = cInnerFieldName;
															REDISKEY strOuterFieldName = cOuterFieldName;
															REDISKEY strVPNFieldName = cVPNFieldName;
															vGetFileds.push_back(strInnerFieldName);
															vGetFileds.push_back(strOuterFieldName);
															vGetFileds.push_back(strVPNFieldName);

															bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strdeviceipportdeviceidkey, vGetFileds, vRedisReplyArray);
															if (bhashHMGet_ok
																&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
															{
																unsigned int rspip = 0;
																unsigned int rspport = 0;
																unsigned long long ullrspIPPort = 0;

																// ��ȡ��������ַ
																if (req_get_net_type == 0)
																{
																	if (vRedisReplyArray[0].str.length() > 0
																		&& vRedisReplyArray[1].str.length() == 0
																		&& vRedisReplyArray[2].str.length() == 0)
																	{
#ifdef WIN32
																		sscanf(vRedisReplyArray[0].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																		sscanf(vRedisReplyArray[0].str.c_str(), "%lld", &ullrspIPPort);
#endif
																	}
																}
																else if (req_get_net_type == 1)// ��ȡ��������ַ
																{
																	if (vRedisReplyArray[0].str.length() > 0
																		&& vRedisReplyArray[1].str.length() > 0
																		&& vRedisReplyArray[2].str.length() == 0)
																	{
#ifdef WIN32
																		sscanf(vRedisReplyArray[1].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																		sscanf(vRedisReplyArray[1].str.c_str(), "%lld", &ullrspIPPort);
#endif
																	}
																}
																else if (req_get_net_type == 2) // ��ȡ��vpn��ַ
																{
																	if (vRedisReplyArray[0].str.length() > 0
																		&& vRedisReplyArray[1].str.length() == 0
																		&& vRedisReplyArray[2].str.length() > 0)
																	{
#ifdef WIN32
																		sscanf(vRedisReplyArray[2].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																		sscanf(vRedisReplyArray[2].str.c_str(), "%lld", &ullrspIPPort);
#endif
																	}
																}

																rspip = (unsigned int)((unsigned long long)(ullrspIPPort) >> 32);
																rspport = ((unsigned int)((unsigned long long)(ullrspIPPort)& 0xffffffff));

																// ��ȡ�ĵ�ַΪ��Ч��ַ
																if (rspip == 0
																	|| rspport == 0)
																{
																}
																else
																{
																	m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0); // ��ȡ���豸������Ϣ
																	char * pload = m_pRedisConnList[e_RC_TT_MainThread]->getvalue(svrname_groupid.c_str(), deviceid_value);
																	if (pload != NULL)
																	{
																		int iTernum = 0;
																		sscanf(pload, "%d", &iTernum);

																		delete pload; // ��ɾ���ᵼ���ڴ�й©
																		
																		SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
																		pdinfo->set_deviceid(uiDeviceid);
																		pdinfo->set_svr_type(uiSvrtype);
																		pdinfo->set_get_svr_type(uiSvrtype);
																		pdinfo->set_groupid(uiGroupid);
																		pdinfo->set_domainname(req_get_svr_domainname);
																		pdinfo->set_ip(rspip);
																		pdinfo->set_port(rspport);

																		pdinfo->set_load(iTernum); //set_load��������[netmp:���д�����relayserver:�ն���������mp:���д�����netmp->mp����mc���ն���������relaymc���ն���������gw���ն���������crs������ʹ����]
																		pdinfo->set_load2(0);//set_load2��������[netmp:�ն���������relayserver:���д�����mp:cpu����;mc:cpu����;gw:cpu����;crs:cpu����]

																		bHaveDeviceinfo = true;
																	}
																}
															}
															else // �޷���ȡ���豸��ip + port
															{
																sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s == get relaymc IPPort error onlinedeviceid=%s !!!\n", device_ipport_deviceid, deviceid_value);
															}
														}
													}													
												}

												i += 2;
											}
										}
										else // ��local_domain_deviceinfo��ʧ��
										{
											sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:local_domain_deviceinfo ==hashHGetAll !!!\n");
											continue;
										}
									}
									else
									{
										// (ָ��)�Ϸ��������Ǳ�������

										// ��ϵͳ��sys_domain_deviceinfo_##domainname��svr_type==3��deviceid
										// ϵͳ����豸��Ϣ[deviceid,svrtype]
										char sysdomaindevinfo_s[128] = { 0 };
										sprintf(sysdomaindevinfo_s, "sys_domain_deviceinfo_%s", req_get_svr_domainname.c_str());
										RedisReplyArray vrra_deviceid_svrtype;
										vrra_deviceid_svrtype.clear();
										bool bHGetAllok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(sysdomaindevinfo_s, vrra_deviceid_svrtype);

										unsigned int nDeviceidandSvrtypeNum = vrra_deviceid_svrtype.size();

										if (bHGetAllok
											&& (nDeviceidandSvrtypeNum > 0)
											&& (nDeviceidandSvrtypeNum % 2 == 0))
										{
											bool bGetdevinfoOK = false;
											for (int i = 0; i < nDeviceidandSvrtypeNum;)
											{
												char deviceid_value[128] = { 0 };
												char svrtype_value[128] = { 0 };
												SR_uint32 uiDeviceid = 0;
												SR_uint32 uiSvrtype = 0;
												sprintf(deviceid_value, "%s", vrra_deviceid_svrtype[i].str.c_str());
												sprintf(svrtype_value, "%s", vrra_deviceid_svrtype[i + 1].str.c_str());
												sscanf(vrra_deviceid_svrtype[i].str.c_str(), "%u", &uiDeviceid);
												sscanf(vrra_deviceid_svrtype[i + 1].str.c_str(), "%u", &uiSvrtype);
												
												// ������������relaymc
												if (uiSvrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
												{
													char sys_svrtype_domainname_s[128] = { 0 };
													sprintf(sys_svrtype_domainname_s, "sys_relaymc_%s", req_get_svr_domainname.c_str());
													
													char *ptrhbtime = NULL;
													m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1); // ��ȡϵͳ��relaymc��������Ϣ
													ptrhbtime = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue(sys_svrtype_domainname_s, deviceid_value);
													if (ptrhbtime != NULL)
													{
														time_t hbdbtime = 0;
#ifdef WIN32
														sscanf(ptrhbtime, "%I64d", &hbdbtime);
														//printf("==SRMsgId_ReqGetDeviceInfo redis db(1) ==key---->> %s, hbdbtime = %I64d\n", deviceid_value, hbdbtime);
#elif defined LINUX
														sscanf(ptrhbtime, "%lld", &hbdbtime);
														//printf("==SRMsgId_ReqGetDeviceInfo redis db(1) ==key---->> %s, hbdbtime = %lld\n", deviceid_value, hbdbtime);
#endif

														delete ptrhbtime;// ��ɾ���ᵼ���ڴ�й©

														time_t timeNow;
														timeNow = time(NULL);

														if (timeNow > hbdbtime + TOLERANCE_TIME) // ��ǰ��ѯʱ�� > ���ݿ�����ʱ�� + ʧЧ������ʱ��
														{
															sr_printf(SR_LOGLEVEL_DEBUG, "***** WARRING ***sys_svrtype_domainname_s(%s)*** key: %d gethashtime > inster db time + TOLERANCE_TIME *****\n", sys_svrtype_domainname_s, deviceid_value);
														}
														else // ����������relaymc
														{
															m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);

															char svrtype_ipport_domainname_deviceid_s[128] = { 0 };
															sprintf(svrtype_ipport_domainname_deviceid_s, "relaymc_ipport_%s_%s", req_get_svr_domainname.c_str(), deviceid_value);

															char cInnerFieldName[128] = { 0 };
															char cOuterFieldName[128] = { 0 };
															char cVPNFieldName[128] = { 0 };

															//char inner_ipprort_value[65] = { 0 };
															//char outer_ipprort_value[65] = { 0 };
															//char vpn_ipprort_value[65] = { 0 };

															sprintf(cInnerFieldName, "%s", "inner");
															sprintf(cOuterFieldName, "%s", "outer");
															sprintf(cVPNFieldName, "%s", "vpn");

															REDISKEY strsvrtypeipportdomainnamedeviceidkey = svrtype_ipport_domainname_deviceid_s;
															REDISFILEDS vGetFileds;
															vGetFileds.clear();
															RedisReplyArray vRedisReplyArray;
															vRedisReplyArray.clear();

															// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
															REDISKEY strInnerFieldName = cInnerFieldName;
															REDISKEY strOuterFieldName = cOuterFieldName;
															REDISKEY strVPNFieldName = cVPNFieldName;
															vGetFileds.push_back(strInnerFieldName);
															vGetFileds.push_back(strOuterFieldName);
															vGetFileds.push_back(strVPNFieldName);

															bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strsvrtypeipportdomainnamedeviceidkey, vGetFileds, vRedisReplyArray);
															if (bhashHMGet_ok
																&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
															{
																unsigned int rspip = 0;
																unsigned int rspport = 0;
																unsigned long long ullrspIPPort = 0;

																// ��ȡ��������ַ
																if (req_get_net_type == 0)
																{
																	if (vRedisReplyArray[0].str.length() > 0
																		&& vRedisReplyArray[1].str.length() == 0
																		&& vRedisReplyArray[2].str.length() == 0)
																	{
#ifdef WIN32
																		sscanf(vRedisReplyArray[0].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																		sscanf(vRedisReplyArray[0].str.c_str(), "%lld", &ullrspIPPort);
#endif
																	}
																}
																else if (req_get_net_type == 1)// ��ȡ��������ַ
																{
																	if (vRedisReplyArray[0].str.length() > 0
																		&& vRedisReplyArray[1].str.length() > 0
																		&& vRedisReplyArray[2].str.length() == 0)
																	{
#ifdef WIN32
																		sscanf(vRedisReplyArray[1].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																		sscanf(vRedisReplyArray[1].str.c_str(), "%lld", &ullrspIPPort);
#endif
																	}
																}
																else if (req_get_net_type == 2) // ��ȡ��vpn��ַ
																{
																	if (vRedisReplyArray[0].str.length() > 0
																		&& vRedisReplyArray[1].str.length() == 0
																		&& vRedisReplyArray[2].str.length() > 0)
																	{
#ifdef WIN32
																		sscanf(vRedisReplyArray[2].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																		sscanf(vRedisReplyArray[2].str.c_str(), "%lld", &ullrspIPPort);
#endif
																	}
																}

																rspip = (unsigned int)((unsigned long long)(ullrspIPPort) >> 32);
																rspport = ((unsigned int)((unsigned long long)(ullrspIPPort)& 0xffffffff));

																// ��ȡ�ĵ�ַΪ��Ч��ַ
																if (rspip == 0
																	|| rspport == 0)
																{
																}
																else
																{
																	m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0); // ��ȡ���豸������Ϣ
																	char * pload = m_pRedisConnList[e_RC_TT_MainThread]->getvalue(sys_svrtype_domainname_s, deviceid_value);
																	if (pload != NULL)
																	{
																		int iTernum = 0;
																		sscanf(pload, "%d", &iTernum);

																		delete pload; // ��ɾ���ᵼ���ڴ�й©

																		bGetdevinfoOK = true;
																		SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
																		pdinfo->set_deviceid(uiDeviceid);
																		pdinfo->set_svr_type(uiSvrtype);
																		pdinfo->set_get_svr_type(uiSvrtype);
																		pdinfo->set_groupid(uiGroupid);
																		pdinfo->set_domainname(req_get_svr_domainname);
																		pdinfo->set_ip(rspip);
																		pdinfo->set_port(rspport);

																		pdinfo->set_load(iTernum); //set_load��������[netmp:���д�����relayserver:�ն���������mp:���д�����netmp->mp����mc���ն���������relaymc���ն���������gw���ն���������crs������ʹ����]
																		pdinfo->set_load2(0);//set_load2��������[netmp:�ն���������relayserver:���д�����mp:cpu����;mc:cpu����;gw:cpu����;crs:cpu����]

																		bHaveDeviceinfo = true;
																	}
																}
															}
															else // �޷���ȡ���豸��ip + port
															{
																sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s == get relaymc IPPort error onlinedeviceid=%s !!!\n", svrtype_ipport_domainname_deviceid_s, deviceid_value);
															}
														}
													}
												}

												i += 2;
											}

											if (!bGetdevinfoOK)
											{
												sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s == not have relaymc info !!!\n", sysdomaindevinfo_s);
												continue;
											}
										}
										else // ��sys_domain_deviceinfo_##domainname��ʧ��
										{
											sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s ==hashHGetAll !!!\n", sysdomaindevinfo_s);
											continue;
										}
									}
								}
								else
								{
									sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetDeviceInfo req_get_svr_domainname=%s not find in redis db(3) local_domain_conf table!!! \n", req_get_svr_domainname.c_str());
									continue;
								}
							} 
							else
							{
								// ��ȡ�������relaymc,Ŀǰֻ��ֱ�ӹ������¼���,�缶��Ŀǰ����֧��sys_domain_info����δά��

								RedisReplyArray vrra_local_domain_level;
								vrra_local_domain_level.clear();
								unsigned int nLocalDomainAndLevel = 0;

								m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
								bool bHGetAllLocalDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_local_domain_level);

								nLocalDomainAndLevel = vrra_local_domain_level.size();

								if (bHGetAllLocalDomain_ok
									&& (nLocalDomainAndLevel > 0)
									&& (nLocalDomainAndLevel % 2 == 0))
								{
									bool bGetdevinfoOK = false;

									for (int idx = 0; idx < nLocalDomainAndLevel;)
									{
										int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
										std::string strdomainname;
										strdomainname.clear();
										strdomainname.assign(vrra_local_domain_level[idx].str);// ����
										iLevel = atoi(vrra_local_domain_level[idx + 1].str.c_str());// level

										// 1����ȡ�������relaymc
										if (iLevel == 0
											&& strdomainname.length() > 0)
										{
											// �鱾����local_domain_deviceinfo��svr_type==3��deviceid
											// ��������豸��Ϣ[deviceid,svrtype]
											RedisReplyArray vrra_deviceid_svrtype;
											vrra_deviceid_svrtype.clear();
											m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
											bool bHGetAllok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_deviceinfo", vrra_deviceid_svrtype);

											unsigned int nDeviceidandSvrtypeNum = vrra_deviceid_svrtype.size();

											if (bHGetAllok
												&& (nDeviceidandSvrtypeNum > 0)
												&& (nDeviceidandSvrtypeNum % 2 == 0))
											{
												for (int i = 0; i < nDeviceidandSvrtypeNum;)
												{
													char deviceid_value[128] = { 0 };
													char svrtype_value[128] = { 0 };
													SR_uint32 uiDeviceid = 0;
													SR_uint32 uiSvrtype = 0;
													sprintf(deviceid_value, "%s", vrra_deviceid_svrtype[i].str.c_str());
													sprintf(svrtype_value, "%s", vrra_deviceid_svrtype[i + 1].str.c_str());
													sscanf(vrra_deviceid_svrtype[i].str.c_str(), "%u", &uiDeviceid);
													sscanf(vrra_deviceid_svrtype[i + 1].str.c_str(), "%u", &uiSvrtype);

													// ������������relaymc
													if (uiSvrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
													{
														char *ptrhbtime = NULL;
														m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1); // ��ȡ���豸������Ϣ
														ptrhbtime = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue(svrname_groupid.c_str(), deviceid_value);
														if (ptrhbtime != NULL)
														{
															time_t hbdbtime = 0;
#ifdef WIN32
															sscanf(ptrhbtime, "%I64d", &hbdbtime);
															//printf("==SRMsgId_ReqGetDeviceInfo redis db(1) ==key---->> %s, hbdbtime = %I64d\n", deviceid_value, hbdbtime);
#elif defined LINUX
															sscanf(ptrhbtime, "%lld", &hbdbtime);
															//printf("==SRMsgId_ReqGetDeviceInfo redis db(1) ==key---->> %s, hbdbtime = %lld\n", deviceid_value, hbdbtime);
#endif

															delete ptrhbtime;// ��ɾ���ᵼ���ڴ�й©

															time_t timeNow;
															timeNow = time(NULL);

															if (timeNow > hbdbtime + TOLERANCE_TIME) // ��ǰ��ѯʱ�� > ���ݿ�����ʱ�� + ʧЧ������ʱ��
															{
																sr_printf(SR_LOGLEVEL_DEBUG, "***** WARRING ***svrname_groupid(%s)*** key: %d gethashtime > inster db time + TOLERANCE_TIME *****\n", svrname_groupid.c_str(), deviceid_value);
															}
															else // ����������relaymc
															{
																m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);

																char device_ipport_deviceid[128] = { 0 };
																sprintf(device_ipport_deviceid, "device_ipport_%s", deviceid_value);

																char cInnerFieldName[128] = { 0 };
																char cOuterFieldName[128] = { 0 };
																char cVPNFieldName[128] = { 0 };

																//char inner_ipprort_value[65] = { 0 };
																//char outer_ipprort_value[65] = { 0 };
																//char vpn_ipprort_value[65] = { 0 };

																sprintf(cInnerFieldName, "%s", "inner");
																sprintf(cOuterFieldName, "%s", "outer");
																sprintf(cVPNFieldName, "%s", "vpn");

																REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
																REDISFILEDS vGetFileds;
																vGetFileds.clear();
																RedisReplyArray vRedisReplyArray;
																vRedisReplyArray.clear();

																// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
																REDISKEY strInnerFieldName = cInnerFieldName;
																REDISKEY strOuterFieldName = cOuterFieldName;
																REDISKEY strVPNFieldName = cVPNFieldName;
																vGetFileds.push_back(strInnerFieldName);
																vGetFileds.push_back(strOuterFieldName);
																vGetFileds.push_back(strVPNFieldName);

																bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strdeviceipportdeviceidkey, vGetFileds, vRedisReplyArray);
																if (bhashHMGet_ok
																	&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
																{
																	unsigned int rspip = 0;
																	unsigned int rspport = 0;
																	unsigned long long ullrspIPPort = 0;

																	// ��ȡ��������ַ
																	if (req_get_net_type == 0)
																	{
																		if (vRedisReplyArray[0].str.length() > 0
																			&& vRedisReplyArray[1].str.length() == 0
																			&& vRedisReplyArray[2].str.length() == 0)
																		{
#ifdef WIN32
																			sscanf(vRedisReplyArray[0].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																			sscanf(vRedisReplyArray[0].str.c_str(), "%lld", &ullrspIPPort);
#endif
																		}
																	}
																	else if (req_get_net_type == 1)// ��ȡ��������ַ
																	{
																		if (vRedisReplyArray[0].str.length() > 0
																			&& vRedisReplyArray[1].str.length() > 0
																			&& vRedisReplyArray[2].str.length() == 0)
																		{
#ifdef WIN32
																			sscanf(vRedisReplyArray[1].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																			sscanf(vRedisReplyArray[1].str.c_str(), "%lld", &ullrspIPPort);
#endif
																		}
																	}
																	else if (req_get_net_type == 2) // ��ȡ��vpn��ַ
																	{
																		if (vRedisReplyArray[0].str.length() > 0
																			&& vRedisReplyArray[1].str.length() == 0
																			&& vRedisReplyArray[2].str.length() > 0)
																		{
#ifdef WIN32
																			sscanf(vRedisReplyArray[2].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																			sscanf(vRedisReplyArray[2].str.c_str(), "%lld", &ullrspIPPort);
#endif
																		}
																	}

																	rspip = (unsigned int)((unsigned long long)(ullrspIPPort) >> 32);
																	rspport = ((unsigned int)((unsigned long long)(ullrspIPPort)& 0xffffffff));

																	// ��ȡ�ĵ�ַΪ��Ч��ַ
																	if (rspip == 0
																		|| rspport == 0)
																	{
																	}
																	else
																	{
																		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0); // ��ȡ���豸������Ϣ
																		char * pload = m_pRedisConnList[e_RC_TT_MainThread]->getvalue(svrname_groupid.c_str(), deviceid_value);
																		if (pload != NULL)
																		{
																			int iTernum = 0;
																			sscanf(pload, "%d", &iTernum);

																			delete pload; // ��ɾ���ᵼ���ڴ�й©

																			bGetdevinfoOK = true;
																			SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
																			pdinfo->set_deviceid(uiDeviceid);
																			pdinfo->set_svr_type(uiSvrtype);
																			pdinfo->set_get_svr_type(uiSvrtype);
																			pdinfo->set_groupid(uiGroupid);
																			pdinfo->set_domainname(strdomainname);
																			pdinfo->set_ip(rspip);
																			pdinfo->set_port(rspport);

																			pdinfo->set_load(iTernum); //set_load��������[netmp:���д�����relayserver:�ն���������mp:���д�����netmp->mp����mc���ն���������relaymc���ն���������gw���ն���������crs������ʹ����]
																			pdinfo->set_load2(0);//set_load2��������[netmp:�ն���������relayserver:���д�����mp:cpu����;mc:cpu����;gw:cpu����;crs:cpu����]
																			
																			bHaveDeviceinfo = true;
																		}
																	}
																}
																else // �޷���ȡ���豸��ip + port
																{
																	sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s == get relaymc IPPort error onlinedeviceid=%s !!!\n", device_ipport_deviceid, deviceid_value);
																}
															}
														}
													}

													i += 2;
												}
											}
											else // ��local_domain_deviceinfo��ʧ��
											{
												sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:local_domain_deviceinfo ==hashHGetAll error!!!\n");
											}
										}
										else if (strdomainname.length() > 0)
										{
											// 2����ȡϵͳ���relaymc
											// ��ϵͳ��sys_domain_deviceinfo_##domainname��svr_type==3��deviceid
											// ϵͳ����豸��Ϣ[deviceid,svrtype]
											char sysdomaindevinfo_s[128] = { 0 };
											sprintf(sysdomaindevinfo_s, "sys_domain_deviceinfo_%s", strdomainname.c_str());
											RedisReplyArray vrra_deviceid_svrtype;
											vrra_deviceid_svrtype.clear();

											m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
											bool bHGetAllok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(sysdomaindevinfo_s, vrra_deviceid_svrtype);

											unsigned int nDeviceidandSvrtypeNum = vrra_deviceid_svrtype.size();

											if (bHGetAllok
												&& (nDeviceidandSvrtypeNum > 0)
												&& (nDeviceidandSvrtypeNum % 2 == 0))
											{
												for (int i = 0; i < nDeviceidandSvrtypeNum;)
												{
													char deviceid_value[128] = { 0 };
													char svrtype_value[128] = { 0 };
													SR_uint32 uiDeviceid = 0;
													SR_uint32 uiSvrtype = 0;
													sprintf(deviceid_value, "%s", vrra_deviceid_svrtype[i].str.c_str());
													sprintf(svrtype_value, "%s", vrra_deviceid_svrtype[i + 1].str.c_str());
													sscanf(vrra_deviceid_svrtype[i].str.c_str(), "%u", &uiDeviceid);
													sscanf(vrra_deviceid_svrtype[i + 1].str.c_str(), "%u", &uiSvrtype);

													// ������������relaymc
													if (uiSvrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
													{
														char sys_svrtype_domainname_s[128] = { 0 };
														sprintf(sys_svrtype_domainname_s, "sys_relaymc_%s", strdomainname.c_str());

														char *ptrhbtime = NULL;
														m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1); // ��ȡϵͳ��relaymc��������Ϣ
														ptrhbtime = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue(sys_svrtype_domainname_s, deviceid_value);
														if (ptrhbtime != NULL)
														{
															time_t hbdbtime = 0;
#ifdef WIN32
															sscanf(ptrhbtime, "%I64d", &hbdbtime);
															//printf("==SRMsgId_ReqGetDeviceInfo redis db(1) ==key---->> %s, hbdbtime = %I64d\n", deviceid_value, hbdbtime);
#elif defined LINUX
															sscanf(ptrhbtime, "%lld", &hbdbtime);
															//printf("==SRMsgId_ReqGetDeviceInfo redis db(1) ==key---->> %s, hbdbtime = %lld\n", deviceid_value, hbdbtime);
#endif

															delete ptrhbtime;// ��ɾ���ᵼ���ڴ�й©

															time_t timeNow;
															timeNow = time(NULL);

															if (timeNow > hbdbtime + TOLERANCE_TIME) // ��ǰ��ѯʱ�� > ���ݿ�����ʱ�� + ʧЧ������ʱ��
															{
																sr_printf(SR_LOGLEVEL_DEBUG, "***** WARRING ***sys_svrtype_domainname_s(%s)*** key: %d gethashtime > inster db time + TOLERANCE_TIME *****\n", sys_svrtype_domainname_s, deviceid_value);
															}
															else // ����������relaymc
															{
																m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);

																char svrtype_ipport_domainname_deviceid_s[128] = { 0 };
																sprintf(svrtype_ipport_domainname_deviceid_s, "relaymc_ipport_%s_%s", strdomainname.c_str(), deviceid_value);

																char cInnerFieldName[128] = { 0 };
																char cOuterFieldName[128] = { 0 };
																char cVPNFieldName[128] = { 0 };

																//char inner_ipprort_value[65] = { 0 };
																//char outer_ipprort_value[65] = { 0 };
																//char vpn_ipprort_value[65] = { 0 };

																sprintf(cInnerFieldName, "%s", "inner");
																sprintf(cOuterFieldName, "%s", "outer");
																sprintf(cVPNFieldName, "%s", "vpn");

																REDISKEY strsvrtypeipportdomainnamedeviceidkey = svrtype_ipport_domainname_deviceid_s;
																REDISFILEDS vGetFileds;
																vGetFileds.clear();
																RedisReplyArray vRedisReplyArray;
																vRedisReplyArray.clear();

																// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
																REDISKEY strInnerFieldName = cInnerFieldName;
																REDISKEY strOuterFieldName = cOuterFieldName;
																REDISKEY strVPNFieldName = cVPNFieldName;
																vGetFileds.push_back(strInnerFieldName);
																vGetFileds.push_back(strOuterFieldName);
																vGetFileds.push_back(strVPNFieldName);

																bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strsvrtypeipportdomainnamedeviceidkey, vGetFileds, vRedisReplyArray);
																if (bhashHMGet_ok
																	&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
																{
																	unsigned int rspip = 0;
																	unsigned int rspport = 0;
																	unsigned long long ullrspIPPort = 0;

																	// ��ȡ��������ַ
																	if (req_get_net_type == 0)
																	{
																		if (vRedisReplyArray[0].str.length() > 0
																			&& vRedisReplyArray[1].str.length() == 0
																			&& vRedisReplyArray[2].str.length() == 0)
																		{
#ifdef WIN32
																			sscanf(vRedisReplyArray[0].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																			sscanf(vRedisReplyArray[0].str.c_str(), "%lld", &ullrspIPPort);
#endif
																		}
																	}
																	else if (req_get_net_type == 1)// ��ȡ��������ַ
																	{
																		if (vRedisReplyArray[0].str.length() > 0
																			&& vRedisReplyArray[1].str.length() > 0
																			&& vRedisReplyArray[2].str.length() == 0)
																		{
#ifdef WIN32
																			sscanf(vRedisReplyArray[1].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																			sscanf(vRedisReplyArray[1].str.c_str(), "%lld", &ullrspIPPort);
#endif
																		}
																	}
																	else if (req_get_net_type == 2) // ��ȡ��vpn��ַ
																	{
																		if (vRedisReplyArray[0].str.length() > 0
																			&& vRedisReplyArray[1].str.length() == 0
																			&& vRedisReplyArray[2].str.length() > 0)
																		{
#ifdef WIN32
																			sscanf(vRedisReplyArray[2].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																			sscanf(vRedisReplyArray[2].str.c_str(), "%lld", &ullrspIPPort);
#endif
																		}
																	}

																	rspip = (unsigned int)((unsigned long long)(ullrspIPPort) >> 32);
																	rspport = ((unsigned int)((unsigned long long)(ullrspIPPort)& 0xffffffff));

																	// ��ȡ�ĵ�ַΪ��Ч��ַ
																	if (rspip == 0
																		|| rspport == 0)
																	{
																	}
																	else
																	{
																		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0); // ��ȡ���豸������Ϣ
																		char * pload = m_pRedisConnList[e_RC_TT_MainThread]->getvalue(sys_svrtype_domainname_s, deviceid_value);
																		if (pload != NULL)
																		{
																			int iTernum = 0;
																			sscanf(pload, "%d", &iTernum);

																			delete pload; // ��ɾ���ᵼ���ڴ�й©

																			bGetdevinfoOK = true;
																			SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
																			pdinfo->set_deviceid(uiDeviceid);
																			pdinfo->set_svr_type(uiSvrtype);
																			pdinfo->set_get_svr_type(uiSvrtype);
																			pdinfo->set_groupid(uiGroupid);
																			pdinfo->set_domainname(strdomainname);
																			pdinfo->set_ip(rspip);
																			pdinfo->set_port(rspport);

																			pdinfo->set_load(iTernum); //set_load��������[netmp:���д�����relayserver:�ն���������mp:���д�����netmp->mp����mc���ն���������relaymc���ն���������gw���ն���������crs������ʹ����]
																			pdinfo->set_load2(0);//set_load2��������[netmp:�ն���������relayserver:���д�����mp:cpu����;mc:cpu����;gw:cpu����;crs:cpu����]

																			bHaveDeviceinfo = true;
																		}
																	}																	
																}
																else // �޷���ȡ���豸��ip + port
																{
																	sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s == get relaymc IPPort error onlinedeviceid=%s !!!\n", svrtype_ipport_domainname_deviceid_s, deviceid_value);
																}
															}
														}
													}

													i += 2;
												}
											}
											else // ��sys_domain_deviceinfo_##domainname��ʧ��
											{
												sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s ==hashHGetAll error !!!\n", sysdomaindevinfo_s);
											}
										}

										idx += 2;
									}

									if (!bGetdevinfoOK)
									{
										sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:local_domain_conf == related domain not have relaymc info !!!\n");

										continue;
									}
								}
								else
								{
									sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:local_domain_conf == not have domain info !!!\n");

									continue;
								}
							}
						} 
						else
						{
							// mc->netmp��relayserver->netmp
							// mc->mp��mc->suiruigw��mc->crs��mc->relayserver
							// mc->relaymc

							m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0); // ȡ���豸�ĸ�����Ϣ
							int num = m_pRedisConnList[e_RC_TT_MainThread]->getdbnum(svrname_groupid.c_str());

							//int size = num > devnums ? devnums : num; // ���ʵ�ʲ�ѯ����ļ�¼���������ѯ��¼����Ĭ��2�����������
							int size = num; // ��ʵ�ʲ�ѯ����ļ�¼����������

							sr_printf(SR_LOGLEVEL_DEBUG, "SRMsgId_ReqGetDeviceInfo getdbnum redis db(0) by svrname_groupid :%s,result(num=%d, size=%d)\n", svrname_groupid.c_str(), num, size);

							if (size > 0)
							{
								bool haseroor = false;
								load = new DevLoadPtr[size];
								if (load != NULL)
								{
									for (int j = 0; j < size; j++)
									{
										load[j] = NULL;
										CNetMpLoad* obj = new CNetMpLoad();
										if (obj != NULL)
										{
											load[j] = obj;
										}
										else
										{
											haseroor = true;
										}
									}
								}
								else
								{
									haseroor = true;
								}

								if (haseroor) // �ڴ��������ֱ�ӷ���
								{
									if (load != NULL)
									{
										for (int k = 0; k < size; k++)
										{
											if (load[k] != NULL)
											{
												delete (load[k]);
												load[k] = NULL;
											}
										}

										delete[]load;
									}

									sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetDeviceInfo before getdbdata redis db(0) malloc memory for load error!!! \n");

									continue;
								}


								int start = 0;
								//int stop = num > devnums ? (-1)*num + 1 : -1; // �� num��20�� > devnums��10����stop = -19��Ҳ����start = 0, stop = 1 // �˴��϶������� modify by chesonghua
								//int stop = num > size ? size-1 : -1;
								int stop = -1;

								//m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);
								int res = m_pRedisConnList[e_RC_TT_MainThread]->getdbdata(svrname_groupid.c_str(), start, stop, load,size); // ��ȡ����load�Ѿ��Ǵ�С���󣬵����ܰ�����ʬnetmp��load

								sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetDeviceInfo getdbdata redis db(0) svrname_groupid(%s), Record num:%d, start=%d, stop=%d\n", svrname_groupid.c_str(), res, start, stop);

								if (res == 0)
								{
									for (int k = 0; k < size; k++)
									{
										if (load[k] != NULL)
										{
											delete (load[k]);
											load[k] = NULL;
										}
									}
									delete[]load;
									load = NULL;

									continue;
								}
								else
								{
									//bool bhasRecord = false;
									int iSendSize = 0;

									for (int jj = 0; jj < res; jj++)
									{
										char *ptr = NULL;
										char cDeviceidbuf[128] = { 0 };
										sprintf(cDeviceidbuf, "%d", load[jj]->deviceid);

										m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1); // ��ȡ���豸������Ϣ
										ptr = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue(svrname_groupid.c_str(), cDeviceidbuf); // ��ȡ��key��Ӧ��value

										//printf("=== %s ====[%d] m_pRedis->gethashvalue =====-----------ptr----------->>> %p\n", __FILE__, __LINE__, ptr);

										if (ptr != NULL)
										{
											time_t dbtime = 0;
#ifdef WIN32
											sscanf(ptr, "%I64d", &dbtime);
											//printf("==SRMsgId_ReqGetDeviceInfo redis db(7) ==key---->> %d, dbtime = %I64d\n", load[jj]->deviceid, dbtime);
#elif defined LINUX
											sscanf(ptr, "%lld", &dbtime);
											//printf("==SRMsgId_ReqGetDeviceInfo redis db(7) ==key---->> %d, dbtime = %lld\n", load[jj]->deviceid, dbtime);
#endif
											//printf("=== %s ====[%d] m_pRedis->gethashvalue =====------ptr != NULL-----delete ptr---->>> %p\n", __FILE__, __LINE__, ptr);

											delete ptr; // ��ɾ���ᵼ���ڴ�й©

											time_t timeNow;
											timeNow = time(NULL);

											if (timeNow > dbtime + TOLERANCE_TIME) // ��ǰ��ѯʱ�� > ���ݿ�����ʱ�� + ʧЧ������ʱ��
											{
												sr_printf(SR_LOGLEVEL_DEBUG, "***** WARRING ***svrname_groupid(%s)*** key: %d gethashtime > inster db time + TOLERANCE_TIME *****\n", svrname_groupid.c_str(), load[jj]->deviceid);
											}
											else // ������netmp
											{
												//SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
												//pdinfo->set_deviceid(load[jj]->deviceid);
												//pdinfo->set_svr_type(req->get_svr_type());
												//pdinfo->set_get_svr_type(req->get_svr_type());
												//pdinfo->set_groupid(uiGroupid);

												//printf("SRMsgId_ReqGetDeviceInfo after getdbnum getdbdata redis db(1): load[%d]->deviceid=%d,  load[%d]->load=%d\n", jj, load[jj]->deviceid, jj, load[jj]->load);

												m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);

												char device_ipport_deviceid[128] = { 0 };
												sprintf(device_ipport_deviceid, "device_ipport_%s", cDeviceidbuf);

												char cInnerFieldName[128] = { 0 };
												char cOuterFieldName[128] = { 0 };
												char cVPNFieldName[128] = { 0 };
												char cMapInternetIPsFieldName[128] = { 0 };

												//char inner_ipprort_value[65] = { 0 };
												//char outer_ipprort_value[65] = { 0 };
												//char vpn_ipprort_value[65] = { 0 };
												//char map_internetips_value[512] = { 0 };// ��ʽ�����ʮ����ip1,���ʮ����ip2,....���ʮ����ipn

												sprintf(cInnerFieldName, "%s", "inner");
												sprintf(cOuterFieldName, "%s", "outer");
												sprintf(cVPNFieldName, "%s", "vpn");
												sprintf(cMapInternetIPsFieldName, "%s", "mapinternetips");

												REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
												REDISFILEDS vGetFileds;
												vGetFileds.clear();
												RedisReplyArray vRedisReplyArray;
												vRedisReplyArray.clear();

												// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
												REDISKEY strInnerFieldName = cInnerFieldName;
												REDISKEY strOuterFieldName = cOuterFieldName;
												REDISKEY strVPNFieldName = cVPNFieldName;
												REDISKEY strMapInternetIPsFieldName = cMapInternetIPsFieldName;
												vGetFileds.push_back(strInnerFieldName);
												vGetFileds.push_back(strOuterFieldName);
												vGetFileds.push_back(strVPNFieldName);
												vGetFileds.push_back(strMapInternetIPsFieldName);

												bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strdeviceipportdeviceidkey, vGetFileds, vRedisReplyArray);
												if (bhashHMGet_ok
													&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
												{
													unsigned int rspip = 0;
													unsigned int rspport = 0;
													unsigned long long ullrspIPPort = 0;

													// ����relayserver�ĵ�ַͨ��restful�����ն�ֱ��ʹ��,�ն˲෢��������relayserverʱ,�����Ϣ��Я��stunserver��ַ(�������д�)
													// ��ȡ��������ַ
													if (req_get_net_type == 0)
													{
														if (req_get_svr_type == DEVICE_SERVER::DEVICE_NETMP
															|| req_get_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC)
														{
															if (vRedisReplyArray[0].str.length() > 0)
															{
#ifdef WIN32
																sscanf(vRedisReplyArray[0].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																sscanf(vRedisReplyArray[0].str.c_str(), "%lld", &ullrspIPPort);
#endif
															}
														} 
														else
														{
															if (vRedisReplyArray[0].str.length() > 0
																&& vRedisReplyArray[1].str.length() == 0
																&& vRedisReplyArray[2].str.length() == 0)
															{
#ifdef WIN32
																sscanf(vRedisReplyArray[0].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
																sscanf(vRedisReplyArray[0].str.c_str(), "%lld", &ullrspIPPort);
#endif
															}
														}
													}
													else if (req_get_net_type == 1)// ��ȡ��������ַ
													{
														if (vRedisReplyArray[0].str.length() > 0
															&& vRedisReplyArray[1].str.length() > 0
															&& vRedisReplyArray[2].str.length() == 0)
														{
#ifdef WIN32
															sscanf(vRedisReplyArray[1].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
															sscanf(vRedisReplyArray[1].str.c_str(), "%lld", &ullrspIPPort);
#endif
														}
													}
													else if (req_get_net_type == 2) // ��ȡ��vpn��ַ
													{
														if (vRedisReplyArray[0].str.length() > 0
															&& vRedisReplyArray[1].str.length() == 0
															&& vRedisReplyArray[2].str.length() > 0)
														{
#ifdef WIN32
															sscanf(vRedisReplyArray[2].str.c_str(), "%I64d", &ullrspIPPort);
#elif defined LINUX
															sscanf(vRedisReplyArray[2].str.c_str(), "%lld", &ullrspIPPort);
#endif
														}
													}

													rspip = (unsigned int)((unsigned long long)(ullrspIPPort) >> 32);
													rspport = ((unsigned int)((unsigned long long)(ullrspIPPort)& 0xffffffff));

													// ��ȡ�ĵ�ַΪ��Ч��ַ
													if (rspip == 0
														|| rspport == 0)
													{
													}
													else
													{
														SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
														pdinfo->set_deviceid(load[jj]->deviceid);
														pdinfo->set_svr_type(req->get_svr_type());
														pdinfo->set_get_svr_type(req->get_svr_type());
														pdinfo->set_groupid(uiGroupid);
														pdinfo->set_ip(rspip);
														pdinfo->set_port(rspport);

														//pdinfo->set_load(load[jj]->load);

														// load[jj]->loadֵ��Ÿ�ʽload1+load2�����£�
														//uidevicetype == 2���netmp��load1 load2,load1Ϊ�ն���,load2Ϊ���д���
														//uidevicetype == 4���relayserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���
														//uidevicetype == 5���mp��load1 load2,load1Ϊcpuʹ����,load2Ϊ���д�����netmp->mp��
														//uidevicetype == 6���gw��load1 load2,load1Ϊcpuʹ����,load2Ϊ�ն���
														//uidevicetype == 9���crs��load1 load2,load1Ϊcpuʹ����,load2Ϊ����ʹ���ʣ���16λ��������,��16λ��ʣ������,��λ��GB��
														//uidevicetype == 11���stunserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���
														int iLoad2 = (int)((unsigned long long)(load[jj]->load) >> 32); // ��32λ netmp:termNum��mc/mp/crs:cpu
														int iLoad = ((int)((unsigned long long)(load[jj]->load) & 0xffffffff)); // ��32λ
														pdinfo->set_load(iLoad); //set_load��������[netmp:���д�����relayserver:�ն���������stunserver:�ն���������mp:���д�����netmp->mp����mc���ն���������gw���ն���������crs������ʹ���ʣ���16λ��������,��16λ��ʣ������,��λ��GB��]
														pdinfo->set_load2(iLoad2);//set_load2��������[netmp:�ն���������relayserver:���д�����stunserver:���д�����mp:cpu����;mc:cpu����;gw:cpu����;crs:cpu����]

														// ��ȡ����netmp��relaymc����ӳ�������ip
														if (vRedisReplyArray[3].str.length() > 0
															&& (req_get_svr_type == DEVICE_SERVER::DEVICE_NETMP
															|| req_get_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC))
														{
															char *pip;
															pip = strtok((char*)(vRedisReplyArray[3].str.c_str()), ",");
															while (pip)
															{
																SRMsgs::RspGetDeviceInfo_MapIPInfo* paddmapip = pdinfo->add_mapinternetips();
																paddmapip->set_mapip(pip);

																pip = strtok(NULL, ",");
															}
														}


														if (req_get_svr_type == DEVICE_SERVER::DEVICE_NETMP)  // mc -> netmp��relayserver -> netmp
														{
#ifdef WIN32
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo netmp redis db(3)== Table:%s, ullrspIPPort = %I64d; redis db(1)==ternum = %d, tx = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#elif defined LINUX
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo netmp redis db(3)== Table:%s, ullrspIPPort = %lld; redis db(1)== ternum = %d, tx = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#endif
														}
														else if (req_get_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC) // mc -> relaymc
														{
#ifdef WIN32
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo relaymc redis db(3)== Table:%s, ullrspIPPort = %I64d; redis db(1)==tx = %d, ternum = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#elif defined LINUX
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo relaymc redis db(3)== Table:%s, ullrspIPPort = %lld; redis db(1)==tx = %d, ternum = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#endif
														}
														else if (req_get_svr_type == DEVICE_SERVER::DEVICE_RELAYSERVER) // mc -> relayserver
														{
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo relayserver outeripport:[len=%d, str=%s]\n", vRedisReplyArray[1].str.length(), vRedisReplyArray[1].str.c_str());
#ifdef WIN32
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo relayserver redis db(3)== Table:%s, ullrspIPPort = %I64d; redis db(1)==tx = %d, ternum = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#elif defined LINUX
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo relayserver redis db(3)== Table:%s, ullrspIPPort = %lld; redis db(1)==tx = %d, ternum = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#endif
														}
														else if (req_get_svr_type == DEVICE_SERVER::DEVICE_MP) // mc -> mp
														{
#ifdef WIN32
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s, ullrspIPPort = %I64d; redis db(1)==cpu = %d, rx = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#elif defined LINUX
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s, ullrspIPPort = %lld; redis db(1)== cpu = %d, rx = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#endif
														}
														else if (req_get_svr_type == DEVICE_SERVER::DEVICE_GW) // mc -> gw
														{
#ifdef WIN32
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s, ullrspIPPort = %I64d; redis db(1)==cpu = %d, ternum = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#elif defined LINUX
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s, ullrspIPPort = %lld; redis db(1)== cpu = %d, ternum = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#endif
														}
														else if (req_get_svr_type == DEVICE_SERVER::DEVICE_CRS) // mc -> crs
														{
#ifdef WIN32
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s, ullrspIPPort = %I64d; redis db(1)==cpu = %d, disk = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#elif defined LINUX
															sr_printf(SR_LOGLEVEL_INFO, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s, ullrspIPPort = %lld; redis db(1)== cpu = %d, disk = %d\n", device_ipport_deviceid, ullrspIPPort, iLoad2, iLoad);
#endif
														}

														bHaveDeviceinfo = true;
														//bhasRecord = true;
														iSendSize++;
														//if (iSendSize == devnums) // ���͵ļ�¼���������ѯ�ļ�¼����Ĭ��2��
														//{
														//	break;
														//}
													} 
												}
												else // �޷���ȡ���豸��ip + port
												{
													sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)== Table:%s == get IPPort error onlinedeviceid=%s !!!\n", device_ipport_deviceid, cDeviceidbuf);
												}
											}
										}
										else // �޷���ȡ���豸������ʱ��
										{
											sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(1)== svrname_groupid(%s) ==gethashvalue-- key->> %d, time_t error !!!\n", svrname_groupid.c_str(), load[jj]->deviceid);
										}

									} // for (int jj = 0; jj < size; jj++)

									//if (bhasRecord == false) // û�м�¼�������м�¼���ǽ�ʬ��¼
									//{
									//	SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
									//	pdinfo->set_deviceid(0);
									//	pdinfo->set_svr_type(req->get_svr_type());
									//	pdinfo->set_get_svr_type(req->get_svr_type());
									//	pdinfo->set_ip(0);
									//	pdinfo->set_port(0);
									//	pdinfo->set_load(0);
									//	pdinfo->set_load2(0);
									//	pdinfo->set_groupid(0);
									//	//pdinfo->set_groupid(uiGroupid);

									//	sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(2)==svrname_groupid(%s)== deviceid_ipport ==gethashvalue--bhasRecord == false error !!!\n", svrname_groupid.c_str());
									//}
								}

								for (int k = 0; k < size; k++)
								{
									if (load[k] != NULL)
									{
										delete (load[k]);
										load[k] = NULL;
									}
								}
								delete[]load;
								load = NULL;

							} //if (size > 0)
							else
							{
								sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(0)==svrname_groupid(%s)=getdbnum-- size <= 0 error !!!\n", svrname_groupid.c_str());

								continue;
							}
						}
					}
				}
				else
				{
					sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo redis db(3)==dev_deviceidandgroupid and submediagroup_groupid== have uiGroupid=0 value  error !!!\n");
				}
			}//for (std::set<SR_uint32>::iterator groupid_itor = sGroupid.begin();groupid_itor != sGroupid.end(); groupid_itor++)

			if (bHaveDeviceinfo == false)
			{
				sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo req_deviceid_s=%s uiReqGroupid=%u getsvrname:%s error !!!\n", req_deviceid_s, uiReqGroupid, getsvrname.c_str());

				return false;
			}
		}
	}//if (m_pRedis)
	else
	{
		//SRMsgs::RspGetDeviceInfo_DeviceInfo *pdinfo = rsp->add_deviceinfolist();
		//pdinfo->set_deviceid(0);
		//pdinfo->set_svr_type(req->get_svr_type());
		//pdinfo->set_get_svr_type(req->get_svr_type());
		//pdinfo->set_ip(0);
		//pdinfo->set_port(0);
		//pdinfo->set_load(0);
		//pdinfo->set_load2(0);
		//pdinfo->set_groupid(0);

		sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetDeviceInfo getsvrname:%s m_pRedis == NULL error !!!\n", getsvrname.c_str());

		return false;
	}

	return true;
}


bool CDevMgr::GetSysDeviceInfo(const SRMsgs::ReqGetSysDeviceInfo* req, SRMsgs::RspGetSysDeviceInfo* rsp)
{
	char req_deviceid_s[128] = { 0 };
	sprintf(req_deviceid_s, "%d", req->deviceid());
	unsigned int req_svr_type = 0;
	unsigned int req_get_svr_type = 0;
	req_svr_type = req->svr_type();
	req_get_svr_type = req->get_svr_type();

	sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_ReqGetSysDeviceInfo OK: param(deviceid,token,svr_type,get_svr_type)=(%d, %s, %d, %d)\n", req->deviceid(), req->token().c_str(), req_svr_type, req_get_svr_type);

	if (req->deviceid() == 0)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSysDeviceInfo req->deviceid == 0 error !!!\n");
		return false;
	}

	// Ŀǰֻ�������������豸������������
	if (req_svr_type != DEVICE_SERVER::DEVICE_DEV)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSysDeviceInfo req_svr_type is not devmgr !!!\n");
		return false;
	}

	// Ŀǰֻ��ȡ��(��)��������ķ���
	std::string svrname_groupid;
	svrname_groupid.clear();

	if (req_get_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC) // devmgr -> relaymc
	{
		svrname_groupid = "relaymc_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetSysDeviceInfo get_svr_type is relaymc_ !!!\n");
	}
	else if (req_get_svr_type == DEVICE_SERVER::DEVICE_RELAYSERVER) // devmgr -> relayserver
	{
		svrname_groupid = "relayserver_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetSysDeviceInfo get_svr_type is relayserver_ !!!\n");
	}
	else if (req_get_svr_type == DEVICE_SERVER::DEVICE_STUNSERVER) // devmgr -> stunserver
	{
		svrname_groupid = "stunserver_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_ReqGetSysDeviceInfo get_svr_type is stunserver_ !!!\n");
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSysDeviceInfo get_svr_type error type !!!\n");
		return false;
	}

	if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
	{
		m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
		char * pgroupid = m_pRedisConnList[e_RC_TT_MainThread]->getvalue("dev_deviceidandgroupid", req_deviceid_s);
		if (pgroupid == NULL)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSysDeviceInfo getvalue groupid from redis db(3) Table:dev_deviceidandgroupid (deviceid)=(%s) return NULL error!!!\n", req_deviceid_s);
			return false;
		}
		else
		{
			//// ������groupid
			//SR_uint32 uiReqGroupid = 0;
			//sscanf(p, "%u", &uiReqGroupid);
			svrname_groupid += pgroupid;

			delete pgroupid; // ��ɾ���ᵼ���ڴ�й©

			// ��ȡ��������
			RedisReplyArray vrra_local_domain_level;
			vrra_local_domain_level.clear();
			unsigned int nLocalDomainAndLevel = 0;
			std::string strLocalDomainname;
			strLocalDomainname.clear();

			bool bHGetAllLocalDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_local_domain_level);

			nLocalDomainAndLevel = vrra_local_domain_level.size();

			if (bHGetAllLocalDomain_ok
				&& (nLocalDomainAndLevel > 0)
				&& (nLocalDomainAndLevel % 2 == 0))
			{
				for (int idx = 0; idx < nLocalDomainAndLevel;)
				{
					int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
					//vrra_local_domain_level[idx].str;// ����
					iLevel = atoi(vrra_local_domain_level[idx + 1].str.c_str());// level
					if (iLevel == 0)
					{
						strLocalDomainname.assign(vrra_local_domain_level[idx].str.c_str());
						break;
					}

					idx += 2;
				}
			}
			
			if (strLocalDomainname.length() > 0)
			{
				// ͬ��ķ�����Ϣ[svrid,heartbeat]
				RedisReplyArray vrra_db1_svrid_heartbeat;
				vrra_db1_svrid_heartbeat.clear();
				m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);
				bool bHGetAllNetmpBeat_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(svrname_groupid.c_str(), vrra_db1_svrid_heartbeat);

				unsigned int nSvridandBeatNum = vrra_db1_svrid_heartbeat.size();

				std::set<unsigned int> vOnlineDeviceidset;
				vOnlineDeviceidset.clear();

				if (bHGetAllNetmpBeat_ok
					&& (nSvridandBeatNum > 0)
					&& (nSvridandBeatNum % 2 == 0))
				{
					for (int i = 0; i < nSvridandBeatNum;)
					{
						SR_uint32 uiDeviceid = 0;
						time_t hbdbtime = 0;
						sscanf(vrra_db1_svrid_heartbeat[i].str.c_str(), "%u", &uiDeviceid);

#ifdef WIN32
						sscanf(vrra_db1_svrid_heartbeat[i + 1].str.c_str(), "%I64d", &hbdbtime);
#elif defined LINUX
						sscanf(vrra_db1_svrid_heartbeat[i + 1].str.c_str(), "%lld", &hbdbtime);
#endif

						time_t timeNow;
						timeNow = time(NULL);

						if (uiDeviceid == 0
							|| timeNow > hbdbtime + TOLERANCE_TIME) // ��ǰ��ѯʱ�� > ���ݿ�����ʱ�� + ʧЧ������ʱ��
						{
							sr_printf(SR_LOGLEVEL_DEBUG, "***** WARRING ***svrname_groupid(%s)*** key: %d gethashtime > inster db time + TOLERANCE_TIME *****\n", svrname_groupid.c_str(), uiDeviceid);
						}
						else // �����������豸
						{
							vOnlineDeviceidset.insert(uiDeviceid);
						}

						i += 2;
					}

					bool bHaveDeviceinfo = false;

					if (vOnlineDeviceidset.size() > 0)
					{
						SRMsgs::RspGetSysDeviceInfo_DomainInfo* padddomaininfo = rsp->add_domaininfos();
						padddomaininfo->set_domainname(strLocalDomainname);
						padddomaininfo->set_level(0);

						for (std::set<unsigned int>::iterator devid_itor = vOnlineDeviceidset.begin();
							devid_itor != vOnlineDeviceidset.end(); devid_itor++)
						{
							unsigned int tempDeviceid = (*devid_itor);
							if (tempDeviceid == 0)
							{
								continue;
							}

							bHaveDeviceinfo = true;

							char cOnlineDeviceidbuf[128] = { 0 };
							sprintf(cOnlineDeviceidbuf, "%u", tempDeviceid);
							SRMsgs::RspGetSysDeviceInfo_DeviceInfo* padddevinfo = padddomaininfo->add_devices();
							padddevinfo->set_deviceid(tempDeviceid);

							m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
							char device_ipport_deviceid[128] = { 0 };
							sprintf(device_ipport_deviceid, "device_ipport_%u", tempDeviceid);

							char cInnerFieldName[128] = { 0 };
							char cOuterFieldName[128] = { 0 };
							char cVPNFieldName[128] = { 0 };
							
							sprintf(cInnerFieldName, "%s", "inner");
							sprintf(cOuterFieldName, "%s", "outer");
							sprintf(cVPNFieldName, "%s", "vpn");

							REDISKEY strdeviceipportdeviceidkey = device_ipport_deviceid;
							REDISFILEDS vGetFileds;
							vGetFileds.clear();
							RedisReplyArray vRedisReplyArray;
							vRedisReplyArray.clear();

							// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
							REDISKEY strInnerFieldName = cInnerFieldName;
							REDISKEY strOuterFieldName = cOuterFieldName;
							REDISKEY strVPNFieldName = cVPNFieldName;
							vGetFileds.push_back(strInnerFieldName);
							vGetFileds.push_back(strOuterFieldName);
							vGetFileds.push_back(strVPNFieldName);

							bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strdeviceipportdeviceidkey, vGetFileds, vRedisReplyArray);
							if (bhashHMGet_ok
								&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
							{
								SRMsgs::RspGetSysDeviceInfo_IPPORTInfo* pSvripportinfo = new SRMsgs::RspGetSysDeviceInfo_IPPORTInfo();
								pSvripportinfo->set_inner(vRedisReplyArray[0].str);
								pSvripportinfo->set_outer(vRedisReplyArray[1].str);
								pSvripportinfo->set_vpn(vRedisReplyArray[2].str);
								padddevinfo->set_allocated_ipport(pSvripportinfo);
							}
							else
							{
								// ��ȡ���豸��ip��port��Ϣʧ��
							}

							m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0); // ��ȡ���豸������Ϣ
							char * pload = m_pRedisConnList[e_RC_TT_MainThread]->getvalue(svrname_groupid.c_str(), cOnlineDeviceidbuf);
							if (pload != NULL)
							{
								unsigned int ternum = 0;
								unsigned int tx = 0;
								unsigned int rx = 0;
								unsigned int cpu = 0;
								unsigned long long ullload = 0;
								
								//uidevicetype == 3��ľ����ն���
								//uidevicetype == 4���relayserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���
								//uidevicetype == 11���stunserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���
								SRMsgs::RspGetSysDeviceInfo_LoadInfo* pSvrloadinfo = new SRMsgs::RspGetSysDeviceInfo_LoadInfo();
								if (req_get_svr_type == DEVICE_SERVER::DEVICE_RELAY_MC)
								{
									sscanf(pload, "%u", &ternum);
								}
								else if (req_get_svr_type == DEVICE_SERVER::DEVICE_RELAYSERVER
									|| req_get_svr_type == DEVICE_SERVER::DEVICE_STUNSERVER)
								{
#ifdef WIN32
									sscanf(pload, "%I64d", &ullload);
#elif defined LINUX
									sscanf(pload, "%lld", &ullload);
#endif
									tx = (unsigned int)(ullload >> 32); // ��32λ
									ternum = ((unsigned int)(ullload & 0xffffffff)); // ��32λ
								}
								pSvrloadinfo->set_ternum(ternum);//�����ն���
								pSvrloadinfo->set_tx(tx);//����(����)����
								pSvrloadinfo->set_rx(rx);//����(����)����
								pSvrloadinfo->set_cpu(cpu);
								padddevinfo->set_allocated_loadinfos(pSvrloadinfo);
							}
							else
							{
								// ��ȡ���豸�ĸ���ʧ��
							}
						}

						if (bHaveDeviceinfo)
						{
						}
						else
						{
							// û������Ҫ����豸
							sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetSysDeviceInfo svrname_groupid:%s get deviceinfo error!!!\n", svrname_groupid.c_str());
							return false;
						}
					}
					else
					{
						// û�����ߵ��豸
						sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetSysDeviceInfo svrname_groupid:%s not have onlinedevice!!!\n", svrname_groupid.c_str());
						return false;
					}
				}
				else
				{
					// ��ȡ�������豸��������Ϣʧ��
					sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetSysDeviceInfo hashHGetAll svrname_groupid:%s  error !!!\n", svrname_groupid.c_str());
					return false;
				}
			} 
			else
			{
				// strLocalDomainnameΪ��
				sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetSysDeviceInfo strLocalDomainname is NULL error !!!\n");
				return false;
			}
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "==SRMsgId_ReqGetSysDeviceInfo svrname_groupid:%s m_pRedis == NULL error !!!\n", svrname_groupid.c_str());
		return false;
	}

	return true;
}

bool CDevMgr::GetSystemCurLoad(const SRMsgs::ReqGetSystemCurLoad* req, SRMsgs::RspGetSystemCurLoad* rsp)
{
	sr_printf(SR_LOGLEVEL_NORMAL, "SRMsgId_ReqGetSystemCurLoad OK: param(deviceid,token)=(%d, %s)\n", req->deviceid(), req->token().c_str());

	if (req->deviceid() == 0)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSystemCurLoad req->deviceid == 0 error !!!\n");
		return false;
	}

	ValueScores** valuescores = NULL;
	typedef ValueScores* ptrValueScores;

	m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3); // ȡ���豸�ĸ�����Ϣ
	int num = m_pRedisConnList[e_RC_TT_MainThread]->getdbnum("dev_deviceidandgroupid");
	
	int size = num; // ��ʵ�ʲ�ѯ����ļ�¼����������

	if (size > 0)
	{
		bool haseroor = false;
		valuescores = new ptrValueScores[size];
		if (valuescores != NULL)
		{
			for (int j = 0; j < size; j++)
			{
				valuescores[j] = NULL;
				ValueScores* obj = new ValueScores();
				if (obj != NULL)
				{
					valuescores[j] = obj;
				}
				else
				{
					haseroor = true;
				}
			}
		}
		else
		{
			haseroor = true;
		}

		if (haseroor) // �ڴ��������ֱ�ӷ���
		{
			if (valuescores != NULL)
			{
				for (int k = 0; k < size; k++)
				{
					if (valuescores[k] != NULL)
					{
						delete (valuescores[k]);
						valuescores[k] = NULL;
					}
				}

				delete[]valuescores;
				valuescores = NULL;
			}

			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSystemCurLoad before getzsetdata redis db(3) malloc memory for load error!!! \n");

			return false;
		}

		int start = 0;
		//int stop = num > devnums ? (-1)*num + 1 : -1; // �� num��20�� > devnums��10����stop = -19��Ҳ����start = 0, stop = 1 // �˴��϶������� modify by chesonghua
		//int stop = num > size ? size-1 : -1;
		int stop = -1;
		int res = m_pRedisConnList[e_RC_TT_MainThread]->getzsetdata("dev_deviceidandgroupid", start, stop, valuescores,size);

		if (res == 0)
		{
			if (valuescores != NULL)
			{
				for (int k = 0; k < size; k++)
				{
					if (valuescores[k] != NULL)
					{
						delete (valuescores[k]);
						valuescores[k] = NULL;
					}
				}

				delete[]valuescores;
			}
			valuescores = NULL;
			return false;
		}
		else
		{
			std::set<SR_uint32> groupidset;
			groupidset.clear();

			for (int jj = 0; jj < res; jj++)
			{
				SR_uint32 uiGroupid = 0;
				sscanf(valuescores[jj]->score, "%u", &uiGroupid);
				if (uiGroupid != 0)
				{
					groupidset.insert(uiGroupid);
				}
			}

			if (groupidset.size() > 0)
			{
				SR_uint32 uicurconfcnt = 0;
				SR_uint32 uicurreccnt = 0;
				SR_uint32 uicurlivecnt = 0;

				// ��������groupid������mc
				std::string mc_groupid;
				mc_groupid.clear();
				mc_groupid = "mc_";

				for (std::set<SR_uint32>::iterator gid_itor = groupidset.begin();
					gid_itor != groupidset.end(); gid_itor++)
				{
					SR_uint32 uiGroupid = 0;
					uiGroupid = (*gid_itor);
					if (uiGroupid != 0)
					{
						char cGroupid[32] = { 0 };
						sprintf(cGroupid, "%u", uiGroupid);

						mc_groupid += cGroupid;

						// ��ǰ���mc��Ϣ[mcid,heartbeat]
						RedisReplyArray vrra_db1_deviceid_beat;
						vrra_db1_deviceid_beat.clear();
						bool bHGetAlldevidandbeat_ok = false;
						unsigned int nDevidandBeatNum = 0;
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);// ��ȡĳgroupid����������������mc/srs
						bHGetAlldevidandbeat_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll(mc_groupid.c_str(), vrra_db1_deviceid_beat);

						nDevidandBeatNum = vrra_db1_deviceid_beat.size();

						std::set<int> vOnlineDeviceidset;
						vOnlineDeviceidset.clear();

						if (bHGetAlldevidandbeat_ok
							&& (nDevidandBeatNum > 0)
							&& (nDevidandBeatNum % 2 == 0))
						{
							sr_printf(SR_LOGLEVEL_DEBUG, " ok SRMsgId_ReqGetSystemCurLoad hashHGetAll redis db(1) Table:%s (bHGetAlldevidandbeat_ok, nDevidandBeatNum)=(%d, %d) \n", mc_groupid.c_str(), bHGetAlldevidandbeat_ok, nDevidandBeatNum);

							for (int j = 0; j < nDevidandBeatNum;)
							{
								char devid_value[128] = { 0 };
								char heartbeat_value[128] = { 0 };
								sprintf(devid_value, "%s", vrra_db1_deviceid_beat[j].str.c_str());
								sprintf(heartbeat_value, "%s", vrra_db1_deviceid_beat[j + 1].str.c_str());

								//char deviceid_netmpid[256] = { 0 };
								int iDevid = 0;
								iDevid = atoi(devid_value);//
								//sprintf(deviceid_netmpid, "deviceid_%d", iNetmpid);

								time_t dbtime = 0;
#ifdef WIN32
								sscanf(heartbeat_value, "%I64d", &dbtime);
#elif defined LINUX
								sscanf(heartbeat_value, "%lld", &dbtime);
#endif

								if (iDevid != 0)
								{
									time_t timeNow;
									timeNow = time(NULL);

									if (timeNow > dbtime + TOLERANCE_TIME) // ��ǰ��ѯʱ�� > ���ݿ�����ʱ�� + ʧЧ������ʱ��
									{
										sr_printf(SR_LOGLEVEL_DEBUG, "***** WARRING ***mc_groupid(%s)*** devid_value: %s gethashtime > inster db time + TOLERANCE_TIME *****\n", mc_groupid.c_str(), devid_value);
									}
									else // ������mc
									{
										vOnlineDeviceidset.insert(iDevid);
									}
								}
								
								j += 2;
							}
						}

						// ��ȡuiGroupid����������mc�ĸ�����Ϣ
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);

						for (std::set<int>::iterator devid_itor = vOnlineDeviceidset.begin();
							devid_itor != vOnlineDeviceidset.end(); devid_itor++)
						{
							int tempDeviceid = (*devid_itor);
							if (tempDeviceid == 0)
							{
								continue;
							}

							char svrname_load_s[128] = { 0 };
							sprintf(svrname_load_s, "%s_%d", mc_groupid.c_str(), tempDeviceid);
							//m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);

							char cTernumFieldName[128] = { 0 };
							char cCpuFieldName[128] = { 0 };
							char cSRcntFieldName[128] = { 0 };
							char cSTDcntFieldName[128] = { 0 };
							char cRECcntFieldName[128] = { 0 };
							char cConfcntFieldName[128] = { 0 };
							char cLivecntFieldName[128] = { 0 };

							sprintf(cTernumFieldName, "%s", "ternum");
							sprintf(cCpuFieldName, "%s", "cpu");
							sprintf(cSRcntFieldName, "%s", "srcnt");
							sprintf(cSTDcntFieldName, "%s", "stdcnt");
							sprintf(cRECcntFieldName, "%s", "reccnt");
							sprintf(cConfcntFieldName, "%s", "confcnt");
							sprintf(cLivecntFieldName, "%s", "livecnt");

							REDISKEY strsvrnamegroupiddeviceidkey = svrname_load_s;
							REDISFILEDS vGetFileds;
							vGetFileds.clear();
							RedisReplyArray vRedisReplyArray;
							vRedisReplyArray.clear();

							// ע�⣺�˴���push_backӰ��hashHMGet���vRedisReplyArray��˳��
							REDISKEY strTernumFieldName = cTernumFieldName;
							REDISKEY strCpuFieldName = cCpuFieldName;
							REDISKEY strSRcntFieldName = cSRcntFieldName;
							REDISKEY strSTDcntFieldName = cSTDcntFieldName;
							REDISKEY strRECcntFieldName = cRECcntFieldName;
							REDISKEY strConfcntFieldName = cConfcntFieldName;
							REDISKEY strLivecntFieldName = cLivecntFieldName;
							vGetFileds.push_back(strTernumFieldName);
							vGetFileds.push_back(strCpuFieldName);
							vGetFileds.push_back(strSRcntFieldName);
							vGetFileds.push_back(strSTDcntFieldName);
							vGetFileds.push_back(strRECcntFieldName);
							vGetFileds.push_back(strConfcntFieldName);
							vGetFileds.push_back(strLivecntFieldName);

							bool bhashHMGet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMGet(strsvrnamegroupiddeviceidkey, vGetFileds, vRedisReplyArray);
							if (bhashHMGet_ok
								&& vRedisReplyArray.size() == vGetFileds.size()) // ��push_back�ĸ���һ��
							{
								SR_uint32 uiReccnt = 0;
								SR_uint32 uiConfcnt = 0;
								SR_uint32 uiLivecnt = 0;

								sscanf(vRedisReplyArray[4].str.c_str(), "%u", &uiReccnt);
								sscanf(vRedisReplyArray[5].str.c_str(), "%u", &uiConfcnt);
								sscanf(vRedisReplyArray[6].str.c_str(), "%u", &uiLivecnt);

								uicurreccnt += uiReccnt;
								uicurconfcnt += uiConfcnt;
								uicurlivecnt += uiLivecnt;
							}
						}

					}// uiGroupid != 0
				}// for (std::set<SR_uint32>::iterator gid_itor = groupidset.begin();gid_itor != groupidset.end(); gid_itor++)

				rsp->set_curconfcnt(uicurconfcnt);
				rsp->set_curlivecnt(uicurlivecnt);
				rsp->set_curreccnt(uicurreccnt);

			}// if (groupidset.size() > 0)
		}

		if (valuescores != NULL)
		{
			for (int k = 0; k < size; k++)
			{
				if (valuescores[k] != NULL)
				{
					delete (valuescores[k]);
					valuescores[k] = NULL;
				}
			}

			delete[]valuescores;
		}
		valuescores = NULL;

	} // size > 0
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_ReqGetSystemCurLoad getdbnum dev_deviceidandgroupid num == 0 error !!!\n");
		return false;
	}

	return true;
}

void CDevMgr::UpdatePeerHeartbeatToDB(const SRMsgs::IndsertodevHeart* ind, time_t timeNow)
{
	char load2_s[128] = { 0 };
	sprintf(load2_s, "%d", ind->load2());
	char deviceid_s[128] = { 0 };
	sprintf(deviceid_s, "%d", ind->deviceid());

	unsigned int uidevicetype = 0;
	uidevicetype = ind->svr_type();

	std::string svrname_groupid;
	svrname_groupid.clear();

	if (uidevicetype == DEVICE_SERVER::DEVICE_MC)
	{
		svrname_groupid = "mc_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndsertodevHeart Recv mc Heart Ind\n");
		if (!ind->has_loadinfo())
		{
			sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndsertodevHeart mc Heart Ind not have loadinfo !!!\n");
			return;
		}
	}
	else if (uidevicetype == DEVICE_SERVER::DEVICE_NETMP)
	{
		svrname_groupid = "netmp_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndsertodevHeart Recv netmp Heart Ind\n");
	}
	else if (uidevicetype == DEVICE_SERVER::DEVICE_RELAY_MC)
	{
		svrname_groupid = "relaymc_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndsertodevHeart Recv relaymc Heart Ind\n");
	}
	else if (uidevicetype == DEVICE_SERVER::DEVICE_RELAYSERVER)
	{
		svrname_groupid = "relayserver_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndsertodevHeart Recv relayserver Heart Ind\n");
	}
	else if (uidevicetype == DEVICE_SERVER::DEVICE_MP)
	{
		svrname_groupid = "mp_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndsertodevHeart Recv mp Heart Ind\n");
	}
	else if (uidevicetype == DEVICE_SERVER::DEVICE_GW)
	{
		svrname_groupid = "gw_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndsertodevHeart Recv gw Heart Ind\n");
	}
	else if (uidevicetype == DEVICE_SERVER::DEVICE_DEV)
	{
		svrname_groupid = "devmgr_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndsertodevHeart Recv devmgr Heart Ind\n");
	}
	else if (uidevicetype == DEVICE_SERVER::DEVICE_CRS)
	{
		svrname_groupid = "crs_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndsertodevHeart Recv crs Heart Ind\n");
	}
	else if (uidevicetype == DEVICE_SERVER::DEVICE_STUNSERVER)
	{
		svrname_groupid = "stunserver_";
		sr_printf(SR_LOGLEVEL_INFO, "SRMsgId_IndsertodevHeart Recv stunserver Heart Ind\n");
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndsertodevHeart ind_svr_type error type !!!\n");
		return;
	}

	//bool bRegister = false;
	iter_mapDeviceHeartbeatTime iter = m_MapDeviceHeartbeatTime.find(atoi(deviceid_s));
	if (iter == m_MapDeviceHeartbeatTime.end())
	{
		//m_MapDeviceHeartbeatTime.insert(valType(atoi(deviceid_s), timeNow));

		sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndsertodevHeart deviceid_s:%s unregistered can't to update db\n", deviceid_s);
	}
	else
	{
		//iter->second.devicetype = uidevicetype;
		//iter->second.time = timeNow; //����map�ж�Ӧ������ʱ��ֵ��token
		//iter->second.token = ind->token();
		//bRegister = true;

		if (m_pRedisConnList[e_RC_TT_MainThread] != NULL)
		{
			m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
			char * p = m_pRedisConnList[e_RC_TT_MainThread]->getvalue("dev_deviceidandgroupid", deviceid_s);

			//printf("SRMsgId_IndsertodevHeart getvalue groupid redis db(3) from Table:dev_deviceidandgroupid (deviceid)=(%s)\n", deviceid_s);

			if (p != NULL)
			{
				svrname_groupid += p;

				m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0); // db0ר�Ŵ洢���豸�ĸ�����Ϣ, ZSET

				if (uidevicetype == DEVICE_SERVER::DEVICE_MC)
				{
					std::string svrname_load;
					svrname_load.clear();
					char svrname_load_s[128] = { 0 };
					sprintf(svrname_load_s, "%s_%s", svrname_groupid.c_str(), deviceid_s);
					svrname_load = svrname_load_s;

					char cTernumFieldName[128] = { 0 };
					char cCpuFieldName[128] = { 0 };
					char cSRcntFieldName[128] = { 0 };
					char cSTDcntFieldName[128] = { 0 };
					char cRECcntFieldName[128] = { 0 };
					char cConfcntFieldName[128] = { 0 };
					char cLivecntFieldName[128] = { 0 };

					char ternum_value[33] = { 0 };
					char cpu_value[33] = { 0 };
					char sr_cnt_value[33] = { 0 };
					char std_cnt_value[33] = { 0 };
					char rec_cnt_value[33] = { 0 };
					char conf_cnt_value[33] = { 0 };
					char live_cnt_value[33] = { 0 };

					sprintf(cTernumFieldName, "%s", "ternum");
					sprintf(cCpuFieldName, "%s", "cpu");
					sprintf(cSRcntFieldName, "%s", "srcnt");
					sprintf(cSTDcntFieldName, "%s", "stdcnt");
					sprintf(cRECcntFieldName, "%s", "reccnt");
					sprintf(cConfcntFieldName, "%s", "confcnt");
					sprintf(cLivecntFieldName, "%s", "livecnt");

					const SRMsgs::IndsertodevHeart_LoadInfo& loadinfo = ind->loadinfo();

					sprintf(ternum_value, "%d", loadinfo.ternum());
					sprintf(cpu_value, "%d", loadinfo.cpu());
					sprintf(sr_cnt_value, "%d", loadinfo.srcnt());
					sprintf(std_cnt_value, "%d", loadinfo.stdcnt());
					sprintf(rec_cnt_value, "%d", loadinfo.reccnt());
					sprintf(conf_cnt_value, "%d", loadinfo.confcnt());
					sprintf(live_cnt_value, "%d", loadinfo.livecnt());

					//REDISKEY strupconfreportidkey = netmp_##groupid_##deviceid_##netid_key;
					REDISVDATA vLoadInfodata;
					vLoadInfodata.clear();

					REDISKEY strTernumFieldName = cTernumFieldName;
					REDISVALUE strternumvalue = ternum_value;
					vLoadInfodata.push_back(strTernumFieldName);
					vLoadInfodata.push_back(strternumvalue);

					REDISKEY strCpuFieldName = cCpuFieldName;
					REDISVALUE strcpuvalue = cpu_value;
					vLoadInfodata.push_back(strCpuFieldName);
					vLoadInfodata.push_back(strcpuvalue);

					REDISKEY strSRcntFieldName = cSRcntFieldName;
					REDISVALUE strsrcntvalue = sr_cnt_value;
					vLoadInfodata.push_back(strSRcntFieldName);
					vLoadInfodata.push_back(strsrcntvalue);

					REDISKEY strSTDcntFieldName = cSTDcntFieldName;
					REDISVALUE strstdcntvalue = std_cnt_value;
					vLoadInfodata.push_back(strSTDcntFieldName);
					vLoadInfodata.push_back(strstdcntvalue);

					REDISKEY strRECcntFieldName = cRECcntFieldName;
					REDISVALUE strreccntvalue = rec_cnt_value;
					vLoadInfodata.push_back(strRECcntFieldName);
					vLoadInfodata.push_back(strreccntvalue);

					REDISKEY strConfcntFieldName = cConfcntFieldName;
					REDISVALUE strconfcntvalue = conf_cnt_value;
					vLoadInfodata.push_back(strConfcntFieldName);
					vLoadInfodata.push_back(strconfcntvalue);

					REDISKEY strLivecntFieldName = cLivecntFieldName;
					REDISVALUE strlivecntvalue = live_cnt_value;
					vLoadInfodata.push_back(strLivecntFieldName);
					vLoadInfodata.push_back(strlivecntvalue);

					bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMSet(svrname_load, vLoadInfodata);
				}
				else if (uidevicetype == DEVICE_SERVER::DEVICE_NETMP
					|| uidevicetype == DEVICE_SERVER::DEVICE_RELAY_MC
					|| uidevicetype == DEVICE_SERVER::DEVICE_RELAYSERVER
					|| uidevicetype == DEVICE_SERVER::DEVICE_MP
					|| uidevicetype == DEVICE_SERVER::DEVICE_GW
					|| uidevicetype == DEVICE_SERVER::DEVICE_DEV
					|| uidevicetype == DEVICE_SERVER::DEVICE_CRS
					|| uidevicetype == DEVICE_SERVER::DEVICE_STUNSERVER)
				{
					if (uidevicetype == DEVICE_SERVER::DEVICE_RELAY_MC)
					{
						m_pRedisConnList[e_RC_TT_MainThread]->setvalue(svrname_groupid.c_str(), load2_s, deviceid_s);
						sr_printf(SR_LOGLEVEL_DEBUG, "SRMsgId_IndsertodevHeart getvalue groupid from redis db(3) Table:dev_deviceidandgroupid (deviceid)=(%s) then setvalue to redis db(0)  Table:%s :(deviceid, load2)=(%s, %s)\n", deviceid_s, svrname_groupid.c_str(), deviceid_s, load2_s);
					} 
					else
					{
						//uidevicetype == 2���netmp��load1 load2,load1Ϊ�ն���,load2Ϊ���д�����netmp->ter��mp��
						//uidevicetype == 4���relayserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���
						//uidevicetype == 5���mp��load1 load2,load1Ϊcpuʹ����,load2Ϊ���д�����netmp->mp��
						//uidevicetype == 6���gw��load1 load2,load1Ϊcpuʹ����,load2Ϊ�ն���
						//uidevicetype == 9���crs��load1 load2,load1Ϊcpuʹ����,load2Ϊ����ʹ���ʣ���16λ��������,��16λ��ʣ������,��λ��GB��
						//uidevicetype == 11���stunserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���
						unsigned long long ulltl = ((unsigned long long)(ind->load1()) << 32) + ind->load2();
						char ternum_cpu_load[256] = { 0 };
#ifdef WIN32
						sprintf(ternum_cpu_load, "%I64d", ulltl);
#elif defined LINUX
						sprintf(ternum_cpu_load, "%lld", ulltl);
#endif	
						m_pRedisConnList[e_RC_TT_MainThread]->setvalue(svrname_groupid.c_str(), ternum_cpu_load, deviceid_s);

						sr_printf(SR_LOGLEVEL_DEBUG, "SRMsgId_IndsertodevHeart getvalue groupid from redis db(3) Table:dev_deviceidandgroupid (deviceid)=(%s) then setvalue to redis db(0)  Table:%s :(deviceid, load1+load2)=(%s, %s)\n", deviceid_s, svrname_groupid.c_str(), deviceid_s, ternum_cpu_load);
					}

					// ���������µ�relaymc��relayserver������Ϣ�浽������db0,ͬʱ֪ͨ�ϼ�devmgr
					if (uidevicetype == DEVICE_SERVER::DEVICE_RELAY_MC
						|| uidevicetype == DEVICE_SERVER::DEVICE_RELAYSERVER
						|| uidevicetype == DEVICE_SERVER::DEVICE_STUNSERVER)
					{
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);

						// ��ȡ��������
						RedisReplyArray vrra_domain_level;
						vrra_domain_level.clear();
						unsigned int nDomainAndLevel = 0;

						bool bHGetAllDomain_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHGetAll("local_domain_conf", vrra_domain_level);

						nDomainAndLevel = vrra_domain_level.size();

						if (bHGetAllDomain_ok
							&& (nDomainAndLevel > 0)
							&& (nDomainAndLevel % 2 == 0))
						{
							for (int idx = 0; idx < nDomainAndLevel;)
							{
								int iLevel = 99999;// ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
								//vrra_domain_level[idx].str;// ����
								iLevel = atoi(vrra_domain_level[idx + 1].str.c_str());// level
								if (iLevel == 0)
								{
									// ���ϼ�devmgrͬ��
									if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr
										&& vrra_domain_level[idx].str.length() > 0)
									{
										SRMsgs::IndSubSvrHeartTodev indsvrheartbeat;
										indsvrheartbeat.set_deviceid(m_uiMyDeviceid);
										indsvrheartbeat.set_token(m_szMyToken);

										SRMsgs::IndSubSvrHeartTodev_DomainInfo* psvrdomaininfo = 0;
										{
											psvrdomaininfo = indsvrheartbeat.add_domaininfos();
											psvrdomaininfo->set_domainname(vrra_domain_level[idx].str);
											psvrdomaininfo->set_level(iLevel);
											SRMsgs::IndSubSvrHeartTodev_ServerInfo* pSvrinfo = 0;
											{
												pSvrinfo = psvrdomaininfo->add_svrinfos();
												pSvrinfo->set_svrtype(uidevicetype);
												pSvrinfo->set_deviceid(ind->deviceid());

												SRMsgs::IndSubSvrHeartTodev_LoadInfo* pSvrloadinfo = new SRMsgs::IndSubSvrHeartTodev_LoadInfo();
												if (uidevicetype == DEVICE_SERVER::DEVICE_RELAY_MC)
												{
													pSvrloadinfo->set_ternum(ind->load2());
													pSvrloadinfo->set_tx(0);
													pSvrloadinfo->set_rx(0);
													pSvrloadinfo->set_cpu(0);
												} 
												else if (uidevicetype == DEVICE_SERVER::DEVICE_RELAYSERVER
													|| uidevicetype == DEVICE_SERVER::DEVICE_STUNSERVER)
												{
													pSvrloadinfo->set_ternum(ind->load2());
													pSvrloadinfo->set_tx(ind->load1());
													pSvrloadinfo->set_rx(0);
													pSvrloadinfo->set_cpu(0);
												}
												pSvrinfo->set_allocated_loadinfos(pSvrloadinfo);
											}
										}

										SerialAndSendDevmgr((AcitiveConnect*)(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr), getMsgIdByClassName(IndSubSvrHeartTodev), &indsvrheartbeat);
									}

									// ���¼�devmgrͬ������
									if (vrra_domain_level[idx].str.length() > 0)
									{
										SRMsgs::IndUpSvrHeartTodev indsvrheartbeat;
										//indsvrheartbeat.set_deviceid(m_uiMyDeviceid);// �¼�devmgr��id
										//indsvrheartbeat.set_token(m_szMyToken);// �¼�devmgr��token

										SRMsgs::IndUpSvrHeartTodev_DomainInfo* psvrdomaininfo = 0;
										{
											psvrdomaininfo = indsvrheartbeat.add_domaininfos();
											psvrdomaininfo->set_domainname(vrra_domain_level[idx].str);
											psvrdomaininfo->set_level(iLevel);
											SRMsgs::IndUpSvrHeartTodev_ServerInfo* pSvrinfo = 0;
											{
												pSvrinfo = psvrdomaininfo->add_svrinfos();
												pSvrinfo->set_svrtype(uidevicetype);
												pSvrinfo->set_deviceid(ind->deviceid());

												SRMsgs::IndUpSvrHeartTodev_LoadInfo* pSvrloadinfo = new SRMsgs::IndUpSvrHeartTodev_LoadInfo();
												if (uidevicetype == DEVICE_SERVER::DEVICE_RELAY_MC)
												{
													pSvrloadinfo->set_ternum(ind->load2());
													pSvrloadinfo->set_tx(0);
													pSvrloadinfo->set_rx(0);
													pSvrloadinfo->set_cpu(0);
												}
												else if (uidevicetype == DEVICE_SERVER::DEVICE_RELAYSERVER
													|| uidevicetype == DEVICE_SERVER::DEVICE_STUNSERVER)
												{
													pSvrloadinfo->set_ternum(ind->load2());
													pSvrloadinfo->set_tx(ind->load1());
													pSvrloadinfo->set_rx(0);
													pSvrloadinfo->set_cpu(0);
												}
												pSvrinfo->set_allocated_loadinfos(pSvrloadinfo);
											}
										}

										// 2������¼�devmgrָ��
										for (iter_mapDeviceHeartbeatTime dev_itor = m_MapDeviceHeartbeatTime.begin();
											dev_itor != m_MapDeviceHeartbeatTime.end(); dev_itor++)
										{
											if (dev_itor->second.devicetype == DEVICE_SERVER::DEVICE_DEV)
											{
												if (dev_itor->second.pClient)
												{
													indsvrheartbeat.set_deviceid(dev_itor->first);// �¼�devmgr��id
													indsvrheartbeat.set_token(dev_itor->second.token);// �¼�devmgr��token

													SerialProtoMsgAndSend(dev_itor->second.pClient, getMsgIdByClassName(IndUpSvrHeartTodev), &indsvrheartbeat);
												}
											}
										}
									}

									break;
								}
								idx += 2;
							}
						}
					}
				}

				delete p; // ��ɾ���ᵼ���ڴ�й©
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SRMsgId_IndsertodevHeart getvalue groupid from redis db(3) Table:dev_deviceidandgroupid (deviceid)=(%s) return NULL error!!!\n", deviceid_s);

				return;
			}
			
			char time_s[256] = { 0 };
#ifdef WIN32
			sprintf(time_s, "%I64d", timeNow);
#elif defined LINUX
			sprintf(time_s, "%lld", timeNow);
#endif	

			m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1); // db1ר�Ŵ洢���豸����ʱ��,HASH
			m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(svrname_groupid.c_str(), time_s, deviceid_s);

			//����map�ж�Ӧ������ʱ��ֵ��token
			iter->second.devicetype = uidevicetype;
			iter->second.time = timeNow; 
			iter->second.token = ind->token();

			sr_printf(SR_LOGLEVEL_DEBUG, "SRMsgId_IndsertodevHeart sethashvalue to redis db(1)  Table:%s :(deviceid, time_s)=(%s, %s)\n", svrname_groupid.c_str(), deviceid_s, time_s);
		}
	}
}

void CDevMgr::GetConfInfoFromDB(const SRMsgs::ReqConfInfoFromDevMgr* req, CConference* pConference, CConfParticipant* pConfParticipant, CConfLiveSetting* pConfLiveSetting
	, CConfRollCallInfo* pConfRollCallInfo, CRollCallList* pRollCallList, CConfPollInfo* pConfPollInfo, CPollList* pPollList, SRMsgs::RspConfInfoToMC* rspsend)
{
	// �Ȳ�ѯredis db2 conference_deviceid���Ƿ��е�ǰmcid

	char confid_s[256] = { 0 };
#ifdef WIN32
	sprintf(confid_s, "%I64d", req->confid());
#elif defined LINUX
	sprintf(confid_s, "%lld", req->confid());
#endif

	int iMcid = -1;
	m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);
	char *ptrRet = NULL;
	ptrRet = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue("conference_deviceid", confid_s); // ��ȡ��key��Ӧ��value
	if (ptrRet != NULL)
	{
		iMcid = atoi(ptrRet);
	}

	// ���ж������ǽ��һ������ֲ��ڶ��mc�ϵ�����
	if (iMcid == req->deviceid())
	{
		//��ȡ���ݿ⣬�жϵ�ǰ������Ƿ���ڣ�
		pConference->SetConfID(req->confid());

		pConference->SelectDB();

		if (pConference->GetExist())
		{
			//��������ô�������ӿڲ�����
			rspsend->set_isok(true);
			rspsend->set_suid(pConference->GetUserID());
			rspsend->set_confid(pConference->GetConfID());
			rspsend->set_confpwd(pConference->GetConfPasswd().c_str());
			rspsend->set_recvpwd(pConference->GetRecvPasswd().c_str());
			rspsend->set_begintime(pConference->GetConfStartTime().c_str());
			rspsend->set_endtime(pConference->GetConfEndTime().c_str());
			rspsend->set_islimited(false);
			rspsend->set_subject(pConference->GetConfSubject().c_str());
			rspsend->set_conftype(pConference->GetConfType());
			rspsend->set_confstartmethod(pConference->GetConfStartType());
			rspsend->set_mutetype(0);
			rspsend->set_ternums(pConference->GetConfMaxterm());

			SRMsgs::RspConfInfoToMC_AgcSetting* pAgcSetting = new SRMsgs::RspConfInfoToMC_AgcSetting();
			pAgcSetting->set_agc_enable(true);
			pAgcSetting->set_target_level_dbfs(1);
			pAgcSetting->set_compression_gain_db(1);
			pAgcSetting->set_limiter_enable(true);
			SRMsgs::RspConfInfoToMC_Confcfg* pConfcfg = new SRMsgs::RspConfInfoToMC_Confcfg();
			//pConfcfg->set_max_participants(pConference->GetConfMaxterm()); //�������ɵ�����ն���
			pConfcfg->set_max_audiomix_num(4); //������������
			pConfcfg->set_max_encoder_num(5);//������������������
			pConfcfg->set_useaudiomixer(1);
			pConfcfg->set_usevideomixer(1);
			pConfcfg->set_mixing_frequency(48000);
			pConfcfg->set_videosize(pConference->GetVideoSize());
			pConfcfg->set_allocated_agc_setting(pAgcSetting);

			rspsend->set_allocated_confcfg(pConfcfg);

			rspsend->set_failurereason("");
			rspsend->set_confreportid(m_pub_confreport_id);
			m_pub_confreport_id++;

			std::map<unsigned int, std::string> partuidnameinfos;
			partuidnameinfos.clear();

			if (pConfParticipant != NULL)
			{
				std::map<unsigned int, ParticipantInfo*> participantInfos;
				participantInfos.clear();

				pConfParticipant->SetConfID(req->confid());
				pConfParticipant->SelectParticipantUserInfo(participantInfos);

				SRMsgs::RspConfInfoToMC_ParticipantInfo* pAddPartInfo = 0;
				for (std::map<unsigned int, ParticipantInfo*>::iterator ppinfo_itor = participantInfos.begin();
					ppinfo_itor != participantInfos.end(); ppinfo_itor++)
				{
					ParticipantInfo* pParticipantInfo = NULL;
					pParticipantInfo = ppinfo_itor->second;
					if (pParticipantInfo != NULL)
					{
						pAddPartInfo = rspsend->add_participants();
						if (pAddPartInfo != 0)
						{
							pAddPartInfo->set_psuid(ppinfo_itor->first);
							pAddPartInfo->set_nickname(pParticipantInfo->m_nickname);
							pAddPartInfo->set_ip(pParticipantInfo->m_ip);
							pAddPartInfo->set_protocoltype(pParticipantInfo->m_protocoltype);
							pAddPartInfo->set_bandwidth(pParticipantInfo->m_bandwidth);
							pAddPartInfo->set_usertype(pParticipantInfo->m_usertype);
							pAddPartInfo->set_devicetype(pParticipantInfo->m_devicetype);
							pAddPartInfo->set_devicecode(pParticipantInfo->m_devicecode);
							pAddPartInfo->set_shortname(pParticipantInfo->m_shortname);
							pAddPartInfo->set_orderno(pParticipantInfo->m_orderno);

							partuidnameinfos.insert(std::make_pair(ppinfo_itor->first, pParticipantInfo->m_nickname));
						}
						delete pParticipantInfo;
					}
				}

				participantInfos.clear();
			}
			rspsend->set_confctrlsuid(pConference->GetCtrlUserID());
			rspsend->set_audioenable(pConference->GetAudioEnable());
			rspsend->set_videoenable(pConference->GetVideoEnable());
			rspsend->set_confname(pConference->GetConfName());
			rspsend->set_encryptalg(pConference->GetEncryptAlg());

			if (pConfLiveSetting != NULL)
			{
				pConfLiveSetting->SetConfID(req->confid());
				bool sel_ret = false;
				sel_ret = pConfLiveSetting->SelectConfLiveSettingInfo();
				if (sel_ret)
				{
					SRMsgs::RspConfInfoToMC_LiveSetting* plivesetting = new SRMsgs::RspConfInfoToMC_LiveSetting();
					plivesetting->set_subject(pConfLiveSetting->GetLivesetSubject());
					plivesetting->set_chairman(pConfLiveSetting->GetLivesetChairman());
					plivesetting->set_abstract(pConfLiveSetting->GetLivesetAbstract());
					plivesetting->set_ispublic(pConfLiveSetting->GetIsPublic());
					plivesetting->set_livepwd(pConfLiveSetting->GetLivePWD());
					plivesetting->set_isuserec(pConfLiveSetting->GetIsUseRec());
					rspsend->set_allocated_livesetinfo(plivesetting);
				}
			}

			rspsend->set_autorec(pConference->GetAutoREC());

			if (pConfRollCallInfo != NULL)
			{
				std::list<RollCallInfo*> rollcallInfos;
				rollcallInfos.clear();

				pConfRollCallInfo->SetConfID(req->confid());
				pConfRollCallInfo->SelectConfCallInfo(rollcallInfos);

				SRMsgs::RspConfInfoToMC_RollCallInfo* pAddrcInfo = 0;
				for (std::list<RollCallInfo*>::iterator prcinfo_itor = rollcallInfos.begin();
					prcinfo_itor != rollcallInfos.end(); prcinfo_itor++)
				{
					RollCallInfo* pRollCallInfo = NULL;
					pRollCallInfo = (*prcinfo_itor);
					if (pRollCallInfo != NULL)
					{
						pAddrcInfo = rspsend->add_rollcalllist();
						if (pAddrcInfo != 0)
						{
							pAddrcInfo->set_rcid(pRollCallInfo->m_callinfo_id);
							pAddrcInfo->set_rcname(pRollCallInfo->m_callinfo_name);
							
							if (pRollCallList != NULL)
							{
								std::list<CallList*> calllist;
								calllist.clear();

								pRollCallList->SetCallInfoID(pRollCallInfo->m_callinfo_id);
								pRollCallList->SelectCallList(calllist);// ��ȡ�����б�������order_no�����ȡ��

								SRMsgs::RspConfInfoToMC_ParticipantInfo* pAddcalllist = 0;
								for (std::list<CallList*>::iterator pcl_itor = calllist.begin();
									pcl_itor != calllist.end(); pcl_itor++)
								{
									CallList* pCallList = NULL;
									pCallList = (*pcl_itor);
									if (pCallList != NULL)
									{
										std::map<unsigned int, std::string>::iterator user_itor = partuidnameinfos.find(pCallList->m_user_id);
										if (user_itor != partuidnameinfos.end())
										{
											pAddcalllist = pAddrcInfo->add_parts();
											pAddcalllist->set_psuid(pCallList->m_user_id);
											pAddcalllist->set_nickname(user_itor->second);
										}

										delete pCallList;
									}
								}
								calllist.clear();
							}
						}
						delete pRollCallInfo;
					}
				}

				rollcallInfos.clear();
			}

			if (pConfPollInfo != NULL)
			{
				std::list<PollInfo*> pollInfos;
				pollInfos.clear();

				pConfPollInfo->SetConfID(req->confid());
				pConfPollInfo->SelectConfPollInfo(pollInfos);

				SRMsgs::RspConfInfoToMC_AutoPollInfo* pAddapInfo = 0;
				for (std::list<PollInfo*>::iterator papinfo_itor = pollInfos.begin();
					papinfo_itor != pollInfos.end(); papinfo_itor++)
				{
					PollInfo* pPollInfo = NULL;
					pPollInfo = (*papinfo_itor);
					if (pPollInfo != NULL)
					{
						pAddapInfo = rspsend->add_autopolllist();
						if (pAddapInfo != 0)
						{
							pAddapInfo->set_apid(pPollInfo->m_pollinfo_id);
							pAddapInfo->set_apname(pPollInfo->m_pollinfo_name);

							if (pPollList != NULL)
							{
								std::list<PollList*> polllist;
								polllist.clear();

								pPollList->SetPollInfoID(pPollInfo->m_pollinfo_id);
								pPollList->SelectPollList(polllist);// ��ȡ��Ѳ�б�������order_no�����ȡ��

								SRMsgs::RspConfInfoToMC_ParticipantInfo* pAddpolllist = 0;
								for (std::list<PollList*>::iterator ppl_itor = polllist.begin();
									ppl_itor != polllist.end(); ppl_itor++)
								{
									PollList* ppPollList = NULL;
									ppPollList = (*ppl_itor);
									if (ppPollList != NULL)
									{
										std::map<unsigned int, std::string>::iterator user_itor = partuidnameinfos.find(ppPollList->m_user_id);
										if (user_itor != partuidnameinfos.end())
										{
											pAddpolllist = pAddapInfo->add_parts();
											pAddpolllist->set_psuid(ppPollList->m_user_id);
											//pAddpolllist->set_nickname(user_itor->second);
										}

										delete ppPollList;
									}
								}
								polllist.clear();
							}
						}
						delete pPollInfo;
					}
				}

				pollInfos.clear();
			}

			partuidnameinfos.clear();

			rspsend->set_permanentenable(pConference->GetPermanentEnable());
		}
		else
		{
			sr_printf(SR_LOGLEVEL_DEBUG, "GetConfInfoFromDB confid=%s is not exist in mysql.\n", confid_s);
			//mysql�����ڣ����ʹ�������ʧ����ʾ
			rspsend->set_isok(false);
			rspsend->set_confid(req->confid());
			rspsend->set_failurereason("conference is not exist");
			rspsend->set_errorcode(0x030006);
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_DEBUG, "GetConfInfoFromDB confid=%s reqmcid=%d is not equal related(in redis) mcid=%d.\n", confid_s, req->deviceid(), iMcid);
		//redis�����ڣ����ʹ�������ʧ����ʾ
		rspsend->set_isok(false);
		rspsend->set_confid(req->confid());
		rspsend->set_failurereason("choose mc not in redis");
		rspsend->set_errorcode(0x030007);
	}
}

void CDevMgr::SyncConfInfo(const SRMsgs::ReqRegister* req, CDevice* pDevice, time_t timeNow)
{
	if (pDevice == NULL)
	{
		return;
	}

	int iDeviceid = pDevice->GetDeviceID();
	
	// �Ը��豸������ݿ�Mysql��Redis���и��´���
	unsigned int uiDeviceType = req->svr_type(); // Ŀǰֻ��mc

	if (req->conflist_size() > 0)
	{
		sr_printf(SR_LOGLEVEL_INFO, "====CDevMgr::SyncConfInfo==== (device_id=%d, conflist_size=%d) --->>\n", iDeviceid, req->conflist_size());
		for (int i = 0; i < req->conflist_size(); i++)
		{
			const SRMsgs::ReqRegister_ConfInfo& confinfo = req->conflist(i);
			SR_uint64 confid = confinfo.confid();

			char deviceid_id[256] = { 0 };
			sprintf(deviceid_id, "deviceid_%d", iDeviceid);

			char deviceid[256] = { 0 };
			sprintf(deviceid, "%d", iDeviceid);
			char confid_s[256] = { 0 };
#ifdef WIN32
			sprintf(confid_s, "%I64d", confid);
#elif defined LINUX
			sprintf(confid_s, "%lld", confid);
#endif

			// �˴�ֻ����ͬ��redis db2 conference_deviceid������
			m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);
			// �Ȳ��Ҹû����Ƿ���redis�У�������ڣ�˵���û�����δ��������
			char *ptrRet = NULL;
			ptrRet = m_pRedisConnList[e_RC_TT_MainThread]->gethashvalue(deviceid_id, confid_s); // ��ȡ��key��Ӧ��value
			if (ptrRet == NULL)
			{
				// �û��鲻����,�Ѿ������������쳣������,��Ҫ����ǰ�������²���

				m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue("conference_deviceid", deviceid, confid_s);
				m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(deviceid_id, deviceid, confid_s);

				//��ȡ��ǰʱ�䴮
				time_t timeNow;
				struct tm *ptmNow;
				char szTime[30];
				timeNow = time(NULL);
				ptmNow = localtime(&timeNow);
				sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);

				//�������ݿ⣬�û���״̬Ϊ����״̬
				//����confreport��
				InsertConfReport(confid, m_pub_confreport_id, szTime, timeNow); // ��confreport��Ϣд�ڴ棬���ڲ���confreport��
				//ͬʱ����device_conf_detail
				InsertDeviceConfDetail(confid, iDeviceid, m_pub_confreport_id, m_pub_device_detail_id, szTime, timeNow); // ��device_conf_detail��Ϣд�ڴ棬���ڲ���device_conf_detail��

				//m_pub_confreport_id++;
				m_pub_device_detail_id++;

				sr_printf(SR_LOGLEVEL_INFO, "   ====CDevMgr::SyncConfInfo====(confid_s=%s, terlist_size=%d) --->>\n", confid_s, confinfo.terlist_size());
				SR_uint32 tempnetmpid = 0;
				for (int j = 0; j < confinfo.terlist_size(); j++)
				{
					const SRMsgs::ReqRegister_TerInfo& terinfo = confinfo.terlist(j);
					SR_uint32 suid = terinfo.suid();
					std::string alias = terinfo.alias();
					SR_uint32 netmpid = terinfo.netmpid();
					SR_uint32 fromtype = terinfo.fromtype();
					SR_uint32 termtype = terinfo.termtype();

					if (tempnetmpid != netmpid)
					{
						tempnetmpid = netmpid;
						char deviceid_netmpid[256] = { 0 };
						sprintf(deviceid_netmpid, "deviceid_%d", netmpid);
						char netmpid_s[256] = { 0 };
						sprintf(netmpid_s, "%d", netmpid);

						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(2);
						m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(deviceid_netmpid, netmpid_s, confid_s);

						InsertDeviceConfDetail(confid, netmpid, m_pub_confreport_id, m_pub_device_detail_id, szTime, timeNow, true);
						m_pub_device_detail_id++;
					}

					// mc��netmp�ĸ�����ʱ�����м���������redis db0 db6 db3����

					//����user_conf_detail��
					AddUserConfDetail(suid, confid, m_pub_confreport_id, m_pub_userrpt_detail_id, (char*)(alias.c_str()), szTime, timeNow, fromtype, termtype);
					m_pub_userrpt_detail_id++;
				}

				m_pub_confreport_id++;
			}
			else
			{
				// �û��黹δ��������һ�����value�ǲ��Ǹ�mc
				int iMcid = 0;
				iMcid = atoi(ptrRet);
				sr_printf(SR_LOGLEVEL_INFO, "==CDevMgr::SyncConfInfo ===iDeviceid=%d [confid_s=%s,iMcid=%d] has in db(2) tab:%s--->>\n", iDeviceid, confid_s, iMcid, deviceid_id);
				if (iMcid != 0)
				{
					// ͬ�������mc��redis�����mc��һ��
					if (iDeviceid != iMcid)
					{
						sr_printf(SR_LOGLEVEL_WARNING, "==CDevMgr::SyncConfInfo ===iDeviceid = %d not is redis mcid=%d--->>\n", iDeviceid,iMcid);
					}
					else
					{
					}
				}
			}
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "==CDevMgr::SyncConfInfo ===iDeviceid = %d has no conflist,so do nothing--->>\n", iDeviceid);
	}
}

void CDevMgr::SyncSubSvrInfo(const SRMsgs::IndSubSvrInfoToDevmgr* ind, time_t timeNow)
{
	// Ŀǰֻͬ��relaymc��relayserver��stunserver����Ϣ
	if (ind->domaininfos_size() > 0)
	{
		for (int i = 0; i < ind->domaininfos_size(); i++)
		{
			const SRMsgs::IndSubSvrInfoToDevmgr_DomainInfo& domaininfo = ind->domaininfos(i);
			
			if (domaininfo.domainname().length() > 0
				&& domaininfo.level() != 99999) // ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
			{
				char domainname_s[128] = { 0 };
				sprintf(domainname_s, "%s", domaininfo.domainname().c_str());
				char sysdomaindevinfo_s[128] = { 0 };
				sprintf(sysdomaindevinfo_s, "sys_domain_deviceinfo_%s", domaininfo.domainname().c_str());				
				int level = domaininfo.level();

				//char subdomain_level_s[64] = { 0 };
				//sprintf(subdomain_level_s, "%d", domaininfo.level()+1);

				//m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);
				//// add
				//if (ind->addordel() == 1)
				//{
				//	m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue("sys_domain_info", subdomain_level_s, domainname_s);
				//}

				for (int j = 0; j < domaininfo.svrinfos_size(); j++)
				{
					const SRMsgs::IndSubSvrInfoToDevmgr_ServerInfo& svrinfo = domaininfo.svrinfos(j);
					unsigned int uisvrtype = svrinfo.svrtype();
					unsigned int uideviceid = svrinfo.deviceid();
					if (uideviceid == 0
						|| (uisvrtype != DEVICE_SERVER::DEVICE_RELAY_MC
						&& uisvrtype != DEVICE_SERVER::DEVICE_RELAYSERVER
						&& uisvrtype != DEVICE_SERVER::DEVICE_STUNSERVER))
					{
						continue;
					}
					else
					{
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(3);

						char svrtype_s[33] = { 0 };
						char deviceid_s[33] = { 0 };
						sprintf(svrtype_s, "%u", uisvrtype);
						sprintf(deviceid_s, "%u", uideviceid);

						char sys_svrtype_dmomain_s[256] = { 0 };
						char svrtype_ipport_dmomain_deviceid_s[256] = { 0 };
						if (uisvrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
						{
							sprintf(sys_svrtype_dmomain_s, "sys_relaymc_%s", domainname_s);
							sprintf(svrtype_ipport_dmomain_deviceid_s, "relaymc_ipport_%s_%u", domainname_s, uideviceid);
						}
						else if (uisvrtype == DEVICE_SERVER::DEVICE_RELAYSERVER)
						{
							sprintf(sys_svrtype_dmomain_s, "sys_relayserver_%s", domainname_s);
							sprintf(svrtype_ipport_dmomain_deviceid_s, "relayserver_ipport_%s_%u", domainname_s, uideviceid);
						}
						else if (uisvrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
						{
							sprintf(sys_svrtype_dmomain_s, "sys_stunserver_%s", domainname_s);
							sprintf(svrtype_ipport_dmomain_deviceid_s, "stunserver_ipport_%s_%u", domainname_s, uideviceid);
						}

						const SRMsgs::IndSubSvrInfoToDevmgr_IPPORTInfo& svripportinfo = svrinfo.ipport();

						// add
						if (ind->addordel() == 1)
						{
							m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(sysdomaindevinfo_s, svrtype_s, deviceid_s);

							char cInnerFieldName[128] = { 0 };
							char cOuterFieldName[128] = { 0 };
							char cVPNFieldName[128] = { 0 };

							char inner_ipprort_value[65] = { 0 };
							char outer_ipprort_value[65] = { 0 };
							char vpn_ipprort_value[65] = { 0 };

							sprintf(cInnerFieldName, "%s", "inner");
							sprintf(cOuterFieldName, "%s", "outer");
							sprintf(cVPNFieldName, "%s", "vpn");
							
							sprintf(inner_ipprort_value, "%s", svripportinfo.inner().c_str());
							sprintf(outer_ipprort_value, "%s", svripportinfo.outer().c_str());
							sprintf(vpn_ipprort_value, "%s", svripportinfo.vpn().c_str());


							REDISKEY strsvrtypeipportdmomaindeviceidkey = svrtype_ipport_dmomain_deviceid_s;
							REDISVDATA vIPPortInfodata;
							vIPPortInfodata.clear();

							REDISKEY strInnerFieldName = cInnerFieldName;
							REDISVALUE strinneripprortvalue = inner_ipprort_value;
							vIPPortInfodata.push_back(strInnerFieldName);
							vIPPortInfodata.push_back(strinneripprortvalue);

							REDISKEY strOuterFieldName = cOuterFieldName;
							REDISVALUE strouteripprortvalue = outer_ipprort_value;
							vIPPortInfodata.push_back(strOuterFieldName);
							vIPPortInfodata.push_back(strouteripprortvalue);

							REDISKEY strVPNFieldName = cVPNFieldName;
							REDISVALUE strvpnipprortvalue = vpn_ipprort_value;
							vIPPortInfodata.push_back(strVPNFieldName);
							vIPPortInfodata.push_back(strvpnipprortvalue);

							bool bhashHMSet_ok = m_pRedisConnList[e_RC_TT_MainThread]->hashHMSet(strsvrtypeipportdmomaindeviceidkey, vIPPortInfodata);
						}
						else if (ind->addordel() == 2)// delete
						{
							m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue(sysdomaindevinfo_s, deviceid_s);
							
							m_pRedisConnList[e_RC_TT_MainThread]->deletevalue(svrtype_ipport_dmomain_deviceid_s);

							// ����
							m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);
							m_pRedisConnList[e_RC_TT_MainThread]->deletevalue(sys_svrtype_dmomain_s, deviceid_s);
							// ����
							m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);
							m_pRedisConnList[e_RC_TT_MainThread]->deletehashvalue(sys_svrtype_dmomain_s, deviceid_s);
						}
					}
				}
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "SyncSubSvrInfo IndSubSvrInfoToDevmgr msg domaininfo error!!!\n");
			}
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "SyncSubSvrInfo IndSubSvrInfoToDevmgr msg domaininfos_size=%d error!!!\n", ind->domaininfos_size());
	}
}

void CDevMgr::UpdateSubSvrHeartbeatToDB(const SRMsgs::IndSubSvrHeartTodev* ind, time_t timeNow)
{
	// Ŀǰֻͬ��relaymc��relayserver��stunserver�ĸ�����Ϣ
	if (ind->domaininfos_size() > 0)
	{
		for (int i = 0; i < ind->domaininfos_size(); i++)
		{
			const SRMsgs::IndSubSvrHeartTodev_DomainInfo& domaininfo = ind->domaininfos(i);

			if (domaininfo.domainname().length() > 0
				&& domaininfo.level() != 99999) // ������0��Ĭ��ֵ(��Ϊ0��ʾ������)
			{
				char domainname_s[128] = { 0 };
				sprintf(domainname_s, "%s", domaininfo.domainname().c_str());
				char sysdomaindevinfo_s[128] = { 0 };
				sprintf(sysdomaindevinfo_s, "sys_domain_deviceinfo_%s", domaininfo.domainname().c_str());
				//int level = domaininfo.level();

				for (int j = 0; j < domaininfo.svrinfos_size(); j++)
				{
					const SRMsgs::IndSubSvrHeartTodev_ServerInfo& svrinfo = domaininfo.svrinfos(j);
					unsigned int uisvrtype = svrinfo.svrtype();
					unsigned int uideviceid = svrinfo.deviceid();
					if (uideviceid == 0
						|| (uisvrtype != DEVICE_SERVER::DEVICE_RELAY_MC
						&& uisvrtype != DEVICE_SERVER::DEVICE_RELAYSERVER
						&& uisvrtype != DEVICE_SERVER::DEVICE_STUNSERVER))
					{
						continue;
					}
					else
					{

						const SRMsgs::IndSubSvrHeartTodev_LoadInfo& svrloadinfo = svrinfo.loadinfos();

						char svrtype_s[33] = { 0 };
						char deviceid_s[33] = { 0 };
						sprintf(svrtype_s, "%u", uisvrtype);
						sprintf(deviceid_s, "%u", uideviceid);

						// ����
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(0);

						char sys_svrtype_dmomain_s[256] = { 0 };
						if (uisvrtype == DEVICE_SERVER::DEVICE_RELAY_MC)
						{
							sprintf(sys_svrtype_dmomain_s, "sys_relaymc_%s", domainname_s);

							char ternum_s[33] = { 0 };
							sprintf(ternum_s, "%d", svrloadinfo.ternum());
							m_pRedisConnList[e_RC_TT_MainThread]->setvalue(sys_svrtype_dmomain_s, ternum_s, deviceid_s);

							sr_printf(SR_LOGLEVEL_DEBUG, "UpdateSubSvrHeartbeatToDB IndSubSvrHeartTodev setvalue to redis db(0) Table:%s :(deviceid, ternum)=(%s, %s)\n", sys_svrtype_dmomain_s, deviceid_s, ternum_s);
						}
						else if (uisvrtype == DEVICE_SERVER::DEVICE_RELAYSERVER
							|| uisvrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
						{
							if (uisvrtype == DEVICE_SERVER::DEVICE_RELAYSERVER)
							{
								sprintf(sys_svrtype_dmomain_s, "sys_relayserver_%s", domainname_s);
							} 
							else if (uisvrtype == DEVICE_SERVER::DEVICE_STUNSERVER)
							{
								sprintf(sys_svrtype_dmomain_s, "sys_stunserver_%s", domainname_s);
							}

							//uidevicetype == 4���relayserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���

							//uidevicetype == 11���stunserver��load1 load2,load1Ϊ���д���,load2Ϊ�ն���
							unsigned long long ulltl = ((unsigned long long)(svrloadinfo.tx()) << 32) + svrloadinfo.ternum();
							char svr_load_s[128] = { 0 };
#ifdef WIN32
							sprintf(svr_load_s, "%I64d", ulltl);
#elif defined LINUX
							sprintf(svr_load_s, "%lld", ulltl);
#endif	
							m_pRedisConnList[e_RC_TT_MainThread]->setvalue(sys_svrtype_dmomain_s, svr_load_s, deviceid_s);

							sr_printf(SR_LOGLEVEL_DEBUG, "UpdateSubSvrHeartbeatToDB IndSubSvrHeartTodev setvalue to redis db(0) Table:%s :(deviceid, tx+ternum)=(%s, %s)\n", sys_svrtype_dmomain_s, deviceid_s, svr_load_s);
						}

						// ����
						m_pRedisConnList[e_RC_TT_MainThread]->selectdb(1);
						char time_s[256] = { 0 };
#ifdef WIN32
						sprintf(time_s, "%I64d", timeNow);
#elif defined LINUX
						sprintf(time_s, "%lld", timeNow);
#endif	
						m_pRedisConnList[e_RC_TT_MainThread]->sethashvalue(sys_svrtype_dmomain_s, time_s, deviceid_s);

						sr_printf(SR_LOGLEVEL_DEBUG, "UpdateSubSvrHeartbeatToDB IndSubSvrHeartTodev sethashvalue to redis db(1)  Table:%s :(deviceid, time_s)=(%s, %s)\n", sys_svrtype_dmomain_s, deviceid_s, time_s);
					}// loadinfos
				}// svrinfos
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "UpdateSubSvrHeartbeatToDB IndSubSvrHeartTodev msg domaininfo error!!!\n");
			}
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "UpdateSubSvrHeartbeatToDB IndSubSvrHeartTodev msg domaininfos_size=%d error!!!\n", ind->domaininfos_size());
	}
}

void* CDevMgr::createTimer(SR_uint64 millisecond, SR_int32 timertype, SR_uint32 timerflag, SR_uint64 timerdata)
{
	SRMC::TimerData* ptd = new SRMC::TimerData(timerflag, timerdata);
	bool isPerio = ((timertype == SRMC::e_periodicity_timer) ? true : false);
	return m_timermanger->CreateTimer(this, ptd, (SR_uint32)millisecond, isPerio);
}

// typedef SR_void (TerMsgProcessThread::*MethodFun_t)(Message* ptmsg);
// if else �� O3 �Ż�֮�� �걬 &Class::Method 2 ������
// ����̫���ӾͿ���ʹ�� sigslot ���� <-> �����
SR_void CDevMgr::OnMessage(SRMC::Message* msg)
{
	SR_uint32 messageid = msg->message_id;
	SRMC::Simple_ScopedPtr autodelete(msg->pdata);
	//printf("messageid=0x%x\n",messageid);

	if (SRMC::e_timerid == messageid)//epol_server
	{
		this->onTimerArrive(static_cast<SRMC::TimerData*>(msg->pdata));
	}
	//else if (e_notify_to_send_heart2devmgr == messageid)
	//{
	//}
}

SR_void CDevMgr::onTimerArrive(const SRMC::TimerData* ptd)
{
	SR_uint32 timerflag = ptd->m_timerflag;
	//sr_printf(SR_LOGLEVEL_DEBUG, "CDevMgr::onTimerArrive timerflag=0x%x\n", timerflag);
	
	switch (timerflag)
	{
	case e_update_selfheartbeat2db_timer:
	{
		sr_printf(SR_LOGLEVEL_DEBUG, "e_update_selfheartbeat2db_timer.\n");
		// 1����ʱ��������ļ������·����仯��������
		CheckConfig();

		// 2����ʱ���redis����
		CheckDBConn();

		// 3�������豸�������Լ�������
		if (m_iSelfDeviceid != 0)
		{
			//char cMyToken[128] = { 0 };
			//sprintf(cMyToken, "%d", m_iSelfDeviceid);

			//SRMsgs::IndsertodevHeart selfhbind;
			//selfhbind.set_svr_type(DEVICE_SERVER::DEVICE_DEV);
			//selfhbind.set_deviceid(m_iSelfDeviceid);
			//selfhbind.set_token(cMyToken);
			//selfhbind.set_load1(0u);
			//selfhbind.set_load2(0u);

			time_t timeNow;
			timeNow = time(NULL);

			//sr_printf(SR_LOGLEVEL_NORMAL, "********UpdateSelfHeartbeatToDB (deviceid, timeNow)=(%d, %"SR_PRIu64")\n", selfhbind.deviceid(), timeNow);

			////��������������ݿ�
			//UpdatePeerHeartbeatToDB(&selfhbind, timeNow);

			UpdateSelfHeartbeatToDB(m_iSelfDeviceid, timeNow);
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "e_update_selfheartbeat2db_timer m_iSelfDeviceid is 0.\n");
		}

		break;
	}
	case e_check_client_heartbeat_timer:
	{
		sr_printf(SR_LOGLEVEL_DEBUG, "e_check_client_heartbeat_timer.\n");
		// ��ʱ��������(�ͻ���)������
		CheckDeviceHeartbeat();
		break;
	}
	case e_write_redis2mysql_timer:
	{
		sr_printf(SR_LOGLEVEL_DEBUG, "e_write_redis2mysql_timer.\n");
		// ��ʱ�����������Ϣ��redisд�����ݿ�
		CheckWriteDB();
		break;
	}
	case e_connect_devmgr_timer:
	{
		//ֻ������devmgr ������ʧ��ʱ�Ż����,������һ�ֵ�ѭ��
		for (std::list<SRMC::DevmgrSockInfo*>::iterator devs_itor = m_devConnMgr->m_devmgrinfomanager->m_devmgrInfos.begin();
			devs_itor != m_devConnMgr->m_devmgrinfomanager->m_devmgrInfos.end(); ++devs_itor)
		{
			SRMC::DevmgrSockInfo* pdsinfo = *devs_itor;
			if ((NULL == pdsinfo->m_sockptr) && (SRMC::e_DevMgr_sock_init == pdsinfo->m_devsockstat))
			{

				pdsinfo->m_sockptr = m_devConnMgr->ConnectDevmgr(pdsinfo->m_ip.c_str(), pdsinfo->m_port);
				//m_devConnMgr->m_devmgrinfomanager->m_current_dev_connectcnt++;//ÿ����һ�μ�1

				//if (m_devConnMgr->m_devmgrinfomanager->m_current_dev_connectcnt == 3)
				//{
				//	PostToTerThread(e_notify_to_check_confcrsinfo, 0);
				//}

				if (false == m_devConnMgr->isConnecting())
				{
					sr_printf(SR_LOGLEVEL_ERROR, "e_connect_devmgr_timer connect new devmgr(ip=%s, port=%d) is failed.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port);
					pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_error;
					pdsinfo->m_sockptr = NULL;
				}
				else
				{
					sr_printf(SR_LOGLEVEL_NORMAL, "e_connect_devmgr_timer connect new devmgr(ip=%s, port=%d) is connecting, m_sockptr=%p.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port, pdsinfo->m_sockptr);
					pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_connecting;
					m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr = pdsinfo->m_sockptr;
					return;
				}
			}
		}

		//��ʱû���ҵ����õ�devmgr,��ʱȥ��֮ǰ�����ϵ�
		for (std::list<SRMC::DevmgrSockInfo*>::iterator devs_itor = m_devConnMgr->m_devmgrinfomanager->m_devmgrInfos.begin();
			devs_itor != m_devConnMgr->m_devmgrinfomanager->m_devmgrInfos.end(); ++devs_itor)
		{
			SRMC::DevmgrSockInfo* pdsinfo = *devs_itor;
			pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_init;
			pdsinfo->m_sockptr = NULL;
		}

		sr_printf(SR_LOGLEVEL_WARNING, "e_connect_devmgr_timer connect all devmgr is failed, so continue search other devmgr.\n");

		createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_connect_devmgr_timer, 0ull);

		break;
	}
	case e_re_register_timer:
	{
		sr_printf(SR_LOGLEVEL_WARNING, "e_re_register_timer for register devmgr again.\n");
		//PostToTerThread(e_notify_to_register_devmgr, 0);

		RegisterToDevmgr();
		break;
	}
	case e_register_self_timer:
	{
		sr_printf(SR_LOGLEVEL_WARNING, "e_register_self_timer for register self again.\n");

		OnRegisterSelf();
		break;
	}
	case e_hearttodevmgr_timer:
	{
		SendHeartBeatToDevmgr();
		break;
	}
	case e_req_up_relaymcinfo_timer:
	{
		ReqUPServerInfoPeric(DEVICE_SERVER::DEVICE_RELAY_MC);
		break;
	}
	case e_req_up_relayserverinfo_timer:
	{
		ReqUPServerInfoPeric(DEVICE_SERVER::DEVICE_RELAYSERVER);
		break;
	}
	case e_req_up_stunserverinfo_timer:
	{
		ReqUPServerInfoPeric(DEVICE_SERVER::DEVICE_STUNSERVER);
		break;
	}
	case e_check_acitiveconnect_timer:
	{
		m_devConnMgr->CheckActiveConnTimer();
		break;
	}
	case e_check_svr_heartbeat_timer:
	{
		if (m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr && (m_szMyToken.length() > 0))
		{
			if (DEVMGR_HEART_FLAGS != m_devConnMgr->m_devmgrinfomanager->m_current_dev_stat)
			{
				if (!m_check_svr_heartbeat_timer)
				{
					m_timermanger->DeleteTimer((TIMERID)m_check_svr_heartbeat_timer);
					m_check_svr_heartbeat_timer = (void*)0;
				}
				((AcitiveConnect*)(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr))->Close();

				//SockErrorData devsockerro(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr);
				//sr_printf(SR_PRINT_ERROR, "devmgr timeout\n");
				//this->onSockError(&devsockerro);

				m_devConnMgr->onActiveConnSockError((void*)(m_devConnMgr->m_devmgrinfomanager->m_current_devmgr_sockptr));
			}
			else
			{
				// sr_printf(SR_PRINT_DEBUG,"devmgr heartbeat normal\n");
				m_devConnMgr->m_devmgrinfomanager->m_current_dev_stat = 0;
			}
		}
		break;
	}
	}
}

void CDevMgr::DeleteAllRedis()
{
	//if (m_pRedis)
	//	m_pRedis->flushall();
}

void CDevMgr::LoadAllRedisDevice()
{	
	/*m_pRedis->selectdb(3);
	int nDeviceLen = m_pRedis->getdbnum("dev_deviceidandgroupid");
	if (nDeviceLen > 0)
	{
		CDeviceID** ppDeviceID = new CDeviceID*[nDeviceLen];
		for (int j = 0; j < nDeviceLen; j++)
		{
			ppDeviceID[j] = new CDeviceID();
		}
		m_pRedis->selectdb(3);
		int res = m_pRedis->getdbdata("dev_deviceidandgroupid", ppDeviceID);
		if (res == 0)
		{
			for (int k = 0; k < nDeviceLen; k++)
			{
				delete ppDeviceID[k];
			}
			delete[] ppDeviceID;
			return;
		}

		for (int j = 0; j < nDeviceLen; j++)
		{ 
			CDeviceID* pDeviceID = new CDeviceID();
			memcpy(pDeviceID, ppDeviceID[j], sizeof(CDeviceID));
			m_pVecDeviceID.push_back(pDeviceID);
			delete ppDeviceID[j];
		}
		delete[] ppDeviceID;

		m_curTime = time(NULL);
		typedef void (CDevMgr::* ACTION)(void*);
		m_pWriteDBThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
			*this, &CDevMgr::Handle_IndServerHeart, (void*)NULL));
	}*/
	return;
}

void CDevMgr::Handle_IndServerHeart(void *pArg)
{
	/*while (1)
	{
		long long curTime = time(NULL);
		if (curTime - m_curTime >= 30)
		{
			if (m_pVecDeviceID.size() > 0)
			{
				CCriticalAutoLock mylock(m_cs);
				std::vector<CDeviceID*>::iterator it;
				it = m_pVecDeviceID.begin();
				while (it != m_pVecDeviceID.end())
				{
					CDeviceID* pDeviceID = *it;
					DeleteData(pDeviceID->deviceid);
					delete pDeviceID;
					m_pVecDeviceID.erase(it);
					it = m_pVecDeviceID.begin();
				}
				m_pVecDeviceID.clear();
			}
			return;
		}
		//Sleep(1000);
		CBaseThread::Sleep(1000);
	}*/
}

//void CDevMgr::DeleteData(int deviceid)
//{
//	if (m_pMainThread == NULL)
//		return;
//
//	//typedef CBufferT<int> CParam;
//	typedef CBufferT<int, void*, void*, void*, void*, void*> CParam;
//	CParam* pParam;
//	pParam = new CParam(NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, deviceid);
//	typedef void (CDevMgr::* ACTION)(void*);
//	m_pMainThread->Put(CFunctorCommand_1<CDevMgr, ACTION>::CreateInstance(
//		*this, &CDevMgr::Handle_DeleteData, (void*)pParam));
//	return;
//}

//void CDevMgr::Handle_DeleteData(void* pArg)
//{
//	if (pArg == NULL)
//	{
//		assert(0);
//		return;
//	}
//	CAutoTrace::GetInstance()->AutoTrace(g_Trace, m_fullpath, "Handle_DeleteData");
//	//printf("Handle_DeleteData\n");
//
//	//typedef CBufferT<int> CParam;
//	typedef CBufferT<int, void*, void*, void*, void*, void*> CParam;
//	CParam* pParam = (CParam*)pArg;
//
//	do
//	{
//		int deviceid  = (int)(pParam->m_Arg1);
//
//		//�Ƿ�����ҵ����ݿ�ע����Ǹ�����Ȼ��������
//		CDevice::GetInstance()->SetDeviceID(deviceid);
//		CDevice::GetInstance()->SetStatus(0);
//		CDevice::GetInstance()->UpdateDB3();
//
//		//��ȡ��ǰʱ�䴮
//		time_t timeNow;
//		struct tm *ptmNow;
//		char szTime[30];
//		timeNow = time(NULL);
//		ptmNow = localtime(&timeNow);
//		sprintf(szTime, "%d.%02d.%02d %02d:%02d:%02d", ptmNow->tm_year + 1900, ptmNow->tm_mon + 1, ptmNow->tm_mday, ptmNow->tm_hour, ptmNow->tm_min, ptmNow->tm_sec);
//		//�����豸ID����device_conf_detail����ʱ��״̬
//		pDeviceConfDetail->SetDeviceID(deviceid);
//		//pDeviceConfDetail->SetRealuseStoptime(szTime);
//		pDeviceConfDetail->UpdateDB2();
//		//�����豸ID�ҵ�conferenceid����conf_report����ʱ��״̬
//		pConfReport->UpdateDB2();
//		//�����豸ID�ҵ�conferenceid����user_conf_detail����ʱ��״̬
//		CUserConfDetail::GetInstance()->UpdateDB4();
//
//		if (m_pRedis)
//		{
//			char deviceidbuf[128] = { 0 };
//			sprintf(deviceidbuf, "%d", deviceid);
//			CDevice::GetInstance()->SetDeviceID(deviceid);
//			if (CDevice::GetInstance()->SelectDB4())
//			{
//				if (CDevice::GetInstance()->GetDeviceType() == 1)
//				{
//					std::string mediagroup_id = "mc_";
//					if (m_pRedis)
//					{
//						m_pRedis->selectdb(3);
//						char * p = m_pRedis->getvalue("dev_deviceidandgroupid", deviceidbuf);
//						if (p != NULL)
//						{
//							mediagroup_id += p;
//							m_pRedis->selectdb(0);
//							m_pRedis->deletevalue(mediagroup_id.c_str(), deviceidbuf);
//							m_pRedis->selectdb(3);
//							m_pRedis->deletevalue("dev_deviceidandgroupid", deviceidbuf);
//						}
//					}
//				}
//				else if (CDevice::GetInstance()->GetDeviceType() == 2)
//				{
//					std::string netmp_id = "netmp_";
//					if (m_pRedis)
//					{
//						m_pRedis->selectdb(3);
//						char * p = m_pRedis->getvalue("dev_deviceidandgroupid", deviceidbuf);
//						if (p != NULL)
//						{
//							netmp_id += p;
//							m_pRedis->selectdb(1);
//							m_pRedis->deletevalue(netmp_id.c_str(), deviceidbuf);
//							m_pRedis->selectdb(3);
//							m_pRedis->deletevalue("dev_deviceidandgroupid", deviceidbuf);
//						}
//					}
//				}
//				else if (CDevice::GetInstance()->GetDeviceType() == 3)
//				{
//					std::string relaymc_id = "relaymc_";
//					if (m_pRedis)
//					{
//						m_pRedis->selectdb(3);
//						char * p = m_pRedis->getvalue("dev_deviceidandgroupid", deviceidbuf);
//						if (p != NULL)
//						{
//							relaymc_id += p;
//							m_pRedis->selectdb(4);
//							m_pRedis->deletevalue(relaymc_id.c_str(), deviceidbuf);
//							m_pRedis->selectdb(3);
//							m_pRedis->deletevalue("dev_deviceidandgroupid", deviceidbuf);
//						}
//					}
//				}
//				else if (CDevice::GetInstance()->GetDeviceType() == 4)
//				{
//					std::string relayserver_id = "relayserver_";
//					if (m_pRedis)
//					{
//						m_pRedis->selectdb(3);
//						char * p = m_pRedis->getvalue("dev_deviceidandgroupid", deviceidbuf);
//						if (p != NULL)
//						{
//							relayserver_id += p;
//							m_pRedis->selectdb(5);
//							m_pRedis->deletevalue(relayserver_id.c_str(), deviceidbuf);
//							m_pRedis->selectdb(3);
//							m_pRedis->deletevalue("dev_deviceidandgroupid", deviceidbuf);
//						}
//					}
//				}
//
//				if (m_pRedis)
//				{
//					m_pRedis->selectdb(2);
//					m_pRedis->deletehashvalue("deviceid_ipport", deviceidbuf);
//
//					//���db2  conference
//					char deviceid_id[256] = { 0 };
//					sprintf(deviceid_id, "deviceid_%d", deviceid);
//
//					//m_pRedis->selectdb(6);
//					int num = m_pRedis->gethashfiledsnum(deviceid_id);
//
//					CHKeys** conf = NULL;
//					conf = new CHKeys*[num];
//					for (int j = 0; j < num; j++)
//					{
//						conf[j] = new CHKeys();
//					}
//					//m_pRedis->selectdb(6);
//					int nlen = m_pRedis->gethashdbdata(deviceid_id, conf, num);
//
//					for (int k = 0; k < nlen; k++)
//					{
//						m_pRedis->selectdb(2);
//						m_pRedis->deletehashvalue("conference_deviceid", conf[k]->filed);
//					}
//					for (int k = 0; k < num; k++)
//					{
//						delete conf[k];
//					}
//					delete[] conf;
//
//					//���db6  deviceid
//					//m_pRedis->selectdb(6);
//					m_pRedis->deletevalue(deviceid_id);
//				}
//			}
//		}
//	} while (0);
//
//	delete pParam;
//	pParam = NULL;
//}