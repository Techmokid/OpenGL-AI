#!/bin/sh

gnome-terminal -- sh -c '
#!/bin/sh
echo "Getting Update Info"
sudo apt update

echo "\n\nUpgrading System"
sudo apt upgrade -y

echo "\n\nUpgrading Distributions"
sudo apt dist-upgrade -y

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
'
