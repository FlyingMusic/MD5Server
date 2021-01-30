#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include "msg_type.h"
    
#define MAXCLIENT    20

int main()
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd < 0) {
        printf("socket fd[%d] is invalid!\n");
        return -1;
    }
    printf("socket[%d] success...\n", socketfd);
    int nBufLen = 0;
    setsockopt(socketfd,SOL_SOCKET,SO_SNDBUF,(char*)&nBufLen,sizeof(nBufLen));
    int on = 1;
    setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR, (void *) &on, sizeof (on));
    unsigned long ul = 1;
    ioctl(socketfd, FIONBIO, &ul);
    int port = 5555;
    char ip[] = "192.168.128.70";
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_aton(ip, &server_addr.sin_addr);

    int ret = bind(socketfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret < 0) {
        printf("bind error: %s\n", strerror(errno));
        close(socketfd);
        return -1;
    } 
    printf("bind success...\n");

    ret = listen(socketfd, 10);
    if(ret < 0) {
        close(socketfd);
        printf("listen error[%s]\n", strerror(errno));
        return -1;
    }
    printf("listen success...\n");
    
    fd_set fdset;

    int clientfd[MAXCLIENT] = {0};
    int maxfd = socketfd;
        
    while(true) {
        FD_ZERO(&fdset);
        FD_SET(socketfd, &fdset);//等待连接的fd
        for(int i = 0;i < MAXCLIENT; i++) {
            if(clientfd[i] != 0) {
                FD_SET(clientfd[i], &fdset);
                if(clientfd[i] > maxfd) {
                    maxfd = clientfd[i];
                }
            }
        }
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        int selret = select(maxfd +1, &fdset, NULL, NULL, &tv);
        if(selret < 0) { //error
            printf("select error[%s]\n", strerror(errno));
            break;
        }
        if(selret == 0) { //time out
            printf("timeout...\n");
            continue;
        }
        if(FD_ISSET(socketfd, &fdset)) { //new client
            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(client_addr));
            socklen_t slen = sizeof(client_addr);
            int connfd = accept(socketfd, (struct sockaddr *)&client_addr, &slen);
            if(connfd < 0) {
                printf("accept error, ret value[%d]\n", ret);
                continue;
            }
            if(slen < 0) {
                close(socketfd);
                printf("accept error, slen[%ul]\n", slen);
                continue;
            }
            printf("accept success:ip[%s], port[%d]\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
            int clientindex = 0;
            for(clientindex = 0; clientindex < MAXCLIENT; ++clientindex) {
                if(clientfd[clientindex] == 0) {
                    clientfd[clientindex] = connfd;
                    break;
                }
            }
            if(clientindex == MAXCLIENT) {
                continue;
            }
        }
        for(int i = 0; i < MAXCLIENT; ++i) {
            if(clientfd[i] == 0) {
                continue;
            }
            if(FD_ISSET(clientfd[i], &fdset)) {
                char recvbuff[1024] = {0};
                ret = recv(clientfd[i], recvbuff, 1024, 0);
                if(ret < 0) {
                    printf("pthreadid[%u]: recv error[%s]\n", pthread_self(), strerror(errno));
                    close(clientfd[i]);
                    clientfd[i] = 0;
                    continue;
                }
                if(ret == 0) {
                    printf("pthreadid[%u]: client close the connectioin\n", pthread_self());
                    close(clientfd[i]);
                    clientfd[i] = 0;
                    continue;
                }
                if(ret < sizeof(msg_head_st)) {
                    //接收到的数据不完整,暂且当做错误
                    printf("incompile data!!!\n");
                    close(clientfd[i]);
                    clientfd[i] = 0;
                }
                printf("pthreadid[%u]: recv data[%d][%s]\n", pthread_self(), ret, recvbuff);

                msg_head_st recvhead;
                memcpy(&recvhead, recvbuff, sizeof(recvhead));
                msg_head_st sendhead;

                if(recvhead.cmd == TIME) {
                    sendhead.cmd = TIME;
                    char sendbuff[1024] = {0};
                    time_t td;
                    time(&td);
                    char dtbuf[128] = {0};
                    strftime(dtbuf, sizeof(dtbuf), "%A %b %d %H:%M:%S %Y\n", localtime(&td));
                    memcpy(sendbuff, &sendhead, sizeof(sendhead));
                    memcpy(sendbuff+sizeof(sendhead), dtbuf, strlen(dtbuf));
                    int bufflen = sizeof(sendhead) + strlen(dtbuf);
                    int sendlen = send(clientfd[i], sendbuff, bufflen, MSG_WAITALL);
                    if(bufflen != sendlen) {
                        close(clientfd[i]);
                        clientfd[i] = 0;
                        printf("send error[%s]\n", strerror(errno));
                        continue;
                    }
                    printf("send success...\n");
                }

            }
        } //for
    } //while
    getchar();
    close(socketfd);
    for(int i = 0; i < MAXCLIENT; ++i) {
        if(clientfd[i] != 0)
            close(clientfd[i]);
    }
    return 0;
}
