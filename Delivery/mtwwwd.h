#ifndef _MTWWWD_H_
#define _MTWWWD_H_

typedef struct HTTP_Request {
    char* method[10];
    char* path[512];
    char* query[4096];
    char* HTTP_ver[64];
} HTTP_Request_t;

#endif