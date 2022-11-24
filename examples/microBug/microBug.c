#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void call_sprintf(char* pBuf)
{
    printf(" => call_sprintf"); fflush(stdout);
    sprintf(pBuf, "abcdefghijklmnopqrstuvwxyz");
}

void call_strftime(char* pBuf)
{
    printf(" => call_strftime"); fflush(stdout);
    time_t rawtime;
    time(&rawtime);
    struct tm *info;
    info = localtime(&rawtime); 
    strftime(pBuf, 64, "%Y%m%d%H%M%S0123456789abcdefg", info);
}

void call_write(char* pBuf, int wlen)
{
    printf(" => call_write_L%d", wlen); fflush(stdout);
    for (int __i=0; __i<=wlen; __i++)
        { pBuf[__i] = 0x88; }
}

void hit_HeapBufferOverflow(char* pBuf, int wlen)
{
    for (int __i=0; __i<=wlen; __i++)
    {
        call_sprintf(pBuf);
        call_strftime(pBuf);
        call_write(pBuf, __i);
    }
}

void hit_HeapUseAfterFree(char* pBuf, int wlen)
{
    free(pBuf);
    for (int __i=0; __i<=wlen; __i++)
    {
        call_sprintf(pBuf);
        call_strftime(pBuf);
        call_write(pBuf, __i);
    }
}

int main(int argc, char* argv[])
{
    printf(" => main");

    int wLength;
    if (argc != 2) { puts(" => argc exit"); return 0; }
    else { wLength = atoi(argv[1]); }

    char* pBadBuffer = (char*) malloc(128*sizeof(char));

    if      (wLength >= 66 && wLength <= 88 ) { hit_HeapUseAfterFree   (pBadBuffer, wLength); }
    else if (wLength >= 99 && wLength <= 666) { hit_HeapBufferOverflow (pBadBuffer, wLength); }
    else { call_sprintf(pBadBuffer); free(pBadBuffer); }
    
    puts(" => normal exit");
    return 0;
}