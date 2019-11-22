#include "sm2.h"
#include "CryptoWrapperAPI.h"
#include "Convert.h"
#include <string.h>

int ChangeSM2HexToString(const unsigned char* key, int key_len, char * out_string, int * out_len)
{
	int ret = 0;
	ret = hextostr(key, key_len, out_string, out_len);

	return ret;
}

int ChangeSM2StringToHex(const char *in_string, int in_len, unsigned char* key, int * key_len)
{
	int ret = 0;
	ret = strtohex(in_string, in_len, key, key_len);
	return ret;
}

int ChangeHexToString(const unsigned char* key, int key_len, char * out_string, int * out_len)
{
	int ret = 0;
	ret = hextostr(key, key_len, out_string, out_len);

	return ret;
}

int ChangeStringToHex(const char *in_string, int in_len, unsigned char* key, int * key_len)
{
	int ret = 0;
	ret = strtohex(in_string, in_len, key, key_len);
	return ret;
}

int CreateSecretKeySM2(char* public_buffer_key, 
						const int public_buffer_len,
						int * public_out_len,
						char* private_buffer_key,
						const int private_buffer_len,
						int *private_out_len)
{
	if ((public_buffer_key == NULL) || (public_buffer_len < 128))
	{
		//printf("CreateSecretKey error. buffer is null or len is less than 16\n");
		return -1;
	}

	if ((private_buffer_key == NULL) || (private_buffer_len < 64))
	{
		//printf("CreateSecretKey error. buffer is null or len is less than 16\n");
		return -1;
	}

	unsigned char  pubKey_x[32];
	unsigned char  pubKey_y[32];
	unsigned char  pubKey_buffer_xy[64];

	unsigned char privkey[32];

	memset(pubKey_x, 0, 32 * sizeof(unsigned char));
	memset(pubKey_y, 0, 32 * sizeof(unsigned char));
	memset(pubKey_buffer_xy, 0, 64 * sizeof(unsigned char));

	memset(privkey, 0, 32 * sizeof(unsigned char));

	int pubKey_x_len = 0;
	int pubKey_y_len = 0;
	int privkeylen = 0;
	int pubKey_buffer_xy_len = 0;
	int i = 0;


	sm2_keygen(pubKey_x, &pubKey_x_len, pubKey_y, &pubKey_y_len, privkey, &privkeylen);

	for (i = 0; i < 32; i++)
	{
		pubKey_buffer_xy[i] = pubKey_x[i];
		//printf("%02x",pubKey_x[i]);
	}
	//printf("\npubKey_x_len = %d \n", pubKey_x_len);

	for (i = 0; i < 32; i++)
	{
		pubKey_buffer_xy[i + 32] = pubKey_y[i];
		//printf("%02x",pubKey_y[i]);
	}
	//printf("\npubKey_y_len = %d \n", pubKey_y_len);

	pubKey_buffer_xy_len = pubKey_x_len + pubKey_y_len;


	//public
	ChangeSM2HexToString(pubKey_buffer_xy, pubKey_buffer_xy_len, public_buffer_key, public_out_len);

	//private
	ChangeSM2HexToString(privkey, privkeylen, private_buffer_key, private_out_len);
	return 0;
}

int EncryptSM2(const char *public_key, const int public_key_len,
	const unsigned char *input_data,
	const int input_length,
	unsigned char *out_data,
	int *output_length)
{
	if (public_key == NULL || input_data == NULL || out_data == NULL)
	{
		//printf("Encrypt error! ,param is null\n");
		return -1;
	}
	if (public_key_len < 128)
	{
		return -1;
	}

	unsigned char pubKey_x[32], pubKey_y[32];

	memset(pubKey_x, 0, 32 * sizeof(unsigned char));
	memset(pubKey_y, 0, 32 * sizeof(unsigned char));

	unsigned char  pubKey_xy[64];
	int pubKey_xy_len = 0;
	int i = 0;

	memset(pubKey_xy, 0, 64 * sizeof(unsigned char));

	ChangeSM2StringToHex(public_key, public_key_len, pubKey_xy, &pubKey_xy_len);

	for (i = 0; i < 32; i++)
	{
		pubKey_x[i] = pubKey_xy[i];
		//printf("%02x",pubKey_x[i]);
	}
	for (i = 0; i < 32; i++)
	{
		pubKey_y[i] = pubKey_xy[i + 32];
		//printf("%02x",pubKey_y[i]);
	}

	sm2_encrypt((unsigned char *)input_data, input_length, pubKey_x, 32, pubKey_y, 32, out_data);

	*output_length = input_length + 64 + 32;

	return 0;

}

int DecryptSM2(const char *private_key, const  int private_key_len,
	const unsigned char *input_data,
	const int input_length,
	unsigned char *out_data, int *output_length)
{
	//printf("input_length = %d\n", input_length);
	if (private_key == NULL || input_data == NULL || out_data == NULL)
	{
		//printf("Encrypt error! ,param is null\n");
		return -1;
	}
	if (private_key_len < 64)
	{
		return -1;
	}

	unsigned char privkey[32];
	int privkey_len = 0;
	memset(privkey, 0, 32 * sizeof(unsigned char));



	ChangeSM2StringToHex(private_key, private_key_len, privkey, &privkey_len);

	if (privkey_len != 32)
	{
		return -1;
	}


	if (sm2_decrypt((unsigned char *)input_data, input_length, privkey, 32, out_data) < 0)
	{
		printf("sm2_decrypt error!\n");
		return -2;
	}
	else
	{
		printf("sm2_decrypt OK!\n");
	}

	*output_length = input_length - 96;
	//printf("output_length = %d\n", *output_length);
	return 0;
}
