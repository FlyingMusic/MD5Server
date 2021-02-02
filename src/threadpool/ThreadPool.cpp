#include <unistd.h>
#include <stdio.h>
#include "ThreadPool.h"

int ThreadPool::start()
{
    m_pids = new pthread_t[m_thread_num];   
    if(m_pids == NULL) {
        return -1;
    }
    for(int i = 0; i < m_thread_num; ++i) {
        if(pthread_create(&m_pids[i], NULL, (void* (*)(void*))m_thread_callback, m_user_data) != 0){
            printf("create pthread fail!\n");
            return -1;
        }
        printf("create pthread[%lu] success!\n", m_pids[i]);
        usleep(200*1000);
    }
    return 0;
}

void ThreadPool::wait()
{
    for(int i = 0; i < m_thread_num; ++i) {
        pthread_join(m_pids[i], NULL);
    }
}

ThreadPool::~ThreadPool()
{
    if(m_pids != NULL) {
        delete[] m_pids;
        m_pids = NULL;
    }
}

