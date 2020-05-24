
#ifndef _HARDWAREINFO_H_
#define _HARDWAREINFO_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stack>
#include <string>
#include <fstream>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>    
#include <fcntl.h>
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)

#elif defined(LINUX) || defined(POSIX)
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <linux/hdreg.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <regex.h>
#endif


typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;

#define MCKEYLEN 44 
#define MAX_NET_CARDS 20
#define CPUID_BUF 16
#define LOOPBACK_IF_NAME "lo"
#define MAC_ADDR_LEN 6
#define PWD_3 "b2f54feba19501e9" //密码三,对应suiruihuijian的MD5加密16位小写
#define PWD_3_CONST 16

#ifdef LINUX
//通过socket获得mac地址
/*这种获取方式必须要配置IP地址，并且网卡启动*/
void getMacListBySocket(unsigned char *mac, int& len);

//分析buffer数据，通过标识得到mac地址
void getMacListFromBuffer(char* buffer, const char* label, unsigned char *mac, int& len);

//使用正则表达式匹配
void getMacListFromBuffer(char* buffer, unsigned char *mac, int& len);

//通过ifconfig获得mac地址 －a显示所有信息
void getMacListByIfConfig(unsigned char *mac, int& len);

//通过读取网卡信息文件获得mac地址，网卡文件不能随意改动
void getMacListByNetFile(unsigned char *mac, int& len);

//获取Mac地址，在linux支持三种方式，目前使用ifconfig命令
void getMacList(unsigned char *mac, int& len);

//通过系统硬件信息mac地址获得加密key
//<param name = "cSystemId">密钥      OUT</param>
//<param name = "printInfo">终端输出信息，给telnet用  OUT</param>
void getSystemId(char *cSystemId, char * printInfo/* = NULL*/);

//获得CPU核数
int GetCpuCores();
//获得CPU信息
int GetCpuInfo(char* info);

//通过 asm 获取cpuId
//bool GetCpuIdByAsm(unsigned char *CpuId, int& len);
////从系统 system 中获取 cpuid
//bool GetCpuIdBySystem(unsigned char * CpuId, int& len);
//获取cpuid
bool getVMCpuId(unsigned char * CpuId, int& len);
//获取cpuid
bool GetCpuId(unsigned char * CpuId, int& len);
#endif	

#endif
