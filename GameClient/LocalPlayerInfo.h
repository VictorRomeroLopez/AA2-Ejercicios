#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

#include <PlayerInfo.h>
#include <PlayerSalt.h>

class LocalPlayerInfo : public PlayerInfo
{
private:
	PlayerSalt saltContainer;

public:
	bool helloPacketSent;
	bool challangePacketReceived;
	bool welcomePacketReceived;

	LocalPlayerInfo();
	LocalPlayerInfo(std::string _nick);

	void SetServerSalt(std::size_t _serverSalt);
	PlayerSalt GetSalt();
};

