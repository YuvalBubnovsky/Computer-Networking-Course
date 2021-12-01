from socket import *
from threading import Thread

serverPort = 6768

serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('', serverPort))
serverSocket.listen(4)
print("Ready to server on port: " + str(serverPort))


def handler(connection, addr):
    while True:
        try:
            data = connection.recv(2048).decode()
            if not data:
                break
            filename = data.split()[1]
            f = open(filename[1:])
            data = f.read()
            print("Recieved Message")
            connection.send("HTTP/1.1 200 OK\r\n\r\n".encode())
            for i in range(0, len(data)):
                connection.send(data[i].encode())
            connection.send("\r\n".encode())
            connection.close()
            print("Connection Closed!")
            break
        except IOError:
            # Send response message for file not found
            connection.send("HTTP/1.1 404 Not found\r\n\r\n".encode())
            connection.send("<html><head></head><body><h1>404 Not found</h1></body></html>\r\n".encode())
            # Close client socket
            connection.close()
            break


while True:
    try:
        connection, addr = serverSocket.accept()
        print("Incoming Connection From: " + str(addr))
        t = Thread(target=handler, args=(connection, addr))
        t.start()
    except OSError:
        break

serverSocket.close()
