import socket,time

HOST = "localhost"#"192.168.1.12"  # The server's hostname or IP address
PORT = 3490  # The port used by the server

def sendMsg(s,msg):
    s.sendall((msg + "\n").encode('utf-8'))

once = True
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    while(True):
        try:
            s.connect((HOST, PORT))
            print("Connected to server")
            break
        except:
            if (once):
                once = False
                print("Waiting for server to go online....")
    sendMsg(s,"Generate New Network: genomes=1, outputs=2, inputs=5, hiddenLayers=3,nodesPerLayer=5")
    while(True):
        print(s.recv(1024).decode("utf-8"))

