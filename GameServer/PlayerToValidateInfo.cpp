#include "PlayerToValidateInfo.h"

PlayerToValidateInfo::PlayerToValidateInfo() {}

PlayerToValidateInfo::PlayerToValidateInfo(NetworkData _networkData, int _clientSalt) : networkData(_networkData)
{
	playerSalt = PlayerSalt(_clientSalt, playerSalt.GetServerSalt());
}

PlayerToValidateInfo::~PlayerToValidateInfo() {}

Challange PlayerToValidateInfo::GetChallange()
{
	return challange;
}

NetworkData PlayerToValidateInfo::GetNetworkData()
{
	return networkData;
}

PlayerSalt PlayerToValidateInfo::GetSalt()
{
	return playerSalt;
}

void PlayerToValidateInfo::GenerateRandomServerSalt()
{
	playerSalt.GenerateRandomServerSalt();
}

Challange PlayerToValidateInfo::GenerateRandomChallange()
{
	challange = Challange(Utils::GenerateRandom(10), Utils::GenerateRandom(10));
	return challange;
}