#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <string>
#include <thread>
#include <map>
#include "Utils.h"
#include "Console.h"

const unsigned short SERVER_PORT = 50000;

enum Message {
	HELLO,
	CHALLANGE,
	WELCOME,
	NONE
};

class SocketManager
{
public:
	static bool BindToPort(sf::UdpSocket* socket, int port) {
		return (socket->bind(port) == sf::Socket::Status::Done);
	}
};

struct Challange {
	int firstNumber;
	int secondNumber;

public:
	Challange() :firstNumber(0), secondNumber(0) {}
	Challange(int _firstNumber, int _secondNumber):firstNumber(_firstNumber), secondNumber(_secondNumber) {}
	int GetResult() {
		return firstNumber + secondNumber;
	}
};

struct IpPort {
	sf::IpAddress ip;
	unsigned short port;

public:
	IpPort():ip("127.0.0.1"), port(50000) {}
	IpPort(sf::IpAddress _ip, unsigned short _port) : ip(_ip), port(_port) {}
};

struct Client {
	IpPort connection;
	float clientSalt;
	float serverSalt;
	Challange challange;

public:
	Client() : connection(IpPort()), clientSalt(0), serverSalt(0), challange(Challange()) {}
	Client(sf::IpAddress _ipClient, unsigned short _port, float _clientSalt) : connection(IpPort(_ipClient, _port)), clientSalt(_clientSalt), serverSalt(0), challange(Challange()) {}
	~Client() {}
};

bool BindSocket(sf::UdpSocket& socket) {
	if (!SocketManager::BindToPort(&socket, SERVER_PORT)) {
		Utils::print("Something went wrong!");
		return false;
	}

	Utils::print("Binded to port " + Utils::intToString(SERVER_PORT));
	return true;
}

bool ClientExists(std::map<sf::IpAddress, unsigned short, float>& map, Client& client) {
	return (map.find(client.connection.ip) != map.end());
}

sf::Socket::Status ReceiveClient(sf::UdpSocket& socket, Client& client, sf::Packet& packet) {
	sf::IpAddress clientIpAdress;
	unsigned short clientPort;
	sf::Socket::Status status = socket.receive(packet, clientIpAdress, clientPort);

	if (status != sf::Socket::Status::Done) {
		return status;
	}
	client = Client(clientIpAdress, clientPort, 0);
}

Message IsMessage(std::string message) {
	if (message == "HELLO") return Message::HELLO;
	else if (message == "CHALLANGE") return Message::CHALLANGE;
	else if (message == "WELCOME") return Message::WELCOME;
	else return Message::NONE;
}

Message GetPacketHeadder(sf::Packet& packet) {
	std::string stringedMessage;
	packet >> stringedMessage;
	return IsMessage(stringedMessage);
}

float GenerateSalt() {
	//todo: generate a real salt
	return 0;
}

int GenerateRandomValueChallange() {
	return rand() % 50 + 1;
}

Client GetClient(sf::IpAddress address, unsigned short port, std::map<sf::IpAddress, unsigned short> &clients) {

}

int main()
{
	PlayerInfo playerInfo;
	sf::UdpSocket socket;
	bool running = true;
	std::map<sf::IpAddress, unsigned short> connectedClients;
	
	if (!BindSocket(socket)) return 1;

	std::thread thread(Console::console, std::ref(running));
	thread.detach();
	
	while (running)
	{
		sf::Packet packet;
		Client* client;
		ReceiveClient(socket, *client, packet);
		
		switch (GetPacketHeadder(packet)) {

		case Message::HELLO:
			packet >> client->clientSalt;
			client->serverSalt = GenerateSalt();
			sf::Packet helloPacket;
			helloPacket << "CHALLANGE" << GenerateRandomValueChallange() << GenerateRandomValueChallange();
			socket.send(helloPacket, client->connection.ip, client->connection.port);
			break;

		case Message::CHALLANGE:
			int result;
			packet >> result;
			std::string challangeClientAddress;
			packet >> challangeClientAddress;
			unsigned short challangeClientPort;
			packet >> challangeClientPort;
			Client client = GetClient(challangeClientAddress, challangeClientPort, connectedClients);

			if (result == client.challange.GetResult()) {

			}

		}

	}

	return 0;
}
