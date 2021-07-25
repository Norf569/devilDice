#include "dice.h"

dice::dice(int x, int y)
{
	srand(time(0));
	xPos = x;
	yPos = y;
	number = 0;
}

void dice::setPos(int x, int y)
{
	xPos = x;
	yPos = y;
}

int dice::getPosX()
{
	return xPos;
}

int dice::getPosY()
{
	return yPos;
}

int dice::getNumber()
{
	return number;
}

void dice::randChNum()
{
	number = rand() % 6 + 1;
}
