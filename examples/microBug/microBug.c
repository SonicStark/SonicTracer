#include <stdio.h>
#include <time.h>

void use_sprintf(char* pBuf)
{
    sprintf(pBuf, "abcdefghijklmnopqrstuvwxyz");
}

void use_strftime(char* pBuf)
{
    time_t rawtime;
    time(&rawtime);
    struct tm *info;
    info = localtime(&rawtime); 
    strftime(pBuf, 64, "%Y%m%d%H%M%S0123456789abcdefg", info);
}

void use_assignment(char* pBuf)
{
    for (int __i=0; __i<=256; __i++)
        { pBuf[__i] = 0x88; }
}

void use_all(char* pBuf)
{
    use_sprintf(pBuf);
    use_strftime(pBuf);
    use_assignment(pBuf);
}

int main()
{
    char overflow_buf[64];
    use_all(overflow_buf);
    return 0;
}