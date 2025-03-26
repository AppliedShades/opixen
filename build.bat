@echo off
g++ -municode -std=c++20 -Iinclude -Isrc/headers -Llib -g0 src/source.cpp lib/glad.c -lglfw3 -o release/HelloWorld -lgdi32
cd release
start HelloWorld