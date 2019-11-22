#include <string.h>
#include "aes.h"
#include "evp.h"
#include "sm4.h"
#include "CryptoWrapperAPI.h"

#define SM4_BLOCK_SIZE 16

typedef enum tagPASSWORD_LENGTH
{
	PASSWORD_LENGTH_128 = 128
}PASSWORD_LENGTH;

#define CHECK_ENCRYPTED_SIZE 4

const int ENCRYPTE_FLAG_AES = 0x01;
const int ENCRYPTE_FLAG_SM4 = 0x02;
const int CHECK_ENCRYPTE_KEY_VAULE = 0xF0;



int CheckEncryptedSM4(const unsigned char *input_data, const size_t input_length)
{
	if (input_length <= CHECK_ENCRYPTED_SIZE)
	{
		printf("CheckEncrypted SM4 error! ,input_length is less than 16\n");
		return -1;
	}

	int nRemainNum = (input_length - CHECK_ENCRYPTED_SIZE) % SM4_BLOCK_SIZE;
	if (nRemainNum != 0)
	{
		printf("CheckEncrypted SM4 error! ,RemainNum is not 0\n");
		return -1;
	}

	unsigned char nLastBuffer[CHECK_ENCRYPTED_SIZE] = { 0 };
	memcpy(nLastBuffer, (void*)(input_data + (input_length - CHECK_ENCRYPTED_SIZE)), CHECK_ENCRYPTED_SIZE);

	if ((size_t)nLastBuffer[0] != ENCRYPTE_FLAG_SM4)
	{
		printf("CheckEncrypted error! ,LastBuffer is not %u\n", input_length);
		return -1;
	}
	size_t i = 0;
	for (i = 1; i < CHECK_ENCRYPTED_SIZE; i++)
	{
		if ((size_t)nLastBuffer[i] != CHECK_ENCRYPTE_KEY_VAULE)
		{
			printf("CheckEncrypted error! ,LastBuffer is not %u\n", input_length);
			return -1;
		}
	}

	return 0;
}

int CheckEncryptedAES(const unsigned char *input_data, const int input_length)
{
	if (input_length <= CHECK_ENCRYPTED_SIZE)
	{
		//printf("CheckEncrypted AES error! ,input_length is less than 16\n");
		return -1;
	}

	int nRemainNum = (input_length - CHECK_ENCRYPTED_SIZE) % AES_BLOCK_SIZE;
	if (nRemainNum != 0)
	{
		//printf("CheckEncrypted AES error! ,RemainNum is not 0\n");
		return -1;
	}

	unsigned char nLastBuffer[CHECK_ENCRYPTED_SIZE] = { 0 };
	memcpy(nLastBuffer, (void*)(input_data + (input_length - CHECK_ENCRYPTED_SIZE)), CHECK_ENCRYPTED_SIZE);
	if ((size_t)nLastBuffer[0] != ENCRYPTE_FLAG_AES)
	{
		//printf("CheckEncrypted error! ,LastBuffer is not %u\n", input_length);
		return -1;
	}
	int i = 0;

	for (i = 1; i < CHECK_ENCRYPTED_SIZE; i++)
	{
		if ((size_t)nLastBuffer[i] != CHECK_ENCRYPTE_KEY_VAULE)
		{
			//printf("CheckEncrypted error! ,LastBuffer is not %u\n", input_length);
			return -1;
		}
	}

	return 0;
}

int EncryptAES(const unsigned char *password, const unsigned char *input_data, const int input_length, unsigned char *out_data, int *output_length)
{
	if (password == NULL || input_data == NULL || out_data == NULL)
	{
		//printf("Encrypt error! ,param is null\n");
		return -1;
	}
	AES_KEY nAesKkey;

	printf("%s %d %s\n",__FUNCTION__,__LINE__,__FILE__);
	if (AES_set_encrypt_key((const unsigned char *)password, PASSWORD_LENGTH_128, &nAesKkey) < 0)
	{
		//printf("AES_set_encrypt_key error!\n");
		return -1;
	}

	*output_length = 0;
	int nBlockSize = input_length / AES_BLOCK_SIZE;
	int nRemainNum = input_length % AES_BLOCK_SIZE;

	int nCurPosition = 0;
	int i = 0;
	for (i = 0; i < nBlockSize; i++)
	{
		AES_encrypt((input_data + nCurPosition), (out_data + nCurPosition), &nAesKkey);
		*output_length += AES_BLOCK_SIZE;
		nCurPosition += AES_BLOCK_SIZE;
	}

	printf("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);

	unsigned char nTemp[AES_BLOCK_SIZE + 1] = { 0 };
	memset(nTemp, 16, sizeof(nTemp));
	if (nRemainNum > 0)
	{
		memset(nTemp, (AES_BLOCK_SIZE - nRemainNum), AES_BLOCK_SIZE);
		memcpy(nTemp, (input_data + nCurPosition), nRemainNum);
	}
	printf("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);

	AES_encrypt(nTemp, (out_data + nCurPosition), &nAesKkey);
	*output_length += AES_BLOCK_SIZE;
	nCurPosition += AES_BLOCK_SIZE;
	printf("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);

	//Push the check data
	memset(out_data + nCurPosition, CHECK_ENCRYPTE_KEY_VAULE, CHECK_ENCRYPTED_SIZE);
	memset(out_data + nCurPosition, ENCRYPTE_FLAG_AES, 1); //set AES flag
	*output_length += CHECK_ENCRYPTED_SIZE;
	nCurPosition += CHECK_ENCRYPTED_SIZE;

	return 0;
}

int DecryptAES(const unsigned char *password, const unsigned char *input_data, const int input_length, unsigned char *out_data, int *output_length)
{
	if (password == NULL || input_data == NULL || out_data == NULL)
	{
		//printf("Encrypt error! ,param is null\n");
		return -1;
	}

	if (0 != CheckEncryptedAES(input_data, input_length))
	{
		//printf("Encrypt error! ,it' not encryped data\n");
		return -1;
	}

	int true_data_len = input_length - CHECK_ENCRYPTED_SIZE;


	AES_KEY nAesKkey;

	if (AES_set_decrypt_key((const unsigned char *)password, PASSWORD_LENGTH_128, &nAesKkey) < 0)
	{
		//printf("Decrypt AES_set_decrypt_key error!\n");
		return -1;
	}

	*output_length = 0;
	int nBlockSize = true_data_len / AES_BLOCK_SIZE;
	int nRemainNum = true_data_len % AES_BLOCK_SIZE;
	if (nRemainNum > 0)
	{
		//printf("Decrypt error! nRemainNum is %d\n", nRemainNum);
		return -1;
	}
	int nCurPosition = 0;
	
	int i = 0;
	for (i = 0; i < nBlockSize; i++)
	{
		AES_decrypt((input_data + nCurPosition), (out_data + nCurPosition), &nAesKkey);
		*output_length += AES_BLOCK_SIZE;
		nCurPosition += AES_BLOCK_SIZE;
	}

	unsigned char nDeleteNum = out_data[nCurPosition - 1];
	if ((int)nDeleteNum > 16)
	{
		//printf("Decrypt error! last 16byte > 16\n");
		return -1;
	}

	//LogINFO("Decrypt ok!, output_length is %u", output_length);
	*output_length = *output_length - nDeleteNum;
	return 0;
}

int EncryptSM4(const unsigned char *password, const unsigned char *input_data, const size_t input_length, unsigned char *out_data, int *output_length)
{
	if (password == NULL || input_data == NULL || out_data == NULL)
	{
		//printf("Encrypt error! ,param is null\n");
		return -1;
	}


	int nBlockSize = input_length / SM4_BLOCK_SIZE;
	int nRemainNum = input_length % SM4_BLOCK_SIZE;
	int nCurPosition = 0;

	sm4_context ctx;
	sm4_setkey_enc(&ctx, password);
	*output_length = 0;

	if (nBlockSize > 0)
	{
		sm4_crypt_ecb(&ctx, 1, (nBlockSize * SM4_BLOCK_SIZE), input_data, out_data);
		*output_length += nBlockSize * SM4_BLOCK_SIZE;
		nCurPosition += nBlockSize * SM4_BLOCK_SIZE;
	}

	
	unsigned char nTemp[SM4_BLOCK_SIZE + 1] = { 0 };
	memset(nTemp, 16, sizeof(nTemp));
	if (nRemainNum > 0)
	{
		memset(nTemp, (SM4_BLOCK_SIZE - nRemainNum), SM4_BLOCK_SIZE);
		memcpy(nTemp, (input_data + nCurPosition), nRemainNum);
	}
	sm4_crypt_ecb(&ctx, 1, SM4_BLOCK_SIZE, nTemp, out_data + *output_length);
	*output_length += SM4_BLOCK_SIZE;
	nCurPosition += SM4_BLOCK_SIZE;


	//Push the check data
	memset(out_data + nCurPosition, CHECK_ENCRYPTE_KEY_VAULE, CHECK_ENCRYPTED_SIZE);
	memset(out_data + nCurPosition, ENCRYPTE_FLAG_SM4, 1); //set sm4 flag
	*output_length += CHECK_ENCRYPTED_SIZE;
	nCurPosition += CHECK_ENCRYPTED_SIZE;

	return 0;
}



int DecryptSM4(const unsigned char *password, const unsigned char *input_data, const int input_length, unsigned char *out_data, int *output_length)
{
	if (password == NULL || input_data == NULL || out_data == NULL)
	{
		printf("Encrypt error! ,param is null\n");
		return -1;
	}

	if (0 != CheckEncryptedSM4(input_data, input_length))
	{
		printf("Encrypt error! ,it' not encryped data\n");
		return -1;
	}

	int true_data_len = input_length - CHECK_ENCRYPTED_SIZE;
	
	int nBlockSize = true_data_len / SM4_BLOCK_SIZE;
	int nRemainNum = true_data_len % SM4_BLOCK_SIZE;
	if (nRemainNum > 0)
	{
		printf("Decrypt error! nRemainNum is %d\n", nRemainNum);
		return -1;
	}

	int nCurPosition = 0;
	*output_length = 0;

	sm4_context ctx;
	sm4_setkey_dec(&ctx, password);
	sm4_crypt_ecb(&ctx, 0, true_data_len, input_data, out_data);
	*output_length = true_data_len;
	nCurPosition = true_data_len;

	unsigned char nDeleteNum = out_data[nCurPosition - 1];
	if ((int)nDeleteNum > 16)
	{
		printf("Decrypt error! last 16byte > 16\n");
		return -1;
	}

	//LogINFO("Decrypt ok!, output_length is %u", output_length);
	*output_length = *output_length - nDeleteNum;
	return 0;
}

unsigned int CreateSecretKeySM4(unsigned char* buff_password, const unsigned int buffer_len)
{
	const unsigned int password_len = 16;
	const char nKey[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~!@#$^*()_+|,.-=";

	if ((buff_password == NULL) || (buffer_len < password_len))
	{
		//printf("CreateSecretKey error. buffer is null or len is less than 16\n");
		return 0;
	}
	memset(buff_password, 0, 16 * sizeof(unsigned char));

	srand((int)time(0));
	int i = 0;
	for (i = 0; i < password_len; i++)
	{
		buff_password[i] = nKey[rand() % (strlen(nKey))];
	}

	return password_len;
}
unsigned int CreateSecretKey(unsigned char* buff_password, const unsigned int buffer_len)
{
	const unsigned int password_len = 16;
	const char nKey[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~!@#$^*()_+|,.-=";

	if ((buff_password == NULL) || (buffer_len < password_len))
	{
		//printf("CreateSecretKey error. buffer is null or len is less than 16\n");
		return 0;
	}
	memset(buff_password, 0, 16 * sizeof(unsigned char));

	srand((int)time(0));
	int i = 0;
	for (i = 0; i < password_len; i++)
	{
		buff_password[i] = nKey[rand() % (strlen(nKey))];
	}

	return password_len;
}



