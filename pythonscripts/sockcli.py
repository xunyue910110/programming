import socket
import time

server = ('192.168.56.101', 20072)
msg = [b'hello', b'welcome', b'xiaoming', b'zhangsan', b'liuliu']

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(server)
for m in msg:
    sock.send(m)
    data = sock.recv(1024)
    print('echo', data)
    time.sleep(2)
sock.close()
