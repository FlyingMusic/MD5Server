#ifndef _INTER_H_
#define _INTER_H_

#include <map>
#include "session.h"
#include "concurrent_queue.h"

typedef std::map<int, session_data_t> fd_session_map_t;
typedef std::map<int, session_data_t>::iterator fd_session_iter_t;

typedef struct md5_inter_st {
    concurrent_queue<int> request_queue;
    concurrent_queue<int> feedback_queue;
    fd_session_map_t fd_session_map;
} *md5_inter_t;

#endif


