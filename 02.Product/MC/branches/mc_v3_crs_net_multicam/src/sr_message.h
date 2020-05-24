/***********************************************************
 *   文件名: sr_message.h
 *     版权: 武汉随锐亿山 2015-2020
 *     作者: Marvin Zhai
 * 文件说明: 借鉴于libjingle库的自带消息处理的线程模型中的
             线程内部通用消息接口定义
 * 创建日期: 2015.10.26
 * 修改日期:
 *   修改人:
 *************************************************************/
#ifndef SRMC_MESSAGE_H
#define SRMC_MESSAGE_H
#include <string.h> // for memset
#include "sr_type.h"
namespace SRMC{

class MessageData 
{
public:
  	MessageData() {}
  	virtual ~MessageData() {}
};

class Message;
class MessageHandler 
{
public:
	MessageHandler(){}
	virtual ~MessageHandler(){}
	virtual SR_void OnMessage(Message* msg) = 0;
private:
  	SUIRUI_DISALLOW_COPY_AND_ASSIGN(MessageHandler);
};

class Message 
{
public:
  Message() { memset(this, 0, sizeof(*this)); }
  MessageHandler *phandler;
  SR_uint32 message_id;
  MessageData *pdata;
};

//线程内部交互消息号定义
typedef enum{
	e_sock_connect_error = 0xbeaf0001U,
	e_sock_connect_ing = 0xbeaf0002U,
	e_sock_connect_ok = 0xbeaf0003U,
	e_sock_rw_error = 0xbeaf0004U,//socket 读写异常等
	e_sock_recv_data = 0xbeaf0005U,
	//e_sock_accept_ok = 0xbeaf0005U,//not use
	/////////////////////////////////
	e_reqconfinfofromdevmgr = 0xbeaf0006U,//ter->devmgr 线程,获取会议信息
	e_rspconfinfotomc = 0xbeaf0007U,//devmgr->ter 线程,返回会议信息
	//e_reqnetmpcreateconf = 0xbeaf0008U,//ter->devmgr 线程,获取NetMp创建会议信息
	e_rspnetmpcreateconf = 0xbeaf0009U,
	e_waitconfinfotimeout = 0xbeaf000aU,//TODO: 与 e_rspconfinfotomc 合并
	e_waitnetmpcreatconftimeout = 0xbeaf000bU,//TODO: 与 e_rspnetmpcreateconf 合并
	//e_netmpcreatconferror = 0xbeaf000cU,
	e_timerid = 0xbeaf000dU,//由I/O复用线程产生,发送到注册线程
	e_update_netmpinfo_inconf = 0xbeaf000eU,//由devmgr->ter 线程,更新会议终端netmp load值
	e_notify_terjoinconf = 0xbeaf000fU,//ter->devmgr
	e_notify_terexitconf = 0xbeaf0010U,//ter->devmgr
	e_notify_netmpsock_error = 0xbeaf0011U,//devmgr->ter
	e_notify_devmgrsock_error = 0xbeaf0012U,//devmgr->ter
	e_notify_confexit = 0xbeaf0013U,//ter->devmgr
	e_notify_connected_netmp = 0xbeaf0014U,//netmp->ter
	e_update_mpinfo_inconf = 0xbeaf0015U,//由devmgr->ter 线程,更新会议终端mp load值
	e_notify_connected_mp = 0xbeaf0016U,//mp->ter
	e_notify_mpsock_error = 0xbeaf0017U,//devmgr->ter
	e_rspmpcreatempi = 0xbeaf0018U,//
	e_rspmpdestroympi = 0xbeaf0019U,
	e_rspmpcreatescreen = 0xbeaf0020U,//
	e_rspmpdestroyscreen = 0xbeaf0021U,
	e_cmdmpseletevideo = 0xbeaf0022U, // mp->mc->ter
	e_notify_to_register_devmgr = 0xbeaf0023U, // devmgr->ter
	e_indmplayoutinfo = 0xbeaf0024U, // mp->mc->ter
	e_notify_gwsock_error = 0xbeaf0025U,//devmgr->ter
	e_notify_connected_gw = 0xbeaf0026U,//gw->ter
	e_update_gwinfo_inconf = 0xbeaf0027U,//由devmgr->ter 线程,更新会议终端gw load值
	e_notify_to_save_licenceinfo = 0xbeaf0028U, // TerminalMgr->ter
	e_notify_to_startup = 0xbeaf0029U, // DeviceManager->DevMgrNetMPProcessThread
	e_notify_crssock_error = 0xbeaf002aU,//devmgr->ter
	e_update_crsinfo_inconf = 0xbeaf002bU,//由devmgr->ter 线程,更新会议终端crs load值
	e_notify_connected_crs = 0xbeaf002cU,//crs->ter
	e_rspcrscreateconf = 0xbeaf002dU,//
	e_cmdcrsseletevideo = 0xbeaf002eU, // crs->mc->ter
	e_rspcrsstartrec = 0xbeaf002fU,//
	e_indcrsfilestorpath = 0xbeaf0030U,//
	e_rspcrsstoprec = 0xbeaf0031U,//
	e_notify_to_send_heart2devmgr = 0xbeaf0032U, // devmgr->ter
	e_notify_to_check_confcrsinfo = 0xbeaf0033U, // devmgr->ter
	e_update_srsinfo_inconf = 0xbeaf0034U,//由devmgr->ter 线程,更新会议srs信息
	e_rspcrsstartlive = 0xbeaf0035U,
	e_rspcrsstoplive = 0xbeaf0036U,
	e_notify_to_end_conf = 0xbeaf0037U,
	e_indcrslivestate = 0xbeaf0038U,//
	e_update_sys_cur_load = 0xbeaf0039U,//
	e_update_relayserverinfo_inconf = 0xbeaf003aU,//
	e_notify_connected_relayserver = 0xbeaf003bU,//
	e_notify_relayserversock_error = 0xbeaf003cU,//
	e_relaysvr_rsp_terjoinconf = 0xbeaf003dU,//
	e_indgwquerymonitorrec = 0xbeaf003eU,//
	e_netmpindterstatisticsinfo = 0xbeaf003fU,//
	e_gwindterstatisticsinfo = 0xbeaf0040U,//
	e_update_relaymcinfo_inconf = 0xbeaf0050U,//
	e_notify_sync_confinfo2devmgr = 0xbeaf0060U,//
	e_rsplicensetomc = 0xbeaf0070U,
	e_notify_to_save_complicenceinfo = 0xbeaf0080U, //TerminalMgr->ter
}E_MessageId;


} //namespace SRMC

#endif // SRMC_MESSAGE_H