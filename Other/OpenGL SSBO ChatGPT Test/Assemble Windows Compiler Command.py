start = "echo off\ncls\n"

f = open("G++ Command.txt",'r')
middle = f.read()
f.close()

middle = middle.replace("\n"," ")
middle = middle.replace("  "," ").replace("  "," ").replace("  "," ").replace("  "," ").replace("  "," ").replace("  "," ").replace("  "," ").replace("  "," ").replace("  "," ")

end = "echo Finished Compiling. If errors were found, hitting enter will run the last known working version\npause\n\ntest.exe\n\npause"

f = open("CompileAndRun_Windows.bat",'w+')
f.write(start + "\n" + middle + "\n" + end)
f.close()
