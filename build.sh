#!/bin/bash
# NOTE: to get all macros set by the compiler: gcc -dM -E file.c

echo "Testing clang++, c++11:"
clang++ -Wno-deprecated -std=c++11 test.c test2.c -o test && ./test

echo ""
echo "Testing g++, c++20:"
g++ test.c test2.c -std=c++20 -o test && ./test

echo ""
echo "Testing clang, c11:"
clang -std=c11 test.c test2.c -o test && ./test

echo ""
echo "Testing gcc, c99, 32bit:"
gcc -m32 -DBUILD_DEBUG -std=c99 -O2 test.c test2.c -o test && ./test

echo ""
echo "Testing mingw-g++:"
x86_64-w64-mingw32-g++ test.c test2.c -o test && wine ./test.exe 2>/dev/null # NOTE: filters out errors

echo ""
echo "Testing mingw-gcc:"
x86_64-w64-mingw32-gcc test.c test2.c -o test && wine ./test.exe 2>/dev/null # NOTE: filters out errors
