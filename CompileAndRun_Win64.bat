echo off
cls

g++ *.cpp *.h -IF:\OpenGL Libraries\freeglut\include -LF:\OpenGL Libraries\freeglut\lib\x64 -lOpenGL32 -lglu32 -lfreeGLUT -lws2_32 -static-libstdc++ -std=c++17 -o AI_Server_Win64.exe
echo Finished Compiling. If errors were found, hitting enter will run the last known working version
pause

AI_Server_Win64.exe