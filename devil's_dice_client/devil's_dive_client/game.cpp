#include "game.h"

game::game() : thread(&game::recSend, this)
{
	window = new sf::RenderWindow(sf::VideoMode(1000, 600), "Devil's dice");
	defDiceShape.setSize(sf::Vector2f(diceSize, diceSize));
	grayDiceShape.setSize(sf::Vector2f(diceSize, diceSize));
	grayDiceShape.setFillColor(sf::Color(192, 192, 192));
	smallDefDiceShape.setSize(sf::Vector2f(sDiceSize, sDiceSize));
	smallGrayDiceShape.setSize(sf::Vector2f(sDiceSize, sDiceSize));
	smallGrayDiceShape.setFillColor(sf::Color(192, 192, 192));
	if (!font.loadFromFile("arial.ttf")) {
		std::cout << "F*cking font";
	}
	text.setFont(font);
	text.setCharacterSize(30);
	text.setFillColor(sf::Color(0, 0, 0));
	sText.setFont(font);
	sText.setCharacterSize(15);
	sText.setFillColor(sf::Color(0, 0, 0));
	plText.setFont(font);
	plText.setCharacterSize(20);
	plText.setFillColor(sf::Color(0, 0, 0));
	/*numValue.setFont(font);
	numValue.setCharacterSize(30);
	numValue.setFillColor(sf::Color(0, 0, 0));
	pName.setFont(font);
	pName.setCharacterSize(30);
	pName.setFillColor(sf::Color(0, 0, 0));*/
	totalPoints = 0;
	rerollAmount = 4;
	winner = -2;
	win = false;
	//
	if (FAILED(WSAStartup(MAKEWORD(2, 1), &ws)))
	{
		std::cout << "Startup failed\n";
	}

	sCon = socket(AF_INET, SOCK_STREAM, NULL);
	if (sCon == INVALID_SOCKET)
	{
		std::cout << "Socket invalid\n";
	}

	ipAdr = new char[256];
	std::cout << "Enter server IP address: ";
	std::cin.getline(ipAdr, 256);

	sockaddr.sin_addr.s_addr = inet_addr(ipAdr);
	sockaddr.sin_port = htons(1111);
	sockaddr.sin_family = AF_INET;
	sizeOfAddr = sizeof(sockaddr);
	//
	//
	setPosition();
}

void game::setPosition()
{
	for (int i = 0; i < amountOfDices; i++)										
	{
		dices[i].setPos(100 + i * (100 + 20), 20);
	}
}

void game::start()
{
	con();
	//
	char temp[256]; //удалить потом
	recv(sCon, (char*)temp, sizeof(temp), NULL);
	recPlAmount();
	//
	//std::cout << playerAmount << '\n';
	rollAllDice();
	first = new bool[playerAmount];
	memset(first, 0, sizeof(first));
	//thread.launch();
	while (window->isOpen())
	{
		sendData();
		recData();
		input();
		draw();
		if (win) {
			double timeStart = time(NULL);
			double timeEnd = time(NULL);
			while (timeEnd - timeStart < 5) {
				timeEnd = time(NULL);
				input();
				draw();
			}
			restart();
		}
	}
}

void game::recSend()
{
	while (window->isOpen()) {
		sendData();
		recData();
	}
}

void game::con()
{
	while (connect(sCon, (SOCKADDR*)&sockaddr, sizeof(sockaddr)) != 0) {
		std::cout << "Connecting...\n";
	}
	recv(sCon, (char*)player, sizeof(player), NULL);
	std::cout << "Connected... " << player << '\n';
	//recPlNum();
}

void game::recPlNum()
{
	recv(sCon, (char*)player, sizeof(player), NULL);
}

void game::restart()
{
	win = false;
	winner = -2;
	rollAllDice();
	memset(chDice, 0, sizeof(chDice));
	rerollB = false;
	rerollAmount = 4;
	pl = new Player[playerAmount];
	memset(first, 0, sizeof(first));
}

void game::recPlAmount()
{
	char plAmountChar[256];
	recv(sCon, (char*)plAmountChar, sizeof(plAmountChar), NULL);
	playerAmount = plAmountChar[0] - '0'; // не работает при игроков > 10
	pl = new Player[playerAmount];
}

void game::input()
{
	sf::Event event;
	while (window->pollEvent(event))
	{
		int cordX, cordY;
		switch (event.type)
		{
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::MouseButtonPressed:
			cordX = event.mouseButton.x;
			cordY = event.mouseButton.y;
			for (int i = 0; i < amountOfDices; i++)
			{
				int xPos = dices[i].getPosX();
				int yPos = dices[i].getPosY();
				if (cordX >= xPos && cordY >= yPos && cordX <= diceSize + xPos && cordY <= diceSize + yPos && rerollAmount > 0)
				{
					choseDice(i);
					rerollAmount--;
					//std::cout << "Pressed " << i + 1 << '\n';
				}
			}
			break;
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Enter && !rerollB)
			{
				rerollB = true;
				rerollAmount = 0;
				reroll();
			}
			break;
		default:
			break;
		}
			
	}
}

void game::rollAllDice()
{
	for (int i = 0; i < amountOfDices; i++)
	{
		dices[i].randChNum();
	}
	getNum();
}

void game::rollOneDice(int diceNum)
{
	dices[diceNum].randChNum();
}

void game::choseDice(int diceNum)
{
	chDice[diceNum] = true;
}

void game::reroll()
{
	for (int i = 0; i < amountOfDices; i++)
	{
		if (chDice[i])
			rollOneDice(i);
	}
	getNum();
}

void game::countTotalPoints()
{
	totalPoints = 0;
	for (int i = 0; i < amountOfDices; i++)
		totalPoints += nums[i];
}

void game::getNum()
{
	for (int i = 0; i < amountOfDices; i++)
	{
		nums[i] = dices[i].getNumber();
	}
	countTotalPoints();
}

void game::recData()
{
	char* winChar = new char[4];
	memset(winChar, 0, sizeof(winChar));
	recv(sCon, (char*)winChar, sizeof(winChar), NULL);
	if (winChar[0] == '1') {
		win = true;
		char* winnerChar = new char[4];
		memset(winnerChar, 0, sizeof(winnerChar));
		recv(sCon, (char*)winnerChar, sizeof(winnerChar), NULL);
		//std::cout << winnerChar << '\n';
		winner = int(winnerChar[0] - '0');
	}
	else {
		for (int i = 0; i < playerAmount; i++) {
			recv(sCon, (char*)pl[i].totalPoints, sizeof(pl[i].totalPoints), NULL);
			//std::cout << totalPoints << '\n';
			for (int j = 0; j < amountOfDices; j++)
			{
				recv(sCon, (char*)pl[i].nums[j], sizeof(pl[i].nums[j]), NULL);
				recv(sCon, (char*)pl[i].chDice[j], sizeof(pl[i].chDice[j]), NULL);
				//std::cout << pl[i].nums[j] << ' ' << pl[i].chDice[j] << '\n';
			}
		}
	}
}

void game::sendData()
{
	char *totalPointsChar = new char[4];
	char *numsChar = new char[4];
	char *chDiceChar = new char[4];
	char* chBool = new char[4];
	char* rerollChar = new char[4];
	//
	memset(totalPointsChar, 0, sizeof(totalPointsChar));
	memset(numsChar, 0, sizeof(numsChar));
	memset(chDiceChar, 0, sizeof(chDiceChar));
	memset(chBool, 0, sizeof(chBool));
	memset(rerollChar, 0, sizeof(rerollChar));
	//
	std::string tmp = std::to_string(totalPoints); //удалить потом
	for (int i = 0; i < tmp.size(); i++)
		totalPointsChar[i] = tmp[i];
	//
	rerollChar[0] = char(rerollB + '0');
	send(sCon, (char*)totalPointsChar, sizeof(totalPointsChar), NULL);
	send(sCon, (char*)rerollChar, sizeof(rerollChar), NULL);
	//std::cout << totalPointsChar << '\n';
	for (int o = 0; o < amountOfDices; o++)
	{
		numsChar[0] = char(nums[o] + '0');
		chDiceChar[0] = char(chDice[o] + '0');
		send(sCon, (char*)numsChar, sizeof(numsChar), NULL);
		send(sCon, (char*)chDiceChar, sizeof(chDiceChar), NULL);
		//std::cout << numsChar << ' ' << chDiceChar << '\n';
	}
}

void game::draw()
{
	/*for (int i = 0; i < amountOfDices; i++)
		std::cout << nums[i] << ' ';
	std::cout << '\n';
	start();*/

	window->clear(sf::Color(0, 225, 0));
	text.setString("You:");
	text.setPosition(20, 20 + 50 - 20);
	window->draw(text);
	for (int i = 0; i < amountOfDices; i++) {
		if (chDice[i])
		{
			grayDiceShape.setPosition(sf::Vector2f(100 + i * (100 + 20), 20));
			window->draw(grayDiceShape);
		}
		else 
		{
			defDiceShape.setPosition(sf::Vector2f(100 + i * (100 + 20), 20));
			window->draw(defDiceShape);
		}
		text.setPosition(sf::Vector2f(100 + 50 - 10 + i * (100 + 20), 20 + 50 - 20));
		text.setString(std::to_string(nums[i]));
		window->draw(text);
	}	
	text.setString("Total Points: " + std::to_string(totalPoints));
	text.setPosition(sf::Vector2f(100, 100 + 20 + 20));
	window->draw(text);
	text.setString("Rick Rolls: " + std::to_string(rerollAmount));
	text.setPosition(sf::Vector2f(100 + 100 + 100 + 20 + 20, 100 + 20 + 20));
	window->draw(text);
	sText.setString("(Press Enter to continue)");
	sText.setPosition(sf::Vector2f(100 + 100 + 100 + 20 + 20, 100 + 20 + 20 + 30));
	window->draw(sText);
	//
	int tmp = 0;
	for (int i = 0; i < playerAmount; i++) {
		if (i == int(player[0] - '0')) {
			tmp++;
			if (winner == i)
			{
				text.setString("You win!");
				text.setPosition(720 + 100, 20 + 50 - 20);
				window->draw(text);
			}
			else if (winner == -1)
			{
				text.setString("Fuck");
				text.setPosition(720 + 100, 20 + 50 - 20);
				window->draw(text);
			}
			continue;
		}
		plText.setString(std::to_string(i + 1));
		plText.setPosition(sf::Vector2f(20, 100 + 20 + 100 + (i - tmp) * 100));
		window->draw(plText);
		for (int j = 0; j < amountOfDices; j++) {
			if (pl[i].chDice[j][0] == '0') {
				smallDefDiceShape.setPosition(sf::Vector2f(60 + j * (10 + 50), 100 + 10 + 100 + (i - tmp) * 100));
				window->draw(smallDefDiceShape);
				plText.setString(pl[i].nums[j]);
				plText.setPosition(sf::Vector2f(60 + 20 + j * (10 + 50), 100 + 20 + 100 + (i - tmp) * 100));
				window->draw(plText);
			}
			else {
				smallGrayDiceShape.setPosition(sf::Vector2f(60 + j * (10 + 50), 100 + 10 + 100 + (i - tmp) * 100));
				window->draw(smallGrayDiceShape);
				if (win == true)
				{
					//std::cout << "yes";
					plText.setString(pl[i].nums[j]);
					plText.setPosition(sf::Vector2f(60 + 20 + j * (10 + 50), 100 + 20 + 100 + (i - tmp) * 100));
					window->draw(plText);
				}
			}
			
		}
		/*std::string plTotalPoints = "";
		for (int i = 0; i < sizeof(pl[i].totalPoints); i++)
		{
			plTotalPoints += pl[i].totalPoints;
		}*/
		if (win || !first[i]) {
			pl[i].totalPointsOld = pl[i].totalPoints;
			first[i] = true;
			//std::cout << winner << '\n';
		}
		plText.setString("Total Points: " + std::string(pl[i].totalPointsOld));
		plText.setPosition(sf::Vector2f(60, 100 + 10 + 100 + (i - tmp) * 100 + 60));
		window->draw(plText);
		if (winner == i)
		{
			plText.setString("Winner!");
			plText.setPosition(sf::Vector2f(60 + 20 + 300 + 50 + 10, 100 + 20 + 100 + (i - tmp) * 100));
			window->draw(plText);
		}
		else if (winner == -1)
		{
			plText.setString("Fuck");
			plText.setPosition(sf::Vector2f(60 + 20 + 300 + 50 + 10, 100 + 20 + 100 + (i - tmp) * 100));
			window->draw(plText);
		}
	}
	//
	window->display();
}
