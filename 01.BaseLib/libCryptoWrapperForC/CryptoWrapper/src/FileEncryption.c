#include <string.h>
#include "aes.h"
#include "evp.h"
#include "sm4.h"
#include "CryptoWrapperAPI.h"
#include "Convert.h"

#include<stdio.h>

#define SM4_BLOCK_SIZE 16

typedef enum tagPASSWORD_LENGTH
{
	PASSWORD_LENGTH_128 = 128
}PASSWORD_LENGTH;


//定义文件加密的块大小(也就是加密算法输入的块大小)
#define BLOCK_SIZE 16

//AES文件加密函数
/*
@param type 加解密的类型
@param fr  需要加密的文件
@param fw  加密后输出的文件
@param key 加密所需的密钥
return
 0  正确
-1 参数错误
-2 文件不可读写
-3 加解密错误
*/
int file_encrypt(SR_Encrypt type, const char* fr, const char* fw, const unsigned char * key)
{
	FILE *fp_r = NULL, *fp_w=NULL;

	if (!key || !fr || !fw )
	{
		return -1;
	}

	//open file
	if ((fp_r = fopen(fr, "rb")) == NULL) {
		fprintf(stderr, "Error reading file.");
		return -2;
	}


	if ((fp_w = fopen(fw, "wb")) == NULL) {
		fprintf(stderr, "Error reading file.");
		fclose(fp_r);
		return -2;
	}
	
	long lFileLen = 0, nBlockSize = 0, nRemainNum = 0;

	fseek(fp_r, 0, SEEK_END);
	lFileLen = ftell(fp_r); //ftell()函数返回文件位置指示符的当前值，即如果现在是在文件结尾，则这个值就是文件长度
	fseek(fp_r, 0, SEEK_SET);

	nBlockSize = lFileLen / AES_BLOCK_SIZE;
	nRemainNum = lFileLen % AES_BLOCK_SIZE;

	//printf("%s %d nBlockSize = %ld\n", __FUNCTION__, __LINE__, nBlockSize);
	//printf("%s %d nRemainNum = %ld\n", __FUNCTION__, __LINE__, nRemainNum);

	unsigned char buf_src[BLOCK_SIZE];
	unsigned char buf_encrypt[BLOCK_SIZE];
	int i, r_size;

	AES_KEY nAesKkey;

	if (AES_set_encrypt_key((const unsigned char *)key, PASSWORD_LENGTH_128, &nAesKkey) < 0)
	{
		//printf("AES_set_encrypt_key error!\n");
		//assert(false);
		return -3;
	}

	//读入初始的文件块
	memset(buf_src, 0, BLOCK_SIZE * sizeof(unsigned char));
	r_size = fread(buf_src, sizeof(unsigned char), BLOCK_SIZE, fp_r);
	//printf("%s %d r_size = %d\n", __FUNCTION__, __LINE__, r_size);
	while (r_size == BLOCK_SIZE) {
		//加密文件快
		memset(buf_encrypt, 0, BLOCK_SIZE * sizeof(unsigned char));
		AES_encrypt(buf_src, buf_encrypt, &nAesKkey);
		//写入到文件
		fwrite(buf_encrypt, sizeof(unsigned char), BLOCK_SIZE, fp_w);
		//读入下一个块
		memset(buf_src, 0, BLOCK_SIZE * sizeof(unsigned char));
		r_size = fread(buf_src, sizeof(unsigned char), BLOCK_SIZE, fp_r);
		//printf("%s %d r_size = %d\n", __FUNCTION__, __LINE__, r_size);
	}

	//如果不是文件结束,返回错误值
	if (!feof(fp_r)) {
		fprintf(stderr, "Error reading file in feof.");
		fclose(fp_r);
		fclose(fp_w);
		return -2;
	}

	unsigned char nTemp[AES_BLOCK_SIZE] = { 0 };
	memset(nTemp, 16, sizeof(nTemp));

	if (nRemainNum > 0)
	{
		memset(nTemp, (AES_BLOCK_SIZE - nRemainNum), AES_BLOCK_SIZE);
		memcpy(nTemp, buf_src, nRemainNum);
	}

	//AES_encrypt(buf, buf, key);
	//加密文件快
	memset(buf_encrypt, 0, BLOCK_SIZE * sizeof(unsigned char));
	AES_encrypt(nTemp, buf_encrypt, &nAesKkey);
	fwrite(buf_encrypt, sizeof(unsigned char), BLOCK_SIZE, fp_w);

	
	fclose(fp_r);
	fclose(fp_w);
	return 0;
}

//AES文件解密密函数
/*
@param type 加解密的类型
@param fr   需要解密的文件, 需要绝对路径
@param fw   解密后输出的文件， 需要
@param key  解密所需的密钥
return
	 0 正确
	-1 参数错误
	-2 文件不可读写
	-3 加解密错误
*/
int file_decrypt(SR_Encrypt type, const char* fr, const char* fw, const unsigned char * key)
{
	FILE *fp_r = NULL, *fp_w = NULL;

	if (!key || !fr || !fw )
	{
		return -1;
	}

	//open file
	if ((fp_r = fopen(fr, "rb")) == NULL) {
		fprintf(stderr, "Error reading file.");
		return -2;
	}

	if ((fp_w = fopen(fw, "wb")) == NULL) {
		fprintf(stderr, "Error reading file.");
		fclose(fp_r);
		return -2;
	}

	long lFileLen = 0, nBlockSize = 0, nRemainNum = 0;

	fseek(fp_r, 0, SEEK_END);
	lFileLen = ftell(fp_r); //ftell()函数返回文件位置指示符的当前值，即如果现在是在文件结尾，则这个值就是文件长度
	fseek(fp_r, 0, SEEK_SET);

	nBlockSize = lFileLen / AES_BLOCK_SIZE;
	nRemainNum = lFileLen % AES_BLOCK_SIZE;

	if (nRemainNum > 0)
	{
		//printf("Decrypt error! nRemainNum is %d\n", nRemainNum);
		return -3;
	}

	unsigned char buf_src[BLOCK_SIZE];
	unsigned char buf_decrypt[BLOCK_SIZE];
	int i = 0, r_size = 0;

	AES_KEY nAesKkey;

	if (AES_set_decrypt_key((const unsigned char *)key, PASSWORD_LENGTH_128, &nAesKkey) < 0)
	{
		//printf("Decrypt AES_set_decrypt_key error!\n");
		return -3;
	}


	for (i = 0; i < nBlockSize; i++)
	{
		if (i == (nBlockSize - 1))
		{
			//解密文件快
			memset(buf_src, 0, BLOCK_SIZE * sizeof(unsigned char));
			r_size = fread(buf_src, sizeof(unsigned char), BLOCK_SIZE, fp_r);

			//printf("1  r_size = %d\n", r_size);
			if (r_size != BLOCK_SIZE)
			{
				printf(" encryption error\n");
				fclose(fp_r);
				fclose(fp_w);
				return  -3;
			}

			memset(buf_decrypt, 0, BLOCK_SIZE * sizeof(unsigned char));
			AES_decrypt(buf_src, buf_decrypt, &nAesKkey);

			unsigned char nDeleteNum = buf_decrypt[BLOCK_SIZE - 1];

			if ((int)nDeleteNum > 16)
			{
				printf("Decrypt error! last 16byte > 16\n");
				fclose(fp_r);
				fclose(fp_w);
				return  -3;
			}

			//LogINFO("Decrypt ok!, output_length is %u", output_length);
			int output_length = BLOCK_SIZE - nDeleteNum;
			//写入到文件
			fwrite(buf_decrypt, sizeof(unsigned char), output_length, fp_w);

			break;

		}
		else
		{
			//解密文件快
			memset(buf_src, 0, BLOCK_SIZE * sizeof(unsigned char));
			r_size = fread(buf_src, sizeof(unsigned char), BLOCK_SIZE, fp_r);
			//printf("2  r_size = %d\n", r_size);

			if (r_size != BLOCK_SIZE)
			{
				printf(" encryption error\n");
				fclose(fp_r);
				fclose(fp_w);
				return  -3;
			}

			memset(buf_decrypt, 0, BLOCK_SIZE * sizeof(unsigned char));
			AES_decrypt(buf_src, buf_decrypt, &nAesKkey);
			//写入到文件
			fwrite(buf_decrypt, sizeof(unsigned char), BLOCK_SIZE, fp_w);
		}
	}


	fclose(fp_r);
	fclose(fp_w);

	return 0;
}


