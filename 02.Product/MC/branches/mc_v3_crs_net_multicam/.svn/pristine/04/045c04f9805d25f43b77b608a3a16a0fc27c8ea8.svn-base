#ifndef _SR_DECRYPT_H_
#define _SR_DECRYPT_H_

#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>
#include <string>
#include <stdint.h>
#include <time.h>
using namespace std;

//#define TAG_CRYPT			"suirui"				//加密解密标志
//#define TAG_CRYPT_LEN		6						//加密解密标志长度
//#define PWD_1				"0348045839473459"		//密码一
//#define PWD_2				"0a57879435980909"		//密码二

#define TAG_CRYPT			"huijian"				//加密解密标志
#define TAG_CRYPT_LEN		7						//加密解密标志长度
#define PWD_1				"471cf5e381de062d"		//密码一,对应suirui的MD5加密16位小写
#define PWD_2				"1c526f6898b01f97"		//密码二,对应huijian的MD5加密16位小写

#define FILE_LIC_NAME       "McLicence.ini"			//licence文件

#define D_EXPIRINGDATE		            "EXPIRINGDATE"
#define D_MAXPORT			            "MAX_PORT"
#define D_SR_COUNT		                "SR_COUNT"
#define D_STANDARD_COUNT	            "STANDARD_COUNT"
#define D_STD_EXPIRINGDATE				"STD_EXPIRINGDATE"
#define D_RECORD_COUNT		            "RECORD_COUNT"
#define D_REC_EXPIRINGDATE				"REC_EXPIRINGDATE"
#define D_CONFERENCE_COUNT			    "CONFERENCE_COUNT"
#define D_CONFERENCE_MAX_COUNT			"MAX_TERPERCONF_COUNT"
#define D_MAX_LIVE_COUNT			    "MAX_LIVE_COUNT"
#define D_LIVE_EXPIRINGDATE				"LIVE_EXPIRINGDATE"
#define D_MAX_MONITOR_COUNT				"MAX_MONITOR_COUNT"
#define D_MONITOR_EXPIRINGDATE          "MONITOR_EXPIRINGDATE"
#define D_MAX_VOICE_COUNT				"MAX_VOICE_COUNT"
#define D_VOICE_EXPIRINGDATE			"VOICE_EXPIRINGDATE"

//解密文件
bool DecryptFile(const char * filename, const char *pwd1, const char *pwd2);
//解密文件到内存
bool DecryptStream(const char * filename, const char *pwd1, const char *pwd2, char *result, char * printInfo = NULL);

#endif