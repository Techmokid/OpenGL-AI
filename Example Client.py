import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 3490  # The port used by the server

def sendMsg(s,msg):
    s.sendall((msg + "\n").encode('utf-8'))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    sendMsg(s,"Data from python 1. Hello!")
    #print(s.recv(1024).decode("utf-8"))
    sendMsg(s,"Data from python 2. Testing?")
    #print(s.recv(1024).decode("utf-8"))
    sendMsg(s,"Generate New Network from scratch")
    sendMsg(s,"Data from python 3. Testing")

