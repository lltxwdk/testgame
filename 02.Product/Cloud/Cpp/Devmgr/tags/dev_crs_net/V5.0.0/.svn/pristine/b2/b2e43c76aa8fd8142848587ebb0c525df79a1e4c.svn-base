
#ifdef WIN32
#include "stdafx.h"
#elif defined LINUX
#include "string.h"
#endif
#include "AutoTrace.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

CAutoTrace* CAutoTrace::GetInstance()
{
	static CAutoTrace g_cAutoTrace;
	return &g_cAutoTrace;
}

CAutoTrace::CAutoTrace()
{

}

// CAutoTrace
void CAutoTrace::AutoTrace(bool bTrace, const char* pFile, const char* pContent)
{
	CCriticalAutoLock mylock(m_cs);
	m_bTrace = bTrace;
	if (!m_bTrace)
		return;

	if (pContent == NULL || strlen(pContent) <= 0)
		return;

	if (pFile == NULL || strlen(pFile) <= 0)
		return;

	if (m_fp == NULL)
		m_fp = fopen(pFile, "w+t");
	if (m_fp == NULL)
		return;

	char buf[1024] = { 0 };

#ifdef WIN32
	SYSTEMTIME  st;
	GetLocalTime(&st);
	sprintf(buf, "%04d/%02d/%02d %02d:%02d:%02d.%d, %s\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, pContent);
#elif defined LINUX
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(buf, "%04d/%02d/%02d %02d:%02d:%02d, %s\n", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, pContent);
#endif

	fwrite(buf, strlen(buf), 1, m_fp);
	fflush(m_fp);
	//fclose(fp);
}

CAutoTrace::~CAutoTrace()
{
	if (m_fp != NULL)
	{
		fflush(m_fp);
		fclose(m_fp);
	}
}
