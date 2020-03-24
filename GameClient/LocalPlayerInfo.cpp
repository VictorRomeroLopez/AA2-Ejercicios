#include "LocalPlayerInfo.h"

LocalPlayerInfo::LocalPlayerInfo() : helloPacketSent(false), challangePacketReceived(false), welcomePacketReceived(false)
{
	saltContainer = PlayerSalt(Utils::GenerateRandomSalt());
}

LocalPlayerInfo::LocalPlayerInfo(std::string _nick) : helloPacketSent(false), challangePacketReceived(false), welcomePacketReceived(false)
{
	saltContainer = PlayerSalt(Utils::GenerateRandomSalt());
	nick = _nick;
}

void LocalPlayerInfo::SetServerSalt(std::size_t _serverSalt)
{
	saltContainer.SetServerSalt(_serverSalt);
}

PlayerSalt LocalPlayerInfo::GetSalt()
{
	return saltContainer;
}

