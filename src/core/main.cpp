#include <stdio.h>
#include "ThreadPool.h"
#include "inter.h"
#include "io_callback.h"
#include "work_callback.h"

int main()
{
    md5_inter_st inter;
    ThreadPool io_thread(1, io_thread_callback, &inter);
    ThreadPool work_thread(10, work_thread_callback, &inter);
    io_thread.wait();
    work_thread.wait();
    return 0;
}
