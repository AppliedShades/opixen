@echo off
clang++ -x c++ -stdlib=libc++ -municode -std=c++20 -Iinclude -Isrc/headers -I"C:\Unsafe\installed\msys2\mingw64\include" -Llib -L"C:\Unsafe\installed\msys2\mingw64\lib" -g0 src/source.cpp lib/glad.c -lglfw3 -lfreetype -lbrotlidec -lharfbuzz -lpng -lbz2 -lz -lgdi32 -o release/HelloWorld -lstdc++
cd release
start HelloWorld