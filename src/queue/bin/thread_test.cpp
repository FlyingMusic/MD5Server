#include <thread>
#include <chrono>
#include <iostream>
#include "concurrent_queue.h"

using namespace std;

static concurrent_queue<int> cq;

void push_fun(int base)
{
    for(int i = 0; i < 9; ++i) {
        cq.push(base*10 + i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
void pop_fun()
{
    while(!cq.empty()) {
        cq.wait_and_pop();        
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
int main()
{
    thread thread_push[5];
    for(int i = 0; i < 5; ++i) {
        thread_push[i] = thread(push_fun, i);
    }
    thread thread_pop(pop_fun);

    for(int i = 0; i < 5; ++i) {
        thread_push[i].join();
    }
    thread_pop.join();
    return 0;
}
