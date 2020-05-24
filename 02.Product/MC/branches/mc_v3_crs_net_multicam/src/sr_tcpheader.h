/***********************************************************
 *   �ļ���: sr_tcpheader.h
 *     ��Ȩ: �人������ɽ 2015-2020
 *     ����: Marvin Zhai
 * �ļ�˵��: MC<->Ter MC<->Netmp\DevMgr��TCP��Ϣͷ����
           : ��Ϣͷ�Ĵ���ͽ��
 * ��������: 2015.05.14
 * �޸�����:
 *   �޸���:
 *************************************************************/
#ifndef SRTCPHEADER_H
#define SRTCPHEADER_H

//#include <string.h>
//#include <endian.h>
#include "sr_type.h"

#ifndef SRMC_SRHEADER_PROTOHEADER_DEFINE
#define SRMC_SRHEADER_PROTOHEADER_DEFINE
namespace SRMC{
///////////////////////////////////////TCP��ͷ����/////////////////////////////////////////////
#pragma pack(push)
#pragma pack(1)
/* little endian bit-order */
typedef struct{
	SR_char    m_s; // 'S'
	SR_char    m_r; // 'R'
	SR_char	   m_version;
	SR_char    m_channelid;
	SR_uint16  m_data_len;//����������ͷ����
	SR_uint16  m_cmdtype;//200 201 202 ...
}S_SRMsgHeader;
//��ͬ���͵���Ϣ,���� m_cmdtype ����
#define SRMSG_CMDTYPE_RELAY_TER_MC (200)
#define SRMSG_CMDTYPE_NETMP_MC     (201)
#define SRMSG_CMDTYPE_DevMgr_MC    (202)

/* little endian bit-order */
typedef struct{
	SR_uint64 m_confid;
	SR_uint32 m_userid;
	SR_uint64 m_mcip;//����mc��ַ 2015-10-21
}S_Header_Context;

/* little endian bit-order */
typedef struct{
	SR_uint32  m_msglen; // ������S_ProtoMsgHeader����ͷ����
	SR_uint32  m_msguid; //
}S_ProtoMsgHeader;
#pragma pack(pop)

/////////////////////////////////////////////////////////////////////////////////////////////
}//namespace SRMC
#endif // #ifndef SRMC_SRHEADER_PROTOHEADER_DEFINE

// 2016-06-20 S_Header_Context �� MC ͸��
// MC ֻ֪�� ���� HeaderContext_Size byte ����
// ������������ʲô����
#ifndef HeaderContext_Size
#define HeaderContext_Size (20u)
#endif

#ifndef isHostBigEndian
/* reinterpret_cast <new_type> (expression)
   reinterpret_cast����������������޹�����֮���ת�����������һ���µ�ֵ��
   ���ֵ������ԭʼ������expressoin������ȫ��ͬ�ı���λ��
   ���������½����˸����Ķ���ı���ģ�Ͷ�û�н��ж�����ת����*/
//û�ж����static inline ��ֹ����澯(not use)
#define isHostBigEndian()\
({\
	static const SR_int32 number = 1;\
	static SR_bool ishostBigEnd;\
	ishostBigEnd = (0 == *reinterpret_cast<const char*>(&number));\
	ishostBigEnd;\
})
#endif

#ifndef SR_swap_16
/* ��ת�ֽ���,��˱�С��,С�˱��� */
#define SR_swap_16(x) \
     ((SR_uint16) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))

#define SR_swap_32(x) \
     ( (((x) & 0xff000000u) >> 24) 	 \
     | (((x) & 0x00ff0000u) >>  8)   \
     | (((x) & 0x0000ff00u) <<  8)   \
     | (((x) & 0x000000ffu) << 24))

#define SR_swap_64(x) \
     ( (((x) & 0xff00000000000000ull) >> 56)	\
     | (((x) & 0x00ff000000000000ull) >> 40)	\
     | (((x) & 0x0000ff0000000000ull) >> 24)	\
     | (((x) & 0x000000ff00000000ull) >> 8)	    \
     | (((x) & 0x00000000ff000000ull) << 8)	    \
     | (((x) & 0x0000000000ff0000ull) << 24)	\
     | (((x) & 0x000000000000ff00ull) << 40)	\
     | (((x) & 0x00000000000000ffull) << 56))

#endif
	 
#if 0
extern void unpackSRMsgHeader(const SR_uint8* buf,S_SRMsgHeader* psrh);
extern void packSRMsgHeader(const S_SRMsgHeader* psrh,SR_uint8* buf);
extern void unpackHeaderContext(const SR_uint8* buf,S_Header_Context* phc);
extern void packHeaderContext(const S_Header_Context* phc,SR_uint8* buf);
extern void unpackProtoMsgHeader(const SR_uint8* buf,S_ProtoMsgHeader* pmh);
extern void packProtoMsgHeader(const S_ProtoMsgHeader* pmh,SR_uint8* buf);
#endif

//�ŵ�:��.ȱ��:û�б������ͼ��
//�������ڴ��ص�������
//memcpy(void* dest, const void* src, size_t n)
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define unpackSRMsgHeader(buf,psrh) memcpy(psrh,buf,sizeof(SRMC::S_SRMsgHeader))
#define packSRMsgHeader(psrh,buf) memcpy(buf,psrh,sizeof(SRMC::S_SRMsgHeader))
//#define unpackHeaderContext(buf,phc) memcpy(phc,buf,sizeof(SRMC::S_Header_Context))
//#define packHeaderContext(phc,buf) memcpy(buf,phc,sizeof(SRMC::S_Header_Context))
#define unpackProtoMsgHeader(buf,pmh)memcpy(pmh,buf,sizeof(SRMC::S_ProtoMsgHeader))
#define packProtoMsgHeader(pmh,buf) memcpy(buf,pmh,sizeof(SRMC::S_ProtoMsgHeader))
#else

#error "Host platform is BIG_ENDIAN,Plz Coding pack/unpack.2016-03-24"

#endif // __BYTE_ORDER == __LITTLE_ENDIAN


#endif//#ifndef SRTCPHEADER_H