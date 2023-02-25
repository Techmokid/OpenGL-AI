import os

for f in os.listdir():
    if os.path.isfile(f) and f.endswith('.cpp'):
        os.system("g++ \"" + f + "\" -lglut -lGLU -lGL -o \"" + f + ".out\"")
