#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_

#ifdef _DEBUG_CONCURRENT_QUEUE_
#include <stdio.h>
#endif

#include <queue>
#include <mutex>
#include <algorithm>
#include <condition_variable>

template<typename Type>
class concurrent_queue
{
    private:
        std::queue<Type> m_queue;
        mutable std::mutex m_mutex;
        std::condition_variable m_cond;
    public:
        void push(Type const& item)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.push(item);
#ifdef  _DEBUG_CONCURRENT_QUEUE_
            printf("concurrent queue: push data[%d]\n", item);
#endif
            m_cond.notify_one();
        }

        bool empty() const
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            return m_queue.empty();
        }

        bool try_pop(Type& value)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if(m_queue.empty())
            {
                return false;
            }

            value = m_queue.front();
            m_queue.pop();
            return true;
        }

        Type wait_and_pop()
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            while(m_queue.empty())
            {
                m_cond.wait(lock);
            }

            Type item = m_queue.front();
            m_queue.pop();
#ifdef  _DEBUG_CONCURRENT_QUEUE_
            printf("concurrent queue: pop data[%d]\n", item);
#endif
            return item;
        }

};

#endif
