#include "Utils.h"
#if defined(OSX) || defined(IOS)
#include <sys/time.h>
#include <mach/mach_time.h>
#endif
#if defined(LINUX)
#include <stdarg.h>
#include <time.h>
#include<sys/time.h>
#endif

#ifdef _WIN32
#include <Mmsystem.h>
#pragma comment( lib,"winmm.lib" )
#endif

#include <sys/types.h> 
#include <sys/timeb.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

uint64_t Utils::Time()
{
#ifdef LINUX
    struct timeval tpTmp;
    gettimeofday(&tpTmp, NULL);
    return (uint64_t)(tpTmp.tv_sec * 1000) + uint64_t(tpTmp.tv_usec/1000);
#else
    struct timeb loTimeb;
    //memset(&loTimeb, 0 , sizeof(timeb));
    ftime(&loTimeb);
    return ((uint64_t)loTimeb.time * 1000) + uint64_t(loTimeb.millitm);
#endif
}

uint64_t Utils::TimeNanos() {
    int64_t ticks = 0;
#if defined(OSX) || defined(IOS)
    static mach_timebase_info_data_t timebase;
    if (timebase.denom == 0) {
        // Get the timebase if this is the first time we run.
        // Recommended by Apple's QA1398.
        //VERIFY(KERN_SUCCESS == mach_timebase_info(&timebase));
        KERN_SUCCESS == mach_timebase_info(&timebase);
    }
    // Use timebase to convert absolute time tick units into nanoseconds.
    ticks = mach_absolute_time() * timebase.numer / timebase.denom;
#elif defined(ANDROID) || defined(LINUX)
    struct timespec ts;
    // TODO: Do we need to handle the case when CLOCK_MONOTONIC
    // is not supported?
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ticks = kNumNanosecsPerSec * static_cast<int64_t>(ts.tv_sec) +
        static_cast<int64_t>(ts.tv_nsec);
#elif defined(WIN32)
    static volatile LONG last_timegettime = 0;
    static volatile int64_t num_wrap_timegettime = 0;
    volatile LONG* last_timegettime_ptr = &last_timegettime;
    DWORD now = timeGetTime();
    // Atomically update the last gotten time
    DWORD old = InterlockedExchange(last_timegettime_ptr, now);
    if (now < old) {
        // If now is earlier than old, there may have been a race between
        // threads.
        // 0x0fffffff ~3.1 days, the code will not take that long to execute
        // so it must have been a wrap around.
        if (old > 0xf0000000 && now < 0x0fffffff) {
            num_wrap_timegettime++;
        }
    }
    ticks = now + (num_wrap_timegettime << 32);
    // TODO: Calculate with nanosecond precision.  Otherwise, we're just
    // wasting a multiply and divide when doing Time() on Windows.
    ticks = ticks * kNumNanosecsPerMillisec;
#endif
    return ticks;
}

void Utils::Sleep(uint32_t millisecond)
{

#ifdef _WIN32
    ::Sleep(millisecond);
#else
    usleep(millisecond * 1000);
#endif
}

//string utils
std::string Utils::String::Format( const char *pszFormat, ... )
{
    va_list pArgList;

    va_start(pArgList, pszFormat);
    std::string strRes = String::FormatV(pszFormat, pArgList);
    va_end(pArgList);

    return strRes;
}

std::string Utils::String::FormatV( const char *pszFormat, va_list pArgList )
{
    std::string strRes;
    int   nSize = 0;
    char *pszBuffer = NULL;

#ifdef WIN32
    nSize = _vscprintf(pszFormat, pArgList);
#else
    va_list pArgCopy;
    va_copy(pArgCopy, pArgList);
    nSize = vsnprintf(NULL, 0, pszFormat, pArgCopy);
    va_end(pArgCopy);
#endif //OS_PLATFORM_WINDOWS

    pszBuffer = new char[nSize + 1];
    pszBuffer[nSize] = 0;

    vsprintf(pszBuffer, pszFormat, pArgList);

    strRes = pszBuffer;
    delete []pszBuffer;

    return strRes;
}

std::string &Utils::String::AppendFormat( std::string &strOrg, const char *pszFormat, ... )
{
    va_list pArgList;

    va_start(pArgList, pszFormat);
    std::string strAppend = String::FormatV(pszFormat, pArgList);
    va_end(pArgList);

    strOrg.append(strAppend.c_str());
    return strOrg;
}

int Utils::String::Split( const std::string &strContent, Utils::StringArray &nValues, const std::string &strKey )
{
    int nStartPos = 0;
    int nFindPos = 0;
    int nItemCount = 0;
    int nKeyLen = (int)strKey.size();
    int nEndPos = (int)strContent.size();

    nValues.clear();

    if( strKey.size() == 0 ) return 0;
    if( nEndPos <= 0 || nEndPos >= (int)strContent.size() )
    {
        nEndPos = (int)strContent.size();
    }

    while( nStartPos < nEndPos )
    {
        nFindPos = (int)strContent.find(strKey.c_str(), nStartPos);
        if( nFindPos < 0 || nFindPos >= nEndPos )
        {
            nFindPos = nEndPos;
        }

        if( nFindPos < nStartPos )
        {
            break;
        }

        if( nFindPos > nStartPos )
        {
            std::string strNewValue = (nFindPos > nStartPos) ? strContent.substr(nStartPos, nFindPos - nStartPos) : std::string("");
            nValues.push_back(strNewValue);
            nItemCount ++;
        }

        nStartPos = nFindPos + nKeyLen;
    }

    return nItemCount;
}

std::string Utils::String::Trim(std::string &s) {
    if (s.empty()) {
        return s;
    }

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    s.erase(s.find_last_not_of("\r\b") + 1);
    s.erase(s.find_last_not_of("\n") + 1);
    return s;
}

std::string Utils::GetErrorInfo()
{
    return Utils::String::Format("error code:%u, discrbie:%s", errno, strerror(errno));
}

namespace Utils{
	
char* itoa(int value, char* string, int radix)
{
  char tmp[33];
  char* tp = tmp;
  int i;
  unsigned v;
  int sign;
  char* sp;
  if (radix > 36 || radix <= 1) {
    return 0;
  }
  sign = (radix == 10 && value < 0);
  if (sign)
    v = -value;
  else
    v = (unsigned)value;
  while (v || tp == tmp) {
    i = v % radix;
    v = v / radix;
    if (i < 10)
      *tp++ = i + '0';
    else
      *tp++ = i + 'a' - 10;
  }
  if (string == 0)
    return 0;
  sp = string;
  if (sign)
    *sp++ = '-';
  while (tp > tmp)
    *sp++ = *--tp;
  *sp = 0;
  return string;
}

void itoa(int num, char str[])
{
  int sign = num, i = 0, j = 0;
  char temp[11];

  if (sign<0) {
    num = -num;
  }

  do {
    temp[i] = num % 10 + '0';
    num /= 10;
    i++;
  } while (num>0);

  if (sign<0) {
    temp[i++] = '-';
  }

  temp[i] = '\0';
  i--;

  while (i >= 0) {
    str[j] = temp[i];
    j++;
    i--;
  }

  str[j] = '\0';
}

int atoi(char s[])
{
  int i = 0, sum = 0, sign;    //那?豕?米?那y?～???谷?邦?1車D?????辰??㊣赤﹞?車|?車?D??

  while (' ' == s[i] || '\t' == s[i]) {
    i++;
  }

  sign = ('-' == s[i]) ? -1 : 1;

  if ('-' == s[i] || '+' == s[i]) {
    i++;
  }

  while (s[i] != '\0') {
    sum = s[i] - '0' + sum * 10;
    i++;
  }

  return sign*sum;
}

std::string GetModuleDirectoryPath()
{
    std::string strPath;

#ifdef WIN32
	/*
    //char szPath[1024] = { 0 };
    TCHAR szPath[MAX_PATH];
    if (::GetModuleFileName(NULL, szPath, 1024) >= 0)
    {
        int iLen = WideCharToMultiByte(CP_ACP, 0, szPath, -1, NULL, 0, NULL, NULL);
        char* chRtn = new char[iLen*sizeof(char)];

        WideCharToMultiByte(CP_ACP, 0, szPath, -1, chRtn, iLen, NULL, NULL);

        strPath = chRtn;
        size_t nPos = strPath.rfind("\\");
        if (std::string::npos == nPos)
        {
            return std::string("");
        }
        if (0 == nPos && strPath.size() > 0) nPos++;
        return strPath.substr(0, nPos);
    }
	*/
#endif

    return strPath;
}


} // namespace Utils
