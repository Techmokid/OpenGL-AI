#!/bin/sh

gnome-terminal -- sh -c '
#!/bin/sh
git add *
git commit -m "Automated Git Push"
git branch -M main
git push -u origin main
'
