
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdlib.h>

#include "sr_common.h"

//����ļ���С
int GetFileLength(const char *filename)
{
	int len = 0;

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)

#elif defined(LINUX) || defined(POSIX)
	int fh = open(filename, O_RDONLY);
	if (fh != -1)
	{
		struct stat filestat;
		if (!fstat(fh, &filestat))
			len = filestat.st_size;
		else
			printf("Stat file [%s] fail: %s.\n", filename, strerror(errno));
		close(fh);
	}
	else
		printf("Open file [%s] fail: %s.\n", filename, strerror(errno));
#endif

	return len;
}

int CompareTwoDateTime(const char *first, const char * second)
{
	// ����ʹ��mysql plus plus�е�DateTime�ṹ��
	//DateTime _first(first);
	//DateTime _second(second);
	//int ret = _first.compare(_second);

	//%Y-%m-%d %H:%M:%S��ʽ��ʱ���ַ����Ƚ�

	int ret = strncmp(first, second, 19);

	printf("CompareTwoDateTime:first=%s\tsecond=%s\tret=%d\n",first,second,ret);
	return ret;
}

EDateCompareResult CompareCurrentEndTime(const char *other)
{
	char curtimebuf[128];
	memset(curtimebuf, '\0', sizeof(curtimebuf));
	time_t ltime;
	time(&ltime);
	struct tm *now = localtime(&ltime);
	strftime(curtimebuf, 128, "%Y-%m-%d %H:%M:%S", now);

	int iResult = CompareTwoDateTime(curtimebuf, other);
	if (iResult >= 0)
	{
		return Current_Is_Same_Or_Later;
	}
	else
	{
		return Current_Is_Early;
	}
}

EDateCompareResult CompareCurrentStartTime(const char *other)
{
	char curtimebuf[128];
	memset(curtimebuf, '\0', sizeof(curtimebuf));
	time_t ltime;
	time(&ltime);
	struct tm *now = localtime(&ltime);
	strftime(curtimebuf, 128, "%Y-%m-%d %H:%M:%S", now);

	int iResult = CompareTwoDateTime(other, curtimebuf);
	if (iResult > 0)
	{
		return Current_Is_Same_Or_Later;
	}
	else
	{
		return Current_Is_Early;
	}
}
//���շֽ���Ϳո��Դ�ַ�����ֳ�һ��map
//<param name = "source">Դ�ַ���    IN</param>
//<param name = "sep1">�ֽ��ַ�      IN</param>
//<param name = "sep2">��ֵ�Եķָ�� IN</param>
//<param name = "ms">��ֺ�map       OUT</param>
void parsePair(map_string &ms, const char *source, const char *sep1, const char *sep2)
{
	int index;
	string str;
	ms.clear();
	char tstr[3000];
	memset(tstr, '\0', sizeof(tstr));
	strncpy(tstr, source, 2999);
	char *token = strtok(tstr, sep1);
	while (token != NULL)
	{
		str = token;
		index = str.find(sep2);
		if (index > 0)
		{
			ms.insert(pair_string(str.substr(0, index), str.substr(index + 1, str.length())));
		}
		token = strtok(NULL, sep1);
	}
}

//��map�в��Ҽ���Ӧ��ֵ�����Ϊ�ַ����飬δ�ҵ�����false
bool getOnePair(map_string &ms, const char * key, char * value)
{
	iterator_string it = ms.find(key);
	if (it != ms.end())
	{
		strcpy(value, ((*it).second).c_str());
		return true;
	}
	return false;
}

//��map�в��Ҽ���Ӧ��ֵ�����Ϊ�ַ�����δ�ҵ�����false
bool getOnePair(map_string &ms, const char * key, string &value)
{
	iterator_string it = ms.find(key);
	if (it != ms.end())
	{
		value = (*it).second;
		return true;
	}
	return false;
}


/****************************************************************************
��������: str_to_hex
��������: �ַ���ת��Ϊʮ������
�������: string �ַ��� cbuf ʮ������ len �ַ����ĳ��ȡ�
�������: ��
*****************************************************************************/
int  strtohex(const char *buf_in, int in_len, unsigned char *buf_out, int &out_len)
{
	unsigned char high;
	unsigned char low;
	int idx, ii = 0;

	for (idx = 0; idx < in_len; idx += 2)
	{
		high = buf_in[idx];
		low = buf_in[idx + 1];

		if (high >= '0' && high <= '9')
			high = high - '0';
		else if (high >= 'A' && high <= 'F')
			high = high - 'A' + 10;
		else if (high >= 'a' && high <= 'f')
			high = high - 'a' + 10;
		else
			return -1;

		if (low >= '0' && low <= '9')
			low = low - '0';
		else if (low >= 'A' && low <= 'F')
			low = low - 'A' + 10;
		else if (low >= 'a' && low <= 'f')
			low = low - 'a' + 10;
		else
			return -1;

		buf_out[ii++] = high << 4 | low;
	}
	out_len = ii;
	return ii;
}

/****************************************************************************
��������: hex_to_str
��������: ʮ������ת�ַ���
�������: ptr �ַ��� buf ʮ������ len ʮ�������ַ����ĳ��ȡ�
�������: ��
*****************************************************************************/
int  hextostr(const unsigned char *buf_in, int in_len, char *buf_out, int &out_len)
{
	out_len = 0;
	for (int i = 0; i < in_len; i++)
	{
		sprintf(buf_out, "%02x", buf_in[i]);
		buf_out += 2;
		out_len = out_len + 2;
	}

	return 0;
}