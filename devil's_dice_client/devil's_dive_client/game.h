#pragma once
#include "dice.h"
#include <iostream>
#include "SFML/Graphics.hpp"
#include <string>
#include <winsock2.h>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

struct Player {
	static const int amountOfDices = 6;
	char totalPoints[4];
	std::string totalPointsOld;
	char nums[amountOfDices][4];
	char chDice[amountOfDices][4];
};

class game
{
private:
	static const int amountOfDices = 6;
	const int diceSize = 100;
	const int sDiceSize = 50;
	char* ipAdr;
	int playerAmount;
	int totalPoints; //!
	int rerollAmount;
	int winner;
	dice dices[amountOfDices];
	int nums[amountOfDices]; //!
	bool chDice[amountOfDices]; //!
	bool rerollB;
	bool win;
	bool* first;
	char player[2]; //!
	int sizeOfAddr;
	WSADATA ws;
	SOCKADDR_IN sockaddr;
	SOCKET sCon;
	Player* pl;
	sf::Thread thread;
	sf::RenderWindow *window;
	sf::RectangleShape defDiceShape;
	sf::RectangleShape grayDiceShape;
	sf::RectangleShape smallDefDiceShape;
	sf::RectangleShape smallGrayDiceShape;
	sf::Font font;
	sf::Text text;
	sf::Text plText;
	sf::Text sText;
	/*sf::Text numValue;
	sf::Text pName;*/


public:
	game();
	void setPosition();
	void start();
	void restart();
	void recSend();
	void recPlNum();
	void recPlAmount();
	void con();
	void input();
	void rollAllDice();
	void rollOneDice(int diceNum);
	void choseDice(int diceNum);
	void reroll();
	void countTotalPoints();
	void getNum();
	void recData();
	void sendData();
	void draw();
};

