/*文件名：hardwareInfo.cpp
* 作者：
*〈版权〉
* 描述：
* 修改人：封英杰
* 修改时间：2019-04-02
* 修改内容：由以前设备网卡mac地址作为授权文件加解密的秘钥key，变成由网卡mac+cpuid+硬盘序列号作为授权文件加解密的秘钥key
*/ 
// hardwareInfo.cpp : Defines getting network info.
//

#include "MD5.h"
#include "hardwareInfo.h"

#ifdef LINUX
//通过socket获得mac地址
/*这种获取方式必须要配置IP地址，并且网卡启动*/
void getMacListBySocket(unsigned char *mac, int& len)
{
	//len = 0;
	//int sockfd = -1, if_cnt = 0, if_idx = 0;
	//struct ifreq if_buf[MAX_NET_CARDS];
	//struct ifconf ifc;


	//if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	//{
	//    printf("Get MAC info, Create socket error!\n");
	//    return;
	//}
	//ifc.ifc_len = sizeof(if_buf);
	//ifc.ifc_buf = (char*)if_buf;

	//if(!ioctl(sockfd, SIOCGIFCONF, (char*)&ifc))
	//{
	//    if_cnt = (ifc.ifc_len)/(sizeof(struct ifreq));
	//    for(if_idx = 0;if_idx < if_cnt; ++if_idx)
	//    {
	//        //ignore local loopback interface
	//        if(strcmp(LOOPBACK_IF_NAME,if_buf[if_idx].ifr_name) == 0 )
	//            //||  if_buf[if_idx].)
	//        {
	//            if(if_cnt == 1)
	//                printf("Please set IP address!\n");
	//            continue;
	//        }
	//        //get mac
	//        if(!(ioctl(sockfd,SIOCGIFHWADDR,(char*)(&if_buf[if_idx]))))
	//        {
	//            memcpy(mac,(unsigned char*)(&(if_buf[if_idx].ifr_hwaddr.sa_data)),MAC_ADDR_LEN);
	//            mac += MAC_ADDR_LEN;
	//            len += MAC_ADDR_LEN;
	//        }
	//        else
	//            printf("Get MAC info, Get hardware address fail: %s.\n",strerror(errno));
	//    }
	//}
	//else
	//    printf("Get MAC info, get iface list fail: %s.\n",strerror(errno));

	//if(sockfd != -1)
	//    close(sockfd);
	//return;
}
//分析buffer数据，通过标识得到mac地址
void getMacListFromBuffer(char* buffer, const char* label, unsigned char *mac, int& len)
{
	//char maclabel[20];
	//len = 0;
	//unsigned int hex = 0;
	//memset(maclabel,'\0',sizeof(maclabel));
	//strcpy(maclabel,label);
	//char* chPos = strstr(buffer,(char*)maclabel);
	//if(chPos == NULL && strcmp(maclabel,"HWaddr") == 0)
	//{
	//    memset(maclabel,'\0',sizeof(maclabel));
	//    strcpy(maclabel,"硬件地址");
	//    chPos = strstr(buffer,(char*)maclabel);
	//}
	//while(chPos > 0)
	//{
	//    chPos += strlen(maclabel) + 1;
	//    len += MAC_ADDR_LEN;
	//    for(int i=0;i<MAC_ADDR_LEN;i++)
	//    {
	//        sscanf(chPos+i*3, "%X", &hex);
	//        mac[0] = (char)hex;
	//        mac+=1;
	//    }
	//    chPos = strstr(chPos+MAC_ADDR_LEN*3,maclabel);
	//}
}
//使用正则表达式匹配
void getMacListFromBuffer(char* buffer, unsigned char *mac, int& len)
{
	len = 0;
	unsigned int hex = 0;
	regex_t preg;
	char errbuf[256];
	char regex[] = "([[:xdigit:]][[:xdigit:]]:){5}[[:xdigit:]][[:xdigit:]]";//"[[:xdigit:]][[:xdigit:]]:[[:xdigit:]][[:xdigit:]]:[[:xdigit:]][[:xdigit:]]:[[:xdigit:]][[:xdigit:]]:[[:xdigit:]][[:xdigit:]]:[[:xdigit:]][[:xdigit:]]";
	bzero(&preg,sizeof(regex_t));
	int errcode = regcomp(&preg,regex,REG_EXTENDED);
	if(errcode != 0)
	{
	    regerror(errcode,&preg,errbuf,sizeof(errbuf));
	    printf("regerror %s\n",errbuf);
	}
	regmatch_t pmatch[MAX_NET_CARDS];
	char* regBegin = buffer;
	errcode = regexec(&preg, regBegin, MAX_NET_CARDS, pmatch, 0);
	if(errcode != 0)
	{
	    regerror(errcode,&preg,errbuf,sizeof(errbuf));
	    printf("regerror %s\n",errbuf);
	}
	while(errcode == 0)
	{
	    char* macBegin = NULL;
	    if(pmatch[0].rm_so > -1)
	    {
	        macBegin = regBegin + pmatch[0].rm_so;
	        regBegin = regBegin + pmatch[0].rm_eo;
	        len += MAC_ADDR_LEN;
	        for(int j=0;j<MAC_ADDR_LEN;j++)
	        {
	            sscanf(macBegin+j*3, "%X", &hex);
	            mac[0] = (char)hex;
	            mac+=1;
	        }
	    }
	    else if(len == 0)
	        break;
	    errcode = regexec(&preg, regBegin, MAX_NET_CARDS, pmatch, 0);

	}
	regfree(&preg);
}
//通过ifconfig获得mac地址 －a显示所有信息
void getMacListByIfConfig(unsigned char *mac, int& len)
{
	FILE *readfp;
	char buffer[MAX_NET_CARDS*1024];
	int readSize;
	memset(buffer,'\0',sizeof(buffer));
	readfp = popen("ip a | grep ether | awk '{print $2}'","r");
	if(readfp != NULL)
	{
	    readSize = fread(buffer,sizeof(unsigned char),sizeof(buffer),readfp);
	    if(readSize > 0)
	    {
	        //getMacListFromBuffer(buffer,"HWaddr",mac,len);
	        getMacListFromBuffer(buffer,mac,len);
	    }
	    pclose(readfp);
	}
}
//通过读取网卡信息文件获得mac地址，网卡文件不能随意改动
void getMacListByNetFile(unsigned char *mac, int& len)
{
	//#define NETADAPTERFILE "/etc/udev/rules.d/70-persistent-net.rules"
	//        tmpfs(DIR_RULES, 0);
	//
	//    int fileLen = GetFileLength(NETADAPTERFILE);
	//    char* fileBuf = (char*)malloc(fileLen+1);
	//    FILE* fd = fopen(NETADAPTERFILE,"r");
	//    if(fd == NULL)
	//    {
	//         printf("Open file fail in getMacListByNetFile: %s.\n",strerror(errno));
	//         free(fileBuf);
	//         tmpfs(DIR_RULES, 1);
	//         return;
	//    }
	//    int readSize = fread(fileBuf,sizeof(unsigned char),fileLen,fd);
	//    if(readSize != fileLen)
	//    {
	//        printf("Read file fail in getMacListByNetFile: %s.\n",strerror(errno));
	//        free(fileBuf);
	//        tmpfs(DIR_RULES, 1);
	//        return;
	//    }
	//    //getMacListFromBuffer(fileBuf,"ATTR{address}==",mac,len);
	//    getMacListFromBuffer(fileBuf,mac,len);
	//    free(fileBuf);
	//    tmpfs(DIR_RULES, 1);
}
//获取Mac地址，在linux支持三种方式，目前使用ifconfig命令
void getMacList(unsigned char *mac, int& len)
{
	getMacListByIfConfig(mac, len);

	//getMacListByNetFile(mac,len);
	//getMacListBySocket(mac,len);
}
//通过系统硬件信息mac地址获得加密key
//<param name = "cSystemId">密钥      OUT</param>
//<param name = "printInfo">终端输出信息，给telnet用  OUT</param>
void getSystemId(char *cSystemId, char * printInfo/* = NULL*/)
{
	char idStr[256];
	memset(idStr, 0, 256);

	//unsigned char CpuNo[8];
	//memset(CpuNo, 0, 8);
	//unsigned int nCpuSerialNoHi, nCpuSerialNoLo;
	//nCpuSerialNoLo = 0;
	//nCpuSerialNoHi = 0;
	//memcpy(CpuNo, &nCpuSerialNoLo, 4);
	//memcpy(CpuNo, &nCpuSerialNoHi, 4);
	int i;
	//for (i = 0; i < 8; i++)
	//{
	//	sprintf(&idStr[i * 2], "%02X", CpuNo[i]);
	//}

	unsigned char macAddr[MAX_NET_CARDS * MAC_ADDR_LEN];
	memset(macAddr, 0, MAX_NET_CARDS * MAC_ADDR_LEN);
	int len;
	getMacList(macAddr, len);
	printf("len = %d\n", len);

	char macStr[MAX_NET_CARDS * MAC_ADDR_LEN * 2];
	memset(macStr, 0, MAX_NET_CARDS * MAC_ADDR_LEN * 2);

	for (i = 0; i < len; i++)
	{
		sprintf(&macStr[i * 2], "%02X", macAddr[i]);
	}

	char tmp[MAX_NET_CARDS][MAC_ADDR_LEN * 2];
	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, macStr, strlen(macStr));

	int syslen = strlen(macStr);
	int maclen_hex = MAC_ADDR_LEN * 2;
	int line = syslen / maclen_hex;

	int j, k;
	for (i = 0; i < line; i++)
	{
		for (j = i + 1; j < line; j++)
		{
			for (k = 0; k < maclen_hex; k++)
			{
				if (tmp[i][k] != tmp[j][k])
				{
					if (tmp[i][k] > tmp[j][k])
					{
						char swap[MAC_ADDR_LEN * 2];
						memset(swap, 0, MAC_ADDR_LEN * 2);
						memcpy(swap, tmp[i], sizeof(swap));
						memcpy(tmp[i], tmp[j], sizeof(swap));
						memcpy(tmp[j], swap, sizeof(swap));
					}
					break;
				}
			}
		}

	}

	//strncat(&idStr[16], (const char *)tmp, line * 12);

	strncat(idStr, (const char *)tmp, line * 12);

	printf("idStr = %s\n", idStr);


	if (printInfo != NULL)
	{
		strcpy(printInfo, "In getSystemId Before MD5 SystemId is:");
		strcat(printInfo, idStr);
	}


	MD5_CTX context;
	unsigned char digest[17];
	char temp[3];
	int length = strlen(idStr);

	MD5Init(&context);
	MD5Update(&context, (unsigned char *)idStr, length);
	MD5Final((unsigned char *)&digest, &context);

	if (printInfo != NULL)
	{
		strcat(printInfo, ".After MD5 digest is:");
	}
	memset(idStr, 0, 256);
	for (int j = 0; j < 8; j++)
	{
		if (printInfo != NULL)
		{
			char cTemp[40];
			memset(cTemp, '\0', sizeof(cTemp));
			sprintf(cTemp, "digest[%d]=%02x, digest[%d]=%02x ", j, digest[j], j + 8, digest[j + 8]);
			strcat(printInfo, cTemp);
		}
		digest[j] = digest[j] ^ digest[j + 8];
	}
	for (int j = 0; j < 8; j++)
	{
		sprintf(temp, "%02x", digest[j]);
		strcat(cSystemId, temp);

	}

	printf("cSystemId : %s\n", cSystemId);

	if (printInfo != NULL)
	{
		strcat(printInfo, ".In getSystemId the SystemId is:");
		strcat(printInfo, cSystemId);
	}
}
int GetCpuCores()
{
	int nprocessor = 0;
	//nprocessor= sysconf(_SC_NUMPROCESSORS_CONF);

	FILE* readfp;
	char buffer[1024] = { 0 };
	int readSize;

	readfp = popen("grep processor /proc/cpuinfo | sort | uniq | wc -l", "r");
	if (readfp != NULL)
	{
		readSize = fread(buffer, sizeof(unsigned char), sizeof(buffer), readfp);
		if (readSize > 0)
		{
			sscanf(buffer, "%d", &nprocessor);
		}
		pclose(readfp);
	}
	return nprocessor;
}

int GetCpuInfo(char* info)
{
	char cBrand[49]; // 用来存储商标字符串，48个字符
	memset(cBrand, 0, 49);
	FILE* readfp;
	char buffer[1024];
	int readSize;

	readfp = popen("grep \"model name\" /proc/cpuinfo | uniq", "r");
	if (readfp != NULL)
	{
		readSize = fread(buffer, sizeof(unsigned char), sizeof(buffer), readfp);
		if (readSize > 0)
		{
			char* chPos = strstr(buffer, ":");
			char* chEndPos = strstr(buffer, "\n");
			if (chEndPos > chPos)
				strncpy(cBrand, chPos + 2, chEndPos - chPos - 2);
			else
				strcpy(cBrand, chPos + 2);
		}
		pclose(readfp);
	}

	strcpy(info, cBrand);
	return 0;
}
/*
*function:通过asm获取CPUID;
*para: CpuId : cpu的ID序列号，len: 序列号的长度;
*return: true : 获取成功 false:获取失败;
*/
//bool GetCpuIdByAsm(unsigned char *CpuId, int& len)
//{
//	int s1 = 0;
//	int s2 = 0;
//	asm volatile
//	(
//		"movl $0x01, %%eax; \n\t"
//		"xorl %%edx, %%edx; \n\t"
//		"cpuid; \n\t"
//		"movl %%edx, %0; \n\t"
//		"movl %%eax, %1; \n\t"
//		: "=m"(s1), "=m"(s2)
//	);
//	if (s1 == 0 && s2 == 0)
//	{
//		return false;
//	}
//	char Cpu[32] = {0};
//	snprintf(Cpu,sizeof(Cpu),"%08X%08X",htonl(s2),htonl(s1));
//	memcpy(CpuId,Cpu,sizeof(Cpu));
//	len = strlen((char*)CpuId);
//	return true;
//}

/*
*function:通过system获取CPUID;
*para: CpuId : cpu的ID序列号，len: 序列号的长度;
*return: true : 获取成功 false:获取失败;
*/

////解析获取cpuid
//int trimSpace(char *inbuf, char *outbuf)
//{
//	char *in = inbuf;
//	char *out = outbuf;
//	int ret = 0;
//	int inLen = strlen(in);
//	if (!inbuf || !outbuf)
//	{
//		ret = -1;
//		/*printf("func trimSpace(char *inbuf, char *outbuf) err: ret = %d->function parameter is null.\n", ret);*/
//		return ret;
//	}
//	else
//	{
//		ret = 1;
//		for (int i = 3; i < inLen; i++)
//		{
//			if (in[i] != ' ')
//			{
//				*out++ = in[i];
//			}
//		}
//	}
//	return ret;
//}
//static void ParseCpuId(const char * FileName, const char * MatchWords, unsigned char * CpuId)
//{
//	//CpuId->c_str();
//	std::ifstream ifs(FileName, std::ios::binary);
//	if (!ifs.is_open())
//	{
//		return;
//	}
//	char line[4096] = { 0 };
//	while (!ifs.eof())
//	{
//		ifs.getline(line, sizeof(line));
//		if (!ifs.good())
//		{
//			break;
//		}
//		char * cpu = strstr(line, MatchWords);
//		char *outbuf = (char *)malloc(CPUID_BUF);
//		if (trimSpace(cpu, outbuf))
//		{
//			/*printf("cpu3 is %s\n",outbuf);*/
//		}
//		if (outbuf == NULL)
//		{
//			continue;
//		}
//		cpu += strlen(MatchWords);
//		memcpy(CpuId, outbuf, CPUID_BUF);
//		if (CpuId != NULL)
//		{
//			break;
//		}
//	}
//	ifs.close();
//}

//bool GetCpuIdBySystem(unsigned char * CpuId, int& len)
//{
//	///CpuId->c_str();
//	const char * DmidecodeResult = ".dmidecode_result.txt";
//	char command[521] = {0};
//	snprintf(command, sizeof(command), "dmidecode -t 4 | grep ID | uniq -c > %s", DmidecodeResult);
//	if(system(command) == 0)
//	{
//		ParseCpuId(DmidecodeResult,"ID:",CpuId);
//	}
//	unlink(DmidecodeResult);
//	len = strlen((char*)CpuId);
//	if (CpuId != NULL)
//	{
//		return true;
//	}
//	return false;
//}
bool getVMCpuId(unsigned char * CpuId, int& len)
{
	unsigned char ArrRandNum[CPUID_BUF] = {0};
	FILE *fp;
	if((fp=fopen("/var/log/.vmcpuid","r"))!=NULL)
	{
		if (fread(ArrRandNum, sizeof(unsigned char),CPUID_BUF, fp) == 0)
		{
			fclose(fp);
			return false;	
		}
	}else
	{
		return false;
	}
	memcpy(CpuId, ArrRandNum, CPUID_BUF);
	len = strlen((char *)CpuId);
	fclose(fp);
	return true;
}
/*
*function:获取CPUID;
*para: CpuId : cpu的ID序列号，len: 序列号的长度;
*return: true : 获取成功 false:获取失败;
*/
bool GetCpuId(unsigned char * CpuId, int& len )
{
	if(getVMCpuId(CpuId,len))
	{
		return true;
	}
	else
	{
		return false;
	}
	//if(getuid() == 0)
	//{
	//	if (GetCpuIdBySystem(CpuId,len))
	//	{
	//		return true;
	//	}
	//}
	//return false;
}

#endif	

/*
int main(int argc, char* argv[])
{
    unsigned char cSystemId[0x40];
    int len;
    memset(cSystemId,0,0x40);
    getSystemId(cSystemId,len);
    char idStr[0x80];
    memset(idStr,0,0x80);
    for (int i=0;i<len;i++)
    {
        sprintf(&idStr[i*2],"%02X",cSystemId[i]);
    }
    printf("%s\n",idStr);
    return 0;
}
*/
