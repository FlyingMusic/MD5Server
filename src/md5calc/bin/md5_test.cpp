#include <stdio.h>
#include <md5.h>

int main()
{
    const char *filename = "/home/chenyifei/tmp/G.png";
    char md5_result[64] = {0};
    if(0 == get_md5_code_for_file(filename, md5_result)) {
        printf("md5 value[%s]\n", md5_result);
    }
    return 0;
}
