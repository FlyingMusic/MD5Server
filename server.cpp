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
#include <sys/epoll.h>
#include "ThreadDeque.h"
#include "ThreadPool.h"
#include "msg_type.h"

#define MAXCLIENT	20
#define MAXEPOLL    100

int main()
{
	start_threads(1);
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
	
	struct  epoll_event evs[MAXEPOLL];	
	int epoll_fd = epoll_create(MAXEPOLL);
	struct  epoll_event ev;
	ev.events = EPOLLIN;	
	ev.data.fd = socketfd;
	ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socketfd, &ev);
	if(ret < 0) {
		printf("epoll_ctl error[%s]\n", strerror(errno));
		return -1;
	}
	int curfd = 1;
	while(true) {
		usleep(1000);
		int ewret = epoll_wait(epoll_fd, evs, curfd, 500);
		if(ewret < 0) { //error
			if(errno == EINTR) {
				continue;
			}
			printf("epoll_wait error[%s]\n", strerror(errno));
			break;
		}
		if(ewret == 0) { //time out
			printf("timeout...\n");
			continue;
		}
		for(int i = 0; i < ewret; i++) {
			if(evs[i].data.fd == socketfd && curfd < MAXEPOLL) { //new client
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
				struct  epoll_event ev;
				ev.events = EPOLLIN;	
				ev.data.fd = connfd;
				ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connfd, &ev);
				if(ret < 0) {
					printf("epoll_ctl error[%s]\n", strerror(errno));
					continue;
				}
				curfd++;
				printf("add connfd[%d] to epoll succes!\n", ev.data.fd);
			} else {
				int tbytes ;
				ioctl(evs[i].data.fd, FIONREAD, &tbytes);
				printf("available data[%d]\n", tbytes);
				if(tbytes <= 0) { //client close the connection
					printf("client close the connfd[%d]\n", evs[i].data.fd);
					close(evs[i].data.fd); 
					ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, evs[i].data.fd, &ev);
					if(ret < 0) {
						printf("epoll_ctl error[%s]\n", strerror(errno));
						continue;
					}
					curfd--;
				} else {
					printf("add task fd[%d]\n", evs[i].data.fd);
					add_tdeque_task(evs[i].data.fd);
				}
			}
		}
	} //while
	printf("server will stop!!!\n");
	getchar();
	wait_threads();
	close(socketfd);
	return 0;
}
