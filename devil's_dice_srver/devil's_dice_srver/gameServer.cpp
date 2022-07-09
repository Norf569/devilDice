#include "gameServer.h"

gameServer::gameServer()
{
	if (FAILED(WSAStartup(MAKEWORD(2, 1), &ws)))
	{
		std::cout << "Startup failed\n";
	}

	sListen = socket(AF_INET, SOCK_STREAM, NULL);
	if (sListen == INVALID_SOCKET)
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

	bind(sListen, (SOCKADDR*)&sockaddr, sizeof(sockaddr));

	plCon.resize(0);
	playerAmount = 0;
	rerollPlayer = 0;

	win = new char[4];
	memset(win, 0, sizeof(win));
	winner = -1;
}

void gameServer::start()
{
	std::cout << "Start...\n";
	lis();
	sendPlAmount();
	pl = new Player[playerAmount];
	chBool = new char[playerAmount];
	memset(chBool, 0, sizeof(chBool));
	while (true)
	{
		winCheck();
		reciveData();
		sendData();
		if (win[0] == '1')
			restart();
	}
}

void gameServer::restart()
{
	memset(win, 0, sizeof(win));
	winner = -1;
	pl = new Player[playerAmount];
}

void gameServer::lis()
{
	std::cout << "Listening...\n";
	std::string temp = "";	
	while (temp != "stop") {
		listen(sListen, SOMAXCONN);
		newCon();
		std::cin >> temp;
	}
	//
	char a[256] = "a";
	for (int i = 0; i < playerAmount; i++)
		send(plCon[i], (char*)a, sizeof(a), NULL);
	//
}

void gameServer::newCon()
{
	SOCKET newConnection;
	newConnection = accept(sListen, (SOCKADDR*)&sockaddr, &sizeOfAddr);
	if (newConnection == 0)
		std::cout << "Connection failed";
	else {
		std::cout << "New Connection...\n";
		plCon.push_back(newConnection);
		playerAmount++;
		sendPlNum(playerAmount-1);
	}
}

void gameServer::sendPlNum(int plNum)
{
	char plNumChar[1];
	plNumChar[0] = char(plNum + '0');
	send(plCon[plNum], (char*)plNumChar, sizeof(plNumChar), NULL); //не работает при количестве игроков > 10 EDIT: вроде уже работает
	std::cout << "plNum send...\n";
}

void gameServer::sendPlAmount()
{
	char plAmountChar[256];
	plAmountChar[0] = char(playerAmount + '0');
	for (int i = 0; i < playerAmount; i++)
		send(plCon[i], (char*)plAmountChar, sizeof(plAmountChar), NULL); //не работает при количестве игроков > 10 EDIT: вроде уже работает
	std::cout << "plAmount send...\n";
}

void gameServer::reciveData()
{
	for (int i = 0; i < playerAmount; i++) {
		recv(plCon[i], (char*)pl[i].totalPoints, sizeof(pl[i].totalPoints), NULL);
		recv(plCon[i], (char*)pl[i].reroll, sizeof(pl[i].reroll), NULL);
		//std::cout << pl[i].totalPoints << '\n';
		for (int j = 0; j < amountOfDices; j++)
		{
			recv(plCon[i], (char*)pl[i].nums[j], sizeof(pl[i].nums[j]), NULL);
			recv(plCon[i], (char*)pl[i].chDice[j], sizeof(pl[i].chDice[j]), NULL);
			//std::cout << pl[i].nums[j] << ' ' << pl[i].chDice[j] << '\n';
		}
	}
	//std::cout << "Recived";
}

void gameServer::sendData()
{
	for (int i = 0; i < playerAmount; i++)
	{
		send(plCon[i], (char*)win, sizeof(win), NULL);
		if (win[0] == '1')
		{
			char* winnerChar = new char[4];
			memset(winnerChar, 0, sizeof(winnerChar));
			winnerChar[0] = char(winner + '0');
			//std::cout << winner << ' ' << winnerChar << '\n';
			send(plCon[i], (char*)winnerChar, sizeof(winnerChar), NULL);
		}
		else {
			for (int j = 0; j < playerAmount; j++)
			{
				send(plCon[i], (char*)pl[j].totalPoints, sizeof(pl[j].totalPoints), NULL);
				for (int o = 0; o < amountOfDices; o++)
				{
					send(plCon[i], (char*)pl[j].nums[o], sizeof(pl[j].nums[o]), NULL);
					send(plCon[i], (char*)pl[j].chDice[o], sizeof(pl[j].chDice[o]), NULL);
				}
			}
		}
	}
	//std::cout << "Send";
}

void gameServer::winCheck()
{
	int rerollPlayer = 0;
	for (int i = 0; i < playerAmount; i++)
	{
		if (pl[i].reroll[0] == '1')
			rerollPlayer++;
	}
	if (rerollPlayer == playerAmount)
	{
		bool draw = true;
		for (int i = 0; i < playerAmount; i++) {
			if (draw && std::string(pl[i].totalPoints) <= "21")
				winner = i, draw = false;
			else if (std::string(pl[i].totalPoints) > std::string(pl[winner].totalPoints) && std::string(pl[i].totalPoints) <= "21")
				winner = i, draw = false;
		}
		win[0] = '1';
	}
}