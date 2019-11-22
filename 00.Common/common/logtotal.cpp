#include "logtotal.h"
#include <sys/timeb.h>
extern int log_bt2;
#ifdef WIN32
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

void total_log(const char* file_name, const char * format,...)
{
    if (0 == log_bt2)
    {
        return;
    }
    
    FILE *fp = NULL;
    fp = fopen(file_name, "a+");
    if (NULL != fp) {
        struct timeb loTimeb;
        //memset(&loTimeb, 0 , sizeof(timeb));
        ftime(&loTimeb);

        time_t now = loTimeb.time;
        struct tm tm_now;
        localtime_s(&tm_now, &loTimeb.time);
        fprintf(fp, "%d-%d-%d_%d:%d:%d.%03d ", 
            tm_now.tm_year+1900,tm_now.tm_mon+1,tm_now.tm_mday,
            tm_now.tm_hour,tm_now.tm_min, tm_now.tm_sec, loTimeb.millitm);
        va_list apptr;
        va_start(apptr,format);
        vfprintf(fp, format, apptr);
        va_end(apptr);
        fclose(fp);
    }
}

void total_log_fp(void *file_point, const char * format, ...)
{
	if (0 == log_bt2)
    {
        return;
    }
    FILE *fp = (FILE*)file_point;
    if (NULL != fp) {
        struct timeb loTimeb;
        //memset(&loTimeb, 0 , sizeof(timeb));
        ftime(&loTimeb);

        time_t now = loTimeb.time;
        struct tm tm_now;
        localtime_s(&tm_now, &loTimeb.time);
        fprintf(fp, "%d-%d-%d_%d:%d:%d.%03d ", 
            tm_now.tm_year+1900,tm_now.tm_mon+1,tm_now.tm_mday,
            tm_now.tm_hour,tm_now.tm_min, tm_now.tm_sec, loTimeb.millitm);
        va_list apptr;
        va_start(apptr, format);
        vfprintf(fp, format, apptr);
        va_end(apptr);
    }
}

#elif defined WEBRTC_IOS
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
void total_log(const char* file_name, const char * format,...)
{
    if (0 == log_bt2)
    {
        return;
    }
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    fprintf(stdout, "[%s]%d-%d-%d_%d:%d:%d ", file_name, 
	    tm_now.tm_year+1900,tm_now.tm_mon+1,tm_now.tm_mday,
	    tm_now.tm_hour,tm_now.tm_min, tm_now.tm_sec);
    va_list apptr;
    va_start(apptr, format);
    vfprintf(stdout, format, apptr);
    fflush(stdout);
    va_end(apptr);
}
#elif defined LINUX
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
void total_log(const char* file_name, const char * format,...)
{
    if (0 == log_bt2)
    {
        return;
    }

    FILE *fp = NULL;
    fp = fopen(file_name, "a+");
    if (NULL != fp) 
    {
        time_t now = time(NULL);
        struct tm tm_now;
        localtime_r(&now, &tm_now);
        fprintf(fp, "%d-%d-%d_%d:%d:%d ",
            tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday,
            tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
        va_list apptr;
        va_start(apptr, format);
        vfprintf(fp, format, apptr);
        fclose(fp);
    }
}

void total_log_fp(void *file_point, const char * format, ...)
{
    if (0 == log_bt2)
    {
        return;
    }
    FILE *fp = (FILE*)file_point;
    if (NULL != fp) {
        time_t now = time(NULL);
        struct tm tm_now;
        localtime_r(&now, &tm_now);
        fprintf(fp, "%d-%d-%d_%d:%d:%d ",
            tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday,
            tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
        va_list apptr;
        va_start(apptr, format);
        vfprintf(fp, format, apptr);
        fclose(fp);
    }
}

#else
#include <android/log.h>
void total_log(const char* file_name, const char * format,...)
{
    va_list apptr;
    va_start(apptr,format);
    __android_log_vprint(ANDROID_LOG_DEBUG, file_name, format, apptr);
    va_end(apptr);
}
#endif

