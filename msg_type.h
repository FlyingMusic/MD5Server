#ifndef __MSG_TYPE_H__
#define __MSG_TYPE_H__

#include <stdio.h>

enum CMD_TYPE {
    CMD_REQUEST = 1,
    CMD_RESPONCE,
	CMD_DATA,
    CMD_FINISH,
    CMD_RESULT,
    CMD_ERROR,
    CMD_UNKNOWN,
};

typedef struct msg_head_st {
    CMD_TYPE cmd;
    int datalen;
    msg_head_st() : cmd(CMD_UNKNOWN), datalen(0){ }
} msg_head_t;

#endif
