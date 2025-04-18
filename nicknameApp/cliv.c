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

int main(int argc, char **argv){
    int sockfd, n;
    char recvline[MAXLINE + 1];
    struct sockaddr_in6 servaddr;
    char nickname[100];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <IPaddress>\n", argv[0]);
        exit(1);
    }
    if ( (sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(13);

    if (inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr) <= 0) {
        fprintf(stderr, "inet_pton error for %s\n", argv[1]);
        exit(1);
    }
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        fprintf(stderr, "connect error: %s\n", strerror(errno));
        exit(1);
    }

    printf("Enter your nickname: ");
    fgets(nickname, sizeof(nickname), stdin);
    
    if (write(sockfd, nickname, strlen(nickname)) < 0) {
        fprintf(stderr, "write error: %s\n", strerror(errno));
        exit(1);
    }
    while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0; /* null terminate */
        if (fputs(recvline, stdout) == EOF) {
            fprintf(stderr, "fputs error\n");
            exit(1);
        }
    }
    if (n < 0) {
        fprintf(stderr, "read error\n");
        exit(1);
    }
    close(sockfd);
    return 0;
}