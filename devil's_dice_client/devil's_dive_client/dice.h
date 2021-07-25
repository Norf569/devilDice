#pragma once
#include <random>
#include <ctime>

class dice
{
private:
	int xPos, yPos;
	int number;
public:
	dice(int x, int y);
	dice() : dice(0, 0) {}
	int getPosX();
	int getPosY();
	int getNumber();
	void randChNum();
	void setPos(int x, int y);
};

