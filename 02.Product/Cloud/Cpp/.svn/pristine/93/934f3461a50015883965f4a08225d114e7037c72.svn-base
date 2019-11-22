#ifndef CBUFFER_H
#define CBUFFER_H

// CBufferT
//template<class T1=int, class T2=int, class T3=int, class T4=int, class T5=int, class T6=int>
template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
class CBufferT
{
public:
	CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
		, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8);
	CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
		, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1);
	CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
		, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2);
	CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
		, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2, T3 Arg3);
	CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
		, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4);
	CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
		, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5);
	CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
		, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6);
	CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
		, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7);
	CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
		, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7, T8 Arg8);

    ~CBufferT();
    
public:
	char*		 m_pData1;
	unsigned int m_nLen1;
	char*		 m_pData2;
	unsigned int m_nLen2;
	char*		 m_pData3;
	unsigned int m_nLen3;
	char*		 m_pData4;
	unsigned int m_nLen4;
	char*		 m_pData5;
	unsigned int m_nLen5;
	char*		 m_pData6;
	unsigned int m_nLen6;
	char*		 m_pData7;
	unsigned int m_nLen7;
	char*		 m_pData8;
	unsigned int m_nLen8;
    T1			 m_Arg1;
    T2			 m_Arg2;
    T3			 m_Arg3;
    T4			 m_Arg4;
    T5			 m_Arg5;
	T6			 m_Arg6;
	T7			 m_Arg7;
	T8			 m_Arg8;
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
CBufferT<T1, T2, T3, T4, T5, T6, T7, T8>::CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
	, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8)
: m_pData1(NULL)
, m_nLen1(0)
, m_pData2(NULL)
, m_nLen2(0)
, m_pData3(NULL)
, m_nLen3(0)
, m_pData4(NULL)
, m_nLen4(0)
, m_pData5(NULL)
, m_nLen5(0)
, m_pData6(NULL)
, m_nLen6(0)
, m_pData7(NULL)
, m_nLen7(0)
, m_pData8(NULL)
, m_nLen8(0)
{
	if (pData1 != NULL && nLen1 > 0) 
	{
		m_pData1 = new char[nLen1+1];
		memset(m_pData1, 0, nLen1+1);
		memcpy(m_pData1, pData1, nLen1);
		m_nLen1 = nLen1;
	}
 	if (pData2 != NULL && nLen2 > 0) 
	{
		m_pData2 = new char[nLen2+1];
		memset(m_pData2, 0, nLen2+1);
		memcpy(m_pData2, pData2, nLen2);
		m_nLen2 = nLen2;
	}
  	if (pData3 != NULL && nLen3 > 0) 
	{
		m_pData3 = new char[nLen3+1];
		memset(m_pData3, 0, nLen3+1);
		memcpy(m_pData3, pData3, nLen3);
		m_nLen3 = nLen3;
	}
  	if (pData4 != NULL && nLen4 > 0) 
	{
		m_pData4 = new char[nLen4+1];
		memset(m_pData4, 0, nLen4+1);
		memcpy(m_pData4, pData4, nLen4);
		m_nLen4 = nLen4;
	}
  	if (pData5 != NULL && nLen5 > 0) 
	{
		m_pData5 = new char[nLen5+1];
		memset(m_pData5, 0, nLen5+1);
		memcpy(m_pData5, pData5, nLen5);
		m_nLen5 = nLen5;
	}
  	if (pData6 != NULL && nLen6 > 0) 
	{
		m_pData6 = new char[nLen6+1];
		memset(m_pData6, 0, nLen6+1);
		memcpy(m_pData6, pData6, nLen6);
		m_nLen6 = nLen6;
	}
	if (pData7 != NULL && nLen7 > 0)
	{
		m_pData7 = new char[nLen7 + 1];
		memset(m_pData7, 0, nLen7 + 1);
		memcpy(m_pData7, pData7, nLen7);
		m_nLen7 = nLen7;
	}
	if (pData8 != NULL && nLen8 > 0)
	{
		m_pData8 = new char[nLen8 + 1];
		memset(m_pData8, 0, nLen8 + 1);
		memcpy(m_pData8, pData8, nLen8);
		m_nLen8 = nLen8;
	}
	m_Arg1 = 0;
    m_Arg2 = 0;
    m_Arg3 = 0;
    m_Arg4 = 0;
    m_Arg5 = 0;
	m_Arg6 = 0;
	m_Arg7 = 0;
	m_Arg8 = 0;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
CBufferT<T1, T2, T3, T4, T5, T6, T7, T8>::CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
	, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1)
: m_pData1(NULL)
, m_nLen1(0)
, m_pData2(NULL)
, m_nLen2(0)
, m_pData3(NULL)
, m_nLen3(0)
, m_pData4(NULL)
, m_nLen4(0)
, m_pData5(NULL)
, m_nLen5(0)
, m_pData6(NULL)
, m_nLen6(0)
, m_pData7(NULL)
, m_nLen7(0)
, m_pData8(NULL)
, m_nLen8(0)
{
	if (pData1 != NULL && nLen1 > 0) 
	{
		m_pData1 = new char[nLen1+1];
		memset(m_pData1, 0, nLen1+1);
		memcpy(m_pData1, pData1, nLen1);
		m_nLen1 = nLen1;
	}
 	if (pData2 != NULL && nLen2 > 0) 
	{
		m_pData2 = new char[nLen2+1];
		memset(m_pData2, 0, nLen2+1);
		memcpy(m_pData2, pData2, nLen2);
		m_nLen2 = nLen2;
	}
  	if (pData3 != NULL && nLen3 > 0) 
	{
		m_pData3 = new char[nLen3+1];
		memset(m_pData3, 0, nLen3+1);
		memcpy(m_pData3, pData3, nLen3);
		m_nLen3 = nLen3;
	}
  	if (pData4 != NULL && nLen4 > 0) 
	{
		m_pData4 = new char[nLen4+1];
		memset(m_pData4, 0, nLen4+1);
		memcpy(m_pData4, pData4, nLen4);
		m_nLen4 = nLen4;
	}
  	if (pData5 != NULL && nLen5 > 0) 
	{
		m_pData5 = new char[nLen5+1];
		memset(m_pData5, 0, nLen5+1);
		memcpy(m_pData5, pData5, nLen5);
		m_nLen5 = nLen5;
	}
  	if (pData6 != NULL && nLen6 > 0) 
	{
		m_pData6 = new char[nLen6+1];
		memset(m_pData6, 0, nLen6+1);
		memcpy(m_pData6, pData6, nLen6);
		m_nLen6 = nLen6;
	}
	if (pData7 != NULL && nLen7 > 0)
	{
		m_pData7 = new char[nLen7 + 1];
		memset(m_pData7, 0, nLen7 + 1);
		memcpy(m_pData7, pData7, nLen7);
		m_nLen7 = nLen7;
	}
	if (pData8 != NULL && nLen8 > 0)
	{
		m_pData8 = new char[nLen8 + 1];
		memset(m_pData8, 0, nLen8 + 1);
		memcpy(m_pData8, pData8, nLen8);
		m_nLen8 = nLen8;
	}
    m_Arg1 = Arg1;
    m_Arg2 = 0;
    m_Arg3 = 0;
    m_Arg4 = 0;
    m_Arg5 = 0;
	m_Arg6 = 0;
	m_Arg7 = 0;
	m_Arg8 = 0;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
CBufferT<T1, T2, T3, T4, T5, T6, T7, T8>::CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
	, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2)
: m_pData1(NULL)
, m_nLen1(0)
, m_pData2(NULL)
, m_nLen2(0)
, m_pData3(NULL)
, m_nLen3(0)
, m_pData4(NULL)
, m_nLen4(0)
, m_pData5(NULL)
, m_nLen5(0)
, m_pData6(NULL)
, m_nLen6(0)
, m_pData7(NULL)
, m_nLen7(0)
, m_pData8(NULL)
, m_nLen8(0)
{
	if (pData1 != NULL && nLen1 > 0) 
	{
		m_pData1 = new char[nLen1+1];
		memset(m_pData1, 0, nLen1+1);
		memcpy(m_pData1, pData1, nLen1);
		m_nLen1 = nLen1;
	}
 	if (pData2 != NULL && nLen2 > 0) 
	{
		m_pData2 = new char[nLen2+1];
		memset(m_pData2, 0, nLen2+1);
		memcpy(m_pData2, pData2, nLen2);
		m_nLen2 = nLen2;
	}
  	if (pData3 != NULL && nLen3 > 0) 
	{
		m_pData3 = new char[nLen3+1];
		memset(m_pData3, 0, nLen3+1);
		memcpy(m_pData3, pData3, nLen3);
		m_nLen3 = nLen3;
	}
  	if (pData4 != NULL && nLen4 > 0) 
	{
		m_pData4 = new char[nLen4+1];
		memset(m_pData4, 0, nLen4+1);
		memcpy(m_pData4, pData4, nLen4);
		m_nLen4 = nLen4;
	}
  	if (pData5 != NULL && nLen5 > 0) 
	{
		m_pData5 = new char[nLen5+1];
		memset(m_pData5, 0, nLen5+1);
		memcpy(m_pData5, pData5, nLen5);
		m_nLen5 = nLen5;
	}
  	if (pData6 != NULL && nLen6 > 0) 
	{
		m_pData6 = new char[nLen6+1];
		memset(m_pData6, 0, nLen6+1);
		memcpy(m_pData6, pData6, nLen6);
		m_nLen6 = nLen6;
	}
	if (pData7 != NULL && nLen7 > 0)
	{
		m_pData7 = new char[nLen7 + 1];
		memset(m_pData7, 0, nLen7 + 1);
		memcpy(m_pData7, pData7, nLen7);
		m_nLen7 = nLen7;
	}
	if (pData8 != NULL && nLen8 > 0)
	{
		m_pData8 = new char[nLen8 + 1];
		memset(m_pData8, 0, nLen8 + 1);
		memcpy(m_pData8, pData8, nLen8);
		m_nLen8 = nLen8;
	}
	m_Arg1 = Arg1;
    m_Arg2 = Arg2;
    m_Arg3 = 0;
    m_Arg4 = 0;
    m_Arg5 = 0;
	m_Arg6 = 0;
	m_Arg7 = 0;
	m_Arg8 = 0;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
CBufferT<T1, T2, T3, T4, T5, T6, T7, T8>::CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
	, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2, T3 Arg3)
: m_pData1(NULL)
, m_nLen1(0)
, m_pData2(NULL)
, m_nLen2(0)
, m_pData3(NULL)
, m_nLen3(0)
, m_pData4(NULL)
, m_nLen4(0)
, m_pData5(NULL)
, m_nLen5(0)
, m_pData6(NULL)
, m_nLen6(0)
, m_pData7(NULL)
, m_nLen7(0)
, m_pData8(NULL)
, m_nLen8(0)
{
	if (pData1 != NULL && nLen1 > 0) 
	{
		m_pData1 = new char[nLen1+1];
		memset(m_pData1, 0, nLen1+1);
		memcpy(m_pData1, pData1, nLen1);
		m_nLen1 = nLen1;
	}
 	if (pData2 != NULL && nLen2 > 0) 
	{
		m_pData2 = new char[nLen2+1];
		memset(m_pData2, 0, nLen2+1);
		memcpy(m_pData2, pData2, nLen2);
		m_nLen2 = nLen2;
	}
  	if (pData3 != NULL && nLen3 > 0) 
	{
		m_pData3 = new char[nLen3+1];
		memset(m_pData3, 0, nLen3+1);
		memcpy(m_pData3, pData3, nLen3);
		m_nLen3 = nLen3;
	}
  	if (pData4 != NULL && nLen4 > 0) 
	{
		m_pData4 = new char[nLen4+1];
		memset(m_pData4, 0, nLen4+1);
		memcpy(m_pData4, pData4, nLen4);
		m_nLen4 = nLen4;
	}
  	if (pData5 != NULL && nLen5 > 0) 
	{
		m_pData5 = new char[nLen5+1];
		memset(m_pData5, 0, nLen5+1);
		memcpy(m_pData5, pData5, nLen5);
		m_nLen5 = nLen5;
	}
  	if (pData6 != NULL && nLen6 > 0) 
	{
		m_pData6 = new char[nLen6+1];
		memset(m_pData6, 0, nLen6+1);
		memcpy(m_pData6, pData6, nLen6);
		m_nLen6 = nLen6;
	}
	if (pData7 != NULL && nLen7 > 0)
	{
		m_pData7 = new char[nLen7 + 1];
		memset(m_pData7, 0, nLen7 + 1);
		memcpy(m_pData7, pData7, nLen7);
		m_nLen7 = nLen7;
	}
	if (pData8 != NULL && nLen8 > 0)
	{
		m_pData8 = new char[nLen8 + 1];
		memset(m_pData8, 0, nLen8 + 1);
		memcpy(m_pData8, pData8, nLen8);
		m_nLen8 = nLen8;
	}
    m_Arg1 = Arg1;
    m_Arg2 = Arg2;
    m_Arg3 = Arg3;
    m_Arg4 = 0;
    m_Arg5 = 0;
	m_Arg6 = 0;
	m_Arg7 = 0;
	m_Arg8 = 0;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
CBufferT<T1, T2, T3, T4, T5, T6, T7, T8>::CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
	, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4)
: m_pData1(NULL)
, m_nLen1(0)
, m_pData2(NULL)
, m_nLen2(0)
, m_pData3(NULL)
, m_nLen3(0)
, m_pData4(NULL)
, m_nLen4(0)
, m_pData5(NULL)
, m_nLen5(0)
, m_pData6(NULL)
, m_nLen6(0)
, m_pData7(NULL)
, m_nLen7(0)
, m_pData8(NULL)
, m_nLen8(0)
{
	if (pData1 != NULL && nLen1 > 0) 
	{
		m_pData1 = new char[nLen1+1];
		memset(m_pData1, 0, nLen1+1);
		memcpy(m_pData1, pData1, nLen1);
		m_nLen1 = nLen1;
	}
 	if (pData2 != NULL && nLen2 > 0) 
	{
		m_pData2 = new char[nLen2+1];
		memset(m_pData2, 0, nLen2+1);
		memcpy(m_pData2, pData2, nLen2);
		m_nLen2 = nLen2;
	}
  	if (pData3 != NULL && nLen3 > 0) 
	{
		m_pData3 = new char[nLen3+1];
		memset(m_pData3, 0, nLen3+1);
		memcpy(m_pData3, pData3, nLen3);
		m_nLen3 = nLen3;
	}
  	if (pData4 != NULL && nLen4 > 0) 
	{
		m_pData4 = new char[nLen4+1];
		memset(m_pData4, 0, nLen4+1);
		memcpy(m_pData4, pData4, nLen4);
		m_nLen4 = nLen4;
	}
  	if (pData5 != NULL && nLen5 > 0) 
	{
		m_pData5 = new char[nLen5+1];
		memset(m_pData5, 0, nLen5+1);
		memcpy(m_pData5, pData5, nLen5);
		m_nLen5 = nLen5;
	}
  	if (pData6 != NULL && nLen6 > 0) 
	{
		m_pData6 = new char[nLen6+1];
		memset(m_pData6, 0, nLen6+1);
		memcpy(m_pData6, pData6, nLen6);
		m_nLen6 = nLen6;
	}
	if (pData7 != NULL && nLen7 > 0)
	{
		m_pData7 = new char[nLen7 + 1];
		memset(m_pData7, 0, nLen7 + 1);
		memcpy(m_pData7, pData7, nLen7);
		m_nLen7 = nLen7;
	}
	if (pData8 != NULL && nLen8 > 0)
	{
		m_pData8 = new char[nLen8 + 1];
		memset(m_pData8, 0, nLen8 + 1);
		memcpy(m_pData8, pData8, nLen8);
		m_nLen8 = nLen8;
	}
	m_Arg1 = Arg1;
    m_Arg2 = Arg2;
    m_Arg3 = Arg3;
    m_Arg4 = Arg4;
    m_Arg5 = 0;
	m_Arg6 = 0;
	m_Arg7 = 0;
	m_Arg8 = 0;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
CBufferT<T1, T2, T3, T4, T5, T6, T7, T8>::CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
	, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5)
: m_pData1(NULL)
, m_nLen1(0)
, m_pData2(NULL)
, m_nLen2(0)
, m_pData3(NULL)
, m_nLen3(0)
, m_pData4(NULL)
, m_nLen4(0)
, m_pData5(NULL)
, m_nLen5(0)
, m_pData6(NULL)
, m_nLen6(0)
, m_pData7(NULL)
, m_nLen7(0)
, m_pData8(NULL)
, m_nLen8(0)
{
	if (pData1 != NULL && nLen1 > 0) 
	{
		m_pData1 = new char[nLen1+1];
		memset(m_pData1, 0, nLen1+1);
		memcpy(m_pData1, pData1, nLen1);
		m_nLen1 = nLen1;
	}
 	if (pData2 != NULL && nLen2 > 0) 
	{
		m_pData2 = new char[nLen2+1];
		memset(m_pData2, 0, nLen2+1);
		memcpy(m_pData2, pData2, nLen2);
		m_nLen2 = nLen2;
	}
  	if (pData3 != NULL && nLen3 > 0) 
	{
		m_pData3 = new char[nLen3+1];
		memset(m_pData3, 0, nLen3+1);
		memcpy(m_pData3, pData3, nLen3);
		m_nLen3 = nLen3;
	}
  	if (pData4 != NULL && nLen4 > 0) 
	{
		m_pData4 = new char[nLen4+1];
		memset(m_pData4, 0, nLen4+1);
		memcpy(m_pData4, pData4, nLen4);
		m_nLen4 = nLen4;
	}
  	if (pData5 != NULL && nLen5 > 0) 
	{
		m_pData5 = new char[nLen5+1];
		memset(m_pData5, 0, nLen5+1);
		memcpy(m_pData5, pData5, nLen5);
		m_nLen5 = nLen5;
	}
  	if (pData6 != NULL && nLen6 > 0) 
	{
		m_pData6 = new char[nLen6+1];
		memset(m_pData6, 0, nLen6+1);
		memcpy(m_pData6, pData6, nLen6);
		m_nLen6 = nLen6;
	}
	if (pData7 != NULL && nLen7 > 0)
	{
		m_pData7 = new char[nLen7 + 1];
		memset(m_pData7, 0, nLen7 + 1);
		memcpy(m_pData7, pData7, nLen7);
		m_nLen7 = nLen7;
	}
	if (pData8 != NULL && nLen8 > 0)
	{
		m_pData8 = new char[nLen8 + 1];
		memset(m_pData8, 0, nLen8 + 1);
		memcpy(m_pData8, pData8, nLen8);
		m_nLen8 = nLen8;
	}
	m_Arg1 = Arg1;
    m_Arg2 = Arg2;
    m_Arg3 = Arg3;
    m_Arg4 = Arg4;
    m_Arg5 = Arg5;
	m_Arg6 = 0;
	m_Arg7 = 0;
	m_Arg8 = 0;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
CBufferT<T1, T2, T3, T4, T5, T6, T7, T8>::CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
	, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
		, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6)
: m_pData1(NULL)
, m_nLen1(0)
, m_pData2(NULL)
, m_nLen2(0)
, m_pData3(NULL)
, m_nLen3(0)
, m_pData4(NULL)
, m_nLen4(0)
, m_pData5(NULL)
, m_nLen5(0)
, m_pData6(NULL)
, m_nLen6(0)
, m_pData7(NULL)
, m_nLen7(0)
, m_pData8(NULL)
, m_nLen8(0)
{
	if (pData1 != NULL && nLen1 > 0) 
	{
		m_pData1 = new char[nLen1+1];
		memset(m_pData1, 0, nLen1+1);
		memcpy(m_pData1, pData1, nLen1);
		m_nLen1 = nLen1;
	}
 	if (pData2 != NULL && nLen2 > 0) 
	{
		m_pData2 = new char[nLen2+1];
		memset(m_pData2, 0, nLen2+1);
		memcpy(m_pData2, pData2, nLen2);
		m_nLen2 = nLen2;
	}
  	if (pData3 != NULL && nLen3 > 0) 
	{
		m_pData3 = new char[nLen3+1];
		memset(m_pData3, 0, nLen3+1);
		memcpy(m_pData3, pData3, nLen3);
		m_nLen3 = nLen3;
	}
  	if (pData4 != NULL && nLen4 > 0) 
	{
		m_pData4 = new char[nLen4+1];
		memset(m_pData4, 0, nLen4+1);
		memcpy(m_pData4, pData4, nLen4);
		m_nLen4 = nLen4;
	}
  	if (pData5 != NULL && nLen5 > 0) 
	{
		m_pData5 = new char[nLen5+1];
		memset(m_pData5, 0, nLen5+1);
		memcpy(m_pData5, pData5, nLen5);
		m_nLen5 = nLen5;
	}
  	if (pData6 != NULL && nLen6 > 0) 
	{
		m_pData6 = new char[nLen6+1];
		memset(m_pData6, 0, nLen6+1);
		memcpy(m_pData6, pData6, nLen6);
		m_nLen6 = nLen6;
	}
	if (pData7 != NULL && nLen7 > 0)
	{
		m_pData7 = new char[nLen7 + 1];
		memset(m_pData7, 0, nLen7 + 1);
		memcpy(m_pData7, pData7, nLen7);
		m_nLen7 = nLen7;
	}
	if (pData8 != NULL && nLen8 > 0)
	{
		m_pData8 = new char[nLen8 + 1];
		memset(m_pData8, 0, nLen8 + 1);
		memcpy(m_pData8, pData8, nLen8);
		m_nLen8 = nLen8;
	}
	m_Arg1 = Arg1;
    m_Arg2 = Arg2;
    m_Arg3 = Arg3;
    m_Arg4 = Arg4;
    m_Arg5 = Arg5;
	m_Arg6 = Arg6;
	m_Arg7 = 0;
	m_Arg8 = 0;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
CBufferT<T1, T2, T3, T4, T5, T6, T7, T8>::CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
	, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
	, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7)
: m_pData1(NULL)
, m_nLen1(0)
, m_pData2(NULL)
, m_nLen2(0)
, m_pData3(NULL)
, m_nLen3(0)
, m_pData4(NULL)
, m_nLen4(0)
, m_pData5(NULL)
, m_nLen5(0)
, m_pData6(NULL)
, m_nLen6(0)
, m_pData7(NULL)
, m_nLen7(0)
, m_pData8(NULL)
, m_nLen8(0)
{
	if (pData1 != NULL && nLen1 > 0)
	{
		m_pData1 = new char[nLen1 + 1];
		memset(m_pData1, 0, nLen1 + 1);
		memcpy(m_pData1, pData1, nLen1);
		m_nLen1 = nLen1;
	}
	if (pData2 != NULL && nLen2 > 0)
	{
		m_pData2 = new char[nLen2 + 1];
		memset(m_pData2, 0, nLen2 + 1);
		memcpy(m_pData2, pData2, nLen2);
		m_nLen2 = nLen2;
	}
	if (pData3 != NULL && nLen3 > 0)
	{
		m_pData3 = new char[nLen3 + 1];
		memset(m_pData3, 0, nLen3 + 1);
		memcpy(m_pData3, pData3, nLen3);
		m_nLen3 = nLen3;
	}
	if (pData4 != NULL && nLen4 > 0)
	{
		m_pData4 = new char[nLen4 + 1];
		memset(m_pData4, 0, nLen4 + 1);
		memcpy(m_pData4, pData4, nLen4);
		m_nLen4 = nLen4;
	}
	if (pData5 != NULL && nLen5 > 0)
	{
		m_pData5 = new char[nLen5 + 1];
		memset(m_pData5, 0, nLen5 + 1);
		memcpy(m_pData5, pData5, nLen5);
		m_nLen5 = nLen5;
	}
	if (pData6 != NULL && nLen6 > 0)
	{
		m_pData6 = new char[nLen6 + 1];
		memset(m_pData6, 0, nLen6 + 1);
		memcpy(m_pData6, pData6, nLen6);
		m_nLen6 = nLen6;
	}
	if (pData7 != NULL && nLen7 > 0)
	{
		m_pData7 = new char[nLen7 + 1];
		memset(m_pData7, 0, nLen7 + 1);
		memcpy(m_pData7, pData7, nLen7);
		m_nLen7 = nLen7;
	}
	if (pData8 != NULL && nLen8 > 0)
	{
		m_pData8 = new char[nLen8 + 1];
		memset(m_pData8, 0, nLen8 + 1);
		memcpy(m_pData8, pData8, nLen8);
		m_nLen8 = nLen8;
	}
	m_Arg1 = Arg1;
	m_Arg2 = Arg2;
	m_Arg3 = Arg3;
	m_Arg4 = Arg4;
	m_Arg5 = Arg5;
	m_Arg6 = Arg6;
	m_Arg7 = Arg7;
	m_Arg8 = 0;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
CBufferT<T1, T2, T3, T4, T5, T6, T7, T8>::CBufferT(char* pData1, unsigned int nLen1, char* pData2, unsigned int nLen2, char* pData3, unsigned int nLen3, char* pData4, unsigned int nLen4
	, char* pData5, unsigned int nLen5, char* pData6, unsigned int nLen6, char* pData7, unsigned int nLen7, char* pData8, unsigned int nLen8
	, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7, T8 Arg8)
: m_pData1(NULL)
, m_nLen1(0)
, m_pData2(NULL)
, m_nLen2(0)
, m_pData3(NULL)
, m_nLen3(0)
, m_pData4(NULL)
, m_nLen4(0)
, m_pData5(NULL)
, m_nLen5(0)
, m_pData6(NULL)
, m_nLen6(0)
, m_pData7(NULL)
, m_nLen7(0)
, m_pData8(NULL)
, m_nLen8(0)
{
	if (pData1 != NULL && nLen1 > 0)
	{
		m_pData1 = new char[nLen1 + 1];
		memset(m_pData1, 0, nLen1 + 1);
		memcpy(m_pData1, pData1, nLen1);
		m_nLen1 = nLen1;
	}
	if (pData2 != NULL && nLen2 > 0)
	{
		m_pData2 = new char[nLen2 + 1];
		memset(m_pData2, 0, nLen2 + 1);
		memcpy(m_pData2, pData2, nLen2);
		m_nLen2 = nLen2;
	}
	if (pData3 != NULL && nLen3 > 0)
	{
		m_pData3 = new char[nLen3 + 1];
		memset(m_pData3, 0, nLen3 + 1);
		memcpy(m_pData3, pData3, nLen3);
		m_nLen3 = nLen3;
	}
	if (pData4 != NULL && nLen4 > 0)
	{
		m_pData4 = new char[nLen4 + 1];
		memset(m_pData4, 0, nLen4 + 1);
		memcpy(m_pData4, pData4, nLen4);
		m_nLen4 = nLen4;
	}
	if (pData5 != NULL && nLen5 > 0)
	{
		m_pData5 = new char[nLen5 + 1];
		memset(m_pData5, 0, nLen5 + 1);
		memcpy(m_pData5, pData5, nLen5);
		m_nLen5 = nLen5;
	}
	if (pData6 != NULL && nLen6 > 0)
	{
		m_pData6 = new char[nLen6 + 1];
		memset(m_pData6, 0, nLen6 + 1);
		memcpy(m_pData6, pData6, nLen6);
		m_nLen6 = nLen6;
	}
	if (pData7 != NULL && nLen7 > 0)
	{
		m_pData7 = new char[nLen7 + 1];
		memset(m_pData7, 0, nLen7 + 1);
		memcpy(m_pData7, pData7, nLen7);
		m_nLen7 = nLen7;
	}
	if (pData8 != NULL && nLen8 > 0)
	{
		m_pData8 = new char[nLen8 + 1];
		memset(m_pData8, 0, nLen8 + 1);
		memcpy(m_pData8, pData8, nLen8);
		m_nLen8 = nLen8;
	}
	m_Arg1 = Arg1;
	m_Arg2 = Arg2;
	m_Arg3 = Arg3;
	m_Arg4 = Arg4;
	m_Arg5 = Arg5;
	m_Arg6 = Arg6;
	m_Arg7 = Arg7;
	m_Arg8 = Arg8;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
CBufferT<T1, T2, T3, T4, T5, T6, T7, T8>::~CBufferT()
{
	if (m_pData1 != NULL && m_nLen1 > 0) 
	{
		delete[] m_pData1;
	}
	if (m_pData2 != NULL && m_nLen2 > 0) 
	{
		delete[] m_pData2;
	}
	if (m_pData3 != NULL && m_nLen3 > 0) 
	{
		delete[] m_pData3;
	}
	if (m_pData4 != NULL && m_nLen4 > 0) 
	{
		delete[] m_pData4;
	}
	if (m_pData5 != NULL && m_nLen5 > 0) 
	{
		delete[] m_pData5;
	}
	if (m_pData6 != NULL && m_nLen6 > 0) 
	{
		delete[] m_pData6;
	}
	if (m_pData7 != NULL && m_nLen7 > 0)
	{
		delete[] m_pData7;
	}
	if (m_pData8 != NULL && m_nLen8 > 0)
	{
		delete[] m_pData8;
	}
}
#endif