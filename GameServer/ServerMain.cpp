#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

#include <SFML/Network.hpp>

#include "AA1_Ejercicio/PlayerInfo.h"
#include "AA1_Ejercicio/SocketManager.h"
#include "AA1_Ejercicio/Utils.h"
#include "AA1_Ejercicio/Console.h"
#include "AA1_Ejercicio/Message.h"
#include "AA1_Ejercicio/Challange.h"
#include "AA1_Ejercicio/HelloPacket.h"

bool BindSocket(sf::UdpSocket& socket) {

	if (!SocketManager::BindToPort(&socket, cns::SERVER_PORT)) return false;
	
	Utils::print("Binded to port " + std::to_string(cns::SERVER_PORT));

	return true;
}

sf::Packet GenerateChallange() {
	sf::Packet generatedPacket;

	generatedPacket << Message::Header::CHALLANGE << Challange(Utils::GenerateRandom(10), Utils::GenerateRandom(10));
	return generatedPacket;
}

void SendChallange(sf::UdpSocket& _socket, sf::Packet& _packet, NetworkData& _networkData) {
	SocketManager::HandleSocketErrors(_socket.send(_packet, _networkData.GetIpAddress(), _networkData.GetPort()));
}

void ReceiveClient(sf::UdpSocket& _socket, std::unordered_map<NetworkData, PlayerInfo, NetworkData::Hasher>& _usersToValidate, bool _running) {
	sf::IpAddress clientIpAdress;
	unsigned short clientPort;
	sf::Packet receivePakcet;
	Message::Header packetHeader; 
	sf::Packet challangePacket; 
	NetworkData networkData;

	while (_running) {
		if (!SocketManager::HandleSocketErrors(_socket.receive(receivePakcet, clientIpAdress, clientPort))) return;

		receivePakcet >> packetHeader;

		switch (packetHeader) {
		case Message::Header::HELLO: {
			unsigned short clientSalt;
			receivePakcet >> clientSalt;
			
			auto it = _usersToValidate.find(NetworkData(clientIpAdress, clientPort));
			if (it != _usersToValidate.end()) {
				challangePacket = GenerateChallange();
				networkData = it->second.GetNetworkData();
				Utils::print(it->second.GetNetworkData().GetIpAddress().toString());
				SendChallange(_socket, challangePacket, networkData);
				break;
			}

			PlayerInfo pI = PlayerInfo({ clientIpAdress, clientPort }, clientSalt);

			Utils::print(clientIpAdress.toString());
			_usersToValidate.insert(std::pair<NetworkData,PlayerInfo>(pI.GetNetworkData(), pI));

			challangePacket = GenerateChallange();
			networkData = pI.GetNetworkData();

			SendChallange(_socket, challangePacket, networkData);
			break;
		}
		case Message::Header::CHALLANGE: {
			break;
		}
		case Message::Header::WELCOME: {
			break;
		}
		default: {
			break;
		}
		}
	}
}

int main()
{
	sf::UdpSocket serverSocket;
	bool running = true;
	std::unordered_map<NetworkData, PlayerInfo, NetworkData::Hasher> usersToValidate;

	if (!BindSocket(serverSocket)) return 1;

#pragma region INIT CONSOLE
	std::thread thread(Console::console, std::ref(running));
	thread.detach();
#pragma endregion

#pragma region INIT RECEIVE
	std::thread receiveClient(ReceiveClient, std::ref(serverSocket), std::ref(usersToValidate), std::ref(running));
	receiveClient.detach();
#pragma endregion

	while (running) {

	}

	return 0;

}
