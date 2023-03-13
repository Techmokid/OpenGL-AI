echo off
cls

g++ *.cpp -I"F:\OpenGL Libraries\freeglut\include" -L"F:\OpenGL Libraries\freeglut\lib\x64" -lOpenGL32 -lglu32 -lfreeGLUT -I"F:\OpenGL Libraries\glew-2.1.0\include" -L"F:\OpenGL Libraries\glew-2.1.0\lib\Release\x64" -lglew32 -lws2_32 -static-libstdc++ -std=c++17 -o SSBO_Test.exe
echo Finished Compiling. If errors were found, hitting enter will run the last known working version
pause

SSBO_Test.exe

pause