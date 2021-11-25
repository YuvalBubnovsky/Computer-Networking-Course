from socket import *
import sys

serverHost = sys.argv[1]
serverPort = int(sys.argv[2])
filename = sys.argv[3]

try:
    clientSocket = socket(AF_INET, SOCK_STREAM)
    clientSocket.connect((serverHost, serverPort))


