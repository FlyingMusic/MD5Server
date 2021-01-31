#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_

#ifdef _DEBUG_CONCURRENT_QUEUE_
#include <stdio.h>
#endif

#include <set>
#include <queue>
#include <mutex>
#include <algorithm>
#include <condition_variable>

template<typename Type>
class concurrent_queue
{
    private:
        std::queue<Type> m_queue;
        std::set<Type> m_set;
        mutable std::mutex m_mutex;
        std::condition_variable m_cond;
    public:
        void push(Type const& item)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if(m_set.find(item) != m_set.end()) {
#ifdef  _DEBUG_CONCURRENT_QUEUE_
            printf("concurrent queue: duplicate data[%d]\n", item);
#endif
                return; // already exist
            }
            m_queue.push(item);
            m_set.insert(item);
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
            if(m_queue.empty()) {
                return false;
            }

            value = m_queue.front();
            m_queue.pop();
            m_set.erase(value);
            return true;
        }

        Type wait_and_pop()
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            while(m_queue.empty()) {
                m_cond.wait(lock);
            }

            Type item = m_queue.front();
            m_queue.pop();
            m_set.erase(item);
#ifdef  _DEBUG_CONCURRENT_QUEUE_
            printf("concurrent queue: pop data[%d]\n", item);
#endif
            return item;
        }

};

#endif
