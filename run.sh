#!/usr/bin/env bash
if [[ -z "${APP_ENV}" ]]; then APP_LOSP_ENV=UNSET; fi
APP_LOSP_ENV=$(echo "${APP_ENV}" | sed 's/[[:space:]&]//g')

clang-20 -O1 -emit-llvm decodeStub.c -c -o decodeStub.bc && llvm-dis-20 decodeStub.bc

/usr/bin/clang++-20 -shared -I /usr/include/llvm-20/ -I /usr/include/llvm-c-20/ -O0 -o ObfString.so StrObfuscate.cpp -fPIC -D $APP_LOSP_ENV
