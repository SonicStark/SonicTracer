#include "calls.h"
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>

void hit_HeapBufferOverflow(char* pBuf, int wlen)
{
    for (int i__=0; i__<=wlen; i__++)
    {
        call_sprintf(pBuf);
        call_strftime(pBuf);
        call_write(pBuf, i__);
    }
}

void hit_HeapUseAfterFree(char* pBuf, int wlen)
{
    delete[] pBuf;
    for (int i__=0; i__<=wlen; i__++)
    {
        call_sprintf(pBuf);
        call_strftime(pBuf);
        call_write(pBuf, i__);
    }
}

/* https://github.com/google/sanitizers/wiki/AddressSanitizerExampleUseAfterFree */
int wiki_UseAfterFree(){
    int *array = new int[100];
    delete [] array;
    return array[1];  // BOOM
}

/* https://github.com/google/sanitizers/wiki/AddressSanitizerExampleHeapOutOfBounds */
int wiki_HeapOutOfBounds(){
    int *array = new int[100];
    array[0] = 0;
    int res = array[1 + 100];  // BOOM
    delete [] array;
    return res;
}

/* https://github.com/google/sanitizers/wiki/AddressSanitizerExampleStackOutOfBounds */
int wiki_StackOutOfBounds(){
    int stack_array[100];
    stack_array[1] = 0;
    return stack_array[1 + 100];  // BOOM
}

/* https://github.com/google/sanitizers/wiki/AddressSanitizerExampleGlobalOutOfBounds */
volatile int global_array[100] = {-1};
int wiki_GlobalOutOfBounds(){
    return global_array[1 + 100];  // BOOM
}

/* https://github.com/google/sanitizers/wiki/AddressSanitizerExampleUseAfterReturn */
volatile int *ptr;
__attribute__((noinline)) void FunctionThatEscapesLocalObject() {
    int local[100];
    ptr = &local[0];
}
int wiki_UseAfterReturn() {
    FunctionThatEscapesLocalObject();
    return ptr[1];
}

/* https://github.com/google/sanitizers/wiki/AddressSanitizerExampleUseAfterScope */
volatile int *p = 0;
int wiki_UseAfterScope() {
    {
        int x = 0;
        p = &x;
    }
    *p = 5;
    return 0;
}

/* https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer */
void *pp;
int wiki_LeakSanitizer() {
    pp = malloc(7);
    pp = 0; // The memory is leaked here.
    return 0;
}

int hit_wiki(int id)
{
    using namespace std;
    cout << " => enter a secret place";
    cout.flush();
    int iret;
    if      (id >= 11200 && id <= 11800) { iret = wiki_UseAfterFree     (); }
    else if (id >= 22200 && id <= 22800) { iret = wiki_HeapOutOfBounds  (); }
    else if (id >= 33200 && id <= 33800) { iret = wiki_StackOutOfBounds (); }
    else if (id >= 55200 && id <= 55800) { iret = wiki_GlobalOutOfBounds(); }
    else if (id >= 66200 && id <= 66800) { iret = wiki_UseAfterReturn   (); }
    else if (id >= 77200 && id <= 77800) { iret = wiki_UseAfterScope    (); }
    else if (id >= 88200 && id <= 88800) { iret = wiki_LeakSanitizer    (); }
    else { iret = id; }
    cout << " => leave this secret place with iret=" << iret;
    cout.flush();
    return iret;
}

void using_wLength(int wlen)
{
    char* pBadBuffer = new char[128];

    if      (wlen >= 66  && wlen <= 88 ) { hit_HeapUseAfterFree   (pBadBuffer, wlen); }
    else if (wlen >= 99  && wlen <= 666) { hit_HeapBufferOverflow (pBadBuffer, wlen); delete[] pBadBuffer; }
    else if (wlen >= 10000 && wlen <= 90000) { hit_wiki(wlen); delete[] pBadBuffer; }
    else { call_sprintf(pBadBuffer); delete[] pBadBuffer; }
}


#ifndef USE_LLVM_LIBFUZZER_STYLE
/////////////////////// Normal entry ///////////////////////
void override_wLength(int* pwlen) {
    using namespace std;
    cout << " => try to override wLength=" << *pwlen << " ";
    if (*pwlen == 0) {
        string wlen_;  cin >> wlen_;
        *pwlen = atoi(wlen_.c_str());
        cout << " => now wLength=" << *pwlen;
    } else { return; }
}

int main(int argc, char* argv[])
{
    std::cout << " => main"; std::cout.flush();

    int wLength;
    if (argc != 2) { puts(" => argc exit"); return 0; }
    else { wLength = atoi(argv[1]); }
    override_wLength(&wLength);
    using_wLength(wLength);

    std::cout << " => normal exit" << std::endl;
    return 0;
}
////////////////////////////////////////////////////////////
#else
////////////////// LLVMFuzzerTestOneInput //////////////////
void override_wLength(int* pwlen, const uint8_t *Data, size_t Size) {
    char* pwlen_s = (char*) calloc(1+Size, sizeof(char));
    memcpy(pwlen_s, Data, Size);
    *pwlen = atoi(pwlen_s);
    free(pwlen_s);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    std::cout << " => LLVMFuzzerTestOneInput"; std::cout.flush();

    int wLength;
    override_wLength(&wLength, Data, Size);
    using_wLength(wLength);

    std::cout << " => TestOneInput exit" << std::endl;
    return 0;
}
////////////////////////////////////////////////////////////
#endif