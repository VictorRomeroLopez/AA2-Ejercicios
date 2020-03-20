#include "PlayerInfo.h"
#include <random>

PlayerInfo::PlayerInfo() : name(""), networkData(NetworkData()), clientSalt(0), serverSalt(0){}

PlayerInfo::PlayerInfo(NetworkData _networkData, int _clientSalt)
	: name(""), networkData(_networkData), clientSalt(_clientSalt), serverSalt(0) {}

PlayerInfo::~PlayerInfo() {}

NetworkData PlayerInfo::GetNetworkData()
{
	return networkData;
}

std::string PlayerInfo::GetName()
{
	return name;
}

int PlayerInfo::GetClientSalt()
{
	return clientSalt;
}

int PlayerInfo::GetServerSalt()
{
	return serverSalt;
}

int PlayerInfo::GenerateRandomClientSalt()
{
	clientSalt = Utils::GenerateRandomSalt();
	return clientSalt;
}

int PlayerInfo::GenerateRandomServerSalt()
{
	serverSalt = Utils::GenerateRandomSalt();
	return serverSalt;
}
