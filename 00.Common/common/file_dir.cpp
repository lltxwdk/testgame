#include "file_dir.h"

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

#include <string.h>
#include <stdio.h>

//char *sr_stringcpy(char * src, const char * dst)
//{
	//char * p = strcpy(src, dst);
	//return p;
//}
//size_t sr_stringlen(const char * src)
//{
	//return strlen(src);
//}

//void* sr_memvoidcpy(void *dst, const void* src, size_t Size)
//{
	//void * p = memcpy(dst, src, Size);
	//return  p;
//}

/************************************************************************/
/*                        创建目录                                       */
/************************************************************************/

int SRCreatDir(const char *pDir)
{
	if (NULL == pDir)
	{
		return 0;
	}

	int iRet = 0;
	char* pszDir = NULL;

	int iLen = strlen(pDir);

	//分配空间
	pszDir = new char[iLen + 2];
	memset(pszDir, 0, (iLen + 2) * sizeof(char));
	memcpy(pszDir, pDir, iLen * sizeof(char));


	//1. 先格式化格式
#if defined(_WIN32)
	for (int i = 0; i < iLen; i++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{
			pszDir[i] = '\\';
		}
	}
#endif

	//在末尾加/
	if (pszDir[iLen - 1] != '\\' && pszDir[iLen - 1] != '/')
	{
#if defined(_WIN32)
		pszDir[iLen] = '\\';
#else
		pszDir[iLen] = '/';
#endif
		pszDir[iLen + 1] = '\0';
	}



	// 创建中间目录  
	for (int i = 1; i <= iLen + 1; i++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{
			pszDir[i] = '\0';
			//如果不存在,创建  

			if(ACCESS(pszDir, 0) == -1) //不存在
			{
				iRet = MKDIR(pszDir);
				if (iRet == -1) //成功返回0 失败返回-1
				{
					printf("Create Dir Error....directory already exists or not\n");
					//return -1;
				}
			}
			//支持linux,将所有\换成/  
#if defined(_WIN32)
			pszDir[i] = '\\';
#else
			pszDir[i] = '/';
#endif
		}
	}


	delete[] pszDir;
	return iRet;
}



/*
int main(int argc, char * argv[])
{
std::string m_savepath = "d:/dmp/ddd";

CreatDir2(m_savepath.c_str());

getchar();

return 0;
}

*/
