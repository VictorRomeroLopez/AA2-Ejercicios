#pragma once
#include <iostream>
#include <thread>
#include <windows.h>
#include <synchapi.h>

#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include "Message.h"
#include "HelloPacket.h"
#include "Challange.h"
#include "SocketManager.h"


void SendHelloPackets(sf::UdpSocket& _socket, unsigned short _clientSalt, bool& helloPacketReceived, bool& helloPacketSent) {
	sf::Packet _helloPacket;
	_helloPacket << Message::Header::HELLO << _clientSalt;

	while (!helloPacketReceived) {
		_socket.send(_helloPacket, cns::SERVER_IP, cns::SERVER_PORT);
		helloPacketSent = true;
		Sleep(3);
	}
}

int main()
{
	unsigned short clientSalt = Utils::GenerateRandomSalt();
	bool helloPacketReceived = false;
	bool helloPacketSent = false;

	sf::UdpSocket socket;
	sf::Packet receivePacket;

#pragma region SETUP HELLO PACKETS

	std::thread helloThread(SendHelloPackets,  std::ref(socket), clientSalt, std::ref(helloPacketReceived), std::ref(helloPacketSent));
	helloThread.detach();

	while (!helloPacketSent) {}

#pragma endregion

	sf::IpAddress serverIpAddress;
	unsigned short serverPort;

	if (!SocketManager::HandleSocketErrors(socket.receive(receivePacket, serverIpAddress, serverPort))) return 1;

	helloPacketReceived = true;

	Message::Header headderPacket = Message::Header::NONE;
	Challange challange;
	unsigned short packet;

	receivePacket >> headderPacket >> challange;

	Utils::print(Message::HeaderToString( headderPacket ) + " " + std::to_string(challange.GetResult()));

	while (true) {

	}

	return 0;
}