#!/bin/bash

gnome-terminal -- sh -c '
#!/bin/bash

g++ *.cpp *.h -lpthread -lglut -lGLU -lGL -std=c++17 -o AI_Server.out

echo Finished Compiling. If errors were found, hitting enter will run the last known working version

printf "%s " "Press enter to run program"
read ans

./AI_Server.out

printf "%s " "Press enter to close terminal"
read ans
'
