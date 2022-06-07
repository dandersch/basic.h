#!/bin/bash
# NOTE: to get all macros set by the compiler: gcc -dM -E file.c
#
# OS+compilers tested here:
#   linux+gcc(++)
#   linux+clang(++)
#   linux+mingw(++)
#   windows+msvc(++)
#   windows+clang-cl.exe(++)
#   windows+clang(++)

echo "Testing clang++, c++11:"
rm test_clangxx 2>/dev/null
clang++ -Wno-deprecated -std=c++11 test.c test2.c -o test_clangxx && ./test_clangxx

echo ""
echo "Testing g++, c++20:"
rm test_gxx 2>/dev/null
g++ test.c test2.c -std=c++20 -o test_gxx && ./test_gxx

echo ""
echo "Testing clang, c11:"
rm test_clang 2>/dev/null
clang -std=c11 test.c test2.c -o test_clang && ./test_clang

echo ""
echo "Testing gcc, c99, 32bit:"
rm test_gcc 2>/dev/null
gcc -m32 -DBUILD_DEBUG -std=c99 -O2 test.c test2.c -o test_gcc && ./test_gcc

echo ""
echo "Testing mingw-g++:"
rm test_mingwxx.exe 2>/dev/null
x86_64-w64-mingw32-g++ test.c test2.c -o test_mingwxx && WINEDEBUG=-all wine ./test_mingwxx.exe

echo ""
echo "Testing mingw-gcc:"
rm test_mingw.exe 2>/dev/null
x86_64-w64-mingw32-gcc test.c test2.c -o test_mingw && WINEDEBUG=-all wine ./test_mingw.exe

echo ""
echo "Testing MSVC, C++14:"
rm test_msvcxx.exe 2>/dev/null
# apparently cl.exe (and clang-cl.exe) sets itself to C/C++ mode depending on
# the extension of the files, which is why we use symlinks here
cl.exe /analyze /std:c++14 test.cpp test2.cpp /link /OUT:test_msvcxx.exe /SUBSYSTEM:CONSOLE && WINEDEBUG=-all wine ./test_msvcxx.exe

echo ""
echo "Testing MSVC, C17:"
rm test_msvc.exe 2>/dev/null
cl.exe /analyze /std:c17 test.c test2.c /link /OUT:test_msvc.exe /SUBSYSTEM:CONSOLE && WINEDEBUG=-all wine ./test_msvc.exe

echo ""
echo "Testing clang-cl.exe, C++11:"
rm test_clang-clxx.exe 2>/dev/null
clang-cl.exe /clang:--std=c++11 test.cpp test2.cpp /link /OUT:test_clang-clxx.exe && WINEDEBUG=-all wine ./test_clang-clxx.exe
echo ""
echo "Testing clang-cl.exe, C17:"
rm test_clang-cl.exe 2>/dev/null
clang-cl.exe /clang:--std=c17 test.c test2.c /link /OUT:test_clang-cl.exe && WINEDEBUG=-all wine ./test_clang-cl.exe

echo ""
echo "Testing clang on windows, C99:"
rm test_clang.exe 2>/dev/null
clang.exe --std=c99 test.c test2.c -o test_clang.exe && WINEDEBUG=-all wine ./test_clang.exe

echo ""
echo "Testing clang++ on windows, C++14:"
rm test_clangxx.exe 2>/dev/null
clang++.exe --std=c++14 test.cpp test2.cpp -o test_clangxx.exe && WINEDEBUG=-all wine ./test_clangxx.exe
