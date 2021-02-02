#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include "md5.h"
#include "msg_type.h"
#include "session.h"
#include "inter.h"


void remove_map_item(fd_session_map_t &fd_session_map, fd_session_iter_t fd_session_iter);
int recv_data(session_data_t session, int fd);
int send_responce(session_data_t session, int fd);
int send_result(session_data_t session, int fd);
int deal_left_data(session_data_t session);

void work_thread_callback(void *arg) {
    while(true) {
        md5_inter_t p_inter = (md5_inter_t)arg;
        concurrent_queue<int> &request_queue = p_inter->request_queue;
        concurrent_queue<int> &feedback_queue = p_inter->feedback_queue;
        fd_session_map_t &fd_session_map = p_inter->fd_session_map;
        int fd = request_queue.wait_and_pop();
        printf("pthreadid[%lu]: get fd[%d]\n", pthread_self(), fd);
        
        fd_session_iter_t fd_session_iter = fd_session_map.find(fd);
        if(fd_session_iter == fd_session_map.end()) {
            fd_session_map[fd] = new session_data_st;
            if(fd_session_map[fd] == NULL) {
                printf("session data malloc fail!\n");
                return;
            }
            fd_session_map[fd]->reset();
        }
        fd_session_iter = fd_session_map.find(fd);
        session_data_t session = fd_session_iter->second;

        if((unsigned int)session->headlen < sizeof(msg_head_st)) { //要接收头
            int recvlen = recv(fd, &(session->head), sizeof(msg_head_st)-session->headlen, MSG_DONTWAIT);
            if(recvlen < 0) {
                if(errno == EAGAIN) {
                    printf("pthreadid[%lu]: recv head again\n", pthread_self());
                    continue;
                }
                printf("pthreadid[%lu]: recv error[%s]\n", pthread_self(), strerror(errno));
                remove_map_item(fd_session_map, fd_session_iter);
                feedback_queue.push(fd);
                continue;
            }
            if(recvlen == 0) {
                printf("pthreadid[%lu]: client close the connectioin\n", pthread_self());
                remove_map_item(fd_session_map, fd_session_iter);
                feedback_queue.push(fd);
                continue;
            }
            session->headlen += recvlen;
            if((unsigned int)session->headlen < sizeof(session->head)) {
                continue;
            }
            //解析头
            switch(session->head.cmd) {
                case CMD_REQUEST: 
                    if(session->state != STATE_INIT) {
                        remove_map_item(fd_session_map, fd_session_iter);
                        feedback_queue.push(fd);
                        continue;
                    }
                    session->state = STATE_START;
                    session->headlen = 0;
                    printf("pthreadid[%lu]: recv request success\n", pthread_self());
                    send_responce(session, fd);
                    break;
                case CMD_DATA:
                    if(session->state != STATE_START && session->state != STATE_DATA)
                    {
                        remove_map_item(fd_session_map, fd_session_iter);
                        feedback_queue.push(fd);
                        continue;
                    }
                    //recv data
                    if(recv_data(session, fd) != 0) {
                        remove_map_item(fd_session_map, fd_session_iter);
                        feedback_queue.push(fd);
                    }
                    session->state = STATE_DATA;
                    break;
                case CMD_FINISH:
                    if(session->state != STATE_DATA) {
                        remove_map_item(fd_session_map, fd_session_iter);
                        feedback_queue.push(fd);
                    }
                    //return result
                    printf("pthreadid[%lu]: recv finish\n", pthread_self());
                    deal_left_data(session);
                    send_result(session, fd);
                    remove_map_item(fd_session_map, fd_session_iter);
                    feedback_queue.push(fd);
                    break;
                default:
                    printf("pthreadid[%lu]: invalid cmd type\n", pthread_self());
            }
        } else {
            if(recv_data(session, fd) != 0) {
                remove_map_item(fd_session_map, fd_session_iter);
                feedback_queue.push(fd);
            }
        }
    }
}
void remove_map_item(fd_session_map_t &fd_session_map, fd_session_iter_t fd_session_iter)
{
    //close(fd_session_iter->first); 不能在关闭另一个线程中epoll中的fd,未定义行为
    delete(fd_session_iter->second);
    fd_session_map.erase(fd_session_iter);
}
int deal_left_data(session_data_t session)
{
    MD5Update(&(session->md5), (unsigned char*)session->buff, session->bufflen);
    return 0;
}
int sum_recv = 0;
int recv_data(session_data_t session, int fd) 
{
    int buffleft = sizeof(session->buff) - session->bufflen;
    int packleft = session->head.datalen - session->datalen;
    int maxlen = buffleft < packleft ? buffleft : packleft;
    int recvlen    = recv(fd, session->buff+session->bufflen, maxlen, MSG_DONTWAIT);
    if(recvlen < 0) {
        if(errno == EAGAIN) {
            printf("pthreadid[%lu]: recv again\n", pthread_self());
            return 0;
        }
        printf("pthreadid[%lu]: recv error[%s]\n", pthread_self(), strerror(errno));
        return -1;
    }
    if(recvlen == 0) {
        printf("pthreadid[%lu]: client close the connectioin\n", pthread_self());
        return -1;
    }
    session->bufflen += recvlen;
    session->datalen += recvlen;
    sum_recv += recvlen;
    printf("pthreadid[%lu]: recv sum data[%d]\n", pthread_self(), sum_recv);
    if(session->bufflen == sizeof(session->buff)) {
        MD5Update(&(session->md5), (unsigned char*)session->buff, session->bufflen);
        session->bufflen = 0;
    }
    if(session->head.datalen == session->datalen) {
        session->datalen = 0;
        session->headlen = 0;
    }
    return 0;
}
int send_responce(session_data_t session, int fd)
{
    msg_head_st head;
    head.cmd = CMD_RESPONCE;
    head.datalen = 0;
    int sendlen = send(fd, &head, sizeof(head), MSG_WAITALL);
    if(sendlen < 0) {
        printf("send to [%d] responce error[%s]\n", fd, strerror(errno));
        return -1;
    }
    printf("send to [%d] responce success\n", fd);
    return 0;
}

int send_result(session_data_t session, int fd) 
{
    unsigned char decrypt[16];
    char result[64] = {0};
    MD5Final(&(session->md5), decrypt);
    for (int i = 0; i < 16; i++) {
        sprintf(result+i*2, "%02X", decrypt[i]);
    }
    char sendbuff[1024] = {0};
    msg_head_st head;
    head.cmd = CMD_RESULT;
    head.datalen = 32;
    memcpy(sendbuff, &head, sizeof(head));
    memcpy(sendbuff+sizeof(head), result, 32);
    int sendlen = send(fd, sendbuff, sizeof(head)+32, MSG_WAITALL);
    if(sizeof(head)+32 != sendlen) {
        printf("send to [%d] result error[%s]\n", fd, strerror(errno));
        return -1;
    }
    printf("send to [%d] result success[%s]\n", fd, result);
    return 0;
}
