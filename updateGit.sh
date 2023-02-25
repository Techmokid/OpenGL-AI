#!/bin/sh

gnome-terminal -- sh -c '
#!/bin/sh
git add *
git commit -m "Automated Git Push"
git branch -M main
git push -u origin main

read -t 5 -p "Press [Enter] key to finish"
'
