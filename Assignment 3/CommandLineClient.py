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
    # If the connection is successful, send a HTTP GET header line
    clientSocket.send(("GET /%s HTTP/1.1\r\n\r\n" % filename).encode())
except IOError:
    print("An error has occurred, please try again\n Closing connection")
    sys.exit(1)

# Next block of code builds a response message from the server to display to the user
response = ""
response_message = clientSocket.recv(4096)
# As long as the server sends information we will add it to the response string to print later
while response_message:
    response += response_message
    response_message = clientSocket.recv(4096)

clientSocket.close()
print("Server Response:", response)
