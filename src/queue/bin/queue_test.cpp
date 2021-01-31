#include "concurrent_queue.h"

int main()
{
    concurrent_queue<int> cq;
    for(int i = 0; i < 5; ++i) {
        cq.push(i);
        cq.push(i);
    }
    for(int i = 0; i < 5; ++i) {
        cq.push(i);
        cq.push(i);
    }
    while(!cq.empty()) {
        cq.wait_and_pop();
    }
    for(int i = 0; i < 5; ++i) {
        cq.push(i);
        cq.push(i);
    }
    while(!cq.empty()) {
        cq.wait_and_pop();
    }
    return 0;
}
