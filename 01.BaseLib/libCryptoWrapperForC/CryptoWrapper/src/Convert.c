/****************************************************************************
函数名称: str_to_hex
函数功能: 字符串转换为十六进制
输入参数: string 字符串 cbuf 十六进制 len 字符串的长度。
输出参数: 无
*****************************************************************************/
#include <stdio.h>
#include "Convert.h"

int  strtohex(const char *buf_in, int in_len, unsigned char *buf_out, int *out_len)
{
	unsigned char high;
	unsigned char low;
	int idx = 0, ii=0;

	for (idx=0; idx< in_len; idx += 2)
	{
		high = buf_in[idx];
		low  = buf_in[idx+1];

		if(high>='0' && high<='9')
			high = high-'0';
		else if(high>='A' && high<='F')
			high = high - 'A' + 10;
		else if(high>='a' && high<='f')
			high = high - 'a' + 10;
		else
			return -1;

		if(low>='0' && low<='9')
			low = low-'0';
		else if(low>='A' && low<='F')
			low = low - 'A' + 10;
		else if(low>='a' && low<='f')
			low = low - 'a' + 10;
		else
			return -1;

		buf_out[ii++] = high<<4 | low;
	}
	*out_len = ii;
	return ii;
}

/****************************************************************************
函数名称: hex_to_str
函数功能: 十六进制转字符串
输入参数: ptr 字符串 buf 十六进制 len 十六进制字符串的长度。
输出参数: 无
*****************************************************************************/
int  hextostr(const unsigned char *buf_in,int in_len, char *buf_out, int *out_len)
{
	*out_len = 0;
	int  i = 0;
	for(i = 0; i < in_len; i++)
	{
		sprintf(buf_out, "%02x",buf_in[i]);
		buf_out += 2;
		*out_len = *out_len + 2;
	}
	
	return 0;
}
