#ifndef _SR_COMMON_H_
#define _SR_COMMON_H_

#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>
#include <string>
#include <stdint.h>
#include <time.h>
using namespace std;

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)

#elif defined(LINUX) || defined(POSIX)
#include <iconv.h>
#include <regex.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#endif

typedef map<string, string> map_string;
typedef pair<string, string> pair_string;
typedef map_string::iterator iterator_string;

const static unsigned char global_encrypt_key_[] = "sr674chensonghua";
const static unsigned char global_encrypt_key_hex_[16] = { 0x73, 0x72, 0x36, 0x37, 0x34, 0x63, 0x68, 0x65, 0x6e, 0x73, 0x6f, 0x6e, 0x67, 0x68, 0x75, 0x61 };

typedef enum _EDateCompareResult
{
	Current_Is_Same_Or_Later = 1,
	Current_Is_Early = 0
}EDateCompareResult;

//获得文件大小
int GetFileLength(const char *filename);

int	CompareTwoDateTime(const char *first, const char * second);

EDateCompareResult CompareCurrentEndTime(const char *other);
EDateCompareResult CompareCurrentStartTime(const char *other);

//按照分界符和空格把源字符串拆分成一个map
//<param name = "source">源字符串    IN</param>
//<param name = "sep1">分界字符      IN</param>
//<param name = "sep2">键值对的分割符 IN</param>
//<param name = "ms">拆分后map       OUT</param>
void parsePair(map_string &ms, const char *source, const char *sep1, const char *sep2);

//从map中查找键对应的值，未找到返回false
bool getOnePair(map_string &ms, const char * key, char * value);
bool getOnePair(map_string &ms, const char * key, string &value);

int strtohex(const char *buf_in, int in_len, unsigned char *buf_out, int &out_len);
int hextostr(const unsigned char *buf_in, int in_len, char *buf_out, int &out_len);

#endif