#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 1024
#define LISTENQ 4

int main(int argc, char **argv){
    int listenfd, connfd;
    socklen_t len;
    char buff[MAXLINE], str[INET6_ADDRSTRLEN+1],
    nickname[MAXLINE];
    time_t ticks;
    struct sockaddr_in6 servaddr, cliaddr;

    if ( (listenfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0){
        fprintf(stderr,"socket error : %s\n", strerror(errno));
        return 1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;

    if(argc == 1)
        servaddr.sin6_addr = in6addr_any;
    else{
        if( inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr) != 1 ){
        printf("ERROR: Address format error\n");
        return -1;
        }
    }
    servaddr.sin6_port = htons(13);
    if ( bind( listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))< 0){
        fprintf(stderr,"bind error : %s\n", strerror(errno));
        return 1;
    }
    if ( listen(listenfd, LISTENQ) < 0){
        fprintf(stderr,"listen error : %s\n", strerror(errno));
        return 1;
    }
    fprintf(stderr,"Waiting for clients ... \n");
    for ( ; ; ) {
        len = sizeof(cliaddr);
        if ( (connfd = accept(listenfd, (struct sockaddr *) &cliaddr,&len)) < 0){
            fprintf(stderr,"accept error : %s\n", strerror(errno));
            continue;
        }
        bzero(str, sizeof(str));
        inet_ntop(AF_INET6, (struct sockaddr *) &cliaddr.sin6_addr, str,sizeof(str));
        printf("Connection from %s\n", str);
        bzero(nickname, sizeof(nickname));
        if (read(connfd, nickname, MAXLINE) > 0) {
            nickname[strcspn(nickname, "\n")] = 0;
            printf("Received nickname: %s\n", nickname);
        } else {
            fprintf(stderr, "read error: %s\n", strerror(errno));
        }
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        if( write(connfd, buff, strlen(buff)) < 0 )
            fprintf(stderr,"write error : %s\n", strerror(errno));
        close(connfd);
    }
}