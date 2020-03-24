#pragma once
#include <PlayerSalt.h>
#include <Challange.h>
#include <NetworkData.h>

class PlayerToValidateInfo
{
private:
	Challange challange;
	NetworkData networkData;
	PlayerSalt playerSalt;

public:
	PlayerToValidateInfo();
	PlayerToValidateInfo(NetworkData _networkData, int _clientSalt);
	~PlayerToValidateInfo();

	Challange GetChallange();
	NetworkData GetNetworkData();
	PlayerSalt GetSalt();

	void GenerateRandomServerSalt();
	Challange GenerateRandomChallange();
};

