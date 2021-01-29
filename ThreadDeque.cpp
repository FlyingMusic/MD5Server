#include <deque>
#include <pthread>
#include <algorithm>

using namespace std;

static deque<int> g_deque;
static pthread_mutex_t deque_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t deque_cond = PTHREAD_COND_INITIALIZER;

void add_tdeque_task(int fd) {
    pthread_mutex_lock(&deque_mutex);

    deque<int>::iterator iter = find(g_deque.begin(), g_deque.end(), fd);
    if(iter == g_deque.end()) { //由于epoll为LT模式，所以这里增加去重，防止线程反复读一个fd
        g_deque.push_back(fd);
    }

    pthread_cond_signal(&deque_cond);
    pthread_mutex_unlock(&deque_mutex);
}

int get_tdeque_task() {
    pthread_mutex_lock(&deque_mutex);
    while(g_deque.size() == 0) {
        pthread_cond_wait(&deque_cond, &deque_mutex);
    }

    int fd = g_deque.get_front();
    g_deque.pop_front();

    pthread_mutex_unlock(&deque_mutex);
    return fd;
}
