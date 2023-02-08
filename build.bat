@echo off
md build
pushd build
cl ../src/main.cpp /nologo /Zi /I"../dp" /MD /link shell32.lib user32.lib opengl32.lib gdi32.lib kernel32.lib  ../dp/glfw3.lib
main.exe
popd