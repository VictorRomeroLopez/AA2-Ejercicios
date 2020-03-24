#pragma once
#include <PlayerSalt.h>
#include <PlayerInfo.h>

#include "PlayerToValidateInfo.h"

class ValidatedPlayerInfo : public PlayerInfo
{
private:
	NetworkData networkData;
	PlayerSalt saltContainer;
	
public:
	ValidatedPlayerInfo();
	ValidatedPlayerInfo(PlayerToValidateInfo _player, std::size_t _id, std::string _nick);

	NetworkData GetNetworkData();
	PlayerSalt GetSalt();
};

