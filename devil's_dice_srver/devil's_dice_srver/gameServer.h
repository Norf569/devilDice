#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <string>

struct Player {
	static const int amountOfDices = 6;
	char totalPoints[4];
	char reroll[4];
	char nums[amountOfDices][4];
	char chDice[amountOfDices][4];
};

class gameServer
{
private:
	static const int amountOfDices = 6;
	char* ipAdr;
	int playerAmount;
	int rerollPlayer;
	int sizeOfAddr;
	int winner;
	char* win;
	char* chBool;
	Player* pl;
	WSADATA ws;
	SOCKET sListen;
	std::vector<SOCKET> plCon;
	SOCKADDR_IN sockaddr;
public:
	gameServer();
	void start();
	void restart();
	void lis();
	void newCon();
	void sendPlNum(int plNum);
	void sendPlAmount();
	void reciveData();
	void sendData();
	void winCheck();
};

