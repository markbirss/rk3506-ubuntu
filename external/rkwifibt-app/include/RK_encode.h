#ifndef __RK_ENCODE_H__
#define __RK_ENCODE_H__

#ifdef __cplusplus
extern "C" {
#endif

int RK_encode_is_utf8(char *buffer, const int size);
int RK_encode_gbk_to_utf8(char* src, int len, char* dst);
int RK_encode_utf8_to_gbk(char* src, int len, char* dst);

#ifdef __cplusplus
}
#endif

#endif
