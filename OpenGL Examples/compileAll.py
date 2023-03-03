import os,platform

for f in os.listdir():
	if os.path.isfile(f) and f.endswith('.cpp'):
		command = ""
		if (platform.system() == "Windows"):
			command = "g++ \"" + f + "\" -IF:/freeglut/include -LF:/freeglut/lib/x64 -w -Wl,-subsystem,windows -lOpenGL32 -lglu32 -lfreeGLUT -std=c++17 -o \"" + f + "_" + platform.system() + ".exe\""
		else:
			command = "g++ \"" + f + "\" -lglut -lGLU -lGL -lpthread -std=c++17 -o \"" + f + "_" + platform.system() + ".out\""
		os.system(command)
		print(command)
