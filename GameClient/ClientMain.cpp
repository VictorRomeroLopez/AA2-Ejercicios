#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <windows.h>
#include <synchapi.h>
#include <time.h>

#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <PlayerSalt.h>
#include <Message.h>
#include <Challange.h>
#include <SocketManager.h>

#include "LocalPlayerInfo.h"

void SendLotsOfPackets(sf::UdpSocket& _socket, sf::Packet _packet, bool& _packetReceived, bool& _packetSent) {

	while (!_packetReceived) {
		_socket.send(_packet, cns::SERVER_IP, cns::SERVER_PORT);
		_packetSent = true;
		Sleep(3);
	}

}

bool IsPlayerConnected(std::unordered_map<std::size_t, PlayerInfo>& _playersConnected, std::size_t& playerid) {
	auto it = _playersConnected.find(playerid);
	return it != _playersConnected.end();
}

void SendACKPacket(sf::UdpSocket& _serverSocket, LocalPlayerInfo& _localPlayerInfo, int idPacket) {
	sf::Packet ackPacket;
	ackPacket << Message::Header::ACK << _localPlayerInfo.GetSalt() << idPacket;

	_serverSocket.send(ackPacket, cns::SERVER_IP, cns::SERVER_PORT);
}

void ReceiveClient(
	sf::UdpSocket& _serverSocket, 
	LocalPlayerInfo& _localPlayerInfo, 
	std::unordered_map<std::size_t,PlayerInfo>& _playersConnected) {

	sf::Packet receivePacket;
	sf::IpAddress serverIpAddress;
	unsigned short serverPort;
	Message::Header header;
	std::size_t clientId;
	sf::Vector2i position;

	//cambiar por una condicion decente
	while (true) {

		if (!SocketManager::HandleSocketErrors(_serverSocket.receive(receivePacket, serverIpAddress, serverPort))) return;

		receivePacket >> header;

		switch (header) {
		case Message::Header::CHALLANGE: {
			if (_localPlayerInfo.challangePacketReceived) break;
			_localPlayerInfo.challangePacketReceived = true;

			Challange challange;
			bool aux;
			unsigned short serverSalt;

			receivePacket >> challange >> serverSalt;
			_localPlayerInfo.SetServerSalt(serverSalt);

			sf::Packet challangeResponsePacket;
			challangeResponsePacket << Message::Header::CHALLANGE << challange.GetResult() << _localPlayerInfo.GetSalt() << _localPlayerInfo.GetNick();

			std::thread challangeThread(SendLotsOfPackets, std::ref(_serverSocket), challangeResponsePacket, std::ref(_localPlayerInfo.welcomePacketReceived), std::ref(aux));
			challangeThread.detach();

			break;
		}
		case Message::Header::WELCOME: {

			if (_localPlayerInfo.welcomePacketReceived) break;
			_localPlayerInfo.welcomePacketReceived = true;

			unsigned short receivedClientSalt;
			unsigned short receivedServerSalt;
			int numPlayersConnected;

			receivePacket >> receivedClientSalt >> receivedServerSalt >> clientId >> position >> numPlayersConnected;

			Utils::print("Welcome " + _localPlayerInfo.GetNick() + '!');

			if (numPlayersConnected == 0) break;

			for (int i = 0; i < numPlayersConnected; i++) {
				PlayerInfo* _playerInfo = new PlayerInfo();
				receivePacket >> *_playerInfo;
				_playersConnected.insert(std::pair<std::size_t, PlayerInfo>(_playerInfo->GetId(), *_playerInfo));
				Utils::print(_playerInfo->GetNick() + " has joined the lobby!");
			}
			
			break;

		}
		case Message::Header::PJOIN: {
			int idPacket;
			PlayerInfo playerJoining;

			receivePacket >> idPacket >> playerJoining;

			auto it = _playersConnected.find(playerJoining.GetId());
			if (it != _playersConnected.end()) break;

			_playersConnected.insert(std::pair<std::size_t, PlayerInfo>(playerJoining.GetId(), playerJoining));
			Utils::print(playerJoining.GetNick() + " has joined the lobby");

			SendACKPacket(_serverSocket, _localPlayerInfo, idPacket);

			break;
		}
		case Message::Header::PLEAVE: {
			int idPacket;
			std::size_t playerLeftId;

			receivePacket >> idPacket >> playerLeftId;

			auto it = _playersConnected.find(playerLeftId);
			if (it == _playersConnected.end()) break;

			Utils::print(it->second.GetNick() + " se ha desconectado.");
			_playersConnected.erase(playerLeftId);

			SendACKPacket(_serverSocket, _localPlayerInfo, idPacket);

			break;

		}
		case Message::Header::MESSAGE: {
			std::string message;
			receivePacket >> message;
			Utils::print(message);
		}
		default:
			break;
		}
	}
}

std::string AskForNick() {
	std::string nick;

	do {
		Utils::print("Nickname: ");
		std::getline(std::cin, nick);
	} while (nick == "");

	return nick;
}

int main()
{
	srand(time(NULL));

	LocalPlayerInfo localPlayerInfo = LocalPlayerInfo(AskForNick());

	sf::UdpSocket socket;
	sf::Packet receivePacket;
	std::string command;
	bool running = true;

	std::unordered_map<std::size_t,PlayerInfo> playersConnected;

#pragma region SETUP HELLO PACKETS

	sf::Packet helloPacket;
	helloPacket << Message::Header::HELLO << localPlayerInfo.GetSalt().GetClientSalt();

	std::thread helloThread(SendLotsOfPackets,  std::ref(socket), helloPacket, std::ref(localPlayerInfo.challangePacketReceived), std::ref(localPlayerInfo.helloPacketSent));
	helloThread.detach();

	while (!localPlayerInfo.helloPacketSent) {}

#pragma endregion

	std::thread receiveThread(ReceiveClient, std::ref(socket), std::ref(localPlayerInfo), std::ref(playersConnected));
	receiveThread.detach();

	while (running) {
		std::cin >> command;

		if (command == "exit") {

			sf::Packet exitPacket;
			exitPacket << Message::Header::EXIT << localPlayerInfo.GetSalt();

			if ( SocketManager::HandleSocketErrors(socket.send(exitPacket, cns::SERVER_IP, cns::SERVER_PORT))) return 1;
			running = false;

		}
	}

	return 0;
}