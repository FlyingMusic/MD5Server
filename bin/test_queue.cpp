#include <stdio.h>
#include "ThreadQueue.h"

int main()
{
    ThreadQueue<int> q;
    q.push(5);
    q.push(4);
    q.push(3);
    q.push(2);
    q.push(1);
    printf("%d\n", q.pop());
    printf("%d\n", q.pop());
    printf("%d\n", q.pop());
    printf("%d\n", q.pop());
    return 0;
}
