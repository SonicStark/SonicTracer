/**
 * This file shows how to make a fuzz driver manually. Temporarily we put
 * it here because we can't find a more suitable one to place.  ╮(╯▽╰)╭
 * 
 * Reference:
 * https://github.com/google/honggfuzz
 * 
 * google/honggfuzz is licensed under the Apache License 2.0
 */

#include <errno.h> //errno; EINTR;
#include <fcntl.h> //open; O_RDONLY;
#include <stdint.h> //uint8_t;
#include <stdio.h> //printf; puts;
#include <stdlib.h> //malloc;
#include <string.h> //memset;
#include <sys/types.h> //ssize_t;
#include <unistd.h> //STDIN_FILENO; read;

/* Maximum size of the input file in bytes (1 MiB) */
#define DRIVER_INPUT_MAX_SIZE (1024ULL * 1024ULL)

/* DRIVER_TEMP_FAILURE_RETRY, but for all OSes */
#ifndef DRIVER_TEMP_FAILURE_RETRY
#define DRIVER_TEMP_FAILURE_RETRY(exp)                                   \
    ({                                                                   \
        __typeof(exp) DRIVER_TEMP_FAILURE_RETRY_RC_;                     \
        do {                                                             \
            DRIVER_TEMP_FAILURE_RETRY_RC_ = (exp);                       \
        } while (DRIVER_TEMP_FAILURE_RETRY_RC_ == -1 && errno == EINTR); \
        DRIVER_TEMP_FAILURE_RETRY_RC_;                                   \
    })
#endif /* ifndef DRIVER_TEMP_FAILURE_RETRY */

void* DRIVER_calloc(size_t sz) {
    void* p = malloc(sz);
    if (p == NULL) {
        printf("[X] Failed malloc(size='%zu')\n", sz);
    }
    memset(p, '\0', sz);
    return p;
}

ssize_t DRIVER_readFd(int fd, uint8_t* buf, size_t fileSz) {
    size_t readSz = 0;
    while (readSz < fileSz) {
        ssize_t sz = DRIVER_TEMP_FAILURE_RETRY(read(fd, &buf[readSz], fileSz - readSz));
        if (sz == 0) {
            break;
        }
        if (sz < 0) {
            return -1;
        }
        readSz += sz;
    }
    return (ssize_t)readSz;
}

static void DRIVER_RunOneInput(const uint8_t* pBuf, size_t vLen);

static int  DRIVER_RunFromFile(int argc, char* argv[]) {
    int         in_fd = STDIN_FILENO;
    const char* fname = "[STDIN]";

    if (argc > 1) { //
        fname = argv[argc - 1];
        if ((in_fd = DRIVER_TEMP_FAILURE_RETRY(open(argv[argc - 1], O_RDONLY))) == -1) {
            printf("[!] Cannot open '%s' as input, using stdin\n", argv[argc - 1]);
            in_fd = STDIN_FILENO;
            fname = "[STDIN]";
        }
    }

    printf("[+] Accepting input from '%s'\n", fname);

    uint8_t* buf = (uint8_t*)DRIVER_calloc(DRIVER_INPUT_MAX_SIZE);
    ssize_t  len = DRIVER_readFd(in_fd, buf, DRIVER_INPUT_MAX_SIZE);
    if (len < 0) {
        printf("[X] Couldn't read data from stdin: %s\n", strerror(errno));
        free(buf);
        return -1;
    }

    DRIVER_RunOneInput(buf, len);
    free(buf);

    puts("[+] Exit DRIVER_RunFromFile");
    return 0;
}

#ifdef DRIVER_LINK_LLVM_LIBFUZZER_STYLE
__attribute__((weak)) int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    printf("[X] Define 'int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)' "
           "in your code to make it work. ");
    // Unlike in libFuzzer, the returned values are useless 
    // here except those other than 0 will be printed to screen. 
    // See https://llvm.org/docs/LibFuzzer.html#rejecting-unwanted-inputs
    return 0;
}

__attribute__((weak)) int LLVMFuzzerInitialize(int *argc, char ***argv) {
    // The returned values are useless but it is suggested that keep it 0
    // just like the usage in libFuzzer.
    // See https://llvm.org/docs/LibFuzzer.html#startup-initialization
    return 0;
}
#endif

/**
 * #### IF (argc > 1)
 *   Firstly try to use the last parameter as
 *   the path of the input file. If fails to open, 
 *   it will read from stdin as a fallback method.
 * #### ELSE
 *   Read from stdin.
 */
int main(int argc, char* argv[]) {
    #ifdef DRIVER_LINK_LLVM_LIBFUZZER_STYLE
    LLVMFuzzerInitialize(&argc, &argv);
    #endif
    return DRIVER_RunFromFile(argc, argv);
}

#ifdef DRIVER_LINK_LLVM_LIBFUZZER_STYLE
static void DRIVER_RunOneInput(const uint8_t* pBuf, size_t vLen) {
    int ret = LLVMFuzzerTestOneInput(pBuf, vLen);
    if (ret != 0) {
        printf("[!] LLVMFuzzerTestOneInput returned '%d' instead of '0'", ret);
    }
}
#else
/**
 * Your fuzzing code definitions
 * 
 * @param pBuf points to the content of input
 * @param vLen size of input
 */
static void DRIVER_RunOneInput(const uint8_t* pBuf, size_t vLen) {
    ;
}
#endif