import socket

HOST = "192.168.1.12"  # The server's hostname or IP address
PORT = 3490  # The port used by the server

def sendMsg(s,msg):
    s.sendall((msg + "\n").encode('utf-8'))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    sendMsg(s,"Generate New Network: genomes=1, outputs=2, inputs=5, hiddenLayers=3,nodesPerLayer=5")
    while(True):
        print(s.recv(1024).decode("utf-8"))

