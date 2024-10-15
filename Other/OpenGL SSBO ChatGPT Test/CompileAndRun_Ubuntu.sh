#!/bin/bash

gnome-terminal -- sh -c '
#!/bin/bash

g++ *.cpp -lpthread -lglut -lGLU -lGL -lGLEW -lglfw -std=c++17 -fconcepts -o test.out

echo Finished Compiling. If errors were found, hitting enter will run the last known working version

printf "%s " "Press enter to run program"
read ans

clear

./test.out

printf "%s " "Press enter to close terminal"
read ans
'
