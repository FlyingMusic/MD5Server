#include "concurrent_queue.h"

int main()
{
    concurrent_queue<int> cq;
    for(int i = 0; i < 5; ++i) {
        cq.push(i);
    }
    for(int i = 0; i < 5; ++i) {
        printf("%d\n", cq.wait_and_pop());
    }
    return 0;
}
