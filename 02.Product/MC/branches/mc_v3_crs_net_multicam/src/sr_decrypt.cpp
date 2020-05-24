#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)

#elif defined(LINUX) || defined(POSIX)
#include <iconv.h>
#include <regex.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#endif

#include "sr_common.h"
#include "sr_decrypt.h"

//½âÃÜLicenseÎÄ¼þ
bool DecryptFile(const char * filename, const char *pwd1, const char *pwd2)
{
	int i;
	int file_len;
	int temp_len;
	FILE * m_file;
	int CRYPT_BUFFER_SIZE;

	int pwd1_len = strlen(pwd1);
	int pwd2_len = strlen(pwd2);
	temp_len = 2 * sizeof(int) + TAG_CRYPT_LEN + pwd2_len;
	file_len = GetFileLength(filename);
	if (file_len <= 0)
	{
		return false;
	}
	CRYPT_BUFFER_SIZE = file_len + temp_len;
	char *buffer = (char*)malloc(CRYPT_BUFFER_SIZE);
	char *buffer_temp = (char*)malloc(100);

	m_file = fopen(filename, "r+b");
	if (m_file == NULL)
	{
		free(buffer);
		free(buffer_temp);
		return false;
	}

	file_len = fread(buffer, sizeof(char), CRYPT_BUFFER_SIZE, m_file);

	for (i = 0; i < TAG_CRYPT_LEN; i++)
	{
		buffer_temp[i] = buffer[i] ^ pwd1[i%pwd1_len];
	}
	if (strncmp(buffer_temp, TAG_CRYPT, TAG_CRYPT_LEN) != 0)
	{
		fclose(m_file);
		free(buffer);
		free(buffer_temp);
		return false;
	}

	for (i = 0; i < file_len; i++)
	{
		buffer[i] ^= pwd1[i%pwd1_len];
	}

	int *data = (int *)&buffer[TAG_CRYPT_LEN];
	int f_len = data[0];
	int p_len = data[1];
	if ((p_len != pwd2_len) || (f_len + temp_len != file_len) || (strncmp((char *)&buffer[temp_len - pwd2_len], pwd2, pwd2_len) != 0))
	{
		fclose(m_file);
		free(buffer);
		free(buffer_temp);
		return false;
	}

	for (i = 0; i < temp_len; i++)
	{
		buffer[i + f_len] ^= pwd2[i%pwd2_len];
	}
	memcpy(buffer_temp, buffer + f_len, temp_len);
	memcpy(buffer, buffer_temp, temp_len);
	fclose(m_file);

	m_file = fopen(filename, "w+b");
	if (m_file == NULL)
	{
		free(buffer);
		free(buffer_temp);
		return false;
	}
	fwrite(buffer, sizeof(char), file_len - temp_len, m_file);
	fclose(m_file);

	free(buffer);
	free(buffer_temp);
	return true;
};
bool DecryptStream(const char * filename, const char *pwd1, const char *pwd2, char *result, char * printInfo/* = NULL*/)
{
	int i;
	int file_len;
	int temp_len;
	FILE * m_file;
	int CRYPT_BUFFER_SIZE;

	int pwd1_len = strlen(pwd1);
	int pwd2_len = strlen(pwd2);
	temp_len = 2 * sizeof(int) + TAG_CRYPT_LEN + pwd2_len;
	file_len = GetFileLength(filename);

	if (printInfo != NULL)
	{
		strcpy(printInfo, "Begin DecryptStream: ");
	}

	if (file_len <= 0)
	{
		if (printInfo != NULL)
		{
			strcat(printInfo, "In DecryptStream GetFileLength <= 0. ");
		}
		return false;
	}
	CRYPT_BUFFER_SIZE = file_len + temp_len;
	char *buffer = (char*)malloc(CRYPT_BUFFER_SIZE);
	char *buffer_temp = (char*)malloc(100);

	result[0] = '\0';

	m_file = fopen(filename, "r+b");
	if (m_file == NULL)
	{
		if (printInfo != NULL)
		{
			strcat(printInfo, "In DecryptStream fopen failed. ");
		}
		free(buffer);
		free(buffer_temp);
		return false;
	}

	file_len = fread(buffer, sizeof(char), CRYPT_BUFFER_SIZE, m_file);

	for (i = 0; i < TAG_CRYPT_LEN; i++)
	{
		buffer_temp[i] = buffer[i] ^ pwd1[i%pwd1_len];

		if (printInfo != NULL)
		{
			char cTemp[80];
			memset(cTemp, '\0', sizeof(cTemp));
			sprintf(cTemp, "buffer[%d]=%02x, pwd1[%d]=%02x, buffer_temp[%d]=%02x; ", i, buffer[i], i%pwd1_len, pwd1[i%pwd1_len], i, buffer_temp[i]);
			strcat(printInfo, cTemp);
		}
	}
	if (strncmp(buffer_temp, TAG_CRYPT, TAG_CRYPT_LEN) != 0)
	{
		if (printInfo != NULL)
		{
			char cTemp[200];
			memset(cTemp, '\0', sizeof(cTemp));
			sprintf(cTemp, "In DecryptStream fread %d Byte is not TAG_CRYPT. ", TAG_CRYPT_LEN);

			strcat(printInfo, cTemp);
			//strcat(printInfo, "In DecryptStream fread 8 Byte is not TAG_CRYPT. ");
		}
		fclose(m_file);
		free(buffer);
		free(buffer_temp);
		return false;
	}

	for (i = 0; i < file_len; i++)
	{
		buffer[i] ^= pwd1[i%pwd1_len];
	}


	int *data = (int *)&buffer[TAG_CRYPT_LEN];
	int f_len = data[0];
	int p_len = data[1];

	if (printInfo != NULL)
	{
		char cTemp[100];
		memset(cTemp, '\0', sizeof(cTemp));
		sprintf(cTemp, "p_len=%d,pwd2_len=%d,f_len+temp_len=%d,file_len=%d,temp_len-pwd2_len=%d. ", p_len, pwd2_len, f_len + temp_len, file_len, temp_len - pwd2_len);
		strcat(printInfo, cTemp);
	}
	if ((p_len != pwd2_len) || (f_len + temp_len != file_len) || (strncmp((char *)&buffer[temp_len - pwd2_len], pwd2, pwd2_len) != 0))
	{
		if (printInfo != NULL)
		{
			strcat(printInfo, "In DecryptStream f_len or p_len or pwd2_len Error: ");
			for (i = 0; i < pwd2_len; i++)
			{
				char tempValue[20];
				memset(tempValue, '\0', sizeof(tempValue));
				sprintf(tempValue, "buffer[%d]=%c,", temp_len - pwd2_len + i, buffer[temp_len - pwd2_len + i]);
				strcat(printInfo, tempValue);
				sprintf(tempValue, "pwd2[%d]=%c; ", i, pwd2[i]);
				strcat(printInfo, tempValue);
			}
		}
		fclose(m_file);
		free(buffer);
		free(buffer_temp);
		return false;
	}

	for (i = 0; i < temp_len; i++)
	{
		buffer[i + f_len] ^= pwd2[i%pwd2_len];
	}
	memcpy(buffer_temp, buffer + f_len, temp_len);
	memcpy(buffer, buffer_temp, temp_len);
	fclose(m_file);

	buffer[file_len - temp_len] = '\0';
	strcpy(result, buffer);

	free(buffer);
	free(buffer_temp);

	if (printInfo != NULL)
	{
		strcat(printInfo, "DecryptStream Successful.");
	}

	return true;
};