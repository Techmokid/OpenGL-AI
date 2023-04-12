import keyboard,time

filepath = "C:/Users/aj200/Documents/GitHub/OpenGL-AI/Ubuntu stuff/UpdateUbuntuOS.sh"

f = open(filepath,'r')
fc = f.readlines()
f.close()

print("Read from file. Proceeding to type entire script in:")
print("5")
time.sleep(1)
print("4")
time.sleep(1)
print("3")
time.sleep(1)
print("2")
time.sleep(1)
print("1")
time.sleep(1)
print("0")
time.sleep(1)

for i in fc:
    for x in i:
        keyboard.write(x)
        time.sleep(0.05)
