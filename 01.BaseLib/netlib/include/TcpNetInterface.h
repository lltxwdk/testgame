#ifndef __TCP_NETINTERFACE_H__
#define __TCP_NETINTERFACE_H__
#include "NetLibCommon.h"

//tcp �¼��ص��ӿ� 
class ITcpEvent
{
public:
    //tcp���ӳɹ��¼�֪ͨ
    virtual void OnConnected() = 0;

    //���ӶϿ��¼�֪ͨ
    virtual void OnDisconnect(int err_code) = 0;

    //���ݵ����¼�֪ͨ, �ӿ�ʵ������Ҫע��,����ֵ�������ѵ������ݳ���, ��СΪ0, ���Ϊdata_len
    virtual int OnRecvData(char* data, int data_len) = 0;
};

//tcp����ӿ�
class ITcpConn
{
public:
    //��ʼ��, ָ�����ͺͽ���buf��С, 
    virtual bool Init(ITcpEvent* ent_handler, int max_sendbuf_len, int max_recvbuf_len) = 0;

    //��������, ʧ�ܷ���false
    virtual bool Connect(char* remote_ip, unsigned short remote_port) = 0;

    //��������, ����ֵ: ����0��ʾ�ɹ�, 0 ��ʾsendbuf����, С��0��ʾ���ִ�����Ҫ���½�������
    virtual int SendData(char* data, int data_len) = 0;

    //�ر�����
    virtual void Close() = 0;

    //�ͷŶ����ڴ�, ���ô˽ӿں�, ����ָ�벻����ʹ��
    virtual void Release() = 0;
};

//tcp �����¼��ص�
class ITcpListenEvent
{
public:
    //�����µ�����, ʵ���������ȶ�new_tcpconn ִ��Init����, ��ʼ��tcp����
    virtual void OnAccept(ITcpConn* new_tcpconn, char* remote_ip, unsigned short remote_port) = 0;

    //����socket��������
    virtual void OnNetError(int err_code) = 0;
};

//tcp �����ӿ�
class ITcpListenner
{
public:
    //��������, ʧ�ܷ���false
    virtual bool Listen(const char* listen_ip, unsigned short listen_port) = 0;

    //�ر�����
    virtual void Close() = 0;

    //�ͷŶ����ڴ�, ���ô˽ӿں�, ����ָ�벻����ʹ��
    virtual void Release() = 0;
};

//tcp trans module �ӿ�
class ITcpTransLib
{
public:
    //��ʼ��, ����ָ���߳������¼��ַ�ģ��(select��epoll)
    virtual bool Init(int thread_num, int mode) = 0;

    //����ʼ��, ֹͣ�����߳�
    virtual void Uninit() = 0;

    //����tcp����socket
    virtual ITcpListenner* CreateTcpListenner(ITcpListenEvent* ent_handler) = 0;

    //������������
    virtual ITcpConn* CreateTcpConn() = 0;
};
ITcpTransLib* CreateTcpTransLib();
void DeleteTcpTransLib(ITcpTransLib* trans_lib);
#endif


