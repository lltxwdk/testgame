/**
 *   文件名: wrapper_msg.h
 *     版权: 武汉随锐亿山 2015-2020
 *     作者: Marvin Zhai
 * 文件说明: wrapper_msg.cpp 的头文件
 * 创建日期: 2015.05.04
 * 修改日期:
 *   修改人:
 */


#ifndef WRAPPER_MSG_H
#define WRAPPER_MSG_H

#include <typeinfo>
#include <iostream>
#include "device_ser.pb.h"

/*

    1.统一分配 uint32 255(对称模块数)*255(消息号)
    2.可分配64个模块对,每个可用消息号为255个
            mc<->netmp  				--- [0-3]
            mc<->ter    				--- [4-7]
            mc<->conference manager		--- [8-11]
*/

/*
	class ReqMCCreateConf;
	class CmdMCDeleteConf;
	class CmdMCRmConf;
	////////////////////
	class RspCreateConf;
	class IndConfManagerHeartBeatOfMC;

*/


#define Msg_ID_MODULE(a) ((a)<<10)


#define Msg_ID_MC_CM_MODULE  Msg_ID_MODULE(0x0)
#define Msg_ID_MC_CM_BASE_1 (Msg_ID_MC_CM_MODULE|0x000)
#define Msg_ID_MC_CM_BASE_2 (Msg_ID_MC_CM_MODULE|0x100)
#define Msg_ID_MC_CM_BASE_3 (Msg_ID_MC_CM_MODULE|0x200)
#define Msg_ID_MC_CM_BASE_4 (Msg_ID_MC_CM_MODULE|0x300)

#define SRMsgId_ReqMCCreateConf                              (Msg_ID_MC_CM_BASE_1+0x1)
#define SRMsgId_CmdMCDeleteConf                              (Msg_ID_MC_CM_BASE_1+0x2)
#define SRMsgId_CmdMCRmConf                             	 (Msg_ID_MC_CM_BASE_1+0x3)
	////////////////////// 


/*
	class ReqTerJoinConf;
	class ReqConfTerLists;
	class ReqSendAssistVideo;
	class IndTerExitConf;
	class IndTerRaiseHand;
	class IndCloseCamera;
	class IndOpenCamera;
	class IndAssistVideoStart;
	class IndAssistVideoClose;
	class CmdChairDropTer;
	class CmdChairTransfer;
	class CmdChairLockVideo;
	class CmdTerMute;
	class CmdCancelMute;
	class CmdTerSelectVideo;
	class CmdTerSelectVideo_TerIdAndVideoSize;
	class CmdTerAlterSelectVideo;
	class CmdTerAlterSelectVideo_TerIdAndVideoSize;
	class CmdChangeName;
	//////////////////////
	class RspTerJoinConf;
	class RspConfTerLists;
	class RspConfTerLists_TerIdAndName;
	class RspSendAssistVideo;
	class IndNewTerJoinedConf;
	class IndTerLeaveConf;
	class IndNotifyTerHandUp;
	class IndChairTransfer;
	class IndTerMute;
	class IndCancelMute;
	class IndChangeName;
	class IndAssistVideoOn;
	class IndTersCloseCamera;
	class IndTersOpenCamera;
	class CmdStartSendVideo;
	class CmdStopSendVideo;

*/

#define Msg_ID_MC_TER_MODULE Msg_ID_MODULE(0x1)
#define Msg_ID_MC_TER_BASE_1 (Msg_ID_MC_TER_MODULE|0x000)
#define Msg_ID_MC_TER_BASE_2 (Msg_ID_MC_TER_MODULE|0x100)
#define Msg_ID_MC_TER_BASE_3 (Msg_ID_MC_TER_MODULE|0x200)
#define Msg_ID_MC_TER_BASE_4 (Msg_ID_MC_TER_MODULE|0x300)

//Ter->MC
#define SRMsgId_ReqTerJoinConf                                    (Msg_ID_MC_TER_BASE_1+0x1)
#define SRMsgId_ReqConfTerLists                                   (Msg_ID_MC_TER_BASE_1+0x2)
#define SRMsgId_ReqSendAssistVideo								  (Msg_ID_MC_TER_BASE_1+0x3)								
#define SRMsgId_IndTerExitConf									  (Msg_ID_MC_TER_BASE_1+0x4)
#define SRMsgId_IndTerRaiseHand                                   (Msg_ID_MC_TER_BASE_1+0x5)
#define SRMsgId_IndCloseCamera 									  (Msg_ID_MC_TER_BASE_1+0x6)			
#define SRMsgId_IndOpenCamera                                     (Msg_ID_MC_TER_BASE_1+0x7)
#define SRMsgId_IndAssistVideoStart                               (Msg_ID_MC_TER_BASE_1+0x8)
#define SRMsgId_IndAssistVideoClose                               (Msg_ID_MC_TER_BASE_1+0x9)
#define SRMsgId_CmdChairDropTer                                   (Msg_ID_MC_TER_BASE_1+0xA)
#define SRMsgId_CmdChairTransfer                                  (Msg_ID_MC_TER_BASE_1+0xB)
#define SRMsgId_CmdChairLockVideo                                 (Msg_ID_MC_TER_BASE_1+0xC)
#define SRMsgId_CmdTerMute                                        (Msg_ID_MC_TER_BASE_1+0xD)
#define SRMsgId_CmdCancelMute                                     (Msg_ID_MC_TER_BASE_1+0xE)
#define SRMsgId_CmdTerSelectVideo                                 (Msg_ID_MC_TER_BASE_1+0xF)
#define SRMsgId_CmdTerSelectVideo_TerIdAndVideoSize               (Msg_ID_MC_TER_BASE_1+0x10)
#define SRMsgId_CmdTerAlterSelectVideo                            (Msg_ID_MC_TER_BASE_1+0x11)
#define SRMsgId_CmdTerAlterSelectVideo_TerIdAndVideoSize          (Msg_ID_MC_TER_BASE_1+0x12)
#define SRMsgId_CmdChangeName                                     (Msg_ID_MC_TER_BASE_1+0x13)  
#define SRMsgId_CmdChairUnLockVideo                               (Msg_ID_MC_TER_BASE_1+0x14)
#define SRMsgId_IndTerBeatToMC									  (Msg_ID_MC_TER_BASE_1+0x15)
#define SRMsgId_IndTerSendLimit                                   (Msg_ID_MC_TER_BASE_1+0x16)
////////////////////// 
//MC->Ter
#define SRMsgId_RspTerJoinConf                                    (Msg_ID_MC_TER_BASE_2+0x1)                    
#define SRMsgId_RspConfTerLists                                   (Msg_ID_MC_TER_BASE_2+0x2)
#define SRMsgId_RspConfTerLists_TerIdAndName                      (Msg_ID_MC_TER_BASE_2+0x3)
#define SRMsgId_RspSendAssistVideo                                (Msg_ID_MC_TER_BASE_2+0x4)
#define SRMsgId_IndNewTerJoinedConf                               (Msg_ID_MC_TER_BASE_2+0x5)
#define SRMsgId_IndTerLeaveConf                                   (Msg_ID_MC_TER_BASE_2+0x6)
#define SRMsgId_IndNotifyTerHandUp                                (Msg_ID_MC_TER_BASE_2+0x7)
#define SRMsgId_IndChairTransfer                                  (Msg_ID_MC_TER_BASE_2+0x8)
#define SRMsgId_IndTerMute                                        (Msg_ID_MC_TER_BASE_2+0x9)
#define SRMsgId_IndCancelMute                                     (Msg_ID_MC_TER_BASE_2+0xA)
#define SRMsgId_IndChangeName                                     (Msg_ID_MC_TER_BASE_2+0xB)
#define SRMsgId_IndAssistVideoOn								  (Msg_ID_MC_TER_BASE_2+0xC)
#define SRMsgId_IndTersCloseCamera                                (Msg_ID_MC_TER_BASE_2+0xD)
#define SRMsgId_IndTersOpenCamera                                 (Msg_ID_MC_TER_BASE_2+0xE)
#define SRMsgId_CmdStartSendVideo                                 (Msg_ID_MC_TER_BASE_2+0xF)
#define SRMsgId_CmdStopSendVideo                                  (Msg_ID_MC_TER_BASE_2+0x10)
#define SRMsgId_IndAssistVideoOff								  (Msg_ID_MC_TER_BASE_2+0x11)
#define SRMsgId_IndChairLock                                      (Msg_ID_MC_TER_BASE_2+0x12)
#define SRMsgId_IndChairUnLock                                    (Msg_ID_MC_TER_BASE_2+0x13)
#define SRMsgId_IndMCBeatToTer									  (Msg_ID_MC_TER_BASE_2+0x14)
#define SRMsgId_IndSendLimits                                     (Msg_ID_MC_TER_BASE_2+0x15)
/*
	class ReqNetMPCreateConf;
	class IndDeleteConf;
	class IndNetMPTerJoined;
	class IndNetMPTerLeft;
	class CmdNetMPTerVideoSelect;
	class CmdNetMPTerVideoSelect_TerVideoSend;
	class CmdNetMPStopVideoSelect;
	class CmdNetMPStopVideoSelect_TerVideoSend;
	class CmdNetMPConfDestroy;
	class IndNetMPHeartBeatOfMC;
	
///////////////////////////////////////	
	class RspMCConfCreate;
	class IndMCHeartBeatOfNetMP;
*/

//MC-NetMp
#define Msg_ID_MC_NETMP_MODULE Msg_ID_MODULE(0x2)
#define Msg_ID_MC_NETMP_BASE_1 (Msg_ID_MC_NETMP_MODULE|0x000)
#define Msg_ID_MC_NETMP_BASE_2 (Msg_ID_MC_NETMP_MODULE|0x100)
#define Msg_ID_MC_NETMP_BASE_3 (Msg_ID_MC_NETMP_MODULE|0x200)
#define Msg_ID_MC_NETMP_BASE_4 (Msg_ID_MC_NETMP_MODULE|0x300)

#define SRMsgId_ReqNetMPCreateConf                              (Msg_ID_MC_NETMP_BASE_1+0x1)
#define SRMsgId_IndDeleteCon                                    (Msg_ID_MC_NETMP_BASE_1+0x2)
#define SRMsgId_IndNetMPTerJoined                               (Msg_ID_MC_NETMP_BASE_1+0x3)
#define SRMsgId_IndNetMPTerLeft                                 (Msg_ID_MC_NETMP_BASE_1+0x4)
#define SRMsgId_CmdNetMPTerVideoSelect                          (Msg_ID_MC_NETMP_BASE_1+0x5)
#define SRMsgId_CmdNetMPTerVideoSelect_TerVideoSend             (Msg_ID_MC_NETMP_BASE_1+0x6)
#define SRMsgId_CmdNetMPStopVideoSelect_TerVideoSend            (Msg_ID_MC_NETMP_BASE_1+0x7)
#define SRMsgId_CmdNetMPConfDestroy                             (Msg_ID_MC_NETMP_BASE_1+0x8)
#define SRMsgId_IndNetMPHeartBeatOfMC                           (Msg_ID_MC_NETMP_BASE_1+0x9)
///////////////////////////////////////	
#define SRMsgId_RspMCConfCreate                                 (Msg_ID_MC_NETMP_BASE_2+0x1)
#define SRMsgId_IndMCHeartBeatOfNetMP                           (Msg_ID_MC_NETMP_BASE_2+0x2)

//Device_SERVER
#define Msg_ID_DEVICE_SERVER_MODULE  Msg_ID_MODULE(0x3)
#define Msg_ID_DEVICE_SERVER_BASE_1 (Msg_ID_DEVICE_SERVER_MODULE|0x000)
#define Msg_ID_DEVICE_SERVER_BASE_2 (Msg_ID_DEVICE_SERVER_MODULE|0x100)
#define Msg_ID_DEVICE_SERVER_BASE_3 (Msg_ID_DEVICE_SERVER_MODULE|0x200)
#define Msg_ID_DEVICE_SERVER_BASE_4 (Msg_ID_DEVICE_SERVER_MODULE|0x300)

/* 0xC00 MC,NetMP Relaymc-> Device Manager */
#define SRMsgId_ReqRegister				                   		(Msg_ID_DEVICE_SERVER_BASE_1+0x1)
#define SRMsgId_ReqUnRegister				                   	(Msg_ID_DEVICE_SERVER_BASE_1+0x2)
#define SRMsgId_ReqGetDeviceInfo				                (Msg_ID_DEVICE_SERVER_BASE_1+0x3)
#define SRMsgId_IndNewTermJoinConf				       			(Msg_ID_DEVICE_SERVER_BASE_1+0x4)
#define SRMsgId_IndTermLeaveConf				       			(Msg_ID_DEVICE_SERVER_BASE_1+0x5)
//#define SRMsgId_RspCreateConf				                   	(Msg_ID_DEVICE_SERVER_BASE_1+0x6)
#define SRMsgId_IndsertodevHeart				                (Msg_ID_DEVICE_SERVER_BASE_1+0x7)
//#define SRMsgId_IndCreateConf						   			(Msg_ID_DEVICE_SERVER_BASE_1+0x8)
//#define SRMsgId_IndEndConf                                    (Msg_ID_DEVICE_SERVER_BASE_1+0x9)
#define SRMsgId_ReqUpdateDeviceInfo                             (Msg_ID_DEVICE_SERVER_BASE_1+0xA)
#define SRMsgId_ReqGetSystemCurLoad                             (Msg_ID_DEVICE_SERVER_BASE_1+0xB)
#define SRMsgId_IndNetMPConfInfoInMC                            (Msg_ID_DEVICE_SERVER_BASE_1+0x10)
// IndCreateConf-->改为 ReqConfInfoFromDevMgr
// RspCreateConf-->改为 IndMCCreateConf
// IndEndConf   -->改为 IndMCEndConf
#define SRMsgId_ReqConfInfoFromDevMgr                           (Msg_ID_DEVICE_SERVER_BASE_1+0x11)
#define SRMsgId_IndMCCreateConf	                                (Msg_ID_DEVICE_SERVER_BASE_1+0x12)
#define SRMsgId_IndMCEndConf                                    (Msg_ID_DEVICE_SERVER_BASE_1+0x13)
#define SRMsgId_IndNetMPConnStatusInMC                          (Msg_ID_DEVICE_SERVER_BASE_1+0x14)
#define SRMsgId_IndCRSStartRec                                  (Msg_ID_DEVICE_SERVER_BASE_1+0x15)
#define SRMsgId_IndCRSStopRec									(Msg_ID_DEVICE_SERVER_BASE_1+0x16)
#define SRMsgId_IndCRSFileStoragePath							(Msg_ID_DEVICE_SERVER_BASE_1+0x17)
#define SRMsgId_IndCRSStartLive									(Msg_ID_DEVICE_SERVER_BASE_1+0x18)
#define SRMsgId_IndCRSStopLive 									(Msg_ID_DEVICE_SERVER_BASE_1+0x19)
#define SRMsgId_IndSubSvrInfoToDevmgr							(Msg_ID_DEVICE_SERVER_BASE_1+0x1A)
#define SRMsgId_IndSubSvrHeartTodev							    (Msg_ID_DEVICE_SERVER_BASE_1+0x1B)
#define SRMsgId_ReqGetSysDeviceInfo				                (Msg_ID_DEVICE_SERVER_BASE_1+0x1C)
#define SRMsgId_IndModifyConfCallList			                (Msg_ID_DEVICE_SERVER_BASE_1+0x1D)
#define SRMsgId_IndTerStatisticsInfo			                (Msg_ID_DEVICE_SERVER_BASE_1+0x1E)


/* 0xD00 Device Manager -> MC,NetMP Relaymc*/
//#define SRMsgId_ReqCreateConf				                    (Msg_ID_DEVICE_SERVER_BASE_2+0x1)
#define SRMsgId_IndDevtoserHeart				                (Msg_ID_DEVICE_SERVER_BASE_2+0x2)
#define SRMsgId_RspRegister				                    	(Msg_ID_DEVICE_SERVER_BASE_2+0x3)
#define SRMsgId_RspUnRegister				                    (Msg_ID_DEVICE_SERVER_BASE_2+0x4)
#define SRMsgId_RspGetDeviceInfo				                (Msg_ID_DEVICE_SERVER_BASE_2+0x5)
//#define SRMsgId_IndCreateConfFail				                (Msg_ID_DEVICE_SERVER_BASE_2+0x6)
#define SRMsgId_RspUpdateDeviceInfo                             (Msg_ID_DEVICE_SERVER_BASE_2+0x7)
// ReqCreateConf-->改为 RspConfInfoToMC
// IndCreateConfFail , ReqCreateConf 合并为一条消息 RspConfInfoToMC -->即删除 IndCreateConfFail 
#define SRMsgId_RspConfInfoToMC                                 (Msg_ID_DEVICE_SERVER_BASE_2+0x8)
#define SRMsgId_RspGetSystemCurLoad                             (Msg_ID_DEVICE_SERVER_BASE_2+0x9)
#define SRMsgId_IndUpSvrInfoToDevmgr                            (Msg_ID_DEVICE_SERVER_BASE_2+0xA)
#define SRMsgId_IndUpSvrHeartTodev                              (Msg_ID_DEVICE_SERVER_BASE_2+0xB)
#define SRMsgId_RspGetSysDeviceInfo				                (Msg_ID_DEVICE_SERVER_BASE_2+0xC)
#define SRMsgId_CmdGWOPMonitor   				                (Msg_ID_DEVICE_SERVER_BASE_2+0xD)
#define SRMsgId_CmdGWOPInvitPart 				                (Msg_ID_DEVICE_SERVER_BASE_2+0x10)

#define getMsgIdByClassName(className) SRMsgId_##className

#define getClassNameByMsgId(msgId) 




namespace SRMC{

/* little endian bit-order */
typedef struct{
	int  m_msglen; /* 不包含头 */
	int  m_msguid;
}S_ProtoMsgHeader;

typedef enum{
	S_BIG_ENDIAN,
	S_LITTLE_ENDIAN
}E_Endian;

class MsgNode{
public:
    MsgNode()
   	{
   		m_msg =(google::protobuf::Message*)0;
		m_header.m_msglen = 0;
		m_header.m_msguid = 0;
	}
	
    ~MsgNode(){} /* 不删除m_msg 指向的内存 */
	google::protobuf::Message* m_msg;
	S_ProtoMsgHeader m_header;	
};
	

/* 运行时获取主机(本端)字节序 */
E_Endian getHostEndian(void);
void unpackPMH(const char* buf,S_ProtoMsgHeader* pmh);
void packPMH(const S_ProtoMsgHeader* pmh, char* buf);


}

namespace SRRELAY{

#define SR_swap_16(x) \
     ((short) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))


#define SR_swap_32(x) \
     ( (((x) & 0xff000000) >> 24) 	\
     | (((x) & 0x00ff0000) >>  8)   \
     | (((x) & 0x0000ff00) <<  8)   \
     | (((x) & 0x000000ff) << 24))


#define SR_swap_64(x) \
      ( (((x) & 0xff00000000000000ull) >> 56)	\
      | (((x) & 0x00ff000000000000ull) >> 40)	\
      | (((x) & 0x0000ff0000000000ull) >> 24)	\
      | (((x) & 0x000000ff00000000ull) >> 8)	\
      | (((x) & 0x00000000ff000000ull) << 8)	\
      | (((x) & 0x0000000000ff0000ull) << 24)	\
      | (((x) & 0x000000000000ff00ull) << 40)	\
      | (((x) & 0x00000000000000ffull) << 56))

#pragma pack(1)
/* little endian bit-order */
typedef struct{
	char    m_s; // 'S'
	char    m_r; // 'R'
	char	m_version;
	char    m_channelid;
	short   m_data_len;
	short   m_cmdtype;
}S_RelayMsgHeader;
#pragma pack()

typedef enum{
	S_BIG_ENDIAN = 0,
	S_LITTLE_ENDIAN
}E_Endian;

class MsgNode{
public:
    MsgNode()
   	{
   		m_msg =(google::protobuf::Message*)0;
	}
	
    ~MsgNode(){} /* 不删除m_msg 指向的内存 */
	google::protobuf::Message* m_msg;
	S_RelayMsgHeader m_header;	
};
	

/* 运行时获取主机(本端)字节序 */
E_Endian getHostEndian(void);
void unpackPRH(const char* buf,S_RelayMsgHeader* prh);
void packPRH(const S_RelayMsgHeader* prh, char* buf);

}

namespace DEVICE_SERVER
{
	//设备类型
	typedef enum tagDeviceType
	{
		DEVICE_MC = 1,
		DEVICE_NETMP = 2,
		DEVICE_RELAY_MC = 3,
		DEVICE_RELAYSERVER = 4,
		DEVICE_MP = 5,
		DEVICE_GW = 6,
		DEVICE_DEV = 7,
		DEVICE_RMS = 8,
		DEVICE_CRS = 9,
		DEVICE_SRS = 10,
		DEVICE_STUNSERVER = 11
	}DeviceType;
}

#endif
