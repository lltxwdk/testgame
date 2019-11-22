#ifndef AES_SM4_SM2_WRAPPER_H
#define AES_SM4_SM2_WRAPPER_H

///************* AES 加密、解密、获取秘钥**************//
//默认属性：AES/ECB/PKCS5Padding mode
//秘钥长度：password must 16 bytes, 128 bits
//****************************************************//

///************* SM4 加密、解密、获取秘钥**************//
//默认属性：SM4 
//秘钥长度：password must 16 bytes, 128 bits
//****************************************************//


#ifdef CRYPTO_EXPORTS
#define CRYPTO_API __declspec(dllexport)
#else
#ifdef CRYPTO_IMPORTS
#define CRYPTO_API __declspec(dllimport)
#else 
#define CRYPTO_API extern
#endif
#endif

#ifdef __cplusplus
extern "C"{
#endif


/********************************************************************************
	description:
	sm2 密钥十六进制到字符串
	input:
	key      输入要转化的密钥buffer
	key_len  输入buffer的长度
	output:
	out_string 输出字符串的buffer 事先分配好， 大小至少是输入key的2倍
	out_len    输出字符串的长度
	return:
	0：失败，非0:成功
********************************************************************************/
CRYPTO_API int ChangeHexToString(const unsigned char* key, int key_len, char * out_string, int * out_len);

/********************************************************************************
description:
sm2 密钥字符串转十六进制
input:
in_string 输入字符串的buffer
in_len    输入字符串的buffer长度
output:
key      输出key的buffer， 事先分配好， 大小至少是输入buffer的1/2
key_len  输出key的长度
return:
0：失败，非0:成功
********************************************************************************/
CRYPTO_API int ChangeStringToHex(const char *in_string, int in_len, unsigned char* key, int * key_len);



/********************************************************************************
description:
	sm2 密钥十六进制到字符串
input: 
	key      输入要转化的密钥buffer
	key_len  输入buffer的长度
output:
	out_string 输出字符串的buffer 事先分配好， 大小至少是输入key的2倍
	out_len    输出字符串的长度
return:
	0：失败，非0:成功
********************************************************************************/
CRYPTO_API int ChangeSM2HexToString(const unsigned char* key, int key_len, char * out_string, int * out_len);

/********************************************************************************
description:
	sm2 密钥字符串转十六进制
input: 
	in_string 输入字符串的buffer
	in_len    输入字符串的buffer长度
output:
	key      输出key的buffer， 事先分配好， 大小至少是输入buffer的1/2
	key_len  输出key的长度
return:
	0：失败，非0:成功
********************************************************************************/
CRYPTO_API int ChangeSM2StringToHex(const char *in_string , int in_len, unsigned char* key, int * key_len);

/********************************************************************************
description:
	获取 SM2 公钥和秘钥
input: 
	public_key :         公钥的buffer， 要求至少128个字节
	public_buffer_len ： 公钥的buffer的长度
	prive_key ：         私钥的buffer， 要求至少64个字节
	prive_buffer_len :   私钥的buffer的长度
output:
	public_key ：   公钥缓存
	prive_key ：    私钥缓存
	pubKey_len ：   公钥的缓存的长度
	privekey_len ： 私钥的缓存长度
return:
	0：失败，非0:成功
********************************************************************************/
CRYPTO_API int CreateSecretKeySM2(char* public_buffer_key, 
									    const int public_buffer_len,  
                                        int * public_out_len, 
										char* private_buffer_key, 
										const int private_buffer_len, 
										int *private_out_len);

/********************************************************************************
description:
使用国密 SM2 加密数据
input:
	public_key:      公钥
	public_key_len：  公钥长度 应该是128个字节
	input_data：  需要被加密的数据
	input_length：被加密数据的长度
output:
	out_data：输出的加密数据，需要在外部实现分配好，且长度至少为 input_length + 96
	output_length：输出的加密数据长度
return:
0：成功，非0:失败
********************************************************************************/


CRYPTO_API int EncryptSM2(const char *public_key, const int public_key_len,
		                              const unsigned char *input_data,
		                              const int input_length,
		                              unsigned char *out_data,
		                              int *output_length );


/********************************************************************************
description:
	使用 SM2 解密数据
input:
	private_key : 私钥
	private_key_len : 私钥长度 应该是64个字节

	input_data：需要被解密的数据
	input_length：被解密数据的长度
output:
	out_data：输出的解密后的原始数据，需要在外部实现分配好，且长度至少为 input_length + 96
	output_length：输出的解密数据长度
return:
	0：成功，非0:失败
********************************************************************************/
CRYPTO_API int DecryptSM2(const char *private_key, const  int private_key_len,
		                  const unsigned char *input_data,
		                  const int input_length,
		                  unsigned char *out_data, int *output_length);


/********************************************************************************
description:
	获取 AES 秘钥
input: 
	buffer_len：密钥缓存的长度，即 buff_password 长度
output:
	buff_password：密钥缓存，至少要分配16个字节
return:
	0：失败，非0:最终得到的密码长度
********************************************************************************/
CRYPTO_API unsigned int CreateSecretKey(unsigned char* buff_password, const unsigned int buffer_len);

/********************************************************************************
description:
	创建 SM4 秘钥 128位
input: 
	buffer_len：密钥缓存的长度，即 buff_password 长度
output:
	buff_password：密钥缓存，至少要分配16个字节
return:
	0：失败，非0:最终得到的密码长度
********************************************************************************/
CRYPTO_API unsigned int CreateSecretKeySM4(unsigned char* buff_password, const unsigned int buffer_len);

/********************************************************************************
description:
检查是否为AES加密数据
input:
input_data：需要被检查的数据
input_length：需要被检查数据的长度
output:
return:
0：加密数据，非0:非加密数据
********************************************************************************/
CRYPTO_API int CheckEncryptedAES(const unsigned char *input_data, const int input_length);


/********************************************************************************
description:
	使用 AES 加密数据
input:
	password：密钥
	input_data：需要被加密的数据
	input_length：被加密数据的长度
output:
	out_data：输出的加密数据，需要在外部实现分配好，且长度至少为 input_length + 32
	output_length：输出的加密数据长度
return:
	0：成功，非0:失败
********************************************************************************/
CRYPTO_API int EncryptAES(const unsigned char *password, const unsigned char *input_data, const int input_length, unsigned char *out_data, int *output_length);

/********************************************************************************
description:
	使用 AES 解密数据
input:
	password：密钥
	input_data：需要被解密的数据
	input_length：被解密数据的长度
output:
	out_data：输出的解密后的原始数据，需要在外部实现分配好，且长度至少为 input_length + 32
	output_length：输出的解密数据长度
return:
	0：成功，非0:失败
********************************************************************************/
CRYPTO_API int DecryptAES(const unsigned char *password, const unsigned char *input_data, const int input_length, unsigned char *out_data, int *output_length);

/********************************************************************************
description:
创建 SM4 秘钥 128位， 16个字节
input:
buffer_len：密钥缓存的长度，即 buff_password 长度
output:
buff_password：密钥缓存，至少要分配16个字节
return:
0：失败，非0:最终得到的密码长度
********************************************************************************/
CRYPTO_API unsigned int CreateSecretKeySM4(unsigned char* buff_password, const unsigned int buffer_len);

/********************************************************************************
description:
	检查是否为SM4加密数据
input:
	input_data：需要被检查的数据
	input_length：需要被检查数据的长度
output:
return:
	0：加密数据，非0:非加密数据
********************************************************************************/
CRYPTO_API int CheckEncryptedSM4(const unsigned char *input_data, const size_t input_length);
/********************************************************************************
description:
使用国密 SM4 加密数据
input:
password：密钥
input_data：需要被加密的数据
input_length：被加密数据的长度
output:
out_data：输出的加密数据，需要在外部实现分配好，且长度至少为 input_length + 32
output_length：输出的加密数据长度
return:
0：成功，非0:失败
********************************************************************************/
CRYPTO_API int EncryptSM4(const unsigned char *password, const unsigned char *input_data, const size_t input_length, unsigned char *out_data, int *output_length);

/********************************************************************************
description:
使用国密 SM4 解密数据
input:
password：密钥
input_data：需要被解密的数据
input_length：被解密数据的长度
output:
out_data：输出的解密后的原始数据，需要在外部实现分配好，且长度至少为 input_length + 32
output_length：输出的解密数据长度
return:
0：成功，非0:失败
********************************************************************************/
CRYPTO_API int DecryptSM4(const unsigned char *password, const unsigned char *input_data, const int input_length, unsigned char *out_data, int *output_length);



/********************************************************************************
加密文件选择的类型， 目前支持AES和SM4加密

********************************************************************************/
typedef enum _ENCRYPT_DECRYPT_TYPE
{
	ENCRYPT_AES = 0,
	ENCRYPT_SM4,
	ENCRYPT_NULL = 8
} SR_Encrypt;



/********************************************************************************
description:
    加密文件
input:
      type 加解密的类型
      fr  需要加密的文件
      key 加密所需的密钥
output
	  fw  加密后输出的文件

return:
0：成功，非0:失败
	- 1 参数错误
	- 2 文件不可读写
	- 3 加解密错误
********************************************************************************/
CRYPTO_API int file_encrypt(SR_Encrypt type, const char* fr, const char* fw, const unsigned char * key);
/********************************************************************************
description:
     解密文件
input:
    type 加解密的类型
	fr  需要解密的文件
	key 解密所需的密钥
output
	fw  解密后输出的文件

return:
0：成功，非0:失败
- 1 参数错误
- 2 文件不可读写
- 3 加解密错误
********************************************************************************/
CRYPTO_API int file_decrypt(SR_Encrypt type, const char* fr, const char* fw, const unsigned char * key);


/********************************************************************************
description:
     从配置文件中读入字符串
	 [suirui_key]
	 key=abcdefd
input:
    section 要读入的域[]
	key    要读入的关键字
	default_value  如果没有找到则输出这个default中内容
	file  读取的文件名
output
    value  输出key关键字的内容， 事先分配好
    size   value buffer的大小

return:
	0：成功，非0:失败

********************************************************************************/
CRYPTO_API int SR_ReadIniString(const char *section, const char *key, char *value, int size, const char *default_value, const char *file);

/********************************************************************************
description:
     从配置文件中读入整形
	 [suirui_key]
	 age=1
input:
    section 要读入的域[]
	key    要读入的关键字
	default_value  如果没有找到则输出这个default中内容
	file  读取的文件名


return:
	key中等号后的值
	没找到则返回default_value
********************************************************************************/

CRYPTO_API int SR_ReadIniInt(const char *section, const char *key, int default_value, const char *file);

/********************************************************************************
description:
     往配置文件中写入字符串
	 [suirui_key]
	 key=abcdefd
input:
    section 要写入的域[]
	key    要写入的关键字
	
    value  写入key关键字的内容
	file  读取的文件名
return:
	0：成功，非0:失败
********************************************************************************/
CRYPTO_API int SR_WriteIniString(const char *section, const char *key, const char *value, const char *file);

#ifdef __cplusplus
}; //extern
#endif

#endif		 //AES_SM4_WRAPPER_H						
