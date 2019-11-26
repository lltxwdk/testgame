/***********************************************************
 *   文件名: proto_msgid_define.h
 *     版权: 随锐科技股份有限公司 2015-2020
 *     作者: Marvin Zhai
 * 文件说明: 定义公共消息
 * 创建日期: 2015.05.04
 * 修改日期:
 *   修改人:
 *************************************************************/

#ifndef COMMON_MSGID_DEFINE_H
#define COMMON_MSGID_DEFINE_H

#define Msg_ID_MODULE(a) ((a)<<10)

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
#define SRMsgId_RspCreateConf				                   	(Msg_ID_DEVICE_SERVER_BASE_1+0x6)
#define SRMsgId_IndsertodevHeart				                (Msg_ID_DEVICE_SERVER_BASE_1+0x7)
#define SRMsgId_IndCreateConf						   			(Msg_ID_DEVICE_SERVER_BASE_1+0x8)
#define SRMsgId_IndEndConf                                      (Msg_ID_DEVICE_SERVER_BASE_1+0x9)
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
#define SRMsgId_IndVASAnalyVideoState   		                (Msg_ID_DEVICE_SERVER_BASE_1+0x1F)
#define SRMsgId_IndVASAnalyVideoResult  		                (Msg_ID_DEVICE_SERVER_BASE_1+0x20)


/* 0xD00 Device Manager -> MC,NetMP Relaymc*/
#define SRMsgId_ReqCreateConf				                    (Msg_ID_DEVICE_SERVER_BASE_2+0x1)
#define SRMsgId_IndDevtoserHeart				                (Msg_ID_DEVICE_SERVER_BASE_2+0x2)
#define SRMsgId_RspRegister				                    	(Msg_ID_DEVICE_SERVER_BASE_2+0x3)
#define SRMsgId_RspUnRegister				                    (Msg_ID_DEVICE_SERVER_BASE_2+0x4)
#define SRMsgId_RspGetDeviceInfo				                (Msg_ID_DEVICE_SERVER_BASE_2+0x5)
#define SRMsgId_IndCreateConfFail				                (Msg_ID_DEVICE_SERVER_BASE_2+0x6)
#define SRMsgId_RspUpdateDeviceInfo                             (Msg_ID_DEVICE_SERVER_BASE_2+0x7)
// ReqCreateConf-->改为 RspConfInfoToMC
// IndCreateConfFail , ReqCreateConf 合并为一条消息 RspConfInfoToMC -->即删除 IndCreateConfFail 
#define SRMsgId_RspConfInfoToMC                                 (Msg_ID_DEVICE_SERVER_BASE_2+0x8)
#define SRMsgId_RspGetSystemCurLoad                             (Msg_ID_DEVICE_SERVER_BASE_2+0x9)
#define SRMsgId_IndUpSvrInfoToDevmgr                            (Msg_ID_DEVICE_SERVER_BASE_2+0xA)
#define SRMsgId_IndUpSvrHeartTodev                              (Msg_ID_DEVICE_SERVER_BASE_2+0xB)
#define SRMsgId_RspGetSysDeviceInfo				                (Msg_ID_DEVICE_SERVER_BASE_2+0xC)
#define SRMsgId_CmdGWOPMonitor   				                (Msg_ID_DEVICE_SERVER_BASE_2+0xD)
#define SRMsgId_CmdGWOPAVConfig   				                (Msg_ID_DEVICE_SERVER_BASE_2+0xE)

#define Msg_ID_MC_TER_MODULE Msg_ID_MODULE(0x1)
#define Msg_ID_MC_TER_BASE_1 (Msg_ID_MC_TER_MODULE|0x000)
#define Msg_ID_MC_TER_BASE_2 (Msg_ID_MC_TER_MODULE|0x100)
#define Msg_ID_MC_TER_BASE_3 (Msg_ID_MC_TER_MODULE|0x200)
#define Msg_ID_MC_TER_BASE_4 (Msg_ID_MC_TER_MODULE|0x300)
/* 0x400 Ter->MC */
#define MC_TER_PROTO_VER 201808101949

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
//#define SRMsgId_CmdTerSelectVideo_TerIdAndVideoSize             (Msg_ID_MC_TER_BASE_1+0x10)
//#define SRMsgId_CmdTerAlterSelectVideo                          (Msg_ID_MC_TER_BASE_1+0x11)
//#define SRMsgId_CmdTerAlterSelectVideo_TerIdAndVideoSize        (Msg_ID_MC_TER_BASE_1+0x12)
#define SRMsgId_CmdChangeName                                     (Msg_ID_MC_TER_BASE_1+0x13)
#define SRMsgId_CmdChairUnLockVideo                               (Msg_ID_MC_TER_BASE_1+0x14)
#define SRMsgId_IndTerBeatToMC                                    (Msg_ID_MC_TER_BASE_1+0x15)
#define SRMsgId_IndTerSendLimit                                   (Msg_ID_MC_TER_BASE_1+0x16)
#define SRMsgId_CmdChairEndConf                                   (Msg_ID_MC_TER_BASE_1+0x17)
#define SRMsgId_IndP2PInfoToMC									  (Msg_ID_MC_TER_BASE_1+0x18)
#define SRMsgId_CmdTextToOtherTer                                 (Msg_ID_MC_TER_BASE_1+0x19)
#define SRMsgId_CmdMCStartRec									  (Msg_ID_MC_TER_BASE_1+0x1A)
#define SRMsgId_CmdMCStopRec									  (Msg_ID_MC_TER_BASE_1+0x1B)
#define SRMsgId_CmdMCStartLive									  (Msg_ID_MC_TER_BASE_1+0x1C)
#define SRMsgId_CmdMCStopLive									  (Msg_ID_MC_TER_BASE_1+0x1D)
#define SRMsgId_IndLiveSettingToMC								  (Msg_ID_MC_TER_BASE_1+0x1E)
#define SRMsgId_CmdMCSyncMonitorInfo							  (Msg_ID_MC_TER_BASE_1+0x1F)
#define SRMsgId_CmdSendAssistVideolabel                           (Msg_ID_MC_TER_BASE_1+0x20)
#define SRMsgId_CmdAssistVideolabelClear						  (Msg_ID_MC_TER_BASE_1+0x21)
#define SRMsgId_RspSendAssistVideoProxy						      (Msg_ID_MC_TER_BASE_1+0x22)
#define SRMsgId_CmdChairMuteAll     						      (Msg_ID_MC_TER_BASE_1+0x23)
#define SRMsgId_CmdChairMixSoundAll 						      (Msg_ID_MC_TER_BASE_1+0x24)
#define SRMsgId_CmdChairLockConf    						      (Msg_ID_MC_TER_BASE_1+0x25)
#define SRMsgId_CmdChairUnLockConf						          (Msg_ID_MC_TER_BASE_1+0x26)
#define SRMsgId_IndTermStatusToMC                                 (Msg_ID_MC_TER_BASE_1+0x27)
#define SRMsgId_IndSendAssistVideoRelease                         (Msg_ID_MC_TER_BASE_1+0x28)
#define SRMsgId_ReqMCCreateScreen	                         	  (Msg_ID_MC_TER_BASE_1+0x29)
#define SRMsgId_CmdMCQueryMonitorRec  							  (Msg_ID_MC_TER_BASE_1+0x2A)
#define SRMsgId_CmdMsgToOtherTer  							      (Msg_ID_MC_TER_BASE_1+0x2B)
#define SRMsgId_CmdMCPlaybackMonitorRec							  (Msg_ID_MC_TER_BASE_1+0x2C)
#define SRMsgId_IndTerPlaybackMonitorRecStateToMC				  (Msg_ID_MC_TER_BASE_1+0x2D)
#define SRMsgId_CmdMCSetMultiScreen                         	  (Msg_ID_MC_TER_BASE_1+0x30)
#define SRMsgId_CmdMCAddSubtitle                         		  (Msg_ID_MC_TER_BASE_1+0x31)
#define SRMsgId_CmdMCStopSubtitle                         		  (Msg_ID_MC_TER_BASE_1+0x32)
#define SRMsgId_RspTerCreateScreen                         		  (Msg_ID_MC_TER_BASE_1+0x33)
#define SRMsgId_IndStdTerSendMediaFormat						  (Msg_ID_MC_TER_BASE_1+0x34)
#define SRMsgId_IndStdTerRecvMediaFormat						  (Msg_ID_MC_TER_BASE_1+0x35)
#define SRMsgId_CmdMCDestoryScreen								  (Msg_ID_MC_TER_BASE_1+0x36)
#define SRMsgId_CmdTerSelectAssistVideo							  (Msg_ID_MC_TER_BASE_1+0x37)
#define SRMsgId_IndCCSBeatToMC								      (Msg_ID_MC_TER_BASE_1+0x38)
#define SRMsgId_ReqConfStatus                                     (Msg_ID_MC_TER_BASE_1+0x39)
#define SRMsgId_CmdMCTransferSpecialtype					      (Msg_ID_MC_TER_BASE_1+0x3A)
#define SRMsgId_CmdPermissionRaiseHand  					      (Msg_ID_MC_TER_BASE_1+0x3B)
#define SRMsgId_CmdMCPutAllHandDown 						      (Msg_ID_MC_TER_BASE_1+0x3C)
#define SRMsgId_CmdChangeConfCryptoKey			                  (Msg_ID_MC_TER_BASE_1+0x3D)
#define SRMsgId_CmdForceMute                                      (Msg_ID_MC_TER_BASE_1+0x40)
#define SRMsgId_ReqLayoutInfo                                     (Msg_ID_MC_TER_BASE_1+0x41)
#define SRMsgId_CmdChangeConfMode                                 (Msg_ID_MC_TER_BASE_1+0x42)
#define SRMsgId_CmdSetStdTerSeeingStyle                           (Msg_ID_MC_TER_BASE_1+0x43)
#define SRMsgId_CmdOpenCamera									  (Msg_ID_MC_TER_BASE_1+0x44)
#define SRMsgId_CmdCloseCamera									  (Msg_ID_MC_TER_BASE_1+0x45)
#define SRMsgId_CmdMCAddParticipants							  (Msg_ID_MC_TER_BASE_1+0x46)
#define SRMsgId_CmdMCDelParticipants							  (Msg_ID_MC_TER_BASE_1+0x47)
#define SRMsgId_CmdMCInvitParticipants							  (Msg_ID_MC_TER_BASE_1+0x48)
#define SRMsgId_CmdMCDisplayTername							      (Msg_ID_MC_TER_BASE_1+0x49)
#define SRMsgId_CmdMCHideTername								  (Msg_ID_MC_TER_BASE_1+0x50)
#define SRMsgId_CmdSetWhitelist									  (Msg_ID_MC_TER_BASE_1+0x51)
#define SRMsgId_CmdSetChairman									  (Msg_ID_MC_TER_BASE_1+0x52)
#define SRMsgId_IndExitGroupMeetingRoom							  (Msg_ID_MC_TER_BASE_1+0x53)
#define SRMsgId_IndGroupMeetingRoomTerBeatToMC					  (Msg_ID_MC_TER_BASE_1+0x54)
#define SRMsgId_CmdSetCallWaitingState    					      (Msg_ID_MC_TER_BASE_1+0x55)
#define SRMsgId_CmdTerJoinGroupMeetingRoom					      (Msg_ID_MC_TER_BASE_1+0x56)
#define SRMsgId_CmdTerExitGroupMeetingRoom					      (Msg_ID_MC_TER_BASE_1+0x57)
#define SRMsgId_ReqConfGroupMeetingRoomTerLists				      (Msg_ID_MC_TER_BASE_1+0x58)
#define SRMsgId_ReqConfRollCallInfo				                  (Msg_ID_MC_TER_BASE_1+0x59)
#define SRMsgId_CmdMCSetConfRollCallList				          (Msg_ID_MC_TER_BASE_1+0x5A)
#define SRMsgId_CmdMCSetConfRollCallState		                  (Msg_ID_MC_TER_BASE_1+0x5B)
#define SRMsgId_CmdMCRollCallTer    			                  (Msg_ID_MC_TER_BASE_1+0x5C)
#define SRMsgId_CmdMCRollCallTerResult			                  (Msg_ID_MC_TER_BASE_1+0x5D)
#define SRMsgId_IndNewAudioSourceJoined			                  (Msg_ID_MC_TER_BASE_1+0x5E)
#define SRMsgId_IndAudioSourceLeave 			                  (Msg_ID_MC_TER_BASE_1+0x5F)
#define SRMsgId_IndNewVideoSourceJoined			                  (Msg_ID_MC_TER_BASE_1+0x60)
#define SRMsgId_IndVideoSourceLeave 			                  (Msg_ID_MC_TER_BASE_1+0x61)
#define SRMsgId_CmdChangeVideoSourceName   		                  (Msg_ID_MC_TER_BASE_1+0x62)
#define SRMsgId_CmdChangeVideoSourcePriority	                  (Msg_ID_MC_TER_BASE_1+0x63)

#define SRMsgId_CmdMCSetAnaly        			                  (Msg_ID_MC_TER_BASE_1+0x70)
#define SRMsgId_CmdMCStartAnaly     			                  (Msg_ID_MC_TER_BASE_1+0x71)
#define SRMsgId_CmdMCStopAnaly       			                  (Msg_ID_MC_TER_BASE_1+0x72)
#define SRMsgId_CmdMCUpdateAnaly      			                  (Msg_ID_MC_TER_BASE_1+0x73)

#define SRMsgId_CmdChangePreset     			                  (Msg_ID_MC_TER_BASE_1+0x74)
#define SRMsgId_CmdFilterNoVideo     			                  (Msg_ID_MC_TER_BASE_1+0x75)
#define SRMsgId_CmdMCUpdateAutoPollInfo							  (Msg_ID_MC_TER_BASE_1+0x76)
#define SRMsgId_CmdMCUpdateParticipantsOrder					  (Msg_ID_MC_TER_BASE_1+0x77)

////////////////////// 
/* 0x500 MC->Ter */
#define TER_MC_PROTO_VER 201808101949

#define SRMsgId_RspTerJoinConf                                    (Msg_ID_MC_TER_BASE_2+0x1)                    
#define SRMsgId_RspConfTerLists                                   (Msg_ID_MC_TER_BASE_2+0x2)
//#define SRMsgId_RspConfTerLists_TerIdAndName                    (Msg_ID_MC_TER_BASE_2+0x3)
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
#define SRMsgId_IndAssistVideoOff                                 (Msg_ID_MC_TER_BASE_2+0x11)
#define SRMsgId_IndChairLock                                      (Msg_ID_MC_TER_BASE_2+0x12)
#define SRMsgId_IndChairUnLock                                    (Msg_ID_MC_TER_BASE_2+0x13)
#define SRMsgId_IndMCBeatToTer                                    (Msg_ID_MC_TER_BASE_2+0x14)
#define SRMsgId_IndSendLimits                                     (Msg_ID_MC_TER_BASE_2+0x15)
#define SRMsgId_IndChairEndConf                                   (Msg_ID_MC_TER_BASE_2+0x16)
#define SRMsgId_CmdReOpenMediaPort                                (Msg_ID_MC_TER_BASE_2+0x17)
#define SRMsgId_IndP2PInfoToTer									  (Msg_ID_MC_TER_BASE_2+0x18)
#define SRMsgId_CmdTerStartP2P									  (Msg_ID_MC_TER_BASE_2+0x19)
#define SRMsgId_CmdTerStopP2P									  (Msg_ID_MC_TER_BASE_2+0x1A)
#define SRMsgId_IndTerSpecialtypeTransfer						  (Msg_ID_MC_TER_BASE_2+0x1B)
#define SRMsgId_IndLiveSettingChanged						  	  (Msg_ID_MC_TER_BASE_2+0x1C)
#define SRMsgId_IndPermissionTerRaiseHand					  	  (Msg_ID_MC_TER_BASE_2+0x1D)
#define SRMsgId_IndTerPutAllHandDown						  	  (Msg_ID_MC_TER_BASE_2+0x1E)
#define SRMsgId_IndAssistVideoTokenOwner					  	  (Msg_ID_MC_TER_BASE_2+0x1F)
#define SRMsgId_IndTextMsgToTers                                  (Msg_ID_MC_TER_BASE_2+0x20)
#define SRMsgId_IndSendAssistVideolabel                           (Msg_ID_MC_TER_BASE_2+0x21)
#define SRMsgId_IndAssistVideolabelClear						  (Msg_ID_MC_TER_BASE_2+0x22)
#define SRMsgId_ReqSendAssistVideoProxy						      (Msg_ID_MC_TER_BASE_2+0x23)
#define SRMsgId_IndTerMuteAll       						      (Msg_ID_MC_TER_BASE_2+0x24)
#define SRMsgId_IndTerMixSoundAll  			    			      (Msg_ID_MC_TER_BASE_2+0x25)
#define SRMsgId_IndTermStatusToTerm                               (Msg_ID_MC_TER_BASE_2+0x26)
#define SRMsgId_IndMPIInfoToTer									  (Msg_ID_MC_TER_BASE_2+0x27)
#define SRMsgId_IndScreenInfoToTer								  (Msg_ID_MC_TER_BASE_2+0x28)
#define SRMsgId_IndConfAllMPIInfoToTer							  (Msg_ID_MC_TER_BASE_2+0x29)
#define SRMsgId_IndTerCRSLiveState							      (Msg_ID_MC_TER_BASE_2+0x2A)
#define SRMsgId_IndMsgToTer                                       (Msg_ID_MC_TER_BASE_2+0x2B)
#define SRMsgId_IndTerQueryMonitorRec                             (Msg_ID_MC_TER_BASE_2+0x2C)
#define SRMsgId_IndTerPlaybackMonitorRecState                     (Msg_ID_MC_TER_BASE_2+0x2D)
#define SRMsgId_CmdTerPlaybackMonitorRec                          (Msg_ID_MC_TER_BASE_2+0x2E)
#define SRMsgId_IndMCBeatToCCS									  (Msg_ID_MC_TER_BASE_2+0x30)
#define SRMsgId_IndMCStartConfToCCS								  (Msg_ID_MC_TER_BASE_2+0x31)
#define SRMsgId_IndMCEndConfToCCS								  (Msg_ID_MC_TER_BASE_2+0x32)
#define SRMsgId_RspConfStatus                                     (Msg_ID_MC_TER_BASE_2+0x33)
#define SRMsgId_IndForceMute                                      (Msg_ID_MC_TER_BASE_2+0x34)
#define SRMsgId_IndAllConfToCCS								      (Msg_ID_MC_TER_BASE_2+0x35)
#define SRMsgId_RspLayoutInfo								      (Msg_ID_MC_TER_BASE_2+0x36)
#define SRMsgId_CmdTerSetMultiScreen							  (Msg_ID_MC_TER_BASE_2+0x37)
#define SRMsgId_IndLayoutInfo									  (Msg_ID_MC_TER_BASE_2+0x38)
#define SRMsgId_IndChangeConfMode								  (Msg_ID_MC_TER_BASE_2+0x39)
#define SRMsgId_IndTerChangeConfCryptoKey   					  (Msg_ID_MC_TER_BASE_2+0x3A)
#define SRMsgId_IndStdTerSeeingStyle							  (Msg_ID_MC_TER_BASE_2+0x40)
#define SRMsgId_CmdUpdatePaticipants							  (Msg_ID_MC_TER_BASE_2+0x41)
#define SRMsgId_IndTerAddSubtitle								  (Msg_ID_MC_TER_BASE_2+0x42)
#define SRMsgId_IndTerStopSubtitle							      (Msg_ID_MC_TER_BASE_2+0x43)
#define SRMsgId_IndTerDisplayOtherTername						  (Msg_ID_MC_TER_BASE_2+0x44)
#define SRMsgId_IndTerHideOtherTername							  (Msg_ID_MC_TER_BASE_2+0x45)
#define SRMsgId_IndChairLockConf								  (Msg_ID_MC_TER_BASE_2+0x46)
#define SRMsgId_IndWhitelistInfo								  (Msg_ID_MC_TER_BASE_2+0x47)
#define SRMsgId_IndTerInWhitelistState							  (Msg_ID_MC_TER_BASE_2+0x48)
#define SRMsgId_IndTerCRSRecState							      (Msg_ID_MC_TER_BASE_2+0x49)
#define SRMsgId_IndTerJoinedGroupMeetingRoom					  (Msg_ID_MC_TER_BASE_2+0x4A)
#define SRMsgId_IndTerEnterGroupMeetingRoom					      (Msg_ID_MC_TER_BASE_2+0x4B)
#define SRMsgId_IndTerLeaveGroupMeetingRoom 				      (Msg_ID_MC_TER_BASE_2+0x4C)
#define SRMsgId_IndMCBeatToGroupMeetingRoomTer				      (Msg_ID_MC_TER_BASE_2+0x4D)
#define SRMsgId_IndTerTransferToOtherGroupMeetingRoom		      (Msg_ID_MC_TER_BASE_2+0x4E)
#define SRMsgId_IndTerConfCallWaitingState            		      (Msg_ID_MC_TER_BASE_2+0x4F)
#define SRMsgId_RspConfGroupMeetingRoomTerLists        		      (Msg_ID_MC_TER_BASE_2+0x50)
#define SRMsgId_RspConfRollCallInfo                  		      (Msg_ID_MC_TER_BASE_2+0x51)
#define SRMsgId_IndTerModifyConfCallList               		      (Msg_ID_MC_TER_BASE_2+0x52)
#define SRMsgId_IndTerConfRollCallState               		      (Msg_ID_MC_TER_BASE_2+0x53)
#define SRMsgId_IndRollCallTer                       		      (Msg_ID_MC_TER_BASE_2+0x54)
#define SRMsgId_IndRollCallTerResult                 		      (Msg_ID_MC_TER_BASE_2+0x55)
#define SRMsgId_IndTerNewAudioSourceJoined            		      (Msg_ID_MC_TER_BASE_2+0x56)
#define SRMsgId_IndTerAudioSourceLeave              		      (Msg_ID_MC_TER_BASE_2+0x57)
#define SRMsgId_IndTerNewVideoSourceJoined            		      (Msg_ID_MC_TER_BASE_2+0x58)
#define SRMsgId_IndTerVideoSourceLeave              		      (Msg_ID_MC_TER_BASE_2+0x59)
#define SRMsgId_IndChangeVideoSourceName            		      (Msg_ID_MC_TER_BASE_2+0x5A)
#define SRMsgId_IndChangeVideoSourcePriority        		      (Msg_ID_MC_TER_BASE_2+0x5B)
#define SRMsgId_IndTerChangeDeviceInfos        		      		  (Msg_ID_MC_TER_BASE_2+0x5C)

#define SRMsgId_IndTerSetAnalyState                  		      (Msg_ID_MC_TER_BASE_2+0x70)
#define SRMsgId_IndTerSelectVasVideo                		      (Msg_ID_MC_TER_BASE_2+0x71)

#define SRMsgId_IndChangePreset                     		      (Msg_ID_MC_TER_BASE_2+0x72)
#define SRMsgId_IndFilterNoVideo                     		      (Msg_ID_MC_TER_BASE_2+0x73)
#define SRMsgId_IndTerAutoPollInfo								  (Msg_ID_MC_TER_BASE_2+0x74)
#define SRMsgId_IndParticipantsOrder							  (Msg_ID_MC_TER_BASE_2+0x75)
#define SRMsgId_IndChangeTerInfo        						  (Msg_ID_MC_TER_BASE_2+0x76)


/////////////////////////////////////////////////////////////////////
#define Msg_ID_MC_NETMP_MODULE Msg_ID_MODULE(0x2)
#define Msg_ID_MC_NETMP_BASE_1 (Msg_ID_MC_NETMP_MODULE|0x000)
#define Msg_ID_MC_NETMP_BASE_2 (Msg_ID_MC_NETMP_MODULE|0x100)
#define Msg_ID_MC_NETMP_BASE_3 (Msg_ID_MC_NETMP_MODULE|0x200)
#define Msg_ID_MC_NETMP_BASE_4 (Msg_ID_MC_NETMP_MODULE|0x300)
/* 0x800 MC->NetMP */
#define SRMsgId_ReqNetMPCreateConf                              (Msg_ID_MC_NETMP_BASE_1+0x1)
//#define SRMsgId_IndDeleteCon                                  (Msg_ID_MC_NETMP_BASE_1+0x2)
#define SRMsgId_IndNetMPTerJoined                               (Msg_ID_MC_NETMP_BASE_1+0x3)
#define SRMsgId_IndNetMPTerLeft                                 (Msg_ID_MC_NETMP_BASE_1+0x4)
#define SRMsgId_CmdNetMPTerVideoSelect                          (Msg_ID_MC_NETMP_BASE_1+0x5)
//#define SRMsgId_CmdNetMPTerVideoSelect_TerVideoSend           (Msg_ID_MC_NETMP_BASE_1+0x6)
//#define SRMsgId_CmdNetMPStopVideoSelect_TerVideoSend          (Msg_ID_MC_NETMP_BASE_1+0x7)
#define SRMsgId_CmdNetMPConfDestroy                             (Msg_ID_MC_NETMP_BASE_1+0x8)
#define SRMsgId_IndNetMPHeartBeatOfMC                           (Msg_ID_MC_NETMP_BASE_1+0x9)
#define SRMsgId_IndVASChannelidToNetMp                          (Msg_ID_MC_NETMP_BASE_1+0xA)
//#define SRMsgId_CmdNetMPStopVideoSelect                       (Msg_ID_MC_NETMP_BASE_1+0x10)
#define SRMsgId_IndNetMPTerAssistVideoOn                        (Msg_ID_MC_NETMP_BASE_1+0x11)
#define SRMsgId_IndNetMPTerAssistVideoOff                       (Msg_ID_MC_NETMP_BASE_1+0x12)
#define SRMsgId_IndNetMPDeleteConf                              (Msg_ID_MC_NETMP_BASE_1+0x13)
#define SRMsgId_IndDeleteOtherNetMP								(Msg_ID_MC_NETMP_BASE_1+0x14)
#define SRMsgId_IndConfAllNetMPs                                (Msg_ID_MC_NETMP_BASE_1+0x15)
#define SRMsgId_IndConfAddNewNetMP                              (Msg_ID_MC_NETMP_BASE_1+0x16)
#define SRMsgId_IndMPIChannelidToNetMp                          (Msg_ID_MC_NETMP_BASE_1+0x17)
#define SRMsgId_IndConfScreenInfoInMP                           (Msg_ID_MC_NETMP_BASE_1+0x18)
#define SRMsgId_IndConfAllMPIInfoToNetMp                        (Msg_ID_MC_NETMP_BASE_1+0x19)
#define SRMsgId_IndNetMPTerSendMediaFormat                      (Msg_ID_MC_NETMP_BASE_1+0x20)
#define SRMsgId_IndNetMPTerRecvMediaFormat                      (Msg_ID_MC_NETMP_BASE_1+0x21)
#define SRMsgId_IndConfAllTerMediaFormatToNetMP                 (Msg_ID_MC_NETMP_BASE_1+0x22)
#define SRMsgId_CmdNetMPTerSelectAssistVideo					(Msg_ID_MC_NETMP_BASE_1+0x23)
#define SRMsgId_IndNetMPTerMediaFormat							(Msg_ID_MC_NETMP_BASE_1+0x24)
#define SRMsgId_IndCRSChannelidToNetMp							(Msg_ID_MC_NETMP_BASE_1+0x25)
#define SRMsgId_IndNetMPChangeTerInfo                           (Msg_ID_MC_NETMP_BASE_1+0x26)


///////////////////////////////////////
/* 0x900 NetMP->MC */
#define SRMsgId_RspMCConfCreate                                 (Msg_ID_MC_NETMP_BASE_2+0x1)
#define SRMsgId_IndMCHeartBeatOfNetMP                           (Msg_ID_MC_NETMP_BASE_2+0x2)
#define SRMsgId_IndTerStatisticsInfoFromNetmp                   (Msg_ID_MC_NETMP_BASE_2+0x3)

/////////////////////////////////////////////////////////////////////
#define Msg_ID_MC_MP_MODULE Msg_ID_MODULE(0x4)
#define Msg_ID_MC_MP_BASE_1 (Msg_ID_MC_MP_MODULE|0x000)
#define Msg_ID_MC_MP_BASE_2 (Msg_ID_MC_MP_MODULE|0x100)
#define Msg_ID_MC_MP_BASE_3 (Msg_ID_MC_MP_MODULE|0x200)
#define Msg_ID_MC_MP_BASE_4 (Msg_ID_MC_MP_MODULE|0x300)
/* 0x1000 MC->MP */
#define SRMsgId_ReqMPCreateMPI                              (Msg_ID_MC_MP_BASE_1+0x1)
#define SRMsgId_CmdMPDestoryMPI                             (Msg_ID_MC_MP_BASE_1+0x2)
#define SRMsgId_IndMPTerJoined                              (Msg_ID_MC_MP_BASE_1+0x3)
#define SRMsgId_IndMPTerLeft                                (Msg_ID_MC_MP_BASE_1+0x4)
#define SRMsgId_ReqMPCreateScreen                           (Msg_ID_MC_MP_BASE_1+0x5)
#define SRMsgId_CmdMPSetScreenLayout                        (Msg_ID_MC_MP_BASE_1+0x6)
#define SRMsgId_CmdMPDestoryScreen                          (Msg_ID_MC_MP_BASE_1+0x7)
#define SRMsgId_CmdMPAddSubtitle                            (Msg_ID_MC_MP_BASE_1+0x8)
#define SRMsgId_CmdMPStopSubtitle                           (Msg_ID_MC_MP_BASE_1+0x9)
#define SRMsgId_IndMPChairTransfer                          (Msg_ID_MC_MP_BASE_1+0xA)
#define SRMsgId_IndtMPTerMuteEnable                         (Msg_ID_MC_MP_BASE_1+0xB)
#define SRMsgId_IndMCHeartBeatToMP                          (Msg_ID_MC_MP_BASE_1+0xC)
#define SRMsgId_CmdMPStartSendVideo                         (Msg_ID_MC_MP_BASE_1+0xD)
#define SRMsgId_IndMPTerSendMediaFormat                     (Msg_ID_MC_MP_BASE_1+0xE)
#define SRMsgId_IndMPTerRecvMediaFormat                     (Msg_ID_MC_MP_BASE_1+0xF)
#define SRMsgId_IndConfAllTerMediaFormatToMP                (Msg_ID_MC_MP_BASE_1+0x10)
#define SRMsgId_CmdMPReOpenNetMPMediaPort					(Msg_ID_MC_MP_BASE_1+0x11)
#define SRMsgId_IndMPTerMuteAllEnable					    (Msg_ID_MC_MP_BASE_1+0x12)
#define SRMsgId_IndMPTerMediaFormat						    (Msg_ID_MC_MP_BASE_1+0x13)
#define SRMsgId_CmdMPDisplayTername                         (Msg_ID_MC_MP_BASE_1+0x14)
#define SRMsgId_CmdMPHideTername                            (Msg_ID_MC_MP_BASE_1+0x15)
#define SRMsgId_IndMPAssistVideoOn                          (Msg_ID_MC_MP_BASE_1+0x16)
#define SRMsgId_IndMPAssistVideoOff                         (Msg_ID_MC_MP_BASE_1+0x17)
#define SRMsgId_IndCRSChannelInfoToMp                       (Msg_ID_MC_MP_BASE_1+0x18)
#define SRMsgId_IndMPTerCameraEnable                        (Msg_ID_MC_MP_BASE_1+0x19)
#define SRMsgId_IndMPSpecialtypeTransfer                    (Msg_ID_MC_MP_BASE_1+0x20)
#define SRMsgId_IndMPChangeVideoSourceName                  (Msg_ID_MC_MP_BASE_1+0x2A)
#define SRMsgId_IndMPNewAudioSourceJoined                   (Msg_ID_MC_MP_BASE_1+0x2B)
#define SRMsgId_IndMPAudioSourceLeave                       (Msg_ID_MC_MP_BASE_1+0x2C)
#define SRMsgId_IndMPNewVideoSourceJoined                   (Msg_ID_MC_MP_BASE_1+0x2D)
#define SRMsgId_IndMPVideoSourceLeave                       (Msg_ID_MC_MP_BASE_1+0x2E)
#define SRMsgId_IndMPChangeVideoSourcePriority              (Msg_ID_MC_MP_BASE_1+0x2F)
#define SRMsgId_IndMPChangeDeviceInfos              		(Msg_ID_MC_MP_BASE_1+0x30)
#define SRMsgId_IndMPChairLock                    			(Msg_ID_MC_MP_BASE_1+0x31)
#define SRMsgId_IndMPChairUnLock                    		(Msg_ID_MC_MP_BASE_1+0x32)
#define SRMsgId_IndMPAutoPollInfo                    		(Msg_ID_MC_MP_BASE_1+0x40)

/* 0x1100 MP->MC */
#define SRMsgId_RspMCCreateMPI                              (Msg_ID_MC_MP_BASE_2+0x1)
//#define SRMsgId_RspMPDestoryMPI                             (Msg_ID_MC_MP_BASE_2+0x2)
#define SRMsgId_RspMCCreateScreen                           (Msg_ID_MC_MP_BASE_2+0x3)
//#define SRMsgId_RspMCDestoryScreen                          (Msg_ID_MC_MP_BASE_2+0x4)
#define SRMsgId_CmdMPSeleteVideo                            (Msg_ID_MC_MP_BASE_2+0x5)
#define SRMsgId_IndMPHeartBeatToMC                          (Msg_ID_MC_MP_BASE_2+0x6)
#define SRMsgId_IndMPLayoutInfo                             (Msg_ID_MC_MP_BASE_2+0x7)
#define SRMsgId_CmdMPSelectAssistVideo                      (Msg_ID_MC_MP_BASE_2+0x8)

/////////////////////////////////////////////////////////////////////
#define Msg_ID_MC_GW_MODULE Msg_ID_MODULE(0x5)
#define Msg_ID_MC_GW_BASE_1 (Msg_ID_MC_GW_MODULE|0x000)
#define Msg_ID_MC_GW_BASE_2 (Msg_ID_MC_GW_MODULE|0x100)
#define Msg_ID_MC_GW_BASE_3 (Msg_ID_MC_GW_MODULE|0x200)
#define Msg_ID_MC_GW_BASE_4 (Msg_ID_MC_GW_MODULE|0x300)
/* 0x1400 MC->GW */
#define SRMsgId_CmdGWInvitParticipants                      (Msg_ID_MC_GW_BASE_1+0x1)
#define SRMsgId_IndMCHeartBeatToGW                          (Msg_ID_MC_GW_BASE_1+0x2)
#define SRMsgId_CmdGWSyncMonitorInfo                        (Msg_ID_MC_GW_BASE_1+0x3)
#define SRMsgId_CmdGWQueryMonitorRec                        (Msg_ID_MC_GW_BASE_1+0x4)
/* 0x1500 GW->MC */
#define SRMsgId_IndGWHeartBeatToMC                          (Msg_ID_MC_GW_BASE_2+0x1)
#define SRMsgId_IndGWQueryMonitorRecToMC                    (Msg_ID_MC_GW_BASE_2+0x2)
#define SRMsgId_IndTerStatisticsInfoFromGW                  (Msg_ID_MC_GW_BASE_2+0x3)

/////////////////////////////////////////////////////////////////////
#define Msg_ID_MC_CRS_MODULE Msg_ID_MODULE(0x6)
#define Msg_ID_MC_CRS_BASE_1 (Msg_ID_MC_CRS_MODULE|0x000)
#define Msg_ID_MC_CRS_BASE_2 (Msg_ID_MC_CRS_MODULE|0x100)
#define Msg_ID_MC_CRS_BASE_3 (Msg_ID_MC_CRS_MODULE|0x200)
#define Msg_ID_MC_CRS_BASE_4 (Msg_ID_MC_CRS_MODULE|0x300)
/* 0x1800 MC->CRS */
#define SRMsgId_ReqCRSCreateConf							(Msg_ID_MC_CRS_BASE_1+0x1)
#define SRMsgId_CmdCRSDestroyConf                           (Msg_ID_MC_CRS_BASE_1+0x2)
#define SRMsgId_IndCRSTerJoined                             (Msg_ID_MC_CRS_BASE_1+0x3)
#define SRMsgId_IndCRSTerLeft                               (Msg_ID_MC_CRS_BASE_1+0x4)
#define SRMsgId_IndConfAllMPIInfoToCRS                      (Msg_ID_MC_CRS_BASE_1+0x5)
#define SRMsgId_IndMPIInfoToCRS                             (Msg_ID_MC_CRS_BASE_1+0x6)
#define SRMsgId_IndScreenInfoToCRS                          (Msg_ID_MC_CRS_BASE_1+0x7)
#define SRMsgId_ReqCRSStartRec                              (Msg_ID_MC_CRS_BASE_1+0x8)
#define SRMsgId_ReqCRSStopRec                               (Msg_ID_MC_CRS_BASE_1+0x9)
#define SRMsgId_CmdCRSReOpenNetMPMediaPort                  (Msg_ID_MC_CRS_BASE_1+0xA)
#define SRMsgId_IndMCHeartBeatToCRS                         (Msg_ID_MC_CRS_BASE_1+0xB)
#define SRMsgId_ReqCRSStartLive                  			(Msg_ID_MC_CRS_BASE_1+0xC)
#define SRMsgId_CmdCRSStopLive			                    (Msg_ID_MC_CRS_BASE_1+0xD)
#define SRMsgId_IndCRSSelectVasVideo                        (Msg_ID_MC_CRS_BASE_1+0x10)
//#define SRMsgId_CmdCRSReOpenNetMPMediaPort                  (Msg_ID_MC_CRS_BASE_1+0xA)
/* 0x1900 CRS->MC */
#define SRMsgId_RspCRSCreateConf                            (Msg_ID_MC_CRS_BASE_2+0x1)
#define SRMsgId_CmdCRSSeleteVideo                           (Msg_ID_MC_CRS_BASE_2+0x2)
#define SRMsgId_IndCRSHeartBeatToMC                         (Msg_ID_MC_CRS_BASE_2+0x3)
#define SRMsgId_RspCRSStartRec                              (Msg_ID_MC_CRS_BASE_2+0x4)
#define SRMsgId_IndCRSFileStorPathToMC                      (Msg_ID_MC_CRS_BASE_2+0x5)
#define SRMsgId_RspCRSStopRec                               (Msg_ID_MC_CRS_BASE_2+0x6)
#define SRMsgId_RspCRSStartLive                             (Msg_ID_MC_CRS_BASE_2+0x7)
#define SRMsgId_IndCRSLiveStateToMC                         (Msg_ID_MC_CRS_BASE_2+0x8)
//#define SRMsgId_RspCreateConfCRS                            (Msg_ID_MC_CRS_BASE_2+0x1)
/* 0x1A00 xxx->xxx */

/////////////////////////////////////////////////////////////////////
#define Msg_ID_MC_RELAYSERVER_MODULE Msg_ID_MODULE(0x7)
#define Msg_ID_MC_RELAYSERVER_BASE_1 (Msg_ID_MC_RELAYSERVER_MODULE|0x000)
#define Msg_ID_MC_RELAYSERVER_BASE_2 (Msg_ID_MC_RELAYSERVER_MODULE|0x100)
#define Msg_ID_MC_RELAYSERVER_BASE_3 (Msg_ID_MC_RELAYSERVER_MODULE|0x200)
#define Msg_ID_MC_RELAYSERVER_BASE_4 (Msg_ID_MC_RELAYSERVER_MODULE|0x300)
/* 0x1C00 MC->RELAYSERVER */
#define SRMsgId_IndMCHeartBeatToRelayserver                 (Msg_ID_MC_RELAYSERVER_BASE_1+0x1)
#define SRMsgId_ReqTerJoinConfToRelayserver                 (Msg_ID_MC_RELAYSERVER_BASE_1+0x2)
/* 0x1D00 RELAYSERVER->MC */
#define SRMsgId_IndRelayserverHeartBeatToMC                 (Msg_ID_MC_RELAYSERVER_BASE_2+0x1)
#define SRMsgId_RspTerJoinConfFromRelayserver               (Msg_ID_MC_RELAYSERVER_BASE_2+0x2)

/////////////////////////////////////////////////////////////////////
#define Msg_ID_MC_VAS_MODULE Msg_ID_MODULE(0x8)
#define Msg_ID_MC_VAS_BASE_1 (Msg_ID_MC_VAS_MODULE|0x000)
#define Msg_ID_MC_VAS_BASE_2 (Msg_ID_MC_VAS_MODULE|0x100)
#define Msg_ID_MC_VAS_BASE_3 (Msg_ID_MC_VAS_MODULE|0x200)
#define Msg_ID_MC_VAS_BASE_4 (Msg_ID_MC_VAS_MODULE|0x300)
/* 0x2000 MC->VAS */
#define SRMsgId_ReqVASCreateConf                    (Msg_ID_MC_VAS_BASE_1+0x1)
#define SRMsgId_CmdVASDestroyConf                   (Msg_ID_MC_VAS_BASE_1+0x2)
#define SRMsgId_CmdVASStartAnaly                    (Msg_ID_MC_VAS_BASE_1+0x3)
#define SRMsgId_CmdVASStopAnaly                     (Msg_ID_MC_VAS_BASE_1+0x4)
#define SRMsgId_CmdVASUpdateAnaly                   (Msg_ID_MC_VAS_BASE_1+0x5)
#define SRMsgId_CmdVASReOpenNetMPMediaPort          (Msg_ID_MC_VAS_BASE_1+0x6)
#define SRMsgId_IndMCHeartBeatToVAS                 (Msg_ID_MC_VAS_BASE_1+0x7)
#define SRMsgId_IndConfAllMPIInfoToVAS              (Msg_ID_MC_VAS_BASE_1+0x8)
#define SRMsgId_IndMPIInfoToVAS                     (Msg_ID_MC_VAS_BASE_1+0x9)
#define SRMsgId_IndScreenInfoToVAS                  (Msg_ID_MC_VAS_BASE_1+0xA)
/* 0x2100 VAS->MC */
#define SRMsgId_RspVASCreateConf                    (Msg_ID_MC_VAS_BASE_2+0x1)
#define SRMsgId_IndVASHeartBeatToMC                 (Msg_ID_MC_VAS_BASE_2+0x2)
#define SRMsgId_IndVASMatchResult                   (Msg_ID_MC_VAS_BASE_2+0x3)
#define SRMsgId_CmdMCVASSeleteVideo                 (Msg_ID_MC_VAS_BASE_2+0x4)

#define getMsgIdByClassName(className) SRMsgId_##className


#ifndef SRMC_DEFINE_TYPE
#define SRMC_DEFINE_TYPE

typedef bool						SR_bool;
typedef char						SR_char;
typedef signed char 				SR_int8;
typedef unsigned char 				SR_uint8;
typedef signed short int 			SR_int16;
typedef unsigned short int 			SR_uint16;
typedef signed int 					SR_int32;
typedef unsigned int 				SR_uint32;
#if defined(__x86_64__) || defined(_WIN64) || defined(__arm64__) || defined(__aarch64__)
typedef signed long  int 			SR_int64;
typedef unsigned long int 			SR_uint64;
#elif defined(__i386__)|| defined(_WIN32)
typedef signed long long int 		SR_int64;
typedef unsigned long long int	 	SR_uint64;
#else
#error "unknow platform"
#endif
typedef unsigned long int 			SR_ptr;
typedef void 						SR_void;
typedef int							SR_socket;

// printf 格式输出的时候 ,使用方式
// size_t a;
// printf("a="SR_PRI_size_t"\n",a);
#if defined(__x86_64__) || defined(_WIN64)
#  define SR__PRI64_PREFIX	"l"
#  define SR_PRI_size_t "lu"
#else
#  define SR__PRI64_PREFIX	"ll"
#  define SR_PRI_size_t 	"u"
#endif

# define SR_PRId64		SR__PRI64_PREFIX "d"
# define SR_PRIu64		SR__PRI64_PREFIX "u"
# define SR_PRIx64		SR__PRI64_PREFIX "x"
/* UPPERCASE hexadecimal notation.  */
# define SR_PRIX64		SR__PRI64_PREFIX "X"

#endif // #ifndef SRMC_DEFINE_TYPE

#ifndef isHostBigEndian
/* reinterpret_cast <new_type> (expression)
   reinterpret_cast运算符是用来处理无关类型之间的转换；它会产生一个新的值，
   这个值会有与原始参数（expressoin）有完全相同的比特位。
   仅仅是重新解释了给出的对象的比特模型而没有进行二进制转换。*/
//没有定义成static inline 防止编译告警(not use)
#define isHostBigEndian()\
({\
	static const SR_int32 number = 1;\
	static SR_bool ishostBigEnd;\
	ishostBigEnd = (0 == *reinterpret_cast<const char*>(&number));\
	ishostBigEnd;\
})
#endif

#ifndef SR_swap_16
/* 翻转字节序,大端变小端,小端变大端 */
#define SR_swap_16(x) \
     ((SR_uint16) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))

#define SR_swap_32(x) \
     ( (((x) & 0xff000000) >> 24) 	\
     | (((x) & 0x00ff0000) >>  8)   \
     | (((x) & 0x0000ff00) <<  8)   \
     | (((x) & 0x000000ff) << 24))

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


#ifndef SRMC_SRHEADER_PROTOHEADER_DEFINE
#define SRMC_SRHEADER_PROTOHEADER_DEFINE
namespace SRMC{
///////////////////////////////////////TCP包头定义/////////////////////////////////////////////
#pragma pack(push)
#pragma pack(1)
/* little endian bit-order */
typedef struct{
	SR_char    m_s; // 'S'
	SR_char    m_r; // 'R'
	SR_char	   m_version;
	SR_char    m_channelid;
	SR_uint16  m_data_len;
	SR_uint16  m_cmdtype;//200 201 202 ...
}S_SRMsgHeader;
//不同类型的消息,根据 m_cmdtype 区分
#define SRMSG_CMDTYPE_RELAY_TER_MC (200)
#define SRMSG_CMDTYPE_NETMP_MC     (201)
#define SRMSG_CMDTYPE_DevMgr_MC    (202)

/* little endian bit-order */
typedef struct{
	SR_uint64 m_confid;
	SR_uint32 m_userid;
	SR_uint64 m_mcip;//新增mc地址 2015-10-21
}S_Header_Context;

/* little endian bit-order */
typedef struct{
	SR_uint32  m_msglen; // 不包含S_ProtoMsgHeader自身头长度
	SR_uint32  m_msguid; //
}S_ProtoMsgHeader;
#pragma pack(pop)
/////////////////////////////////////////////////////////////////////////////////////////////
}//namespace SRMC
#endif

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
		DEVICE_STUNSERVER = 11,
		DEVICE_VAS = 12
    }DeviceType;
}

#endif
