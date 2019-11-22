#ifndef _LOGE__H_
#define _LOGE__H_

void total_log(const char* file_name, const char * format,...);
void total_log_fp(void *file_point, const char * format, ...);
int netlogger_initial(const char *server_ip,const int port);
int netlogger_printto(const char *data);
#endif
