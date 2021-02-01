#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#include <pthread.h>

typedef void (*ThreadPoolCallback)(void*);

class ThreadPool {
    public:
        ThreadPool(int thread_num, ThreadPoolCallback thread_callback, void *user_data) : m_pids(NULL),
            m_thread_num(thread_num), m_thread_callback(thread_callback), m_user_data(user_data) {}
        int start();
        void wait();
        ~ThreadPool();
    private:
        pthread_t *m_pids;
        int m_thread_num;
        ThreadPoolCallback m_thread_callback;
        void *m_user_data;
};

#endif
