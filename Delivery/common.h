#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/socket.h> /*basic socket definitions*/
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h> /* for inet_pton ++*/
#include <stdarg.h> /*For variable argument functions, like err_n_die*/ //Maybe move to main file
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>

#define TRUE 1
#define FALSE 0

#define BUFSIZE 4096 //how many bytes we send at a time
#define SA struct sockaddr

#define MAX_PENDING_CONNECTIONS 100

#endif