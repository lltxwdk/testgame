/**
 *   文件名: wrapper_msg.cpp
 *     版权: 武汉随锐亿山 2015-2020
 *     作者: Marvin Zhai
 * 文件说明: 消息的打解包
 * 创建日期: 2015.05.04
 * 修改日期:
 *   修改人:
 */
#include "wrapper_msg.h"

namespace SRMC{

using namespace SRMC;


class HostEndian{
public: 
	HostEndian()
	{
		union{
			int  a;
			char b;
		} uCE;
		uCE.a = 1;
		if(uCE.b == 1){
			m_hostEndian = S_LITTLE_ENDIAN;
		}
		else{
			m_hostEndian = S_BIG_ENDIAN;
		}		
	}
	
	E_Endian getHostE(void) const
	{
		return m_hostEndian;
	}
private:
	E_Endian m_hostEndian;
};

static HostEndian s_hostEndian;

SRMC::E_Endian getHostEndian(void)
{
	return s_hostEndian.getHostE();
}


void unpackPMH(const char* buf,S_ProtoMsgHeader* pmh)
{		
	if((!pmh)||(!buf))
		return;
	
	if(S_LITTLE_ENDIAN == SRMC::getHostEndian())
	{
		const unsigned int* p = (const unsigned int*)buf;
		pmh->m_msglen = *p; 
		pmh->m_msguid = *(p+1);	
	}
	else
	{
		char *pp = (char*)pmh;
		pp[0] = *(buf+3);
		pp[1] = *(buf+2);
		pp[2] = *(buf+1);
		pp[3] = *(buf+0);
		pp[4] = *(buf+7);
		pp[5] = *(buf+6);
		pp[6] = *(buf+5);
		pp[7] = *(buf+4);
	}	
}

void packPMH(const S_ProtoMsgHeader* pmh, char* buf)
{
	if((!pmh)||(!buf))
		return;
	
	if(S_LITTLE_ENDIAN == SRMC::getHostEndian())
	{
		unsigned int* p = (unsigned int*)buf;
		 *p     = pmh->m_msglen; 
		 *(p+1) = pmh->m_msguid;	
	}
	else
	{
		const char *pp = (const char*)pmh;
		buf[0] = *(pp+3);
		buf[1] = *(pp+2);
		buf[2] = *(pp+1);
		buf[3] = *(pp+0);
		buf[4] = *(pp+7);
		buf[5] = *(pp+6);
		buf[6] = *(pp+5);
		buf[7] = *(pp+4);
	}
}


}

 
namespace SRRELAY{

using namespace SRRELAY;


class HostEndian{
public: 
	HostEndian()
	{
		union{
			int  a;
			char b;
		} uCE;
		uCE.a = 1;
		if(uCE.b == 1){
			m_hostEndian = S_LITTLE_ENDIAN;
		}
		else{
			m_hostEndian = S_BIG_ENDIAN;
		}		
	}
	
	E_Endian getHostE(void) const
	{
		return m_hostEndian;
	}
private:
	E_Endian m_hostEndian;
};

static HostEndian s_hostEndian;

SRRELAY::E_Endian getHostEndian(void)
{
	return s_hostEndian.getHostE();
}

	    
void unpackPRH(const char* buf,S_RelayMsgHeader* prh)
{		
	if((!prh)||(!buf))
		return;
	
	memcpy(prh,buf,sizeof(S_RelayMsgHeader));
	if (S_LITTLE_ENDIAN == SRRELAY::getHostEndian())
	{		
		/* do nothing */
	}
	else
	{
		prh->m_data_len = SR_swap_16(prh->m_data_len);
		prh->m_cmdtype = SR_swap_16(prh->m_cmdtype);
		//prh->m_confid = SR_swap_64(prh->m_confid);
		//prh->m_userid = SR_swap_32(prh->m_userid);
	}		
}

void packPRH(const S_RelayMsgHeader* prh, char* buf)
{
	if((!prh)||(!buf))
		return;
	
	memcpy(buf,prh,sizeof(S_RelayMsgHeader));	
	
	if (S_LITTLE_ENDIAN == SRRELAY::getHostEndian())
	{
		/* do nothing */
	}
	else
	{
		S_RelayMsgHeader* p = (S_RelayMsgHeader*)buf;
		p->m_data_len = SR_swap_16(p->m_data_len);
		p->m_cmdtype = SR_swap_16(p->m_cmdtype);
		//p->m_confid = SR_swap_64(p->m_confid);
		//p->m_userid = SR_swap_32(p->m_userid);
	}
}
}
