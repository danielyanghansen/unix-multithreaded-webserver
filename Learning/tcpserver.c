#include "common.h" 
#include <stdio.h>
#include <stddef.h>

void err_n_die(const char *fmt, ...) {
    int errno_save;
    va_list ap;

    //all system calls can set errno
    errno_save = errno;

    //print fmt+args to stdout
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    //print error message if errno was set
    if (errno_save != 0) {
        fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }
    va_end(ap);
    exit(1);
}

char *bin2hex(const unsigned char *input, size_t len) {
    char *result;
    char *hexits = "01233456789ABCDEF";

    if (input == NULL || len <= 0) {
        return NULL;
    }
    int resultlength = (len*3)+1;

    result = malloc(resultlength);
    bzero(result, resultlength);

    for (int i=0; i<len; i++) {
        result[i*3] = hexits[input[i] >> 4];
        result[(i*3)+1] = hexits[input[i] & 0x0F];
        result[(i*3)+2] = ' '; //for readability
    }
    return result;
}

int main(int argc, char**argv) {
    int listenfd, connfd, n;
    struct sockaddr_in servaddr;
    uint8_t buff[MAXLINE+1];
    uint8_t recvline[MAXLINE+1];

    //trying to creat a socket and listen to it
    //AF_INET SOCK_STREAM -> TCP Internet Socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_n_die("socket error");
    }

    //setup address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = DEFAULT_SERVER_PORT;

    if ((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0 ) {
        err_n_die("bind error");
    }
    if ((listen(listenfd, 10)) < 0 ) {
        err_n_die("listen error");
    }

    //=======MAIN LOOP=======
    //accept and handle connections
    for(;;) {
        struct sockaddr_in addr;
        socklen_t addr_len;

        printf("waiting for a connection on port %s\n", DEFAULT_SERVER_PORT);
        fflush(stdout);

        connfd = accept(listenfd, (SA *) NULL, NULL);

        memset(recvline, 0, MAXLINE);

        //read client message
        while ((n = read(connfd, recvline, MAXLINE -1 )) > 0) {
            fprintf(stdout, "\n%s\n\n%s", bin2hex(recvline, n), recvline);

            //detect the end of message
            if (recvline[n-1] == '\n') {
                break;
            }

            memset(recvline, 0, MAXLINE); //Clean input
        }
        if (n < 0) {
            err_n_die("read error");
        }

        //send a simple response
        snprintf((char *)buff, sizeof(buff), "HTTP/0.9 200OK\r\n\r\nHello");


    }
}