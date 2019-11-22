/***********************************************************
 *   �ļ���: sr_message.h
 *     ��Ȩ: �人������ɽ 2015-2020
 *     ����: Marvin Zhai
 * �ļ�˵��: �����libjingle����Դ���Ϣ������߳�ģ���е�
             �߳��ڲ�ͨ����Ϣ�ӿڶ���
 * ��������: 2015.10.26
 * �޸�����:
 *   �޸���:
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

//�߳��ڲ�������Ϣ�Ŷ���
typedef enum{
	e_timerid = 0xbeaf0001U,//��I/O�����̲߳���,���͵�ע���߳�
}E_MessageId;


} //namespace SRMC

#endif // SRMC_MESSAGE_H