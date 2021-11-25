from socket import *
import sys

# We receive user input through the command line as CommandLineClient.py server-host server-port filename
serverHost = sys.argv[1]
serverPort = int(sys.argv[2])
filename = "." + sys.argv[3]  # We add the . here because the server uses a split function to parse incoming requests

# Next step is to parse the arguments and create a TCP socket, we then attempt to connect to the designated server
try:
    clientSocket = socket(AF_INET, SOCK_STREAM)
    print("Socket Created")
    clientSocket.connect((serverHost, serverPort))
    print("Connected to: " + serverHost + " on port: " + str(serverPort))
    # If the connection is successful, send a HTTP GET header line
    clientSocket.send(("/ " + filename).encode())
    response = (clientSocket.recv(2048)).decode()
    print("Server Response:\n", response)
except IOError:
    print("An error has occurred, please try again\r\n Closing client connection")
    sys.exit(1)

clientSocket.close()
