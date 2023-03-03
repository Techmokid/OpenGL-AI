echo off
cls
g++ *.cpp *.h -lpthread -lglut -lGLU -lGL -std=c++17 -o AI_Server_Win64-.out
echo Finished Compiling. If errors were found, hitting enter will run the last known working version
pause

AI_Server_Win64-.out