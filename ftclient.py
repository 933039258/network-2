###################################################################
# Author: Peng Zhang
# CS 372 Fall 2019
# Due date: 12/1/2019
# client program to transfer file and read the directory
# Reference: 
# https://docs.python.org/3.5/library/struct.html#classes
# https://docs.python.org/3/library/functions.html#func-bytes
# https://docs.python.org/3/library/socket.html
# https://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data
###################################################################
import sys
import socket
from os import path
from struct import *
import time
###########################################################################
# This funciton is use to connect the server with host name and port
# The variable is host name/address and port number
# reference: https://docs.python.org/3/library/socket.html
###########################################################################
def connectSocket(host_name, port):
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.connect((host_name, port))
    return connection

# Main function
if __name__ == '__main__':
    #initialize the variables
    host_name=""
    server_port=0
    data_port=0
    file = ""
    #check if the argument is legal
    if (len(sys.argv) != 5 and len(sys.argv) != 6):
        error("To run: python3 ftclient.py <host name> <server port> <command> <file name if command is -g> <data port>");
    #declare the variables
    if (len(sys.argv) is 5 and sys.argv[3] == '-l'):
        data_port = int(sys.argv[4])

    elif (len(sys.argv) is 6 and sys.argv[3] == '-g'):
        data_port = int(sys.argv[5])
        file = sys.argv[4]
    else:
        error("To run: python3 ftclient.py <host name> <server port> <command> <file name if command is -g> <data port>");
    host_name = sys.argv[1]
    server_port = int(sys.argv[2])
    command = sys.argv[3]
    # Connect to server
    con = connectSocket(host_name, server_port)

    ###########################################################################
    # connect to server send message and port
    # The pack() unpack() are python struct library to format the data for using of send()
    # the code of pack() unpack() cited from https://docs.python.org/3.5/library/struct.html#classes
    # the code of bytes(), str() are cited from built in function to format the message that sent to server
    # bytes() are reference from: https://docs.python.org/3/library/functions.html#func-bytes
    ############################################################################
    msg = bytes(command + "\0", encoding="UTF-8") #format command into byte[flag]
    nm = pack('i', data_port)#pack data_port and format in integer
    #send command and client port
    con.sendall(msg)
    con.send(nm)

    #send and reveive data
    #command is -l, that means list directory
    if command == "-l":
        time.sleep(0.1)
        readD1 = connectSocket(host_name, data_port)
        print("Receiving directory structure from {}: {}".format(host_name, data_port))
        sz = unpack("i", readD1.recv(4))
        dt = str(readD1.recv(sz[0]), encoding="UTF-8").split("\x00")
        for x in dt:
            print(x)
        readD1.close()
   
    if command == "-g":
        l = len(file)
        senL = pack('i', l)#pack the length, format as integer
        senM = bytes(file + "\0", encoding="UTF-8") #format the file name
        #send the data, cited from https://docs.python.org/3/library/socket.html
        con.send(senL);
        con.sendall(senM)
        time.sleep(0.1)
        readD = connectSocket(host_name, data_port)     
        read_size = unpack("i", readD.recv(4))
        #receive file from server
        #the receive file code are cited from https://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data
        read_data = ""
        while len(read_data) < read_size[0]:
            Gp= str(readD.recv(read_size[0] - len(read_data)), encoding="UTF-8")
            if not Gp:
                break
            read_data+=Gp

        #copy the file from server, create a newfile and wirte the received data into it
        new_file = file.split('.', 1)[0] + "_copy." + file.split('.', 1)[1]
        newf = open(new_file, "w")
        newf.write(read_data)
        newf.close
        print("File transfer complete!")
        readD.close()
    con.close()

