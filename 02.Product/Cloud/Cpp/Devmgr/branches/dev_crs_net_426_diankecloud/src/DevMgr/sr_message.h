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
	e_timerid = 0xbeaf0001U,//由I/O复用线程产生,发送到注册线程
}E_MessageId;


} //namespace SRMC

#endif // SRMC_MESSAGE_H