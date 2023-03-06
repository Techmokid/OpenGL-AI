#!/bin/sh

gnome-terminal -- sh -c '
#!/bin/sh
echo "Getting Update Info"
sudo apt update

echo "\n\nUpgrading System"
sudo apt upgrade -y

echo "\n\nUpgrading Distributions"
sudo apt dist-upgrade -y

echo "\n\nCleaning Up..."
sudo apt autoremove -y

echo "\n\nCompleted OS Updates!"
echo "\nRebooting now..."

reboot

'
