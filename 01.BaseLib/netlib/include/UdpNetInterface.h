#ifndef __UDPNETINTERFACE_H__
#define __UDPNETINTERFACE_H__
#include "TypeDef.h"
#include "NetLibCommon.h"

//udp�¼��ص��ӿ�
class IUdpEvent
{
public:
    //��ȡ�ڴ�
    virtual RawPacket* GetMem() = 0;

    //�ڴ�δʹ�ã��������ϲ㴦��
    virtual void ReleaseMem(RawPacket* packet) = 0;

    //udp�յ�����֪ͨ
    virtual void OnRecvData(RawPacket* pack, unsigned int remote_ip, unsigned short remote_port) = 0;
    
    //udp��������
    virtual void OnNetError(int err_code) = 0;
};

//udp����ӿ�
class IUdpTrans
{
public:
    //��ʼ������,local_portΪ0ʱ, ������ϵͳ����Ķ˿�
    virtual bool Init(IUdpEvent* ev_handler, char* local_ip, unsigned short& local_port) = 0;

    //��������
    virtual bool SendData(char* data, int data_len,
        unsigned int remote_ip, unsigned short remote_port) = 0;

    //�ر�udp trans
    virtual void Close() = 0;

    //�ͷŶ���, ���ô˷�����, ����ָ�벻����Ч
    virtual void Release() = 0;
};

//udp trans module �ӿ�
class IUdpTransLib
{
public:
    //��ʼ��, ָ���߳������¼�����ģ��(select��epoll)
    virtual bool Init(int thread_num, int mode) = 0;

    //�ͷ�udp����ģ��, ֹͣ�����¼�ѭ���߳�
    virtual void Uninit() = 0;

    //����udp socket, thread_idΪ[in out]����, С��0ʱ, ���������߳�id, ����,ָʾ��socketʹ�õ��߳�
    virtual IUdpTrans* CreateUdpSocket(int& thread_id) = 0;

    //�ͷ�udp socket
    virtual void DeleteUdpSocket(IUdpTrans* udp_soc) = 0;
};

//����udp trans module
IUdpTransLib* CreateUdpTransLib();

//ɾ��udp trans module
void DeleteUdpTransLib(IUdpTransLib* trans_lib);
#endif



