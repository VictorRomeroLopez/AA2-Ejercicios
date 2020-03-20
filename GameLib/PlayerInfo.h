#pragma once
#include <SFML/Network.hpp>
#include "NetworkData.h"
#include "Utils.h"

class PlayerInfo
{
	NetworkData networkData;
	std::string name;
	int clientSalt;
	int serverSalt;

public:

#pragma region CONSTUCTORS

	PlayerInfo();
	PlayerInfo(NetworkData _socket, int _clientSalt);
	~PlayerInfo();

#pragma endregion

#pragma region GETTERS & SETTERS

	NetworkData GetNetworkData();
	std::string GetName();
	int GetClientSalt();
	int GetServerSalt();

#pragma endregion
	
	int GenerateRandomClientSalt();
	int GenerateRandomServerSalt();

};