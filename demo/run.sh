#!/usr/bin/env bash

/usr/bin/clang-18 -O1 -fpass-plugin=../ObfString.so main.c -o test
