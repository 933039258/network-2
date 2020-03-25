/**********************************************************************************
* Author: Peng Zhang
* CS 372 Fall 2019
* Due date: 12/1/2019
* Server program to transfer file.
* Reference: 
* https://beej.us/guide/bgnet/html/#sendrecv
* http://www.linuxhowtos.org/C_C++/socket.htm
* https://stackoverflow.com/questions/34943835/convert-four-bytes-to-integer-using-c/34944089
************************************************************************************/
#include "ftserver.h"
#include <iostream>
#include <sys/io.h>
#include <unistd.h>
#include <fstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

// this function is transfer byte to int, socket format is in byte to receive data
// cited from https://stackoverflow.com/questions/34943835/convert-four-bytes-to-integer-using-c/34944089
int bytesToInt(byte* bytes, int size = 4)
{
	int addr = bytes[0] & 0xFF;
	addr |= ((bytes[1] << 8) & 0xFF00);
	addr |= ((bytes[2] << 16) & 0xFF0000);
	addr |= ((bytes[3] << 24) & 0xFF000000);
	return addr;
}
//this function is trans int to byte, to send data 
//cited from https://stackoverflow.com/questions/5585532/c-int-to-byte-array
void  intToByte(int i, byte* bytes, int size = 4)
{
	memset(bytes, 0, sizeof(byte) * size);
	bytes[0] = (byte)(0xff & i);
	bytes[1] = (byte)((0xff00 & i) >> 8);
	bytes[2] = (byte)((0xff0000 & i) >> 16);
	bytes[3] = (byte)((0xff000000 & i) >> 24);
}
//initial function
SocKet::SocKet(int tport)
{
	port = tport;
	vFileName.clear();
	if (init(sockSrv,port) == false)
	{
		cout << "Failed Init !" << endl;
		exit(0);
	}
}
//default function
SocKet::~SocKet()
{
    close(sockSrv);
}
//function to initialize the socket, create a connection
//some code cited from https://beej.us/guide/bgnet/html/#sendrecv
bool SocKet::init(SOCKET& sockCon, int port)
{
    sockCon = socket(AF_INET, SOCK_STREAM, 0);	
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(struct sockaddr_in));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int retVal = bind(sockCon, (struct sockaddr*)&sockAddr, sizeof(SOCKADDR_IN));
	if (retVal == -1) {
		cout << "Failed Bind !" << endl;
		return false;
	}
	if (listen(sockCon, MAXLINK) == -1)
	{
		cout << "Failed Listen !" << endl;
		return false;
	}
	return true;
}

//function to get diretory
void SocKet::getDir()
{
	char buf[256];
    DIR *filedir;
    struct dirent *ptr;
	vFileName.clear();
	getcwd(buf, 256);
    if ((filedir=opendir(buf)) == NULL)
    {
        return;
    }
    while ((ptr=readdir(filedir)) != NULL)//read the file name 
    {
        if(ptr->d_type == DT_REG)
        {
            vFileName.push_back(std::string(ptr->d_name)+"\n");//put the directory into stack
        }
    }
    closedir(filedir);
}
//function to get file
int SocKet::getFile(string filename)
{
	//initialize variables
	string line= "";
	int len = 0;
	ifstream fp;
	fp.open(filename.c_str(), ios::in);//open file
	if (fp.is_open() == true)//if the file can be opened
	{
		while (getline(fp, line))//get the text from the file
		{
			line = line + "\n";
			len += line.length();
			file.push_back(line);
		}
	}
	return len;
}
//funtion to reveive data all the time if the server has be opened
void SocKet::Loop()
{
	//initialize the variables
	SOCKADDR_IN clAddr;
	int len = sizeof(sockaddr);
	char buf[MAXBUF];
	char getport[8];
	SOCKET sockConn;
	SOCKET sockData;
	SOCKET sockDrv;
	sockData = 0;
	int data_port;
	byte dsize[5];
	//server opened
	while (true)
	{
		memset(buf, 0, MAXBUF);
		memset(getport, 0, 8);
		memset(dsize, 0, 5);
		sockConn = accept(sockSrv, (sockaddr*)NULL, NULL);//connect socket
		if (sockConn == -1)
		{
			continue;
		}
		int f=recv(sockConn, buf, 3, 0); //receive the command
		f = recv(sockConn, getport, 4, 0); //receive the data port
		data_port = bytesToInt((byte*)getport); //format
		cout << "connected to :" << data_port << endl;
		if (init(sockDrv, data_port) == false)//if socket cannot be initialize, close
		{
			close(sockConn);
			continue;
		}
		else //the socket is connected successful
		{
			int iSend;
			string dir;
			int lendir;
			switch (buf[1])//check the command
			{
			case 'l': // if the command is -l
			{
				sockData = accept(sockDrv, (sockaddr*)NULL, NULL); //connect socket
				if (sockData == -1)//if the socket error, close
				{
					close(sockConn);
					close(sockDrv);
					continue;
				}
				getDir();//list the directory
				dir = "";
				for (size_t i = 0; i < vFileName.size(); i++)//check the file name 
				{
					dir += vFileName[i];
				}
                lendir=dir.length();
				intToByte(lendir, dsize);//format to send data
				iSend = send(sockData, (const char*)dsize, 4, 0);//send
				iSend = send(sockData, dir.c_str(), dir.length(), 0);//send
				if (iSend == -1)
				{
					cout << "failed" << endl;
				}
				break;
			}
			case 'g'://if the command is -g
			{
				f = recv(sockConn, buf, 4, 0);//get the file name
				int lenName = bytesToInt((byte*)buf);
				f = recv(sockConn, buf, lenName, 0);
				int lenfile=getFile(buf);//get the file
				string res = lenfile <= 0 ? "FILE NOT FOUND!" : "FOUND!";//check if the file here
				intToByte(res.size(), dsize);//format to send
				iSend = send(sockConn, (const char*)dsize, 4, 0);
				iSend=send(sockConn, res.c_str(), res.length(), 0);
				if (lenfile <= 0)
				{
					break;
				}

				sockData = accept(sockDrv, (sockaddr*)NULL, NULL);//connect socket
				if (sockData == -1)
				{
					close(sockConn);
					close(sockDrv);
					continue;
				}
				intToByte(lenfile, dsize);
				iSend = send(sockData, (const char*)dsize, 4, 0);
				for (size_t i = 0; i < file.size(); i++)//send file text
				{
					send(sockData, file[i].c_str(), file[i].length(), 0);
				}
				break;
			}
			default:
				break;
			}
		}
		close(sockDrv);
		close(sockConn);
		close(sockData);
	}
}

//main function
int main(int argc,char*argv[])
{
    if(argc<=1) // check if enter the port
    {
        cout<<"To run: a.out <port>"<<endl;
        return 0;
    }
    int port=atoi(argv[1]); //port is integer, atoi from char to int
    cout<<"Server open on "<<port<<endl; //server open successful
	
	SocKet s(port);
	s.Loop();
	return 1;
}