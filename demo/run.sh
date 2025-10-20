#!/usr/bin/env bash

/usr/bin/clang-20 -fpass-plugin=../ObfString.so -O0 main.c -o test
