from socket import *
import sys

# Defining which port is needed to access the server
serverPort = 6768
# Preparing a server socket
serverSocket = socket(AF_INET, SOCK_STREAM)
# Binding the socket to listen to all IP's trying to connect on the designated port
serverSocket.bind(('', serverPort))
serverSocket.listen(1)  # Server will only handle 1 connection at a time
while True:
    # Establish the connection
    print("Ready to serve..")
    connectionSocket, addr = serverSocket.accept()
    try:
        # Next few lines of code are decoding the incoming bits, turning them back to a string
        # and then parsing said string as a file name, if the file exists on the server a HTTP
        # header will be sent back to user signaling everything is ok and then the requested file
        # will be encoded and sent over to the client.
        message = connectionSocket.recv(4096).decode()
        filename = message.split()[1]
        f = open(filename[1:])
        outputdata = f.read()
        # Send one HTTP header line into socket
        connectionSocket.send("HTTP/1.1 200 OK\r\n\r\n".encode())
        # Send the content of the requested file to the client
        for i in range(0, len(outputdata)):
            connectionSocket.send(outputdata[i].encode())
        connectionSocket.send("\r\n".encode())
        connectionSocket.close()
    except IOError:
        # Send response message for file not found
        connectionSocket.send("HTTP/1.1 404 Not found\r\n\r\n".encode())
        connectionSocket.send("<html><head></head><body><h1>404 Not found</h1></body></html>\r\n".encode())
        # Close client socket
        connectionSocket.close()
        break

serverSocket.close()
sys.exit()
