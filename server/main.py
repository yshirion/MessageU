import socket
from dbs import DataBase
from request import *
import threading
import time

def readport(name):
    try:
        with open(name, 'r') as file:
            port = int(file.read())
            #check range of port
            if port < 0 or port > 65535:
                print("The port number is invalid.")
                exit()
    except:
        print("There is problem with thr port file.")
        exit()
    return port

    

def connect_by_socket(port, db):        
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        print(port)
        sock.bind(('', port))
        sock.listen()
        i=1
        while True: #connecting with threads to provide multi clients
            
            conn, address = sock.accept()
            print("connect number: ",i)
            i +=1
            print("client with address", address, "connect.")            
            thread = threading.Thread(target=thread_request, args=[conn,db])
            thread.start()

def thread_request(conn, db):
    try:
        service = Request(conn, db)
        time.sleep(2)
        service.run()
    except:
        print("There is problem with the request or create data tables.")
    finally:
        conn.close()
           


def main():
    port_file = "myport.info"
    db_file = "server.db"

    port = readport(port_file)

    #Create database class with the given file, to handling all database requests
    try:
        connect_by_socket(port,db_file)
    except:
        print("problem in create server!!!")
    
    
if __name__ == '__main__':
     main()
