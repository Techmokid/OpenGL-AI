#!/bin/sh

echo "Updating core libraries"
sudo apt update
sudo apt upgrade -y
sudo apt dist-upgrade -y

echo "\n\nUpgrading OS"
sudo do-release-upgrade -d
sudo apt full-upgrade -y

echo "\n\nCompleted OS Updates!"

echo "\n\nSetting up OpenGL"
sudo apt install build-essential -y
sudo apt-get install freeglut3-dev -y
sudo apt-get install libglfw3 -y
sudo apt-get install libglfw3-dev -y
sudo apt-get install -y libglew-dev

echo "\n\nCleaning Up..."
sudo apt autoremove -y

echo "\nRebooting now..."

reboot

