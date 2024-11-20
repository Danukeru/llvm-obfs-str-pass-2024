# LLVM STRING OBFUSCATION LIKE IT'S 2024  

Tested on llvm 18 through 20.

## Some Notes

- Tested with the repo builds from https://apt.llvm.org/
- `/usr/bin/clang++-18 -shared -O0 -o ObfString.so StrObfuscate.cpp -fPIC` command in `run.sh`
- `/usr/bin/clang-18 -O1 -fpass-plugin=../ObfString.so main.c -o test` command under `demo` dir's `run.sh`
- Default build uses a `decodeStub` that's implemented inline with llvm IRBuilder calls etc.
- Pass `-D OBFS_XOR_EXTERN` is you want to build the module version that uses the external `decodeStub.ll` 
- `clang-18 -O1 -emit-llvm decodeStub.c -c -o decodeStub.bc && llvm-dis-18 decodeStub.bc` command used to generate `decodeStub.ll` from `decodeStub.c`
