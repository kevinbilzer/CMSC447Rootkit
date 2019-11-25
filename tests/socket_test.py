import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('', 1337))

print("entering while")
print("recving")
print(s.recv(512))
print("sending data")
s.sendall(b'\xdc')
print(s.recv(512))
