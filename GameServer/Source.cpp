#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <string>
#include <thread>
#include "Utils.h"
#include "Console.h"

const int SERVER_PORT = 50000;

class SocketManager
{
public:
	static bool BindToPort(sf::UdpSocket* socket, int port) {
		return (socket->bind(port) == sf::Socket::Status::Done);
	}
};

struct Client {
	sf::IpAddress ipClient;
	int port;

public:
	Client(sf::IpAddress _ipClient, int _port) :ipClient(_ipClient), port(_port) {}
	~Client() {}
};

int main()
{
	PlayerInfo playerInfo;
	sf::UdpSocket socket;
	bool running = true;

	if (!SocketManager::BindToPort(&socket, SERVER_PORT)) {
		Utils::print("Something went wrong!");
		return 1;
	}

	Utils::print("Binded to port " + Utils::intToString(SERVER_PORT));

	std::thread thread(Console::console, std::ref(running));
	thread.detach();
	
	while (running)
	{

	}

	return 0;
}