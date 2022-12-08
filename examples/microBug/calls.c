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
    for (int i__=0; i__<=wlen; i__++)
        { pBuf[i__] = 0x88; }
}