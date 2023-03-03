echo off
cls

g++ *.cpp *.h -lpthread -IF:\freeglut\include -LF:\freeglut\lib -w -Wl,-subsystem,windows -lOpenGL32 -lglu32 -lfreeGLUT -std=c++17 -m32 -o AI_Server_Win32.exe
echo Finished Compiling. If errors were found, hitting enter will run the last known working version
pause

AI_Server_Win32.exe