import socket, time

def run():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 7777))
    
    # 1. Login
    sock.sendall(b"CONNECT\naccept-version:1.2\nhost:stomp\nlogin:guest\npasscode:1234\n\n\0")
    time.sleep(0.5)
    print("Guest Connected: " + sock.recv(1024).decode())

    # 2. Send Message to the channel bahaa is listening to
    sock.sendall(b"SEND\ndestination:germany_spain\n\nHello from Python!\n\0")
    print("Sent Message from Python")
    
    sock.close()

run()
