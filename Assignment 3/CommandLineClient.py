from socket import *
import sys

# We receive user input through the command line as CommandLineClient.py server-host server-port filename
serverHost = sys.argv[1]
serverPort = int(sys.argv[2])
filename = sys.argv[3]

# Next step is to parse the arguments and create a TCP socket, we then attempt to connect to the designated server
try:
    clientSocket = socket(AF_INET, SOCK_STREAM)
    clientSocket.connect((serverHost, serverPort))
    # If the connection is successful, send a request
    # the "/" is added because the WebServer splits incoming messages at index 1
    clientSocket.send(("/ "+filename).encode())
    response = clientSocket.recv(4096).decode()

    clientSocket.close()
    print("Server Response:\n", response)
except IOError:
    print("An error has occurred, please try again\r\n Closing client connection")
    sys.exit(1)

# Next block of code builds a response message from the server to display to the user
#response = clientSocket.recv(4096).decode()

#clientSocket.close()
#print("Server Response:\n", response)
