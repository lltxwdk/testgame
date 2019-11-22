#ifndef CONVERT_H_H_
#define CONVERT_H_H_

int strtohex(const char *buf_in, int in_len, unsigned char *buf_out, int *out_len);
int hextostr(const unsigned char *buf_in,int in_len, char *buf_out, int *out_len);

#endif
