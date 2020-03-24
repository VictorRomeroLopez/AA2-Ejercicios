#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <unordered_map>
#include <mutex>
#include <windows.h>
#include <synchapi.h>

#include <SFML/Network.hpp>

#include "PlayerToValidateInfo.h"
#include "CriticalPacket.h"
#include "ValidatedPlayerInfo.h"
#include "SocketManager.h"
#include "Console.h"
#include "Message.h"
#include "Challange.h"

/*
#include "AA1_Ejercicio/PlayerInfo.h"
#include "AA1_Ejercicio/SocketManager.h"
#include "AA1_Ejercicio/Utils.h"
#include "AA1_Ejercicio/Console.h"
#include "AA1_Ejercicio/Message.h"
#include "AA1_Ejercicio/Challange.h"
#include "AA1_Ejercicio/HelloPacket.h"
*/
std::mutex mtx;

typedef std::unordered_map<std::size_t, ValidatedPlayerInfo>::iterator ValidatePlayerIterator;

void AddAllPlayersToPacket(sf::Packet& _packet, std::unordered_map<std::size_t, ValidatedPlayerInfo>& _validatedUsers) {

	for (std::pair<std::size_t, ValidatedPlayerInfo> info : _validatedUsers)
		_packet << info.second.GetId() << info.second.GetNick() << info.second.GetPosition();

}

std::size_t GetHash(NetworkData _networkData) {

	return (std::hash<std::string>()(_networkData.GetIpAddress().toString())
		^ (std::hash<unsigned short>()(_networkData.GetPort()) << 1));

}

bool BindSocket(sf::UdpSocket& socket) {

	if (!SocketManager::BindToPort(&socket, cns::SERVER_PORT)) return false;
	
	Utils::print("Binded to port " + std::to_string(cns::SERVER_PORT));
	return true;

}

sf::Packet GenerateChallange(PlayerToValidateInfo& playerInfo, bool _generateNewChallange = true) {

	sf::Packet generatedPacket;

	if (_generateNewChallange) {

		playerInfo.GenerateRandomChallange();
		playerInfo.GenerateRandomServerSalt();

	}

	generatedPacket << Message::Header::CHALLANGE << playerInfo.GetChallange() << playerInfo.GetSalt().GetServerSalt();
	return generatedPacket;

}

void SendChallange(sf::UdpSocket& _socket, sf::Packet& _packet, NetworkData& _networkData) {

	SocketManager::HandleSocketErrors(_socket.send(_packet, _networkData.GetIpAddress(), _networkData.GetPort()));

}

void ClientTimeout(ValidatedPlayerInfo& _playerInfo, std::unordered_map<std::size_t, ValidatedPlayerInfo>& _validatedUsers) {

	_playerInfo.timeDifference = clock();
	double diff;

	do {

		if (!_playerInfo.connectedUser) return;
		diff = (clock() - _playerInfo.timeDifference) / (double)(CLOCKS_PER_SEC);

	} while (diff < 10.0f);

	auto it = _validatedUsers.find(_playerInfo.GetId());
	if (it == _validatedUsers.end()) return;

	Utils::print(_playerInfo.GetNick() + " timed out!");
	_validatedUsers.erase(_playerInfo.GetId());

}

void ManageCriticalPackets(sf::UdpSocket& _socket, 
	std::map<int, CriticalPacket>& _criticalPackets,
	bool& _running,
	std::unordered_map<std::size_t, ValidatedPlayerInfo>& _validatedUsers) {

	while (_running) {
		mtx.lock();
		for (std::pair<unsigned short, CriticalPacket> packet : _criticalPackets) {
			
			auto it = _validatedUsers.find(GetHash(packet.second.GetNetworkData()));

			if (it == _validatedUsers.end()) {
				_criticalPackets.erase(packet.first);
				continue;
			}

			_socket.send(packet.second.GetPacket(), packet.second.GetNetworkData().GetIpAddress(), packet.second.GetNetworkData().GetPort());

			Sleep(3);
		}
		mtx.unlock();
	}

}

bool ValidateClient(PlayerSalt _playerSalt, NetworkData _networkData, std::unordered_map<std::size_t, ValidatedPlayerInfo>& _validatedUsers, ValidatePlayerIterator& validateIterator)
{
	validateIterator = _validatedUsers.find(GetHash(_networkData));
	return validateIterator != _validatedUsers.end() && _playerSalt == validateIterator->second.GetSalt();
}

void ReceiveClient(sf::UdpSocket& _socket, 
	bool _running, 
	std::unordered_map<NetworkData, PlayerToValidateInfo, NetworkData::Hasher>& _usersToValidate, 
	std::unordered_map<std::size_t, ValidatedPlayerInfo>& _validatedUsers, 
	std::map<int, CriticalPacket>& _criticalPackets
	) {

	NetworkData clientNetworkData;
	sf::IpAddress clientIpAdress;
	unsigned short clientPort;
	sf::Packet receivePakcet;
	Message::Header packetHeader;
	int criticalPacketCounter = 0;

	while (_running) {

		if (!SocketManager::HandleSocketErrors(_socket.receive(receivePakcet, clientIpAdress, clientPort))) return;

		clientNetworkData = NetworkData(clientIpAdress, clientPort);

		receivePakcet >> packetHeader;

		switch (packetHeader) {

		case Message::Header::HELLO: {

			unsigned short clientSalt;
			receivePakcet >> clientSalt;

			auto validatedUsersIterator = _validatedUsers.find(GetHash(clientNetworkData));
			if (validatedUsersIterator != _validatedUsers.end()) { break; }

			auto it = _usersToValidate.find(clientNetworkData);
			if (it != _usersToValidate.end()) {

				SendChallange(_socket, GenerateChallange(it->second, false), it->second.GetNetworkData());
				break;

			}

			PlayerToValidateInfo pI = PlayerToValidateInfo(clientNetworkData, clientSalt);
			SendChallange(_socket, GenerateChallange(pI), pI.GetNetworkData());
			_usersToValidate.insert(std::pair<NetworkData, PlayerToValidateInfo>(pI.GetNetworkData(), pI ));
			break;

		}
		case Message::Header::CHALLANGE: {

			unsigned short challangeResponse;
			PlayerSalt playerSalt;
			std::string nick;

			receivePakcet >> challangeResponse >> playerSalt >> nick;

			mtx.lock();
			auto it = _usersToValidate.find(clientNetworkData);
			if (it == _usersToValidate.end()) break;

			if (challangeResponse == it->second.GetChallange().GetResult() && playerSalt == it->second.GetSalt()) {

				ValidatedPlayerInfo* validatedPlayer = new ValidatedPlayerInfo(it->second, GetHash(it->second.GetNetworkData()), nick);
				sf::Packet welcomePacket;
				int numUsersConnected = _validatedUsers.size();

				//Build welcome packet
				welcomePacket << Message::Header::WELCOME << playerSalt << validatedPlayer->GetId() << validatedPlayer->GetPosition() << numUsersConnected;
				AddAllPlayersToPacket(welcomePacket, _validatedUsers);

				//Send welcome packet
				_socket.send(welcomePacket, validatedPlayer->GetNetworkData().GetIpAddress(), validatedPlayer->GetNetworkData().GetPort());

				for (std::pair<std::size_t, ValidatedPlayerInfo> vPlayer : _validatedUsers) {

					sf::Packet pjoined;
					pjoined << Message::Header::PJOIN << criticalPacketCounter << validatedPlayer->GetId() << validatedPlayer->GetNick()<< validatedPlayer->GetPosition();

					CriticalPacket cP = CriticalPacket(pjoined, vPlayer.second.GetNetworkData());
					_criticalPackets.insert(std::pair<int, CriticalPacket>(criticalPacketCounter, cP));
					criticalPacketCounter++;

					SocketManager::HandleSocketErrors(_socket.send(pjoined, vPlayer.second.GetNetworkData().GetIpAddress(), vPlayer.second.GetNetworkData().GetPort()));

				}

				_validatedUsers.insert(std::pair<std::size_t, ValidatedPlayerInfo>(validatedPlayer->GetId(), *validatedPlayer));
				_usersToValidate.erase(validatedPlayer->GetNetworkData());
				
				//Thread client timeout
				//std::thread clientTimeout(ClientTimeout, std::ref(*validatedPlayer), std::ref(_validatedUsers));
				//clientTimeout.detach();

			}
			mtx.unlock();

			break;

		}
		case Message::Header::EXIT: {

			PlayerSalt playerSalt;

			receivePakcet >> playerSalt;
			mtx.lock();
			ValidatePlayerIterator validateIterator;
			if (ValidateClient(playerSalt, clientNetworkData, _validatedUsers, validateIterator)) {

				ValidatedPlayerInfo playerInfo = validateIterator->second;

				validateIterator->second.connectedUser = false;
				_validatedUsers.erase(GetHash(validateIterator->second.GetNetworkData()));

				for (std::pair<std::size_t, ValidatedPlayerInfo> data : _validatedUsers) {

					sf::Packet userGone;
					userGone << Message::Header::PLEAVE << criticalPacketCounter << playerInfo.GetId();

					CriticalPacket cP = CriticalPacket(userGone, data.second.GetNetworkData());
					_criticalPackets.insert(std::pair<int, CriticalPacket>(criticalPacketCounter, cP));
					criticalPacketCounter++;

					SocketManager::HandleSocketErrors(_socket.send(userGone, data.second.GetNetworkData().GetIpAddress(), data.second.GetNetworkData().GetPort()));

				}

			}
			mtx.unlock();

			break;

		}
		case Message::Header::ACK: {
			PlayerSalt playerSalt;
			int idPacket;

			receivePakcet >> playerSalt >> idPacket;

			mtx.lock();
			ValidatePlayerIterator validatePlayerIterator;
			if (ValidateClient(playerSalt, clientNetworkData, _validatedUsers, validatePlayerIterator)) {
				auto it = _criticalPackets.find(idPacket);
				if (it == _criticalPackets.end()) break;

				_criticalPackets.erase(it);
			}
			mtx.unlock();

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

	std::map<int, CriticalPacket> criticalPackets;

	std::unordered_map<NetworkData, PlayerToValidateInfo , NetworkData::Hasher> usersToValidate;
	std::unordered_map<std::size_t, ValidatedPlayerInfo> validatedUsers;

	if (!BindSocket(serverSocket)) return 1;

#pragma region INIT CONSOLE
	std::thread thread(Console::console, std::ref(running));
	thread.detach();
#pragma endregion

	std::thread criticalPacketsThread(ManageCriticalPackets, 
		std::ref(serverSocket),
		std::ref(criticalPackets),
		std::ref(running),
		std::ref(validatedUsers));

	criticalPacketsThread.detach();

#pragma region INIT RECEIVE
	std::thread receiveClient(ReceiveClient
		, std::ref(serverSocket)
		, std::ref(running)
		, std::ref(usersToValidate)
		, std::ref(validatedUsers)
		, std::ref(criticalPackets)
	);
	receiveClient.detach();
#pragma endregion

	while (running) {

	}

	return 0;

}
