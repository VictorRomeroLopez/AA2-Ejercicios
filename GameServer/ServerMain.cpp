#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <unordered_map>
#include <mutex>
#include <windows.h>
#include <synchapi.h>

#include <SFML/Network.hpp>
#include <Console.h>
#include <Challange.h>
#include <Message.h>
#include <SocketManager.h>

#include "PlayerToValidateInfo.h"
#include "CriticalPacket.h"
#include "ValidatedPlayerInfo.h"
#include "ServerMain.h"

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


std::size_t GetHash(NetworkData _networkData) {

	return (std::hash<std::string>()(_networkData.GetIpAddress().toString())
		^ (std::hash<unsigned short>()(_networkData.GetPort()) << 1));

}

void AddAllPlayersToPacket(ValidatedPlayerInfo actualUser,sf::Packet& _packet, std::unordered_map<std::size_t, ValidatedPlayerInfo>& _validatedUsers) {
	auto it = _validatedUsers.find(GetHash(actualUser.GetNetworkData()));
	int numPlayers = 0;

	for (auto i = _validatedUsers.begin(); i != it; i++) {
		numPlayers++;
	}

	_packet << numPlayers;

	for (auto i = _validatedUsers.begin(); i != it; i++) {
		_packet << i->second.GetId() << i->second.GetNick() << i->second.GetPosition();
	}
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

void WarnOtherPlayersPlayerLeft(
	std::unordered_map<std::size_t, ValidatedPlayerInfo>& _validatedUsers,
	int& criticalPacketCounter,
	ValidatedPlayerInfo& _playerInfo,
	std::map<int, CriticalPacket>& _criticalPackets, 
	sf::UdpSocket& _socket
){
	for (std::pair<std::size_t, ValidatedPlayerInfo> data : _validatedUsers) {

		sf::Packet userGone;
		userGone << Message::Header::PLEAVE << criticalPacketCounter << _playerInfo.GetId();

		CriticalPacket cP = CriticalPacket(userGone, data.second.GetNetworkData());
		_criticalPackets.insert(std::pair<int, CriticalPacket>(criticalPacketCounter, cP));
		criticalPacketCounter++;

		SocketManager::HandleSocketErrors(_socket.send(userGone, data.second.GetNetworkData().GetIpAddress(), data.second.GetNetworkData().GetPort()));
	}
}

void ClientTimeout(sf::UdpSocket& _socket,
	ValidatedPlayerInfo& _playerInfo,
	std::unordered_map<std::size_t, ValidatedPlayerInfo>& _validatedUsers,
	std::map<int, CriticalPacket>& _criticalPackets,
	int& criticalPacketCounter) {

	_playerInfo.timeDifference = clock();
	double diff;

	do {

		if (!_playerInfo.connectedUser) return;
		diff = (clock() - _playerInfo.timeDifference) / (double)(CLOCKS_PER_SEC);

	} while (diff < cns::TIME_CLIENTS_TIMEOUT);

	mtx.lock();

	auto it = _validatedUsers.find(_playerInfo.GetId());
	if (it == _validatedUsers.end()) return;

	Utils::print(_playerInfo.GetNick() + " timed out!");
	_validatedUsers.erase(_playerInfo.GetId());
	WarnOtherPlayersPlayerLeft(_validatedUsers, criticalPacketCounter, _playerInfo, _criticalPackets, _socket);

	mtx.unlock();

}

void ManageCriticalPackets(sf::UdpSocket& _socket, 
	std::map<int, CriticalPacket>& _criticalPackets,
	bool& _running,
	std::unordered_map<std::size_t, ValidatedPlayerInfo>& _validatedUsers) {

	try {
		while (_running) {

			mtx.lock();

			for (std::pair<unsigned short, CriticalPacket> packet : _criticalPackets) {
			
				auto it = _validatedUsers.find(GetHash(packet.second.GetNetworkData()));

				if (it == _validatedUsers.end()) {
					Utils::print("Erassing packet");
					_criticalPackets.erase(packet.first);
					continue;
				}

				_socket.send(packet.second.GetPacket(), packet.second.GetNetworkData().GetIpAddress(), packet.second.GetNetworkData().GetPort());

			}
			mtx.unlock();

			Sleep(500);
		}
	}
	catch (int e) {
		Utils::print("Error at ManageCriticalErrors. Code " + std::to_string(e));
	}

}

void SendWelcomePacket(std::unordered_map<size_t, ValidatedPlayerInfo>& _validatedUsers,
	PlayerSalt& playerSalt,
	ValidatedPlayerInfo* validatedPlayer,
	sf::UdpSocket& _socket,
	int& criticalPacketCounter,
	std::map<int, CriticalPacket>& _criticalPackets,
	std::unordered_map<NetworkData, PlayerToValidateInfo, NetworkData::Hasher>& _usersToValidate)
{
	sf::Packet welcomePacket;

	mtx.lock();

	//Build welcome packet
	welcomePacket << Message::Header::WELCOME << playerSalt << validatedPlayer->GetId() << validatedPlayer->GetPosition();
	AddAllPlayersToPacket(*validatedPlayer, welcomePacket, _validatedUsers);

	//Send welcome packet
	Utils::print("packet sent " + Message::HeaderToString(Message::Header::WELCOME));
	_socket.send(welcomePacket, validatedPlayer->GetNetworkData().GetIpAddress(), validatedPlayer->GetNetworkData().GetPort());

	for (std::pair<std::size_t, ValidatedPlayerInfo> vPlayer : _validatedUsers) {

		sf::Packet pjoined;
		pjoined << Message::Header::PJOIN << criticalPacketCounter << validatedPlayer->GetId() << validatedPlayer->GetNick() << validatedPlayer->GetPosition();

		CriticalPacket cP = CriticalPacket(pjoined, vPlayer.second.GetNetworkData());
		_criticalPackets.insert(std::pair<int, CriticalPacket>(criticalPacketCounter, cP));
		criticalPacketCounter++;

		Utils::print("packet sent " + Message::HeaderToString(Message::Header::PJOIN));
		SocketManager::HandleSocketErrors(_socket.send(pjoined, vPlayer.second.GetNetworkData().GetIpAddress(), vPlayer.second.GetNetworkData().GetPort()));

	}

	_validatedUsers.insert(std::pair<std::size_t, ValidatedPlayerInfo>(validatedPlayer->GetId(), *validatedPlayer));
	_usersToValidate.erase(validatedPlayer->GetNetworkData());

	mtx.unlock();

	//Thread client timeout
	std::thread clientTimeout(ClientTimeout, std::ref(_socket), std::ref(*validatedPlayer), std::ref(_validatedUsers), std::ref(_criticalPackets), std::ref(criticalPacketCounter));
	clientTimeout.detach();
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

		if (Utils::GetRandomFloat() < cns::PERCENT_PACKETLOSS) {
			receivePakcet >> packetHeader;
			continue;
		}

		clientNetworkData = NetworkData(clientIpAdress, clientPort);

		receivePakcet >> packetHeader;
		Utils::print("packet gotcha " + Message::HeaderToString(packetHeader));

		switch (packetHeader) {
		case Message::Header::HELLO: {
			try {
				unsigned short clientSalt;
				receivePakcet >> clientSalt;

				mtx.lock();

				auto validatedUsersIterator = _validatedUsers.find(GetHash(clientNetworkData));
				if (validatedUsersIterator != _validatedUsers.end()) {
					Utils::print("entra aqui");

					mtx.unlock();

					SendWelcomePacket(_validatedUsers, 
						validatedUsersIterator->second.GetSalt(), 
						&validatedUsersIterator->second, 
						_socket, 
						criticalPacketCounter, 
						_criticalPackets, 
						_usersToValidate);
					break;
				}
				else {
					mtx.unlock();
				}


				auto it = _usersToValidate.find(clientNetworkData);
				if (it != _usersToValidate.end()) {

					Utils::print("packet sent " + Message::HeaderToString(Message::Header::CHALLANGE));
					SendChallange(_socket, GenerateChallange(it->second, false), it->second.GetNetworkData());
					break;

				}

				PlayerToValidateInfo pI = PlayerToValidateInfo(clientNetworkData, clientSalt);
				Utils::print("packet sent " + Message::HeaderToString(Message::Header::CHALLANGE));
				SendChallange(_socket, GenerateChallange(pI), pI.GetNetworkData());
				_usersToValidate.insert(std::pair<NetworkData, PlayerToValidateInfo>(pI.GetNetworkData(), pI ));

			}
			catch (int e) {
				Utils::print("Error at HELLO. Code " + std::to_string(e));
				_running = false;
			}
			break;

		}
		case Message::Header::CHALLANGE: {
			try{
				unsigned short challangeResponse;
				PlayerSalt playerSalt;
				std::string nick;

				receivePakcet >> challangeResponse >> playerSalt >> nick;

				auto it = _usersToValidate.find(clientNetworkData);
				if (it == _usersToValidate.end()) break;

				if (challangeResponse == it->second.GetChallange().GetResult() && playerSalt == it->second.GetSalt()) {

					ValidatedPlayerInfo* validatedPlayer = new ValidatedPlayerInfo(it->second, GetHash(it->second.GetNetworkData()), nick);
					SendWelcomePacket(_validatedUsers, playerSalt, validatedPlayer, _socket, criticalPacketCounter, _criticalPackets, _usersToValidate);

				}
			}
			catch (int e) {
				Utils::print("Error at CHALLANGE. Code " + std::to_string(e));
				_running = false;
			}
			break;

		}
		case Message::Header::EXIT: {
			
			try {

				PlayerSalt playerSalt;

				receivePakcet >> playerSalt;

				ValidatePlayerIterator validateIterator;

				mtx.lock();

				if (ValidateClient(playerSalt, clientNetworkData, _validatedUsers, validateIterator)) {

					ValidatedPlayerInfo playerInfo = validateIterator->second;

					validateIterator->second.connectedUser = false;
					_validatedUsers.erase(GetHash(validateIterator->second.GetNetworkData()));
					
					mtx.unlock();

					WarnOtherPlayersPlayerLeft(_validatedUsers, criticalPacketCounter, playerInfo, _criticalPackets, _socket);
				}
				else {

					mtx.unlock();
				}

			}
			catch (int e){
				Utils::print("Error at EXIT. Code " + std::to_string(e));
				_running = false;
			}
			break;
		}
		case Message::Header::ACK: {
			try {
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
			}
			catch (int e) {
				Utils::print("Error at ACK. Code " + std::to_string(e));
				_running = false;
			}
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

#pragma region CRITICAL_PACKETS_THREAD

	std::thread criticalPacketsThread(ManageCriticalPackets, 
		std::ref(serverSocket),
		std::ref(criticalPackets),
		std::ref(running),
		std::ref(validatedUsers));

	criticalPacketsThread.detach();

#pragma endregion

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
		Utils::print("Critical packets: " + std::to_string(criticalPackets.size()));
		Utils::print("Validated users: " + std::to_string(validatedUsers.size()));
		Sleep(500);
	}

	return 0;

}
