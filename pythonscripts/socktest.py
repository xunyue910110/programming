import socket

server = ('192.168.56.101', 20072)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(server)
sock.listen(5)
conn, address = sock.accept()
print('connect by ', address)
while True:
    data = conn.recv(1024)
    if not data :
        break
    print(data)
    conn.send(data)
sock.close()
