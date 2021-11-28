from socket import *
import threading

stop = threading.Event()
serverPort = 6768


def handler(connection, addr):
    print(f"A New Connection Has Been Established: {addr} is connected\n")
    connected = True
    while connected:
        message = connection.recv(2048).decode()
        if stop.is_set():
            break
    connection.close()


def main():
    serverSocket = socket(AF_INET, SOCK_STREAM)
    serverSocket.bind(('', serverPort))
    serverSocket.listen()

    while True:
        connection, addr = serverSocket.accept()
        thread = threading.Thread(target=handler, args=(connection, addr))
        thread.start()
        print(f"Number Of Threads : {threading.active_count() - 1}\n")


if __name__ == "__main__":
    main_thread = threading.Thread(target=main)
    main_thread.start()
    main_thread.join(timeout=5)
    stop.set()
