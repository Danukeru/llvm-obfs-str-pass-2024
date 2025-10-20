#!/usr/bin/env bash

clang-20 -O1 -emit-llvm decodeStub.c -c -o decodeStub.bc && llvm-dis-20 decodeStub.bc

/usr/bin/clang++-20 -shared -I /usr/include/llvm-20/ -I /usr/include/llvm-c-20/ -O0 -o ObfString.so StrObfuscate.cpp -fPIC
