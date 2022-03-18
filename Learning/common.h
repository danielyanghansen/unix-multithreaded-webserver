#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/socket.h> /*basic socket definitions*/
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h> /* for inet_pton ++*/
#include <stdarg.h> /*For variable argument functions, like err_n_die*/
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <stddef.h>

//Useful Constants
#define SERVER_PORT 8081 

#define MAXLINE 2048
#define SA struct sockaddr

void err_n_die(const char *fmtm, ...);
char *bin2hex(const unsigned char *input, size_t len);



#endif