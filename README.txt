Author: Peng Zhang
The program is to list directory or transfer file
Note: some references are list in the program file.
####################################################################
There are four main file for this program:
ftserver.cpp
ftclient.py
ftserver.h
makefile
*You need put the ftserver.cpp and ftserver.h in a directory and put ftclient.py in another directory
#####################################################################
To run the server:
make
a.out <server port>
#####################################################################
To run the client:
If you want to list the directory:
python3 ftclient.py <host name> <server port> -l <data port>

If you want to transfer the file:
python3 ftclient.py <host name> <server port> -g <file name> <data port>
#######################################################################
Example:
ftserver.cpp:
make
a.out 4578

ftclient.py:
python3 ftclient.py flip1 4578 -l 4579
python3 ftclient.py flip1 4578 -g file.txt 4579
########################################################################
Note: if you entered -l and want to -g to transfer file then, it may has error on python because the data port has been used,
you just need to change a data port and try it again.