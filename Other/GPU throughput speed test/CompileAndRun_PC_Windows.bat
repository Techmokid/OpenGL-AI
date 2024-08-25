echo off
cls

g++ *.cpp -I"F:\OpenGL Libraries\freeglut\include" -L"F:\OpenGL Libraries\freeglut\lib\x64" -I"F:\OpenGL Libraries\glew-2.1.0\include" -L"F:\OpenGL Libraries\glew-2.1.0\lib\Release\x64" -I"F:\OpenGL Libraries\glfw-3.3.8.bin.WIN64\include" -L"F:\OpenGL Libraries\glfw-3.3.8.bin.WIN64\lib-mingw-w64" -lopengl32 -lpthread -lfreeglut -lglfw3 -lglu32 -lfreeGLUT -lglew32 -lws2_32 -lgdi32 -static-libstdc++ -std=c++17 -fconcepts -g -o SpeedTest.exe
echo Finished Compiling. If errors were found, hitting enter will run the last known working version
pause

SpeedTest.exe

pause