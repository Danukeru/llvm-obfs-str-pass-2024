// clang -Os -shared -emit-llvm -c decodeStub.c -o decodeStube.bs && llvm-dis < decodeStub.bs

#include <stdint.h>

void decode(char *string, int length) {
    char *p = string;
    while (p && length-- > 0) {
        *(p++) ^= 0xAF;
    }
}
