from socket import socket, AF_INET, SOCK_DGRAM
 
ADDRESS = "192.168.43.246" # M5Stack address
PORT = 5555
 
s = socket(AF_INET, SOCK_DGRAM)
 
while True:
    msg = input("> ")
    s.sendto(msg.encode(), (ADDRESS, PORT))
 
s.close()

