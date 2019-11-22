#ifndef __UDPNETINTERFACE_H__
#define __UDPNETINTERFACE_H__
#include "TypeDef.h"
#include "NetLibCommon.h"

//udp事件回调接口
class IUdpEvent
{
public:
    //获取内存
    virtual RawPacket* GetMem() = 0;

    //内存未使用，交回至上层处理
    virtual void ReleaseMem(RawPacket* packet) = 0;

    //udp收到数据通知
    virtual void OnRecvData(RawPacket* pack, unsigned int remote_ip, unsigned short remote_port) = 0;
    
    //udp发生错误
    virtual void OnNetError(int err_code) = 0;
};

//udp传输接口
class IUdpTrans
{
public:
    //初始化方法,local_port为0时, 返回由系统分配的端口
    virtual bool Init(IUdpEvent* ev_handler, char* local_ip, unsigned short& local_port) = 0;

    //发送数据
    virtual bool SendData(char* data, int data_len,
        unsigned int remote_ip, unsigned short remote_port) = 0;

    //关闭udp trans
    virtual void Close() = 0;

    //释放对象, 调用此方法后, 对象指针不在有效
    virtual void Release() = 0;
};

//udp trans module 接口
class IUdpTransLib
{
public:
    //初始化, 指明线程数和事件监听模型(select或epoll)
    virtual bool Init(int thread_num, int mode) = 0;

    //释放udp传输模块, 停止所有事件循环线程
    virtual void Uninit() = 0;

    //创建udp socket, thread_id为[in out]参数, 小于0时, 输出分配的线程id, 否则,指示此socket使用的线程
    virtual IUdpTrans* CreateUdpSocket(int& thread_id) = 0;

    //释放udp socket
    virtual void DeleteUdpSocket(IUdpTrans* udp_soc) = 0;
};

//创建udp trans module
IUdpTransLib* CreateUdpTransLib();

//删除udp trans module
void DeleteUdpTransLib(IUdpTransLib* trans_lib);
#endif



