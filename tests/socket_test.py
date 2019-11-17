import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.bind(('', 1337))

s.listen(5)

print("entering while")

while True:
    c, addr = s.accept()
    print('got connection from ' + str(addr))
    print("recving data")
    print(c.recv(13))
    print("sending data")
    c.sendall(b'aa')
    print("closing connection")
    c.close()
