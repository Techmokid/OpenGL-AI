#!/bin/bash

rm AI_Server_WSL2.out
g++ *.cpp *.h -lpthread -lglut -lGLEW -lglfw -lGLU -lGL -std=c++17 -o AI_Server_WSL2.out
./AI_Server_WSL2.out

printf "%s " "Press enter to close terminal"
read ans
