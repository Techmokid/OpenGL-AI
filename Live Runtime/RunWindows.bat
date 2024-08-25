@echo off
echo Starting compilation...

g++ *.cpp -lopengl32 -lpthread -lfreeglut -lglfw3 -lglu32 -lfreeGLUT -lglew32 -lws2_32 -lgdi32 -std=c++17 -fconcepts -fpermissive -o AI_Server_Win64.exe

echo Finished Compiling. If compilation errors were found, hitting enter will run the last known working version
set /p ans=Press enter to run program

AI_Server_Win64.exe
set /p ans=Press enter to close terminal