
//#include "vld.h"
#include <string.h>
#include <stdio.h>
#include "CryptoWrapperAPI.h"

#if defined WIN32 || defined _WIN64
#include <windows.h>
//#include "CSPInterface.h"
#elif defined LINUX
#include <stdlib.h>
#include <unistd.h>
#endif

#include <iostream>  
#include <string>
#include <stdlib.h>  


int TestAES()
{
	printf("This is AES test\n");
	//要加密的数据
	char encrypt_input_data[] = "hello suiui, this is a test for AES!";

	
	//加密数据的长度
	size_t encrypt_input_length = 0;

	encrypt_input_length = strlen(encrypt_input_data);

	printf("要加密的数据: \n");
	printf("%s\n", encrypt_input_data);
	
	//创建密钥
	unsigned char key[20];
	int ret = 0;
	memset(key, 0, 20 * sizeof(unsigned char));


	ret = CreateSecretKey(key, 20);
	if (ret == 0)
	{
		printf("create AES key failed \n");
		exit(0);//TODO
	}
	else
	{
		printf("Curent AES key len: %d , The AES key; %s\n", ret, (char *)key);
	}

	//分配相应的buffer 存储加密数据和解密数据 
	//需要分配的buffer大小， 
	int len = 0;
	unsigned char * outdata_e = NULL;
	unsigned char * outdata_d = NULL;

	len = sizeof(encrypt_input_data) + 32;
	//printf("encrypt input data len = %d\n", len);
	
	outdata_e = (unsigned char *)malloc(len * sizeof(unsigned char));
	if (!outdata_e)
	{	
		printf("malloc outdata_e failed\n");
		exit(0);//TODO
	}
	outdata_d = (unsigned char *)malloc(len* sizeof(unsigned char));
	if (!outdata_d)
	{
		
		printf("malloc outdata_d failed\n");
		exit(0);//TODO
	}

	memset(outdata_e, 0, len * sizeof(unsigned char));
	
	int encrypt_outdata_e_length = 0;

	printf("加密中.............\n");
	if (0 != EncryptAES(key, (const unsigned char *)encrypt_input_data, encrypt_input_length, outdata_e, &encrypt_outdata_e_length))
	{
		printf("EncryptAES error, key:%s\n", key);
		exit(0); //TODO
	}

	int decrypt_outdata_d_length = 0;

	memset(outdata_d, 0, len * sizeof(unsigned char));
	printf("解密中.............\n");
	if (0 != DecryptAES(key, outdata_e, encrypt_outdata_e_length, outdata_d, &decrypt_outdata_d_length))
	{
		printf("DecryptAES error, key:%s\n", key);
		exit(0); //TODO
	}

	printf("解密数据长度:\n");

	for (int i = 0; i < decrypt_outdata_d_length; i++)
	{
		printf("%c", outdata_d[i]);
	}
	printf("\n");


	free(outdata_d);
	free(outdata_e);
	return 0;
}


int TestSM4()
{
	printf("This is SM4 test\n");
	//要加密的数据
	char encrypt_input_data[] = "hello suiui, this is a test for SM4!";


	//加密数据的长度
	size_t encrypt_input_length = 0;

	encrypt_input_length = strlen(encrypt_input_data);

	printf("要加密的数据: \n");
	printf("%s\n", encrypt_input_data);

	//创建密钥
	unsigned char key[20];
	int ret = 0;
	memset(key, 0, 20 * sizeof(unsigned char));


	ret = CreateSecretKeySM4(key, 20);
	if (ret == 0)
	{
		printf("create SM4 key failed \n");
		exit(0);//TODO
	}
	else
	{
		printf("Curent SM4 key len: %d , The SM4 key; %s\n", ret, (char *)key);
	}

	//分配相应的buffer 存储加密数据和解密数据 
	//需要分配的buffer大小， 
	int len = 0;
	unsigned char * outdata_e = NULL;
	unsigned char * outdata_d = NULL;

	len = sizeof(encrypt_input_data) + 32;
	//printf("encrypt input data len = %d\n", len);

	outdata_e = (unsigned char *)malloc(len * sizeof(unsigned char));
	if (!outdata_e)
	{
		printf("malloc outdata_e failed\n");
		exit(0);//TODO
	}
	outdata_d = (unsigned char *)malloc(len* sizeof(unsigned char));
	if (!outdata_d)
	{

		printf("malloc outdata_d failed\n");
		exit(0);//TODO
	}

	memset(outdata_e, 0, len * sizeof(unsigned char));

	int encrypt_outdata_e_length = 0;

	printf("加密中.............\n");
	if (0 != EncryptSM4(key, (const unsigned char *)encrypt_input_data, encrypt_input_length, outdata_e, &encrypt_outdata_e_length))
	{
		printf("EncryptSM4 error, key:%s\n", key);
		exit(0); //TODO
	}

	int decrypt_outdata_d_length = 0;

	memset(outdata_d, 0, len * sizeof(unsigned char));
	printf("解密中.............\n");
	if (0 != DecryptSM4(key, outdata_e, encrypt_outdata_e_length, outdata_d, &decrypt_outdata_d_length))
	{
		printf("DecryptSM4 error, key:%s\n", key);
		exit(0); //TODO
	}

	printf("解密数据长度:\n");

	for (int i = 0; i < decrypt_outdata_d_length; i++)
	{
		printf("%c", outdata_d[i]);
	}
	printf("\n");


	free(outdata_d);
	free(outdata_e);
	return 0;
}
#define maxsize 20
static size_t  mygetline(char **lineptr, size_t *n, FILE *stream)
{
	size_t count = 0;
	int buf;

	if (*lineptr == NULL)
	{
		*n = maxsize;
		*lineptr = (char  *)malloc(*n);
	}

	if ((buf = fgetc(stream)) == EOF)
	{
		return -1;
	}

	while ((buf = fgetc(stream)) != EOF)
	{
		if (buf == '\n')
		{
			count += 1;
			break;
		}

		count++;

		*(*lineptr + count - 1) = buf;
		*(*lineptr + count) = '\0';

		if (*n <= count)
			*lineptr = (char  *)realloc(*lineptr, count * 2);
	}

	return count;
}

int TestSm2()
{
	printf("This is SM2 test\n");

	//要加密的数据
	char encrypt_input_data[] = "123456abcdefghixyz+-x12*";


	//加密数据的长度
	int encrypt_input_length = 0;

	encrypt_input_length = strlen(encrypt_input_data);

	printf("要加密的数据: \n");
	printf("%s\n", encrypt_input_data);

	//创建密钥
	char pubKey[129];
	char privkey[65];
	int  pubKey_len = 0;
	int privkey_len = 0;

	memset(pubKey, 0, 129 * sizeof(char));
	memset(privkey, 0, 65 * sizeof(char));

	if (CreateSecretKeySM2(pubKey, 129, &pubKey_len, privkey, 65, &privkey_len) != 0)
	{
		printf("create SM2 key failed\n");
		exit(0);//TODO
	}

	printf("pubKey_len = %d    pubKey = %s\n", pubKey_len, pubKey);
	printf("privkey_len = %d   privkey = %s\n", privkey_len, privkey);


	//分配相应的buffer 存储加密数据和解密数据 
	//需要分配的buffer大小， 
	int len = 0;
	unsigned char * outdata_e = NULL;
	unsigned char * outdata_d = NULL;

	len = sizeof(encrypt_input_data) + 96;
	//printf("encrypt input data len = %d\n", len);

	outdata_e = (unsigned char *)malloc(len * sizeof(unsigned char));
	if (!outdata_e)
	{
		printf("malloc outdata_e failed\n");
		exit(0);//TODO
	}
	outdata_d = (unsigned char *)malloc(len* sizeof(unsigned char));
	if (!outdata_d)
	{

		printf("malloc outdata_d failed\n");
		exit(0);//TODO
	}

	memset(outdata_e, 0, len * sizeof(unsigned char));

	printf("加密中.............\n");
	int outdata_e_len = 0;

	if (EncryptSM2(pubKey, 128, (const unsigned char *)encrypt_input_data, encrypt_input_length, outdata_e, &outdata_e_len) != 0)
	{
		printf("encryption data failed\n");
		exit(0);//TODO
	}
	
	memset(outdata_d, 0, len * sizeof(unsigned char));
	printf("解密中.............\n");
	int outdata_d_len = 0;
	if (DecryptSM2(privkey, 64, outdata_e, outdata_e_len, outdata_d, &outdata_d_len) != 0)
	{
		printf("decryption data failed\n");
		exit(0);
		//TODO
	}

    printf("解密数据:%s\n", outdata_d);
	/*for (int i = 0; i < outdata_d_len; i++)
	{
		printf("%c", outdata_d[i]);
	}
	printf("\n");*/


    //char result[100] = { 0 };
    //int result_len = 100;
    ////const unsigned char* key, int key_len, char * out_string, int & out_len
    //hextostr(outdata_d, outdata_d_len, result, result_len);
    //printf("result:%s\n", result);

	free(outdata_e);
	free(outdata_d);
	

	return 0;
}

int TestSm2_forFile()
{
	printf("This is SM2 test\n");

	//要加密的数据
	char encrypt_input_data[] = "0123456789abcdefghjkld";


	//加密数据的长度
	int encrypt_input_length = 0;

	encrypt_input_length = strlen(encrypt_input_data);

	printf("要加密的数据: \n");
	printf("%s\n", encrypt_input_data);

	//创建密钥
	char pubKey[129];
	char privkey[65];
	int  pubKey_len = 0;
	int privkey_len = 0;

	memset(pubKey, 0, 129 * sizeof(char));
	memset(privkey, 0, 65 * sizeof(char));

	if (CreateSecretKeySM2(pubKey, 129, &pubKey_len, privkey, 65, &privkey_len) != 0)
	{
		printf("create SM2 key failed\n");
		exit(0);//TODO
	}

	printf("pubKey_len = %d    pubKey = %s\n", pubKey_len, pubKey);
	printf("privkey_len = %d   privkey = %s\n", privkey_len, privkey);


	//分配相应的buffer 存储加密数据和解密数据 
	//需要分配的buffer大小， 
	int len = 0;
	unsigned char * outdata_e = NULL;
	unsigned char * outdata_d = NULL;

	len = sizeof(encrypt_input_data) + 96;
	//printf("encrypt input data len = %d\n", len);

	outdata_e = (unsigned char *)malloc(len * sizeof(unsigned char));
	if (!outdata_e)
	{
		printf("malloc outdata_e failed\n");
		exit(0);//TODO
	}
	outdata_d = (unsigned char *)malloc(len* sizeof(unsigned char));
	if (!outdata_d)
	{

		printf("malloc outdata_d failed\n");
		exit(0);//TODO
	}

	memset(outdata_e, 0, len * sizeof(unsigned char));

	printf("加密中.............\n");
	int outdata_e_len = 0;

	if (EncryptSM2(pubKey, 128, (const unsigned char *)encrypt_input_data, encrypt_input_length, outdata_e, &outdata_e_len) != 0)
	{
		printf("encryption data failed\n");
		exit(0);//TODO
	}
	//-------------------------------
	//-----------------------------------

	int sm2_len = (outdata_e_len * 2) + 1;

	char * sm2_sm4_string = (char *)malloc(sm2_len * sizeof(char));
	if (NULL == sm2_sm4_string)
	{
		printf("malloc error\n");
		exit(0);
	}

	memset(sm2_sm4_string, 0, sm2_len * sizeof(char));
	int tmp_len = 0;
	ChangeSM2HexToString(outdata_e, outdata_e_len, sm2_sm4_string, &tmp_len);

	printf("tmp len = %d  %s\n", tmp_len, sm2_sm4_string);

	const char *file = "1.sde";
	const char *section = "suirui_key";
	const char *name_key = "key";

	//write name key value pair
	if (SR_WriteIniString(section, name_key, sm2_sm4_string, file) != 0)
	{
		printf("write name pair to ini file fail\n");
		exit(0);
	}

	free(sm2_sm4_string);


	char buf[1024] = { 0 };

	if (SR_ReadIniString(section, name_key, buf, 1024, "", file) != 0)
	{
		printf("read ini file fail\n");
		exit(0); //TODO
	}
	else
	{
		printf("%s  =  %s\n", name_key, buf);
	}


	//----------------------------------

	memset(outdata_d, 0, len * sizeof(unsigned char));
	printf("解密中.............\n");
	int outdata_d_len = 0;
	if (DecryptSM2(privkey, 64, outdata_e, outdata_e_len, outdata_d, &outdata_d_len) != 0)
	{
		printf("decryption data failed\n");
		exit(0);
		//TODO
	}

	printf("解密数据:%s\n", outdata_d);
	

	free(outdata_e);
	free(outdata_d);


	return 0;
}
int TestFile()
{
	printf("This is File Encrypt test\n");

	unsigned char buff_password[] = "01234567890abcef";

	printf("文件加密中.............\n");
	//if (file_encrypt(ENCRYPT_AES, "1.jpg", "1.en", buff_password) != 0)
	//{
	//	printf("file encrypt failed\n");
	//	exit(0);//TODO
	//}

	printf("文件解密中.............\n");
	if (file_decrypt(ENCRYPT_AES, "1.en", "3.jpg", buff_password) != 0)
	{
	printf("file decrypt failed\n");
	exit(0);
	}

	return 0;
}

#ifdef WIN32

typedef int(__stdcall *GetDeviceList)(DWORD *lpdwDeviceNum, DWORD *lpdwDeviceIdleNum, char *pDeviceNameList);
typedef HANDLE(__stdcall *OpenDevice)(char *pDeviceName);
typedef int(__stdcall *GenerateKeyPair)(HANDLE hDev, char *pPrivateKey, char *pPublicKey);
typedef int(__stdcall *ImportPrivateKey)(HANDLE hDev, char *pPrivateKey);
typedef int(__stdcall *Encrypt)(HANDLE hDev, char *pPublicKey, char *pDataIn, char *pDataOut);
typedef int(__stdcall *Decrypt)(HANDLE hDev, char *pEncrypt, char *pDataOut);

typedef int(__stdcall *CloseDevice)(HANDLE hDev);
typedef int(__stdcall *Digest)(HANDLE hDev, char *pDataIn, char *pDataOut);
typedef int(__stdcall *GetDeviceSnr)(HANDLE hDev, char *pSnr);
typedef int(__stdcall *Signature)(HANDLE hDev, char *pDegist, char *pSignature);
typedef int(__stdcall *Verify)(HANDLE hDev, char *pPublicKey, char *pDigest, char *pSignature);

//USB生成密钥加密，随锐解密
int TestSM2_USBKEY_EN_SUIRUI_DE()
{
    printf("\n\n----USBKEY 加密 随锐解密----\n");
    //USBKEY 加密
    //转16进制
    unsigned char data_original[1024] = "123456sdffd+1234*x-123456sdffd+1234*x-123dafsadfs456sdffd+1234*x-";
    printf("原始数据:%s\n", data_original);
    char data_encrypt[1024] = { 0 };
    char data_last[1024] = { 0 };

    char data_hexdata[2048] = { 0 };
    int data_hex_len = 0;
    ChangeSM2HexToString(data_original, strlen((char*)data_original), data_hexdata, &data_hex_len);

    HINSTANCE hInstLibrary = LoadLibraryA(("CSPInterface.dll"));
    if (hInstLibrary == NULL)
    {
        FreeLibrary(hInstLibrary);
        printf("LoadLibrary error!\n");
        getchar();
        return 0;
    }
    DWORD lpdwDeviceNum[1000] = { 0 };
    DWORD lpdwDeviceIdleNum[1000] = { 0 };
    char pDeviceNameList[1000] = { 0 };
    GetDeviceList getlist = (GetDeviceList)GetProcAddress(hInstLibrary, "CSP_GetDeviceList");
    OpenDevice open = (OpenDevice)GetProcAddress(hInstLibrary, "CSP_OpenDevice");
    CloseDevice close = (CloseDevice)GetProcAddress(hInstLibrary, "CSP_CloseDevice");
    GenerateKeyPair key = (GenerateKeyPair)GetProcAddress(hInstLibrary, "CSP_GenerateKeyPair");
    ImportPrivateKey import = (ImportPrivateKey)GetProcAddress(hInstLibrary, "CSP_ImportPrivateKey");
    Encrypt encrypt = (Encrypt)GetProcAddress(hInstLibrary, "CSP_Encrypt");
    Decrypt decrypt = (Decrypt)GetProcAddress(hInstLibrary, "CSP_Decrypt");
    getlist(lpdwDeviceNum, lpdwDeviceIdleNum, pDeviceNameList);
    HANDLE handle = open(pDeviceNameList);
    int result = -1;
    char public_key[1000] = { 0 };
    char private_key[1000] = { 0 };
    result = key(handle, private_key, public_key);
    printf("USBKEY 密钥:%s\n", private_key);
    printf("USBKEY 公钥:%s\n", public_key);
    if (result != 0)
    {
        printf("key error\n");
        return 0;
    }
    result = import(handle, private_key);
    if (result != 0)
    {
        printf("import error\n");
        return 0;
    }
    
    encrypt(handle, public_key, data_hexdata, data_encrypt);
    printf("USBKEY 加密后的数据:%s\n", data_encrypt);
    //SUIRUI 解密
    unsigned char encode_data[1024] = "";
    int encode_data_len = 0;
    ChangeSM2StringToHex(data_encrypt, strlen(data_encrypt), encode_data, &encode_data_len);
    unsigned char decode_data[1024] = { 0 };
    int decode_data_len = 0;

    if (DecryptSM2(private_key, 64, encode_data, encode_data_len, decode_data, &decode_data_len) != 0)
    {
        printf("decryption data failed\n");
    }
    printf("随锐解密:%s\n", decode_data);
    close(handle);
    FreeLibrary(hInstLibrary);
    return 0;
}

//随锐生成密钥加密，USB解密
int TestSM2_SUIRUI_EN_USBKEY_DE()
{
    printf("\n\n----随锐加密 USBKEY解密----\n");
    //SUIRUI 加密
    char encrypt_input_data[] = "123456sdffd+1234*x-123456sdffd+1234*x-123dafsadfs456sdffd+1234*x-";
    int encrypt_input_length = 0;
    encrypt_input_length = strlen(encrypt_input_data);
    printf("原始数据: %s\n", encrypt_input_data);
    char pubKey[129] = {0};
    char privkey[65] = {0};
    int  pubKey_len = 0;
    int privkey_len = 0;

    if (CreateSecretKeySM2(pubKey, 129, &pubKey_len, privkey, 65, &privkey_len) != 0)
    {
        printf("create SM2 key failed\n");
        exit(0);//TODO
    }
    printf("随锐密钥:%s\n", privkey);
    printf("随锐公钥:%s\n", pubKey);
    unsigned char * outdata_e = NULL;
    unsigned char * outdata_d = NULL;
    int len = sizeof(encrypt_input_data) + 96;
    outdata_e = (unsigned char *)malloc(len * sizeof(unsigned char));
    outdata_d = (unsigned char *)malloc(len* sizeof(unsigned char));
    memset(outdata_e, 0, len * sizeof(unsigned char));

    int outdata_e_len = 0;
    if (EncryptSM2(pubKey, 128, (const unsigned char *)encrypt_input_data, encrypt_input_length, outdata_e, &outdata_e_len) != 0)
    {
        printf("encryption data failed\n");
        exit(0);//TODO
    }

    //USBKEY解密
    HINSTANCE hInstLibrary = LoadLibraryA(("CSPInterface.dll"));//注意此处必须有_T()函数。
    if (hInstLibrary == NULL)
    {
        FreeLibrary(hInstLibrary);
        printf("LoadLibrary error!\n");
        getchar();
        return 0;
    }

    DWORD lpdwDeviceNum[1000] = { 0 };
    DWORD lpdwDeviceIdleNum[1000] = { 0 };
    char pDeviceNameList[1000] = { 0 };

    GetDeviceList getlist = (GetDeviceList)GetProcAddress(hInstLibrary, "CSP_GetDeviceList");
    OpenDevice open = (OpenDevice)GetProcAddress(hInstLibrary, "CSP_OpenDevice");
    CloseDevice close = (CloseDevice)GetProcAddress(hInstLibrary, "CSP_CloseDevice");
    GenerateKeyPair key = (GenerateKeyPair)GetProcAddress(hInstLibrary, "CSP_GenerateKeyPair");
    ImportPrivateKey import = (ImportPrivateKey)GetProcAddress(hInstLibrary, "CSP_ImportPrivateKey");
    Encrypt encrypt = (Encrypt)GetProcAddress(hInstLibrary, "CSP_Encrypt");
    Decrypt decrypt = (Decrypt)GetProcAddress(hInstLibrary, "CSP_Decrypt");
    getlist(lpdwDeviceNum, lpdwDeviceIdleNum, pDeviceNameList);
    HANDLE handle = open(pDeviceNameList);
     int result = import(handle, privkey);
    if (result != 0)
    {
        printf("import error\n");
        return 0;
    }

    char decode_data[1024] = { 0 };
    int decode_data_len = 0;
    char last_data[1024] = { 0 };
    ChangeSM2HexToString(outdata_e, outdata_e_len, decode_data, &decode_data_len);
    printf("随锐加密后的数据:%s\n", decode_data);
    decrypt(handle, decode_data, last_data);

    //printf("USBKEY 解密的数据:%s\n", last_data);

    unsigned char data_str_last[1024] = { 0 };
    int data_str_last_len = 0;
    ChangeSM2StringToHex(last_data, strlen(last_data), data_str_last, &data_str_last_len);

    printf("USBKEY 解密的数据:%s\n", last_data);

    printf("data str last:%s\n", data_str_last);

    close(handle);
    FreeLibrary(hInstLibrary);
    return 0;
}

//USB 密钥对，用随锐加解密，用USB解密
int TestUSBKEY_SR_EN_DE_USB_DE()
{
    printf("\n\n----USB 密钥对，用随锐加解密，用USB解密----\n");
    //USBKEY 加密
    char data_original[1024] = "123456sdffd+1234*x-123456sdffd+1234*x-123dafsadfs456sdffd+1234*x-";
    char data_encrypt[1024] = { 0 };
    printf("原始数据:%s\n", data_original);

    HINSTANCE hInstLibrary = LoadLibraryA(("CSPInterface.dll"));
    if (hInstLibrary == NULL)
    {
        FreeLibrary(hInstLibrary);
        printf("LoadLibrary error!\n");
        getchar();
        return 0;
    }
    DWORD lpdwDeviceNum[1000] = { 0 };
    DWORD lpdwDeviceIdleNum[1000] = { 0 };
    char pDeviceNameList[1000] = { 0 };

    GetDeviceList getlist = (GetDeviceList)GetProcAddress(hInstLibrary, "CSP_GetDeviceList");
    OpenDevice open = (OpenDevice)GetProcAddress(hInstLibrary, "CSP_OpenDevice");
    CloseDevice close = (CloseDevice)GetProcAddress(hInstLibrary, "CSP_CloseDevice");
    GenerateKeyPair key = (GenerateKeyPair)GetProcAddress(hInstLibrary, "CSP_GenerateKeyPair");
    ImportPrivateKey import = (ImportPrivateKey)GetProcAddress(hInstLibrary, "CSP_ImportPrivateKey");
    Encrypt encrypt = (Encrypt)GetProcAddress(hInstLibrary, "CSP_Encrypt");
    Decrypt decrypt = (Decrypt)GetProcAddress(hInstLibrary, "CSP_Decrypt");
    getlist(lpdwDeviceNum, lpdwDeviceIdleNum, pDeviceNameList);
    HANDLE handle = open(pDeviceNameList);
    int result = -1;
    char public_key[1000] = { 0 };
    char private_key[1000] = { 0 };
    result = key(handle, private_key, public_key);
    printf("USBKEY 密钥:%s\n", private_key);
    printf("USBKEY 公钥:%s\n", public_key);
    if (result != 0)
    {
        printf("key error\n");
        return 0;
    }
    result = import(handle, private_key);
    if (result != 0)
    {
        printf("import error\n");
        return 0;
    }


    //随锐加解密
    int len = 0;
    unsigned char * outdata_e = NULL;
    unsigned char * outdata_d = NULL;

    len = strlen(data_original) + 96;

    outdata_e = (unsigned char *)malloc(len * sizeof(unsigned char));
    if (!outdata_e)
    {
        printf("malloc outdata_e failed\n");
        exit(0);//TODO
    }
    outdata_d = (unsigned char *)malloc(len* sizeof(unsigned char));
    if (!outdata_d)
    {

        printf("malloc outdata_d failed\n");
        exit(0);//TODO
    }

    memset(outdata_e, 0, len * sizeof(unsigned char));

    printf("随锐加密中.............\n");
    int outdata_e_len = 0;

    if (EncryptSM2(public_key, 128, (const unsigned char *)data_original, strlen(data_original), outdata_e, &outdata_e_len) != 0)
    {
        printf("encryption data failed\n");
        exit(0);//TODO
    }

    memset(outdata_d, 0, len * sizeof(unsigned char));
    printf("随锐解密中.............\n");
    int outdata_d_len = 0;
    if (DecryptSM2(private_key, 64, outdata_e, outdata_e_len, outdata_d, &outdata_d_len) != 0)
    {
        printf("decryption data failed\n");
        exit(0);
        //TODO
    }

    printf("随锐解密数据:%s\n", outdata_d);
    


    printf("USBKEY解密中.............\n");
    char decode_data[1024] = { 0 };
    int decode_data_len = 0;
    char last_data[1024] = { 0 };
    ChangeSM2HexToString(outdata_e, outdata_e_len, decode_data, &decode_data_len);
    printf("随锐加密后的数据:%s\n", decode_data);
    decrypt(handle, decode_data, last_data);

    unsigned char data_str_last[1024] = { 0 };
    int data_str_last_len = 0;
    ChangeSM2StringToHex(last_data, strlen(last_data), data_str_last, &data_str_last_len);

    printf("USBKEY 解密的数据:%s\n", last_data);

    printf("data str last:%s\n", data_str_last);

    close(handle);
    FreeLibrary(hInstLibrary);
    free(outdata_e);
    free(outdata_d);
    return 0;
}

//USB 密钥对，用USB加解密，用随锐解密
int TestSUIRUIKEY_USB_EN_DE_SR_DE()
{
    char pubKey[129] = { 0 };
    char privkey[65] = { 0 };
    int  pubKey_len = 0;
    int privkey_len = 0;

    if (CreateSecretKeySM2(pubKey, 129, &pubKey_len, privkey, 65, &privkey_len) != 0)
    {
        printf("create SM2 key failed\n");
        exit(0);//TODO
    }
    printf("随锐密钥:%s\n", privkey);
    printf("随锐公钥:%s\n", pubKey);


    HINSTANCE hInstLibrary = LoadLibraryA(("CSPInterface.dll"));//注意此处必须有_T()函数。
    if (hInstLibrary == NULL)
    {
        FreeLibrary(hInstLibrary);
        printf("LoadLibrary error!\n");
        getchar();
        return 0;
    }
    DWORD lpdwDeviceNum[1000] = { 0 };
    DWORD lpdwDeviceIdleNum[1000] = { 0 };
    char pDeviceNameList[1000] = { 0 };

    GetDeviceList getlist = (GetDeviceList)GetProcAddress(hInstLibrary, "CSP_GetDeviceList");
    OpenDevice open = (OpenDevice)GetProcAddress(hInstLibrary, "CSP_OpenDevice");
    CloseDevice close = (CloseDevice)GetProcAddress(hInstLibrary, "CSP_CloseDevice");
    GenerateKeyPair key = (GenerateKeyPair)GetProcAddress(hInstLibrary, "CSP_GenerateKeyPair");
    ImportPrivateKey import = (ImportPrivateKey)GetProcAddress(hInstLibrary, "CSP_ImportPrivateKey");
    Encrypt encrypt = (Encrypt)GetProcAddress(hInstLibrary, "CSP_Encrypt");
    Decrypt decrypt = (Decrypt)GetProcAddress(hInstLibrary, "CSP_Decrypt");
    getlist(lpdwDeviceNum, lpdwDeviceIdleNum, pDeviceNameList);
    HANDLE handle = open(pDeviceNameList);

    int result = -1;
    //set private key
    result = import(handle, privkey);
    if (result != 0)
    {
        printf("import error, private_key:%s\n", privkey);
        return 0;
    }

    unsigned char data_original[1024] = "123456sdffd+1234*x-123456sdffd+1234*x-123dafsadfs456sdffd+1234*x-";
    char data_encrypt[1024] = { 0 };
    char data_last[1024] = { 0 };

    char data_hexdata[2048] = { 0 };
    int data_hex_len = 0;
    ChangeSM2HexToString(data_original, strlen((char*)data_original), data_hexdata, &data_hex_len);
    printf("USB加密中...\n");
    encrypt(handle, pubKey, data_hexdata, data_encrypt);
    printf("USB解密中...\n");
    decrypt(handle, data_encrypt, data_last);

    unsigned char data_str_last[1024] = { 0 };
    int data_str_last_len = 0;
    ChangeSM2StringToHex(data_last, strlen(data_last), data_str_last, &data_str_last_len);
    printf("USB解密结果:%s\n", data_str_last);



    //SUIRUI 解密
    unsigned char encode_data[1024] = "";
    int encode_data_len = 0;
    ChangeSM2StringToHex(data_encrypt, strlen(data_encrypt), encode_data, &encode_data_len);
    unsigned char decode_data[1024] = { 0 };
    int decode_data_len = 0;

    if (DecryptSM2(privkey, 64, encode_data, encode_data_len, decode_data, &decode_data_len) != 0)
    {
        printf("decryption data failed\n");
    }
    printf("随锐解密结果:%s\n", decode_data);

    close(handle);
    FreeLibrary(hInstLibrary);

    return 0;
}

int TestUSBKEY()
{
    HINSTANCE hInstLibrary = LoadLibraryA(("CSPInterface.dll"));//注意此处必须有_T()函数。
    if (hInstLibrary == NULL)
    {
        FreeLibrary(hInstLibrary);
        printf("LoadLibrary error!\n");
        getchar();
        return 0;
    }
    DWORD lpdwDeviceNum[1000] = {0};
    DWORD lpdwDeviceIdleNum[1000] = { 0 };
    char pDeviceNameList[1000] = {0};

    GetDeviceList getlist = (GetDeviceList)GetProcAddress(hInstLibrary, "CSP_GetDeviceList");
    OpenDevice open = (OpenDevice)GetProcAddress(hInstLibrary, "CSP_OpenDevice");
    CloseDevice close = (CloseDevice)GetProcAddress(hInstLibrary, "CSP_CloseDevice");
    GenerateKeyPair key = (GenerateKeyPair)GetProcAddress(hInstLibrary, "CSP_GenerateKeyPair");
    ImportPrivateKey import = (ImportPrivateKey)GetProcAddress(hInstLibrary, "CSP_ImportPrivateKey");
    Encrypt encrypt = (Encrypt)GetProcAddress(hInstLibrary, "CSP_Encrypt");
    Decrypt decrypt = (Decrypt)GetProcAddress(hInstLibrary, "CSP_Decrypt");
    getlist(lpdwDeviceNum, lpdwDeviceIdleNum, pDeviceNameList);
    HANDLE handle = open(pDeviceNameList);
    
    int result = -1;
    //get key
    char public_key[1000] = { 0 };
    char private_key[1000] = { 0 };
    result = key(handle, private_key, public_key);
    if (result != 0)
    {
        printf("key error\n");
        return 0;
    }
    //set private key
    result = import(handle, private_key);
    if (result != 0)
    {
        printf("import error, private_key:%s\n", private_key);
        return 0;
    }

    unsigned char data_original[1024] = "123456sdffd+1234*x-123456sdffd+1234*x-123dafsadfs456sdffd+1234*x-";
    char data_encrypt[1024] = { 0 };
    char data_last[1024] = { 0 };

    char data_hexdata[2048] = { 0 };
    int data_hex_len = 0;
    ChangeSM2HexToString(data_original, strlen((char*)data_original), data_hexdata, &data_hex_len);
   
    encrypt(handle, public_key, data_hexdata, data_encrypt);
    decrypt(handle, data_encrypt, data_last);

    //printf("last data:%s\n", data_last);
    unsigned char data_str_last[1024] = { 0 };
    int data_str_last_len = 0;
    ChangeSM2StringToHex(data_last, strlen(data_last), data_str_last, &data_str_last_len);

    printf("data str last:%s\n", data_str_last);
    close(handle);
    FreeLibrary(hInstLibrary);

    return 0;
}
#endif


//usbkey生成密钥，linux加密，usbkey解密
int LINUX_EN()
{
    char data_original[1024] = "123456";
    char data_encrypt[1024] = { 0 };
    printf("原始数据:%s\n", data_original);

    char public_key[1000] = "9C475B3E30D31411E3BF36899227E344922334F89A6D20064E56035B61519108D6F7507303DBAF979DCCCF4B5BC3B9A6FE9CBB7ED12C49E2A85053538AC426D1";
    char private_key[1000] = "2625649879757B00930DCEEDC0FABA51F295F5C7D292CA89BF8A780238E3331D";

    //随锐加解密
    int len = 0;
    unsigned char * outdata_e = NULL;
    unsigned char * outdata_d = NULL;

    len = strlen(data_original) + 96;

    outdata_e = (unsigned char *)malloc(len * sizeof(unsigned char));
    if (!outdata_e)
    {
        printf("malloc outdata_e failed\n");
        exit(0);//TODO
    }
    outdata_d = (unsigned char *)malloc(len* sizeof(unsigned char));
    if (!outdata_d)
    {

        printf("malloc outdata_d failed\n");
        exit(0);//TODO
    }

    memset(outdata_e, 0, len * sizeof(unsigned char));

    printf("SUIRUI ENCODE.............\n");
    int outdata_e_len = 0;

    if (EncryptSM2(public_key, 128, (const unsigned char *)data_original, strlen(data_original), outdata_e, &outdata_e_len) != 0)
    {
        printf("encryption data failed\n");
        exit(0);//TODO
    }

    printf("SUIRUI ENCODE DATA:");
    for (int i = 0; i < outdata_e_len; i++)
    {
        printf("%02X", outdata_e[i]);
    }
    printf("\n");
    memset(outdata_d, 0, len * sizeof(unsigned char));
    printf("SUIRUI DECODE.............\n");
    int outdata_d_len = 0;
    if (DecryptSM2(private_key, 64, outdata_e, outdata_e_len, outdata_d, &outdata_d_len) != 0)
    {
        printf("decryption data failed\n");
        exit(0);
        //TODO
    }

    printf("SUIRUI DECODE RESULT:%s\n", outdata_d);


    return 0;
}

//usbkey生成密钥，linux加密，usbkey解密
int WINDOWS_DE()
{

    char privkey[1024] = "2625649879757B00930DCEEDC0FABA51F295F5C7D292CA89BF8A780238E3331D";

    //USBKEY解密
    HINSTANCE hInstLibrary = LoadLibraryA(("CSPInterface.dll"));//注意此处必须有_T()函数。
    if (hInstLibrary == NULL)
    {
        FreeLibrary(hInstLibrary);
        printf("LoadLibrary error!\n");
        getchar();
        return 0;
    }

    DWORD lpdwDeviceNum[1000] = { 0 };
    DWORD lpdwDeviceIdleNum[1000] = { 0 };
    char pDeviceNameList[1000] = { 0 };

    GetDeviceList getlist = (GetDeviceList)GetProcAddress(hInstLibrary, "CSP_GetDeviceList");
    OpenDevice open = (OpenDevice)GetProcAddress(hInstLibrary, "CSP_OpenDevice");
    CloseDevice close = (CloseDevice)GetProcAddress(hInstLibrary, "CSP_CloseDevice");
    GenerateKeyPair key = (GenerateKeyPair)GetProcAddress(hInstLibrary, "CSP_GenerateKeyPair");
    ImportPrivateKey import = (ImportPrivateKey)GetProcAddress(hInstLibrary, "CSP_ImportPrivateKey");
    Encrypt encrypt = (Encrypt)GetProcAddress(hInstLibrary, "CSP_Encrypt");
    Decrypt decrypt = (Decrypt)GetProcAddress(hInstLibrary, "CSP_Decrypt");
    getlist(lpdwDeviceNum, lpdwDeviceIdleNum, pDeviceNameList);
    HANDLE handle = open(pDeviceNameList);
    int result = import(handle, privkey);
    if (result != 0)
    {
        printf("import error\n");
        return 0;
    }

    char decode_data[1024] ="3C45548F48747073B12028FB8FFD4C8816D1C27A0AACEAFD0649D9400E0B41074EFEBBB2F00C83FDEC706BA0FC169B3631A0F547365733C0E130DBBDAC31DE0404781051B8EE5320C6479547154A0DE637C6EB9805E1CA8E35C99A6EAD9296BEB6835C";
    int decode_data_len = strlen(decode_data);
    char last_data[1024] = { 0 };
    //ChangeSM2HexToString(outdata_e, outdata_e_len, decode_data, decode_data_len);
    printf("SUIRUI DECODE:%s\n", decode_data);
    decrypt(handle, decode_data, last_data);

    printf("USBKEY 解密的数据:%s\n", last_data);
    close(handle);
    FreeLibrary(hInstLibrary);
    return 0;
}

int TestDecryptSM2()
{
    printf("\n\n----USBKEY ENCODE, SUIRUI DECODE， NO USBKEY----\n");
    char privkey[65] = "2625649879757B00930DCEEDC0FABA51F295F5C7D292CA89BF8A780238E3331D";
    unsigned char encode_data[1024] = "";
    int encode_data_len = 0;

    //temp
    char o_data[1024] = "636A627FFD7AB8CDAFF90B8B86E2D5A4442711640A71900E45AE2018D80393B7A948379029905CCCBCAC76A9239004B19C655CF1CF7230DCEA0B3B254D223D6838D584AF47CC5F8163709F04C96CE0FA360C7E66D08635712E6FC9703963227093CBD47A4D";
    int  o_len = strlen(o_data);
    ChangeSM2StringToHex(o_data, o_len, encode_data, &encode_data_len);
    printf("USBKEY ENCODE DATA:%s\n", o_data);

    unsigned char decode_data[1024] = { 0 };
    int decode_data_len = 0;

    if (DecryptSM2(privkey, 64, encode_data, encode_data_len, decode_data, &decode_data_len) != 0)
    {
        printf("decryption data failed\n");
        //TODO
    }

    printf("SUIRUI DECODE DATA:%s\n", decode_data);
    return 0;
}

int TestAES2()
{
	printf("This is AES2 test\n");
	//要加密的数据
	char encrypt_input_data[] = "hello suiui, this is a test for AES!";


	//加密数据的长度
	size_t encrypt_input_length = 0;

	encrypt_input_length = strlen(encrypt_input_data);

	printf("要加密的数据: \n");
	printf("%s\n", encrypt_input_data);

	//创建密钥
	unsigned char key[17] = "0123456789abcdef";


	//分配相应的buffer 存储加密数据和解密数据 
	//需要分配的buffer大小， 
	int len = 0;
	unsigned char * outdata_e = NULL;

	unsigned char * outdata_d = NULL;

	len = sizeof(encrypt_input_data) + 32;
	//printf("encrypt input data len = %d\n", len);

	outdata_e = (unsigned char *)malloc(len * sizeof(unsigned char));
	if (!outdata_e)
	{
		printf("malloc outdata_e failed\n");
		exit(0);//TODO
	}

	memset(outdata_e, 0, len * sizeof(unsigned char));

	int encrypt_outdata_e_length = 0;

	printf("加密中.............\n");
	if (0 != EncryptAES(key, (const unsigned char *)encrypt_input_data, encrypt_input_length, outdata_e, &encrypt_outdata_e_length))
	{
		printf("EncryptAES error, key:%s\n", key);
		exit(0); //TODO
	}

	for (int i = 0; i < encrypt_outdata_e_length; i++)
	{
		printf("%02x ", outdata_e[i]);
	}
	printf("\n");


	char * outdata_string = NULL;
	outdata_string = (char *)malloc((2 * encrypt_outdata_e_length + 1) * sizeof(char));
	if (!outdata_string)
	{
		printf("malloc outdata_string failed\n");
		exit(0);//TODO
	}

	int out_string_len = 0;
	ChangeSM2HexToString(outdata_e, encrypt_outdata_e_length, outdata_string, &out_string_len);

	printf("out_string_len = %d\n", out_string_len);

	std::string tmp(outdata_string, out_string_len);


	printf("outdata_string = %s  size= %d \n", tmp.c_str(),tmp.length());

#if 0
	int decrypt_outdata_d_length = 0;


	outdata_d = (unsigned char *)malloc(len* sizeof(unsigned char));
	if (!outdata_d)
	{

		printf("malloc outdata_d failed\n");
		exit(0);//TODO
	}
	memset(outdata_d, 0, len * sizeof(unsigned char));
	printf("解密中.............\n");
	if (0 != DecryptAES(key, outdata_e, encrypt_outdata_e_length, outdata_d, &decrypt_outdata_d_length))
	{
		printf("DecryptAES error, key:%s\n", key);
		exit(0); //TODO
	}

	printf("解密数据长度:\n");

	for (int i = 0; i < decrypt_outdata_d_length; i++)
	{
		printf("%c", outdata_d[i]);
	}
	printf("\n");


	free(outdata_d);
#endif
	free(outdata_e);
	free(outdata_string);

	return 0;
}

int TestAES3()
{
	printf("This is AES3 test\n");
	//要加密的数据
	char decrypt_input_data[] = "81e69fc5672a1affda1c07ffdfa6196965a63bcea0afae2e517e24fe381162e53b25a96058dfc7851afaa788c791c27c01f0f0f0";
	//创建密钥
	unsigned char key[17] = "0123456789abcdef";


	unsigned char * outdata_e = NULL;

	outdata_e = (unsigned char *)malloc(strlen(decrypt_input_data) * sizeof(unsigned char));
	if (!outdata_e)
	{
		printf("malloc outdata_e failed\n");
		exit(0);//TODO
	}

	int outdata_e_len = 0;

	ChangeSM2StringToHex(decrypt_input_data, strlen(decrypt_input_data), outdata_e, &outdata_e_len);

	for (int i = 0; i < outdata_e_len; i++)
	{
		printf("%02x ",outdata_e[i]);
	}
	printf("\n");


	unsigned char * outdata_d = NULL;

	outdata_d = (unsigned char *)malloc(outdata_e_len* sizeof(unsigned char));
	if (!outdata_d)
	{

		printf("malloc outdata_d failed\n");
		exit(0);//TODO
	}
	memset(outdata_d, 0, outdata_e_len * sizeof(unsigned char));

	int decrypt_outdata_d_length = 0;

	printf("解密中.............\n");
	if (0 != DecryptAES(key, outdata_e, outdata_e_len, outdata_d, &decrypt_outdata_d_length))
	{
		printf("DecryptAES error, key:%s\n", key);
		exit(0); //TODO
	}

	printf("解密数据长度:\n");

	for (int i = 0; i < decrypt_outdata_d_length; i++)
	{
		printf("%c", outdata_d[i]);
	}
	printf("\n");


	free(outdata_d);
	free(outdata_e);
#if 0
	//加密数据的长度
	size_t encrypt_input_length = 0;

	encrypt_input_length = strlen(encrypt_input_data);

	printf("要加密的数据: \n");
	printf("%s\n", encrypt_input_data);

	//创建密钥
	unsigned char key[17] = "0123456789abcdef";


	//分配相应的buffer 存储加密数据和解密数据 
	//需要分配的buffer大小， 
	int len = 0;
	unsigned char * outdata_e = NULL;

	unsigned char * outdata_d = NULL;

	len = sizeof(encrypt_input_data) + 32;
	//printf("encrypt input data len = %d\n", len);

	outdata_e = (unsigned char *)malloc(len * sizeof(unsigned char));
	if (!outdata_e)
	{
		printf("malloc outdata_e failed\n");
		exit(0);//TODO
	}

	memset(outdata_e, 0, len * sizeof(unsigned char));

	int encrypt_outdata_e_length = 0;

	printf("加密中.............\n");
	if (0 != EncryptAES(key, (const unsigned char *)encrypt_input_data, encrypt_input_length, outdata_e, &encrypt_outdata_e_length))
	{
		printf("EncryptAES error, key:%s\n", key);
		exit(0); //TODO
	}

	for (int i = 0; i < encrypt_outdata_e_length; i++)
	{
		printf("%02x ", outdata_e[i]);
	}
	printf("\n");


	char * outdata_string = NULL;
	outdata_string = (char *)malloc((2 * encrypt_outdata_e_length + 1) * sizeof(char));
	if (!outdata_string)
	{
		printf("malloc outdata_string failed\n");
		exit(0);//TODO
	}

	int out_string_len = 0;
	ChangeSM2HexToString(outdata_e, encrypt_outdata_e_length, outdata_string, &out_string_len);

	printf("out_string_len = %d\n", out_string_len);

	std::string tmp(outdata_string, out_string_len);


	printf("outdata_string = %s  size= %d \n", tmp.c_str(), tmp.length());

	int decrypt_outdata_d_length = 0;


	outdata_d = (unsigned char *)malloc(len* sizeof(unsigned char));
	if (!outdata_d)
	{

		printf("malloc outdata_d failed\n");
		exit(0);//TODO
	}
	memset(outdata_d, 0, len * sizeof(unsigned char));
	printf("解密中.............\n");
	if (0 != DecryptAES(key, outdata_e, encrypt_outdata_e_length, outdata_d, &decrypt_outdata_d_length))
	{
		printf("DecryptAES error, key:%s\n", key);
		exit(0); //TODO
	}

	printf("解密数据长度:\n");

	for (int i = 0; i < decrypt_outdata_d_length; i++)
	{
		printf("%c", outdata_d[i]);
	}
	printf("\n");


	free(outdata_d);
#endif


	return 0;
}

int main(int argc, char* argv[])
{
	printf("hello, world\n");
    
    unsigned char data_original[1024] = "123456sdffd+1234*x-";

    ////16
    //char data_hexdata[2048] = { 0 };
    //int data_hex_len = 0;
    //ChangeSM2HexToString(data_original, strlen((char*)data_original), data_hexdata, data_hex_len);

    //printf("data_hexdata:%s\n", data_hexdata);

    ////char
    //unsigned char data_last[1024] = { 0 };
    //int data_last_len = 0;
    //ChangeSM2StringToHex(data_hexdata, data_hex_len, data_last, data_last_len);

	//TestAES();
	//TestFile();
	//81e69fc5672a1affda1c07ffdfa6196965a63bcea0afae2e517e24fe381162e53b25a96058dfc7851afaa788c791c27c01f0f0f0
	TestAES2();
	TestAES3();

	//TestSM4();

	//TestSm2();
	
	//TestSm2_forFile();

	//TestFile();
    //TestUSBKEY();
//#ifdef WIN32
    //TestSM2_USBKEY_EN_SUIRUI_DE();
   // TestSM2_SUIRUI_EN_USBKEY_DE();
    //TestUSBKEY_SR_EN_DE_USB_DE();
    //TestSUIRUIKEY_USB_EN_DE_SR_DE();
//#endif
//
    //LINUX_EN();
    //WINDOWS_DE();
//    TestDecryptSM2();

	getchar();

	return 0;
}
