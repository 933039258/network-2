/**********************************************************************************
* Author: Peng Zhang
* CS 372 Fall 2019
* Due date: 12/1/2019
* Server program Header file
************************************************************************************/
#pragma once
#include <vector>
#include <string>
using namespace std;
#define MAXLINK 10
#define MAXBUF 2048
#define SOCKET int
#define byte unsigned char
#define SOCKADDR_IN  struct sockaddr_in

class SocKet
{
public:
	SocKet(int port);
	~SocKet();
	bool init(SOCKET& sockCon, int port);
	void getDir();
	int getFile(string filename);
	void Loop();
private:
	vector<string> vFileName;
	vector<string> file;
	int port;
	SOCKET sockSrv;//socket
	SOCKET sockClient;
};