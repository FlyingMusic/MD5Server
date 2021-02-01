#ifndef _SESSION_H_
#define _SESSION_H_

#include "msg_type.h"
#include "md5.h"

#define MAX_BUFF_LEN    1024

enum session_state {
    STATE_INIT = 0, //刚初始化或复位阶段
    STATE_START = 1, //接收到连接请求
    STATE_DATA = 2, //正在接收数据
    STATE_RESPONCE = 3, //回复结果阶段
};
typedef struct session_data_st {
    msg_head_st head;
    int headlen;
    char buff[MAX_BUFF_LEN];
    int bufflen;
    int datalen;
    int state;
    MD5_CTX md5;
    void reset() {
        headlen = 0;
        memset(buff,0,  sizeof(buff));
        bufflen = 0;
        datalen = 0;
        state = 0;
        MD5Init(&md5);
    }
} *session_data_t;

#endif
