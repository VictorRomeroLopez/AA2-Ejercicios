#include "ValidatedPlayerInfo.h"

ValidatedPlayerInfo::ValidatedPlayerInfo() {}

ValidatedPlayerInfo::ValidatedPlayerInfo(PlayerToValidateInfo _player, std::size_t _id, std::string _nick) :
	networkData(_player.GetNetworkData())
{
	playerId = _id;
	nick = _nick;
	position = Utils::GenerateRandomMapPosition();
	saltContainer = PlayerSalt(_player.GetSalt());
}

NetworkData ValidatedPlayerInfo::GetNetworkData()
{
	return networkData;
}

PlayerSalt ValidatedPlayerInfo::GetSalt()
{
	return saltContainer;
}
