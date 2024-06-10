#!/bin/bash
# NOTE: to get all macros set by the compiler: gcc -dM -E file.c
#
# OS+compilers combinations tested here:
#   linux   + gcc(++)
#   linux   + clang(++)
#   linux   + mingw(++)
#   windows + msvc(++)
#   windows + clang-cl.exe(++)
#   windows + clang(++)

set -e
mkdir -p bin
rm -f *.obj

echo "Testing clang++, c++11:"
rm -f bin/test_clangxx 2>/dev/null
clang++ -g -I .. -I ./ -Wno-deprecated -std=c++11 test.c -o bin/test_clangxx && ./bin/test_clangxx

echo ""
echo "Testing g++, c++20:"
rm -f test_gxx 2>/dev/null
g++ -g -I .. -I ./ test.c -std=c++20 -o bin/test_gxx && ./bin/test_gxx

echo ""
echo "Testing clang, c11:"
rm -f test_clang 2>/dev/null
clang -g -I .. -I ./ -std=c11 test.c -o bin/test_clang && ./bin/test_clang

echo ""
echo "Testing gcc, c99, 32bit:"
rm -f test_gcc 2>/dev/null
gcc -g -I .. -I ./ -m32 -DBUILD_DEBUG -std=c99 -O2 test.c -o bin/test_gcc && ./bin/test_gcc

echo ""
echo "Testing mingw-g++:"
rm -f test_mingwxx.exe 2>/dev/null
x86_64-w64-mingw32-g++ -g -I .. -I ./ test.c -o bin/test_mingwxx && WINEDEBUG=-all wine ./bin/test_mingwxx.exe

echo ""
echo "Testing mingw-gcc:"
rm -f test_mingw.exe 2>/dev/null
x86_64-w64-mingw32-gcc -g -I .. -I ./ test.c -o bin/test_mingw && WINEDEBUG=-all wine ./bin/test_mingw.exe

echo ""
echo "Testing MSVC, C++14:"
rm -f test_msvcxx.exe 2>/dev/null
# apparently cl.exe (and clang-cl.exe) sets itself to C/C++ mode depending on
# the extension of the files, which is why we use symlinks here
cl.exe /I .. -I ./ /std:c++14 test.cpp /link /OUT:bin/test_msvcxx.exe /SUBSYSTEM:CONSOLE && WINEDEBUG=-all wine ./bin/test_msvcxx.exe
# switches to create debuggable .exe with separate pdb file
#cl.exe /I .. /Zi /std:c++14 test.cpp /link /DEBUG:FULL /OUT:bin/test_msvcxx.exe /PDB:bin/test_msvcxx.pdb /SUBSYSTEM:CONSOLE

echo ""
echo "Testing MSVC, C17:"
rm -f test_msvc.exe 2>/dev/null
cl.exe /I .. /I ./ /std:c17 test.c /link /OUT:bin/test_msvc.exe /SUBSYSTEM:CONSOLE && WINEDEBUG=-all wine ./bin/test_msvc.exe

echo ""
echo "Testing clang-cl.exe, C++11:"
rm -f test_clang-clxx.exe 2>/dev/null
clang-cl.exe /clang:--std=c++11 /I .. /I ./ test.cpp /link /OUT:bin/test_clang-clxx.exe && WINEDEBUG=-all wine ./bin/test_clang-clxx.exe
echo ""
echo "Testing clang-cl.exe, C17:"
rm -f test_clang-cl.exe 2>/dev/null
clang-cl.exe /clang:--std=c17 /I .. /I ./ test.c /link /OUT:bin/test_clang-cl.exe && WINEDEBUG=-all wine ./bin/test_clang-cl.exe

echo ""
echo "Testing clang on windows, C99:"
rm -f test_clang.exe 2>/dev/null
clang.exe --std=c99 -I .. -I ./ test.c -o bin/test_clang.exe && WINEDEBUG=-all wine ./bin/test_clang.exe

echo ""
echo "Testing clang++ on windows, C++14:"
rm -f test_clangxx.exe 2>/dev/null
clang++.exe --std=c++14 -I .. -I ./ test.cpp -o bin/test_clangxx.exe && WINEDEBUG=-all wine ./bin/test_clangxx.exe

rm -f *.obj
rm -f *.pdb
