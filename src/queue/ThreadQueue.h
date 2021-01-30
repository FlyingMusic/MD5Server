#ifndef _THREAD_QUEUE_H_
#define _THREAD_QUEUE_H_

#include <queue>
#include <mutex>
#include <algorithm>
#include <condition_variable>

static std::mutex g_mutex;
static std::condition_variable g_cond;

template<typename T>
class ThreadQueue {
    public:
        ThreadQueue(isDup = false) : m_isDup(isDup){ }
        void push(T item);
        T pop();
    private:
        std::queue<T> m_queue;
        bool m_isDup;
};

template<typename T>
void ThreadQueue<T>::push(T item)
{
    std::unique_lock<std::mutex> lock(g_mutex);
    queue<T>::iterator iter = find(m_queue.begin(), m_queue.end(), item);
    if(iter == g_deque.end()) {
        m_queue.push(item); 
    }
    g_cond.notify();
}

template<typename T>
T ThreadQueue<T>::pop()
{
    std::unique_lock<std::mutex> lock(g_mutex);
    g_cond.wait( lock, [](){ return m_queue.size() > 0} );
    T item = m_queue.front();
    m_queue.pop();
    return item;
}
#endif
