import os,platform

for f in os.listdir():
    if os.path.isfile(f) and f.endswith('.cpp'):
        command = "g++ \"" + f + "\" -IF:/freeglut/include -LF:/freeglut/lib -w -Wl,-subsystem,windows -lOpenGL32 -lglu32 -lfreeGLUT -std=c++17 -m32 -o \"" + f + "_" + platform.system() + "_" + platform.release() + ".exe\""
        os.system(command)
        print(command)
